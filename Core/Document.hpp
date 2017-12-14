//
// Created on 2017/12/03 at 10:11.
//

#ifndef DATASTRUCTUREPROJECT_DOCUMENT_HPP
#define DATASTRUCTUREPROJECT_DOCUMENT_HPP

#include "CharString.hpp"
#include "InformationExtracter.hpp"
#include "CharStringList.hpp"
#include "Spider.hpp"
#include "XmlParser.hpp"
#include "Dictionary.hpp"

#include <fstream>

class Document
{
public:
    int Id;
    CharStringList Words;
    CharString PostTitle;
    CharString PostContent;

    void UpdateFromUrl(const std::wstring& url, const Dictionary& dictionary);
    void AssignId(const int id);
    int CountWords(const CharString& word) const;
};


inline void Document::UpdateFromUrl(const std::wstring& url, const Dictionary& dictionary)
{
    XmlNode* xmlRoot = nullptr;

    try
    {
        const auto htmlStd = Spider::GetHtmlByUrl(url);
        const CharString html(htmlStd);

        xmlRoot = XmlParser::ParseXml(html);

        auto extracter = InformationExtracter(xmlRoot);

        PostContent = extracter.GetPostContent();
        PostTitle = extracter.GetPostTitle();

        auto split = PostContent;
        split.Concat(PostTitle);

        Words = std::move(dictionary.WordSplit(split));
        delete xmlRoot;
    }
    catch (const std::exception&)
    {
        // Some errors may occur in GetPostContent() or GetPostTitle().
        // So xmlRoot won't be deleted due to try-catch blocks.
        delete xmlRoot;
        throw std::exception();
    }
}

inline void Document::AssignId(const int id)
{
    Id = id;
}

inline int Document::CountWords(const CharString& word) const
{
    auto count = 0;
    for (const auto& i : Words)
    {
        if (i == word)
        {
            count++;
        }
    }

    return count;
}


#endif //DATASTRUCTUREPROJECT_DOCUMENT_HPP
