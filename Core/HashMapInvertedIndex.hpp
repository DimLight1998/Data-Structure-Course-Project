//
// Created on 2017/12/03 at 11:10.
//

#ifndef DATASTRUCTUREPROJECT_HASHMAPINVERTEDINDEX_HPP
#define DATASTRUCTUREPROJECT_HASHMAPINVERTEDINDEX_HPP
#include "HashMap.hpp"
#include "CharString.hpp"
#include "InvertedIndexNode.hpp"


class HashMapInvertedIndex
{
public:
    HashMap<CharString, InvertedIndexNode, CharString::Hasher, CharString::HashMin, CharString::HashMax> Core;

    void AddOccurrence(const CharString& word, Document* document, const int times);

    class IntHasher
    {
    public:
        int operator()(const int i) const
        {
            return i % (HashMax - HashMin);
        }

        static const int HashMax = 100000;
        static const int HashMin = 0;
    };

    HashMap<int, int, IntHasher, IntHasher::HashMin, IntHasher::HashMax> Query(const CharStringList& queryList);
};


inline void HashMapInvertedIndex::AddOccurrence(const CharString& word, Document* document, const int times)
{
    auto location = Core.Locate(word);

    if (location == Core.EmptyIterator())
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

inline HashMap<int, int, HashMapInvertedIndex::IntHasher, HashMapInvertedIndex::IntHasher::HashMin, HashMapInvertedIndex
               ::IntHasher::HashMax> HashMapInvertedIndex::Query(const CharStringList& queryList)
{
    HashMap<int, int, IntHasher, IntHasher::HashMin, IntHasher::HashMax> results;

    for (const auto& item : queryList)
    {
        auto location = Core.Locate(item);

        if (location != Core.EmptyIterator())
        {
            for (const auto& documentOccurrence : location->DocumentOccurrenceList)
            {
                auto documentId = documentOccurrence.first->Id;
                auto occurrence = documentOccurrence.second;
                auto idLocation = results.Locate(documentId);

                if (idLocation != results.EmptyIterator())
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
#endif //DATASTRUCTUREPROJECT_HASHMAPINVERTEDINDEX_HPP
