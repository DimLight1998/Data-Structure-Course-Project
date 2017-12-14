//
// Created on 2017/10/25 at 23:47.
//

#ifndef DATASTRUCTUREPROJECT_STACK_HPP
#define DATASTRUCTUREPROJECT_STACK_HPP

#include <stdexcept>


/// \brief A stack class.
/// \tparam TElement The type of the element in the list.
template <typename TElement>
class Stack
{
public:
    /// \brief Push an element in to the stack.
    /// \param element The element to push.
    void Push(const TElement& element);

    /// \brief Pop the top element of the stack.
    /// \throw std::underflow_error if there is no element in the stack.
    void Pop();

    /// \brief Get the top element of the stack.
    /// \return The top element of the stack.
    /// \throw std::underflow_error if there is no element in the stack.
    TElement Top();

    /// \brief Test if the stack is empty.
    /// \return True if the stack is empty, otherwise false.
    bool IsEmpty();

    bool Contains(const TElement& element);
    bool Contains(const std::function<bool(const TElement& element)> prediction);

    virtual ~Stack();

private:
    /// \brief The node class used in the class.
    class StackNode
    {
    public:
        TElement Element;
        StackNode* Next = nullptr;
        explicit StackNode(const TElement& element);
    };


    /// \brief Pointer to the top of the stack.
    StackNode* _topNode = nullptr;

    /// \brief Cached size.
    int _size = 0;
};


template <typename TElement>
Stack<TElement>::StackNode::StackNode(const TElement& element)
    : Element(element)
{
}


template <typename TElement>
void Stack<TElement>::Push(const TElement& element)
{
    if (_topNode == nullptr)
    {
        _topNode = new StackNode(element);
    }
    else
    {
        auto next = _topNode;
        _topNode = new StackNode(element);
        _topNode->Next = next;
    }

    _size++;
}


template <typename TElement>
void Stack<TElement>::Pop()
{
    if (_topNode == nullptr)
    {
        throw std::underflow_error("Stack underflow in Stack::Pop()");
    }
    auto next = _topNode->Next;
    delete _topNode;
    _topNode = next;

    _size--;
}


template <typename TElement>
TElement Stack<TElement>::Top()
{
    if (_topNode == nullptr)
    {
        throw std::underflow_error("Stack underflow in Stack::Top()");
    }
    return _topNode->Element;
}


template <typename TElement>
bool Stack<TElement>::IsEmpty()
{
    return _topNode == nullptr;
}

template<typename TElement>
inline bool Stack<TElement>::Contains(const TElement & element)
{
    auto walker = _topNode;
    while (walker != nullptr)
    {
        if (walker->Element == element)
        {
            return true;
        }
        else
        {
            walker = walker->Next;
        }
    }

    return false;
}

template<typename TElement>
inline bool Stack<TElement>::Contains(const std::function<bool(const TElement&element)> prediction)
{
    auto walker = _topNode;
    while (walker != nullptr)
    {
        if (prediction(walker->Element))
        {
            return true;
        }
        else
        {
            walker = walker->Next;
        }
    }

    return false;
}


template <typename TElement>
Stack<TElement>::~Stack()
{
    while (_topNode != nullptr)
    {
        auto next = _topNode->Next;
        delete _topNode;
        _topNode = next;
    }
}


#endif //DATASTRUCTUREPROJECT_STACK_HPP
