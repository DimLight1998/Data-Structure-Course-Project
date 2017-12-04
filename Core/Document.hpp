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

    void UpdateFromUrl(const std::wstring& url, const Dictionary& dictionary);
    void AssignId(const int id);
    int CountWords(const CharString& word) const;
};


inline void Document::UpdateFromUrl(const std::wstring& url, const Dictionary& dictionary)
{
    const auto htmlStd = Spider::GetHtmlByUrl(url);
    const CharString html(htmlStd);

    const auto xmlRoot = XmlParser::ParseXml(html);
    auto extracter = InformationExtracter(xmlRoot);

    auto content = extracter.GetPostContent();
    content.Concat(extracter.GetPostTitle());

//    // todo Remove log.
//    std::wofstream flog;
//    flog.imbue(std::locale("chs"));
//    flog.open("cache.txt",std::ios::app);
//    flog << content.ToStdWstring() << L"\n\n\n\n\n";
//    flog.close();

    Words = std::move(dictionary.WordSplit(content));
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
