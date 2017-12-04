//
// Created on 2017/12/03 at 00:36.
//

#ifndef DATASTRUCTUREPROJECT_SORTEDLIST_HPP
#define DATASTRUCTUREPROJECT_SORTEDLIST_HPP

#include <functional>


/// \brief A linked list which keeps sorted.
/// \tparam TElement Type of the element.
/// \tparam TLess A Function returning a boolean value to determine the order of the elements.
/// Will be called like <code>less(elem1, elem2)</code>, meaning elem1 &lt; elem2 if it returns true.
template <typename TElement, typename TLess>
class SortedList
{
private:
    class LinkedListNode;

public:
    class Iterator;

    /// \brief Get the first element satisfying the prediction.
    /// \param prediction The prediction of the elements should satisfy.
    /// \return Iterator to the element.
    Iterator FirstOf(const std::function<bool(const TElement&)>& prediction);

    /// \brief Insert an element to the container.
    /// \param element Element to insert.
    void Insert(const TElement& element);

    /// \brief Remove the first element satisfying the prediction from the container.
    /// \param prediction The prediction of the elements should satisfy.
    void RemoveFirstOf(const std::function<bool(const TElement&)>& prediction);

    /// \brief Modify an element in the container.
    /// \param iterator Element to modify.
    /// \param modification Modification function. Called like modification(*iterator).
    void Modify(const Iterator& iterator, const std::function<void(TElement&)>& modification);

    virtual ~SortedList();
private:
    class LinkedListNode
    {
    public:
        friend class Iterator;


        TElement Element;
        LinkedListNode* Next = nullptr;


        explicit LinkedListNode(const TElement& element)
            : Element(element)
        {
        }
    };


    LinkedListNode* _head = nullptr;
    LinkedListNode* _tail = nullptr;

public:
    SortedList() = default;

    SortedList(const SortedList& rhs)
    {
        (*this) = rhs;
    }

    void operator=(const SortedList& rhs)
    {
        if (rhs._head != nullptr)
        {
            _head = new LinkedListNode(rhs._head->Element);
        }

        auto sourceWalker = rhs._head->Next;
        auto destinationWalker = _head;
        while (sourceWalker != nullptr)
        {
            destinationWalker->Next = new LinkedListNode(sourceWalker->Element);
            sourceWalker = sourceWalker->Next;
            destinationWalker = destinationWalker->Next;
        }
    }

    SortedList(SortedList&& rhs) noexcept
        : _head(rhs._head)
    {
        rhs._head = nullptr;
    }

public:
    Iterator begin();
    Iterator end();

    /// \brief Iterator of the SortedList.
    class Iterator
    {
    public:
        friend class SortedList;


        void operator++(int)
        {
            _item = _item->Next;
        }

        void operator++()
        {
            _item = _item->Next;
        }

        TElement& operator*() const
        {
            return _item->Element;
        }


        bool operator!=(const Iterator& rhs)
        {
            return (_item != rhs._item);
        }

        bool operator==(const Iterator& rhs)
        {
            return _item == rhs._item;
        }

    private:
        LinkedListNode* _item = nullptr;


        explicit Iterator(LinkedListNode* item)
            : _item(item)
        {
        }
    };
};


template <typename TElement, typename TLess>
typename SortedList<TElement, TLess>::Iterator SortedList<TElement, TLess>::begin()
{
    return Iterator(_head);
}


template <typename TElement, typename TLess>
typename SortedList<TElement, TLess>::Iterator SortedList<TElement, TLess>::end()
{
    return Iterator(_tail);
}


template <typename TElement, typename TLess>
typename SortedList<TElement, TLess>::Iterator SortedList<TElement, TLess>::FirstOf(
    const std::function<bool(const TElement&)>& prediction
)
{
    for (auto i = begin(); i != end(); ++i)
    {
        if (prediction(*i))
        {
            return i;
        }
    }

    return end();
}


template <typename TElement, typename TLess>
void SortedList<TElement, TLess>::Insert(const TElement& element)
{
    if (_head == nullptr)
    {
        _head = new LinkedListNode(element);
        return;
    }

    if (TLess()(element, _head->Element))
    {
        auto temp = _head;
        _head = new LinkedListNode(element);
        _head->Next = temp;
        return;
    }

    auto slow = _head;
    auto fast = _head->Next;

    while (fast != nullptr && TLess()(fast->Element, element))
    {
        fast = fast->Next;
        slow = slow->Next;
    }

    slow->Next = new LinkedListNode(element);
    slow->Next->Next = fast;
}


template <typename TElement, typename TLess>
void SortedList<TElement, TLess>::RemoveFirstOf(const std::function<bool(const TElement&)>& prediction)
{
    if (_head == nullptr)
    {
        return;
    }

    if (prediction(*_head))
    {
        auto temp = _head;
        _head = _head->Next;
        delete temp;
        return;
    }

    auto slow = _head;
    auto fast = _head->Next;

    while (fast != nullptr && !prediction(*fast))
    {
        fast = fast->Next;
        slow = slow->Next;
    }

    if (fast == nullptr)
    {
        return;
    }

    slow->Next = fast->Next;
    delete fast;
}


template <typename TElement, typename TLess>
void SortedList<TElement, TLess>::Modify(
    const typename Iterator& iterator, const std::function<void(TElement&)>& modification
)
{
    modification(*iterator);

    // Remove the node from the linked list.
    LinkedListNode* inserting;

    if (_head == iterator._item)
    {
        inserting = _head;
        _head = _head->Next;
    }
    else
    {
        auto slow = _head;
        auto fast = _head->Next;

        while (fast != iterator._item)
        {
            fast = fast->Next;
            slow = slow->Next;
        }

        inserting = fast;
        slow->Next = fast->Next;
    }

    // Insert the node to the linked list.
    if (_head == nullptr)
    {
        _head = inserting;
        return;
    }

    if (TLess()(inserting->Element, _head->Element))
    {
        auto temp = _head;
        _head = inserting;
        _head->Next = temp;
        return;
    }

    auto slow = _head;
    auto fast = _head->Next;

    while (fast != nullptr && TLess()(fast->Element, inserting->Element))
    {
        fast = fast->Next;
        slow = slow->Next;
    }

    slow->Next = inserting;
    slow->Next->Next = fast;
}


template <typename TElement, typename TLess>
SortedList<TElement, TLess>::~SortedList()
{
    while (_head != nullptr)
    {
        auto next = _head->Next;
        delete _head;
        _head = next;
    }
}


#endif //DATASTRUCTUREPROJECT_SORTEDLIST_HPP
