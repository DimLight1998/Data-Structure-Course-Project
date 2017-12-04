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
    HashMap<CharString, InvertedIndexNode, CharString::Hasher, CharString::HashMin, CharString::HashMax> InvertedIndex;
};


#endif //DATASTRUCTUREPROJECT_HASHMAPINVERTEDINDEX_HPP
