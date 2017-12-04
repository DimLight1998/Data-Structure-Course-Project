#pragma once
#include "CharString.hpp"
#include "LinkedList.hpp"
#include "Spider.hpp"
#include <fstream>

// This class is slow but I'm too lazy to optimize it.
class Statistics
{
public:
    void AddWord(const CharString& word);
    void GenerateCsvFile(const CharString& filePath);
    void GenerateWordCloud(const CharString& csvFilePath, const CharString& picturePath);
private:
    LinkedList<CharString> _words;
    LinkedList<int> _freqs;
};

void Statistics::AddWord(const CharString& word)
{
    auto index = _words.IndexOf(word);
    if (index != -1)
    {
        _freqs[index]++;
    }
    else
    {
        _words.InsertAt(word, 0);
        _freqs.InsertAt(1, 0);
    }
}

void Statistics::GenerateCsvFile(const CharString& filePath)
{
    std::wfstream out;
    out.imbue(std::locale("chs"));
    out.open(filePath.ToStdWstring(), std::ios::trunc | std::ios::out);

    for (auto i = 0; i < _words.GetLength(); i++)
    {
        out << _words.GetItemAt(i).ToStdWstring() << L"," << _freqs.GetItemAt(i) << L"\n";
    }

    out.close();
}

void Statistics::GenerateWordCloud(const CharString& csvFilePath, const CharString& picturePath)
{
    Spider::GenerateWordCloud(csvFilePath.ToStdWstring(), picturePath.ToStdWstring());
}
