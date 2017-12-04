#include <iostream>
#include <locale>
#include <fstream>
#include <iomanip>
#include <omp.h>
#include <vector>
#include "Spider.hpp"
#include "Dictionary.hpp"
#include "Document.hpp"
#include "AvlTreeInvertedIndex.hpp"
#include "CsvUtility.hpp"
#include "GuiCore.hpp"

using namespace std;

int main()
{
    const locale loc("chs");

    Dictionary dict;

    cout << "Constructing dictionaries, please wait.\n";
    dict.AddDictionary("./Professional.dic");
    dict.AddDictionary("./Universal.dic");

    wifstream fin;
    wofstream fout;
    wstring finReader;

    fin.imbue(loc);
    fout.imbue(loc);

    // Read urls and save them in a vector.
    fin.open(L"./url.csv");

    getline(fin, finReader);

    vector<wstring> urls;
    urls.reserve(2000);
    cout << "Reading urls.\n";

    while (getline(fin, finReader, L'\n'))
    {
        urls.push_back(finReader);
    }

    // Download documents.
    // todo free
    AvlTree<int, Document*, less<int>> allDocuments;
    const auto size = urls.size();

    AvlTreeInvertedIndex invertedIndex;


#pragma omp parallel for num_threads(16)
    for (auto i = 0; i < static_cast<int>(size); i++)
    {
        // Parse this line.
        const CharString cs(urls[i]);
        auto list = CsvUtility::CsvDecode(cs);

        const auto id = stoi(list.GetItemAt(0).ToStdWstring());
        auto url = list.GetItemAt(1);
        url = url.GetSubstring(1, url.GetLength() - 1);

        auto document = new Document();

        try
        {
            document->AssignId(id);
            document->UpdateFromUrl(url.ToStdWstring(), dict);
        }
        catch (const exception&)
        {
            wcout << L"Failed: " << url.ToStdWstring() << endl;
            delete document;
            continue;
        }

#pragma omp critical
        {
            cout << "Start parsing for url #" << setw(4) << i << ", in thread " << omp_get_thread_num() << endl;
            cout.flush();
            allDocuments.Insert(document->Id, document);
        }

        auto& wordList = document->Words;
        AvlTree<CharString, int, less<CharString>> countedWords;

        for (const auto& word : wordList)
        {
            if (!countedWords.Contains(word))
            {
                countedWords.Insert(word, 0);
                const auto increase = document->CountWords(word);
#pragma omp critical
                {
                    invertedIndex.AddOccurrence(word, document, increase);
                }
            }
        }
    }

    // Now we have constructed the inverted index.
    // This is the console application. We need to load the queries and perform them.

    fin.close();
    fin.open(L"query.txt");

    fout.imbue(locale::classic());
    fout.open(L"result.txt");

    auto queryCount = 0;

    while (getline(fin, finReader))
    {
        cout << "Performing query #" << queryCount;

        CharString cs(finReader);
        CharStringList slices = Split(cs,L' ');
        auto result = invertedIndex.Query(slices);
        result.InorderTraversal(
            [&fout](const int& id,const int& times)-> void
            {
                fout << L'(' << id << L',' << times << L") ";
            }
        );

        fout << endl;
    }

    function<void(const int&, Document* const &)> deleteFunction = [](const int& id, Document*const& document)->void
    {
        delete document;
    };

    allDocuments.InorderTraversal(deleteFunction);

    return 0;
}
