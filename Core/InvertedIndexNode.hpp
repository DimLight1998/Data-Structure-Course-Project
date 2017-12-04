//
// Created on 2017/12/03 at 10:49.
//

#ifndef DATASTRUCTUREPROJECT_INVERTEDINDEXNODE_HPP
#define DATASTRUCTUREPROJECT_INVERTEDINDEXNODE_HPP

#include "CharString.hpp"
#include "SortedList.hpp"
#include "Document.hpp"
#include <utility>

class InvertedIndexNode
{
public:
    class Less
    {
    public:
        bool operator()(const std::pair<Document*, int>& lhs, const std::pair<Document*, int>& rhs) const
        {
            return lhs.second < rhs.second;
        }
    };

    CharString Word = CharString(std::wstring());
    int FileLevelOccurrence = 0;
    int WordLevelOccurrence = 0;
    SortedList<std::pair<Document*, int>, Less> DocumentOccurrenceList;

    void AddOccurrence(Document* document, int times);

    explicit InvertedIndexNode(const CharString& word)
        : Word(word)
    {
    }

    // Need to provide a default constructor.
    InvertedIndexNode() = default;

    InvertedIndexNode(const InvertedIndexNode& rhs)
        : Word(rhs.Word),
          FileLevelOccurrence(rhs.FileLevelOccurrence),
          WordLevelOccurrence(rhs.WordLevelOccurrence),
          DocumentOccurrenceList(rhs.DocumentOccurrenceList)
    {
    }

    void operator=(const InvertedIndexNode& rhs)
    {
        Word = rhs.Word;
        FileLevelOccurrence = rhs.FileLevelOccurrence;
        WordLevelOccurrence = rhs.WordLevelOccurrence;
        DocumentOccurrenceList = rhs.DocumentOccurrenceList;
    }
};


inline void InvertedIndexNode::AddOccurrence(Document* document, int times)
{
    auto location = DocumentOccurrenceList.FirstOf(
        [document](const std::pair<Document*, int>& node)-> bool
        {
            return node.first == document;
        }
    );

    if (location == DocumentOccurrenceList.end())
    {
        DocumentOccurrenceList.Insert(std::make_pair(document, times));
        FileLevelOccurrence++;
    }
    else
    {
        DocumentOccurrenceList.Modify(location,
                                      [times](std::pair<Document*, int>& node)-> void
                                      {
                                          node.second += times;
                                      }
        );
    }

    WordLevelOccurrence += times;
}


#endif //DATASTRUCTUREPROJECT_INVERTEDINDEXNODE_HPP
