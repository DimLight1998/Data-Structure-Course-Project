#pragma once

#include <iostream>
#include <locale>
#include <fstream>
#include <iomanip>
#include <omp.h>
#include <vector>
#include <unordered_map>
#include <functional>
#include "Dictionary.hpp"
#include "Document.hpp"
#include "AvlTreeInvertedIndex.hpp"
#include "CsvUtility.hpp"

// todo delete documents

public ref class GuiCore
{
public:
    delegate void Complete();
    delegate void ProgressReport(double);

    Complete^ DictionaryLoadComplete = gcnew Complete(EmptyComplete);
    ProgressReport^ ParsingProgressReport = gcnew ProgressReport(EmptyProgressReport);
public:
    void InitializeDictionary();
    void ProcessUrls();
    void DestroyDictionary();

    System::Collections::Generic::Dictionary<int, int>^ Query(System::String^ query);

    GuiCore();
    ~GuiCore();
    !GuiCore();

    static void EmptyComplete() {}
    static void EmptyProgressReport(double) {}

    System::String^ GetPostTitle(int documentId);
    System::String^ GetPostContent(int documentId);

private:
    Dictionary* _dictionary = nullptr;
    AvlTreeInvertedIndex* _invertedIndex = nullptr;

    AvlTree<int, Document*, std::less<int>>* _allDocuments = nullptr;
};

GuiCore::GuiCore()
{
    _dictionary = new Dictionary();
    _invertedIndex = new AvlTreeInvertedIndex();
    _allDocuments = new AvlTree<int, Document*, std::less<int>>();
}

inline GuiCore::!GuiCore()
{
    delete _dictionary;
    _dictionary = nullptr;

    delete _invertedIndex;
    _invertedIndex = nullptr;

    delete _allDocuments;
    _allDocuments = nullptr;
}

inline System::String ^ GuiCore::GetPostTitle(int documentId)
{
    return gcnew System::String(_allDocuments->Search(documentId)->PostTitle.ToStdWstring().c_str());   
}

inline System::String ^ GuiCore::GetPostContent(int documentId)
{
    return gcnew System::String(_allDocuments->Search(documentId)->PostContent.ToStdWstring().c_str());
}

inline GuiCore::~GuiCore()
{
    delete _dictionary;
    _dictionary = nullptr;

    delete _invertedIndex;
    _invertedIndex = nullptr;

    delete _allDocuments;
    _allDocuments = nullptr;
}

inline void GuiCore::InitializeDictionary()
{
    _dictionary->AddDictionary("./Professional.dic");
    _dictionary->AddDictionary("./Universal.dic");
    DictionaryLoadComplete();
}

inline void GuiCore::ProcessUrls()
{
    std::wifstream fin;
    std::wstring finReader;
    fin.imbue(std::locale("chs"));
    fin.open(L"./url.csv");

    getline(fin, finReader);

    std::vector<std::wstring> urls;
    urls.reserve(2000);

    while (getline(fin, finReader, L'\n'))
    {
        urls.push_back(finReader);
    }

    const auto size = urls.size();

    auto progressCount = 0;
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
            document->UpdateFromUrl(url.ToStdWstring(), *_dictionary);
        }
        catch (const std::exception&)
        {
            delete document;
            continue;
        }

#pragma omp critical
        {
            _allDocuments->Insert(document->Id, document);
            progressCount++;
            ParsingProgressReport(static_cast<double>(progressCount) / static_cast<double>(size));
        }

        auto& wordList = document->Words;
        AvlTree<CharString, int, std::less<CharString>> countedWords;

        for (const auto& word : wordList)
        {
            if (!countedWords.Contains(word))
            {
                countedWords.Insert(word, 0);
                const auto increase = document->CountWords(word);
#pragma omp critical
                {
                    _invertedIndex->AddOccurrence(word, document, increase);
                }
            }
        }
    }
}

inline void GuiCore::DestroyDictionary()
{
    delete _dictionary;
    _dictionary = nullptr;
}

inline System::Collections::Generic::Dictionary<int, int>^ GuiCore::Query(System::String ^ query)
{
    auto list = query->Split(gcnew array<wchar_t>{L' '}, System::StringSplitOptions::RemoveEmptyEntries);
    std::vector<std::wstring> queries;

    for (auto i = 0; i < list->Length; i++)
    {
        auto queryWord = list[i];
        using namespace System::Runtime::InteropServices;
        auto chars = (const wchar_t*)(Marshal::StringToHGlobalUni(queryWord)).ToPointer();
        queries.push_back(chars);
        Marshal::FreeHGlobal(System::IntPtr((void*)chars));
    }

    CharStringList charStringList;
    for (const auto& i : queries)
    {
        CharString charString(i);
        charStringList.Append(charString);
    }

    auto result = _invertedIndex->Query(charStringList);
    auto ret = gcnew System::Collections::Generic::Dictionary<int, int>();
    for (const auto& i : result)
    {
        ret->Add(i.first, i.second);
    }

    return ret;
}
