//
// Created on 2017/10/31 at 12:54.
//

#ifndef DATASTRUCTUREPROJECT_DICTIONARY_HPP
#define DATASTRUCTUREPROJECT_DICTIONARY_HPP

#include <fstream>
#include <iostream>
#include "LinkedList.hpp"
#include "CharStringList.hpp"


/// \brief A class used to split words.
class Dictionary
{
public:
    /// \brief Add a dictionary to the instance.
    /// \param filePath Path to the dictionary file.
    /// \note The class will use those dictionaries to check if a string is a word or not.
    /// The dictionary file is a text file,
    /// each line of which is a word and there is not blank line at the end of the file.
    void AddDictionary(const std::string& filePath);

    /// \brief Test if a string is a word in the dictionary.
    /// \param word A string to be tested.
    /// \return True if it is a word, otherwise false.
    bool ContainsWord(const CharString& word) const;

    /// \brief Split the sentence to a list of words.
    /// \param sentence The sentence to be splited.
    /// \return A list containing all the words in it.
    /// \note All the control characters, punctuations, letters and numbers are ignored.
    CharStringList WordSplit(const CharString& sentence) const;

    virtual ~Dictionary();
private:
    /// \brief Pointer to the hash table used for word looking up.
    CharStringList* _hashTable = nullptr;

    /// \brief Cached maximium length of the words.
    int _maxWordLength = 1;

    /// \brief Test if a character is a stop word.
    /// \param word The character to be tested.
    /// \return True if <code>character</code> is a stop word, otherwise false.
    /// \note A stop word is one of the characters, the punctuations, the letters and the numbers.
    static bool IsStopWord(wchar_t word);
};


inline void Dictionary::AddDictionary(const std::string& filePath)
{
    if (_hashTable == nullptr)
    {
        _hashTable = new CharStringList[CharString::HashMax - CharString::HashMin];
    }

    std::wifstream fin;
    fin.imbue(std::locale("chs"));
    fin.open(filePath);

    std::wstring readingLine;
    auto co = 0;

    while (getline(fin, readingLine))
    {
        co++;
        CharString item;
        item.FromStdWstring(readingLine);
        _hashTable[item.GetHashCode() - CharString::HashMin].Append(item);

        if (item.GetLength() > _maxWordLength)
        {
            _maxWordLength = item.GetLength();
        }
    }
}


inline bool Dictionary::ContainsWord(const CharString& word) const
{
    if (_hashTable == nullptr)
    {
        return false;
    }

    const auto hashCode = word.GetHashCode();

    return _hashTable[hashCode - CharString::HashMin].IndexOf(word) != -1;
}


inline CharStringList Dictionary::WordSplit(const CharString& sentence) const
{
    CharStringList ret;
    auto right = sentence.GetLength();

    while (right > 0)
    {
        auto wordFound = false;
        const auto start = _maxWordLength > right ? right : _maxWordLength;
        for (auto i = start; i > 1; i--)
        {
            if (right - i >= 0)
            {
                const auto substring = sentence.GetSubstring(right - i, right);
                if (ContainsWord(substring))
                {
                    ret.InsertAt(substring, 0);
                    right = right - i;
                    wordFound = true;
                    break;
                }
            }
        }

        if (!wordFound)
        {
            const auto adding = sentence[right - 1];
            if (!IsStopWord(adding))
            {
                ret.InsertAt(sentence.GetSubstring(right - 1, right), 0);
            }

            right--;
        }
    }

    return ret;
}

inline Dictionary::~Dictionary()
{
    delete[] _hashTable;
}


inline bool Dictionary::IsStopWord(const wchar_t word)
{
    return
        word == L' ' || word == L'\0' || word == L'\n' || word == L'\t' ||
        word == L'`' || word == L'~' || word == L'!' || word == L'@' || word == L'#' ||
        word == L'$' || word == L'%' || word == L'^' || word == L'&' || word == L'*' ||
        word == L'(' || word == L')' || word == L'-' || word == L'=' || word == L'_' ||
        word == L'+' || word == L'[' || word == L']' || word == L'\\' || word == L'{' ||
        word == L'}' || word == L'|' || word == L';' || word == L'\'' || word == L':' ||
        word == L'"' || word == L',' || word == L'.' || word == L'/' || word == L'<' ||
        word == L'>' || word == L'?' || word == L'·' || word == L'！' || word == L'￥' ||
        word == L'…' || word == L'（' || word == L'）' || word == L'—' || word == L'【' ||
        word == L'】' || word == L'、' || word == L'；' || word == L'：' || word == L'‘' ||
        word == L'’' || word == L'“' || word == L'”' || word == L'，' || word == L'。' ||
        word == L'《' || word == L'》' || word == L'/' || word == L'？' || word == L'q' ||
        word == L'w' || word == L'e' || word == L'r' || word == L't' || word == L'y' ||
        word == L'u' || word == L'i' || word == L'o' || word == L'p' || word == L'a' ||
        word == L's' || word == L'd' || word == L'f' || word == L'g' || word == L'h' ||
        word == L'j' || word == L'k' || word == L'l' || word == L'z' || word == L'x' ||
        word == L'c' || word == L'v' || word == L'b' || word == L'n' || word == L'm' ||
        word == L'Q' || word == L'W' || word == L'E' || word == L'R' || word == L'T' ||
        word == L'Y' || word == L'U' || word == L'I' || word == L'O' || word == L'P' ||
        word == L'A' || word == L'S' || word == L'D' || word == L'F' || word == L'G' ||
        word == L'H' || word == L'J' || word == L'K' || word == L'L' || word == L'Z' ||
        word == L'X' || word == L'C' || word == L'V' || word == L'B' || word == L'N' ||
        word == L'M' || word == L'1' || word == L'2' || word == L'3' || word == L'4' ||
        word == L'5' || word == L'6' || word == L'7' || word == L'8' || word == L'9' ||
        word == L'0' || word == 160;
}


#endif //DATASTRUCTUREPROJECT_DICTIONARY_HPP
