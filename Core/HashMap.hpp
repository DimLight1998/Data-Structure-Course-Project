//
// Created on 2017/11/29 at 17:17.
//

#ifndef DATASTRUCTUREPROJECT_HASHMAP_HPP
#define DATASTRUCTUREPROJECT_HASHMAP_HPP

#include "LinkedList.hpp"
#include <array>


template <typename TKey, typename TValue, typename THash, int THashMin, int THashMax>
class HashMap
{
public:
    /// \brief Insert a record to the dictionary.
    /// \param key Key of the record.
    /// \param value Value of the record.
    /// \note If there is a record with the same key in the dictionary, its value will be overwritten.
    void Insert(const TKey& key, const TValue& value);

    /// \brief Test if the dictionary has a record with the given key.
    /// \param key Key to be checked in the instance.
    /// \return True if the dictionary has a record with the given key, otherwise false.
    bool Contains(const TKey& key);

    /// \brief Find the value of the record of the key in the dictionary.
    /// \param key Key of the record.
    /// \return Value of the record.
    /// \note If there isn't a record with the key in the dictionary,
    /// a default value produced by default constructor of <code>TValue</code> will be returned.
    TValue Search(const TKey& key);

    /// \brief Remove a record from the dictionary.
    /// \param key Key of the record.
    /// \note If there isn't a record with the key in the dictionary, nothing will happen.
    void Remove(const TKey& key);

    virtual ~HashMap();

    void Travelsal(const std::function<void(const TKey&, const TValue&)>& traversalFunction);
private:
    class MapEntry
    {
    public:
        TKey Key;
        TValue Value;


        MapEntry(const TKey& key, const TValue& value)
            : Key(key), Value(value)
        {
        }
    };


    LinkedList<MapEntry>* _hashTable = new LinkedList<MapEntry>[THashMax - THashMin];
public:
    class ReadWriteIterator
    {
        friend class HashMap;
    public:
        TValue& operator*()
        {
            return _item->Value;
        }

        TValue* operator->()
        {
            return &(_item->Value);
        }

        bool operator==(const ReadWriteIterator& rhs)
        {
            return _item == rhs._item;
        }

        bool operator!=(const ReadWriteIterator& rhs)
        {
            return _item != rhs._item;
        }

        explicit ReadWriteIterator(MapEntry* item) : _item(item)
        {
        }

    private:
        MapEntry* _item = nullptr;
    };

    ReadWriteIterator EmptyIterator()
    {
        return ReadWriteIterator(nullptr);
    }

    ReadWriteIterator Locate(const TKey& key);
};


template <typename TKey, typename TValue, typename THash, int THashMin, int THashMax>
void HashMap<TKey, TValue, THash, THashMin, THashMax>::Insert(const TKey& key, const TValue& value)
{
    auto hash = THash()(key);

    std::function<bool(const MapEntry&)> equal = [&key](const MapEntry& entry) -> bool
    {
        return entry.Key == key;
    };

    try
    {
        auto result = _hashTable[hash].GetFirstOf(equal);
        result.Value = value;
    }
    catch (std::logic_error error)
    {
        _hashTable[hash].Append(MapEntry(key, value));
    }
}


template <typename TKey, typename TValue, typename THash, int THashMin, int THashMax>
bool HashMap<TKey, TValue, THash, THashMin, THashMax>::Contains(const TKey& key)
{
    auto hash = THash()(key);

    std::function<bool(const MapEntry&)> equal = [&key](const MapEntry& entry) -> bool
    {
        return entry.Key == key;
    };

    return _hashTable[hash].ContainsIf(equal);
}


template <typename TKey, typename TValue, typename THash, int THashMin, int THashMax>
TValue HashMap<TKey, TValue, THash, THashMin, THashMax>::Search(const TKey& key)
{
    auto hash = THash()(key);

    auto equal = [&key](const MapEntry& entry) -> bool
    {
        return entry.Key == key;
    };

    try
    {
        auto result = _hashTable[hash].GetFirstOf(equal);
        return result.Value;
    }
    catch (std::logic_error error)
    {
        return TValue();
    }
}


template <typename TKey, typename TValue, typename THash, int THashMin, int THashMax>
void HashMap<TKey, TValue, THash, THashMin, THashMax>::Remove(const TKey& key)
{
    auto hash = THash()(key);

    auto equal = [&key](const MapEntry& entry) -> bool
    {
        return entry.Key == key;
    };

    _hashTable[hash].RemoveFirstOf(equal);
}


template <typename TKey, typename TValue, typename THash, int THashMin, int THashMax>
HashMap<TKey, TValue, THash, THashMin, THashMax>::~HashMap()
{
    delete[] _hashTable;
}

template <typename TKey, typename TValue, typename THash, int THashMin, int THashMax>
void HashMap<TKey, TValue, THash, THashMin, THashMax>::Travelsal(
    const std::function<void(const TKey&, const TValue&)>& traversalFunction)
{
    auto itemTraversal = [&traversalFunction](const MapEntry& entry)->void
    {
        traversalFunction(entry.Key, entry.Value);
    };

    for(auto i = 0;i<THashMax-THashMin;i++)
    {
        _hashTable[i].Iterate(itemTraversal);
    }
}

template <typename TKey, typename TValue, typename THash, int THashMin, int THashMax>
typename HashMap<TKey, TValue, THash, THashMin, THashMax>::ReadWriteIterator
HashMap<TKey, TValue, THash, THashMin, THashMax>::Locate(const TKey& key)
{
    auto hash = THash()(key);

    try
    {
        std::function<bool(const MapEntry&)> equal = [&key](const MapEntry& entry)-> bool
        {
            return entry.Key == key;
        };

        const auto& result = _hashTable[hash].GetFirstOf(equal);

        return ReadWriteIterator(const_cast<MapEntry*>(&result));
    }
    catch (std::logic_error)
    {
        return EmptyIterator();
    }
}


#endif //DATASTRUCTUREPROJECT_HASHMAP_HPP
