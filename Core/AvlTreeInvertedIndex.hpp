//
// Created on 2017/12/03 at 11:10.
//

#ifndef DATASTRUCTUREPROJECT_AVLTREEINVERTEDINDEX_HPP
#define DATASTRUCTUREPROJECT_AVLTREEINVERTEDINDEX_HPP

#include "AvlTree.hpp"
#include "CharStringList.hpp"
#include "InvertedIndexNode.hpp"

class AvlTreeInvertedIndex
{
public:
    AvlTree<CharString, InvertedIndexNode, std::less<CharString>> Core;

    /// \brief
    /// \param word
    /// \param document
    /// \param times
    /// \note If the word is not indexed, it will be created automatically.
    void AddOccurrence(const CharString& word, Document* document, int times);

    AvlTree<int, int, std::less<int>> Query(const CharStringList& queryList);
};

inline void AvlTreeInvertedIndex::AddOccurrence(const CharString& word, Document* document, const int times)
{
    auto location = Core.Locate(word);

    if (location == Core.end())
    {
        InvertedIndexNode insertingNode(word);
        insertingNode.AddOccurrence(document, times);
        Core.Insert(word, insertingNode);
    }
    else
    {
        location->AddOccurrence(document, times);
    }
}

inline AvlTree<int, int, std::less<int>> AvlTreeInvertedIndex::Query(const CharStringList& queryList)
{
    AvlTree<int, int, std::less<int>> results;

    for (const auto& item : queryList)
    {
        auto location = Core.Locate(item);

        if (location != Core.end())
        {
            for (const auto& documentOccurence : location->DocumentOccurrenceList)
            {
                auto documentId = documentOccurence.first->Id;
                auto occurrence = documentOccurence.second;
                auto idLocation = results.Locate(documentId);

                if (idLocation != results.end())
                {
                    *idLocation += occurrence;
                }
                else
                {
                    results.Insert(documentId, occurrence);
                }
            }
        }
    }

    return results;
}


#endif //DATASTRUCTUREPROJECT_AVLTREEINVERTEDINDEX_HPP
