//
// Created on 2017/12/03 at 11:10.
//

#ifndef DATASTRUCTUREPROJECT_AVLTREEINVERTEDINDEX_HPP
#define DATASTRUCTUREPROJECT_AVLTREEINVERTEDINDEX_HPP

#include "AvlTree.hpp"
#include "CharStringList.hpp"
#include "InvertedIndexNode.hpp"

class SortBySecond
{
public:
    bool operator()(const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) const
    {
        return lhs.second > rhs.second;
    }
};

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

    LinkedList<std::pair<int, int>> Query(const CharStringList& queryList);
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

inline LinkedList<std::pair<int, int>> AvlTreeInvertedIndex::Query(const CharStringList& queryList)
{
    AvlTree<int, int, std::less<int>> results;
    AvlTree<int, int, std::less<int>> documentRichness;

    for (const auto& item : queryList)
    {
        auto location = Core.Locate(item);

        if (location != Core.end())
        {
            for (const auto& documentOccurence : location->DocumentOccurrenceList)
            {
                auto done = false;
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

                idLocation = documentRichness.Locate(documentId);
                if (idLocation != documentRichness.end())
                {
                    *idLocation += 1;
                }
                else
                {
                    documentRichness.Insert(documentId, 1);
                }
            }
        }
    }

    SortedList<std::pair<int, int>, SortBySecond> sortedResult;

    const std::function<void(const int&, const int&)> iterationFunction =
        [&sortedResult](const int& id,const int& richness)-> void
    {
        sortedResult.Insert(std::make_pair(id, richness));
    };

    documentRichness.InorderTraversal(iterationFunction);

    LinkedList<std::pair<int, int>> ret;
    for(const auto& item:sortedResult)
    {
        ret.Append(std::make_pair(item.first, results.Search(item.first)));
    }

    return ret;
}


#endif //DATASTRUCTUREPROJECT_AVLTREEINVERTEDINDEX_HPP
