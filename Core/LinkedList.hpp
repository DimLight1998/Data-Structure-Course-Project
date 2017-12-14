//
// Created on 2017/10/26 at 11:17.
//

#ifndef DATASTRUCTUREPROJECT_LINKEDLIST_HPP
#define DATASTRUCTUREPROJECT_LINKEDLIST_HPP

#include <stdexcept>
#include <functional>


/// \brief A linked-list class.
/// \tparam TElement The type of the element in the list.
template <typename TElement>
class LinkedList
{
public:
    /// \brief Insert an element into the list.
    /// \param element The element to insert.
    /// \param index The index of insertion.
    /// After the insertion, the index of <code>element</code> is <code>index</code>.
    /// \throw std::out_of_range if <code>index</code> is invalid.
    void InsertAt(const TElement& element, int index);

    /// \brief Remove an element from the list.
    /// \param index The index of the element to be removed.
    /// \throw std::out_of_range if <code>index</code> is invalid.
    void RemoveAt(int index);

    /// \brief Get the index of the first occurrence of the element in the list.
    /// \param element The element to find.
    /// \return -1 if the element is not in the list, otherwise the index of the first occurrence of the element.
    int IndexOf(const TElement& element) const;

    /// \brief Append an element at the end of the list.
    /// \param element The element to append.
    /// \todo Use tail pointer to accelerate.
    void Append(const TElement& element);

    /// \brief Get the item at a specific position in the instance.
    /// \param index The position of the item to get.
    /// \return The item at the position <code>index</code>.
    /// \throw std::out_of_range if <code>index</code> is invalid.
    const TElement& GetItemAt(int index) const;

    /// \brief Check if the element is in the instance.
    /// \param element The element to check.
    /// \return True if the element is in the instance, otherwise false.
    bool Contains(const TElement& element) const;

    /// \brief Check if there are elements in the instance satisfying the prediction.
    /// \param prediction The prediction of the elements should satisfy.
    /// \return True if there are such elements in the instance.
    bool ContainsIf(std::function<bool(const TElement&)>& prediction);

    /// \brief Get the first element satisfying the prediction.
    /// \param prediction The prediction of the element should satisfy.
    /// \return The first element satisfying the prediction.
    /// \throws <code>std::logic_error</code> if no such element is in the linked list.
    const TElement& GetFirstOf(std::function<bool(const TElement&)>& prediction);

    /// \brief Remove the first element satisfying the prediction.
    /// \param prediction The prediction of the element should satisfy.
    /// \note Nothing will happen if there is no such element.
    void RemoveFirstOf(std::function<bool(const TElement&)>& prediction);

    /// \brief Get the length of the list.
    /// \return The length of the list.
    int GetLength() const;

    /// \brief Iterate the list and call a function on each item.
    /// \param visitFunction The function to be called on each item.
    void Iterate(const std::function<void(const TElement&)>& visitFunction);

    LinkedList() = default;
    LinkedList(LinkedList&& rhs) noexcept;

    void operator=(LinkedList&& rhs) noexcept;

    virtual ~LinkedList();

    TElement& operator[](int index) const;
private:
    /// \brief The node class used in the class.
    class LinkedListNode
    {
    public:
        TElement Element;
        LinkedListNode* Next = nullptr;

        explicit LinkedListNode(const TElement& element);
    };


    /// \brief Pointer to the head node of the linked list.
    LinkedListNode* _headNode = nullptr;

    /// \brief The cached length.
    int _length = 0;
public:
    class ForwardIterator
    {
        friend class LinkedList;
    public:
        TElement& operator*() const
        {
            return _item->Element;
        }

        void operator++(int)
        {
            _item = _item->Next;
        }

        void operator++()
        {
            _item = _item->Next;
        }

        TElement* operator->() const
        {
            return &(_item->Element);
        }

        bool operator==(const ForwardIterator& rhs) const
        {
            return _item == rhs._item;
        }

        bool operator!=(const ForwardIterator& rhs) const
        {
            return _item != rhs._item;
        }

    private:
        LinkedListNode* _item;

        explicit ForwardIterator(LinkedListNode* item) : _item(item)
        {
        }
    };

    ForwardIterator begin() const { return ForwardIterator(_headNode); }
    ForwardIterator end() const { return ForwardIterator(nullptr); }
};


template <typename TElement>
LinkedList<TElement>::LinkedListNode::LinkedListNode(const TElement& element)
    : Element(element)
{
}


template <typename TElement>
void LinkedList<TElement>::InsertAt(const TElement& element, int index)
{
    if (index < 0 || index > _length)
    {
        throw std::out_of_range("LinkedList index out of range in LinkedList::InsertAt()");
    }

    if (index == 0)
    {
        auto newHead = new LinkedListNode(element);
        newHead->Next = _headNode;
        _headNode = newHead;
    }
    else
    {
        auto walker = _headNode;

        for (auto i = 1; i < index; i++)
        {
            walker = walker->Next;
        }

        auto next = walker->Next;

        walker->Next = new LinkedListNode(element);
        walker->Next->Next = next;
    }

    _length++;
}


template <typename TElement>
void LinkedList<TElement>::RemoveAt(int index)
{
    if (index < 0 || index >= _length)
    {
        throw std::out_of_range("LinkedList index out of range in LinkedList::RemoveAt()");
    }

    if (index == 0)
    {
        auto head = _headNode;
        _headNode = head->Next;
        delete head;
    }
    else
    {
        auto walker = _headNode;

        for (auto i = 1; i < index; i++)
        {
            walker = walker->Next;
        }

        auto next = walker->Next;
        walker->Next = next->Next;
        delete next;
    }

    _length--;
}


template <typename TElement>
int LinkedList<TElement>::IndexOf(const TElement& element) const
{
    auto index = 0;
    auto walker = _headNode;

    if (walker == nullptr)
    {
        return -1;
    }

    while (walker->Element != element)
    {
        walker = walker->Next;
        index++;

        if (walker == nullptr)
        {
            index = -1;
            break;
        }
    }

    return index;
}


template <typename TElement>
LinkedList<TElement>::~LinkedList()
{
    while (_headNode != nullptr)
    {
        auto next = _headNode->Next;
        delete _headNode;
        _headNode = next;
    }
}


template <typename TElement>
void LinkedList<TElement>::Append(const TElement& element)
{
    InsertAt(element, _length);
}


template <typename TElement>
const TElement& LinkedList<TElement>::GetItemAt(int index) const
{
    return (*this)[index];
}


template <typename TElement>
int LinkedList<TElement>::GetLength() const
{
    return _length;
}


template <typename TElement>
void LinkedList<TElement>::Iterate(const std::function<void(const TElement&)>& visitFunction)
{
    if (_headNode == nullptr)
    {
    }
    auto walker = _headNode;
    while (walker != nullptr)
    {
        visitFunction(walker->Element);
        walker = walker->Next;
    }
}


template <typename TElement>
LinkedList<TElement>::LinkedList(LinkedList&& rhs) noexcept
{
    _headNode = rhs._headNode;
    _length = rhs._length;

    rhs._headNode = nullptr;
}

template <typename TElement>
void LinkedList<TElement>::operator=(LinkedList&& rhs) noexcept
{
    _headNode = rhs._headNode;
    _length = rhs._length;

    rhs._headNode = nullptr;
}


template <typename TElement>
TElement& LinkedList<TElement>::operator[](int index) const
{
    if (index < 0 || index >= _length)
    {
        throw std::out_of_range("LinkedList out of range in LinkedList::GetItemAt()");
    }
    auto walker = _headNode;
    for (auto i = 0; i < index; i++)
    {
        walker = walker->Next;
    }

    return walker->Element;
}


template <typename TElement>
bool LinkedList<TElement>::Contains(const TElement& element) const
{
    auto found = false;
    auto walker = _headNode;
    while (walker != nullptr)
    {
        if (walker->Element == element)
        {
            found = true;
            break;
        }
        walker = walker->Next;
    }

    return found;
}


template <typename TElement>
bool LinkedList<TElement>::ContainsIf(std::function<bool(const TElement&)>& prediction)
{
    auto found = false;
    auto walker = _headNode;
    while (walker != nullptr)
    {
        if (prediction(walker->Element))
        {
            found = true;
            break;
        }

        walker = walker->Next;
    }

    return found;
}


template <typename TElement>
const TElement& LinkedList<TElement>::GetFirstOf(std::function<bool(const TElement&)>& prediction)
{
    auto walker = _headNode;
    while (walker != nullptr)
    {
        if (prediction(walker->Element))
        {
            return walker->Element;
        }

        walker = walker->Next;
    }

    throw std::logic_error("No such element in LinkedList::GetFirstOf");
}


template <typename TElement>
void LinkedList<TElement>::RemoveFirstOf(std::function<bool(const TElement&)>& prediction)
{
    if (_length == 0)
    {
        return;
    }

    if (_length == 1 && prediction(_headNode->Element))
    {
        auto temp = _headNode;
        _headNode = nullptr;
        delete temp;
    }
    else
    {
        auto fast = _headNode->Next;
        auto slow = _headNode;

        while (!prediction(fast->Element))
        {
            fast = fast->Next;
            if (fast == nullptr)
            {
                return;
            }

            slow = slow->Next;
        }

        slow->Next = fast->Next;
        fast->Next = nullptr;
        delete fast;
    }
}


#endif //DATASTRUCTUREPROJECT_LINKEDLIST_HPP
