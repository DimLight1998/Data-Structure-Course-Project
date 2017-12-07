//
// Created on 2017/11/29 at 10:27.
//

#ifndef DATASTRUCTUREPROJECT_AVLTREE_HPP
#define DATASTRUCTUREPROJECT_AVLTREE_HPP

#include <algorithm>
#include <utility>
#include "SortedList.hpp"

/// \brief A dictionary implemented with AVL tree.
/// \tparam TKey Type of the key.
/// \tparam TValue Type of the value, need to have a default constructor.
/// \tparam TLess A Function returning a boolean value to determine the order of the key,
/// Will be called like <code>less(key1, key2)</code>, meaning key1 &lt; key2 if it returns true.
template <typename TKey, typename TValue, typename TLess>
class AvlTree
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

    void InorderTraversal(const std::function<void(const TKey&, const TValue&)>& traversalFunction);

    AvlTree() = default;
    AvlTree(const AvlTree&) = delete;

    AvlTree(AvlTree&& rhs) noexcept:
        _root(rhs._root)
    {
        rhs._root = nullptr;
    }


    virtual ~AvlTree();
private:
    /// \brief The tree node used by <code>AvlTree</code>.
    class AvlTreeNode
    {
    public:
        TKey Key;
        TValue Value;
        AvlTreeNode* Left = nullptr;
        AvlTreeNode* Right = nullptr;
        int Height = 1;


        AvlTreeNode(const TKey& key, const TValue& value)
            : Key(key), Value(value)
        {
        }

        AvlTreeNode(const TKey& key, TValue&& value)
            : Key(key), Value(value)
        {
        }


        virtual ~AvlTreeNode()
        {
            delete Left;
            delete Right;
        }


        /// \brief Get the height of a node or a null node.
        /// \param node Pointer to the node (or <code>nullptr</code>).
        /// \return The height of the node, or 0 if the node is null.
        static int GetHeight(AvlTreeNode* node)
        {
            if (node == nullptr)
            {
                return 0;
            }

            return node->Height;
        }

        void InorderTraversal(const std::function<void(const TKey&, const TValue&)>& traversalFunction)
        {
            if (Left != nullptr)
            {
                Left->InorderTraversal(traversalFunction);
            }

            traversalFunction(Key, Value);

            if (Right != nullptr)
            {
                Right->InorderTraversal(traversalFunction);
            }
        }
    };

private:
    AvlTreeNode* _root = nullptr;

    /// \brief Rotate the tree if it is not balanced.(Case LL)
    /// \param node Root of the minimal unbalanced tree.
    /// \return New root of the minimal unbalanced tree.
    static AvlTreeNode* RotateLeftLeft(AvlTreeNode* node);

    /// \brief Rotate the tree if it is not balanced.(Case RR)
    /// \param node Root of the minimal unbalanced tree.
    /// \return New root of the minimal unbalanced tree.
    static AvlTreeNode* RotateRightRight(AvlTreeNode* node);

    /// \brief Rotate the tree if it is not balanced.(Case LR)
    /// \param node Root of the minimal unbalanced tree.
    /// \return New root of the minimal unbalanced tree.
    static AvlTreeNode* RotateLeftRight(AvlTreeNode* node);

    /// \brief Rotate the tree if it is not balanced.(Case RL)
    /// \param node Root of the minimal unbalanced tree.
    /// \return New root of the minimal unbalanced tree.
    static AvlTreeNode* RotateRightLeft(AvlTreeNode* node);

    /// \brief Insert a record to a given subtree.
    /// \param tree The subtree which the record will be inserted to.
    /// \param key Key of the record.
    /// \param value Value of the record.
    /// \return New root of the subtree (the root may change because of rotation).
    static AvlTreeNode* InsertToTree(AvlTreeNode* tree, const TKey& key, const TValue& value);

    /// \brief Remove a record from a given subtree.
    /// \param tree The subtree which the record will be removed from.
    /// \param key Key of the record.
    /// \return New root of the subtree (the root may change because of ratation).
    static AvlTreeNode* RemoveInTree(AvlTreeNode* tree, const TKey& key);

public:
    class ReadWriteIterator
    {
        friend class AvlTree;
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

    private:
        AvlTreeNode* _item = nullptr;

        explicit ReadWriteIterator(AvlTreeNode* item) : _item(item)
        {
        }
    };

    ReadWriteIterator begin() { return ReadWriteIterator(_root); }
    ReadWriteIterator end() { return ReadWriteIterator(nullptr); }


    /// \brief Locate the element with given key.
    /// \param key Key of the locating element.
    /// \return Iterator to the element, end() if not found.
    ReadWriteIterator Locate(const TKey& key);

public:
    class PairLess
    {
    public:
        bool operator()(const std::pair<TKey, TValue>& lhs, const std::pair<TKey, TValue>& rhs)
        {
            return TLess()(lhs.first, rhs.first);
        }
    };

    SortedList<std::pair<TKey, TValue>, PairLess> ToSortedList();
};


template <typename TKey, typename TValue, typename TLess>
void AvlTree<TKey, TValue, TLess>::Insert(const TKey& key, const TValue& value)
{
    _root = InsertToTree(_root, key, value);
}

template <typename TKey, typename TValue, typename TLess>
bool AvlTree<TKey, TValue, TLess>::Contains(const TKey& key)
{
    auto comparing = _root;
    while (true)
    {
        if (comparing == nullptr)
        {
            return false;
        }

        if (TLess()(comparing->Key, key))
        {
            comparing = comparing->Right;
        }
        else if (TLess()(key, comparing->Key))
        {
            comparing = comparing->Left;
        }
        else
        {
            return true;
        }
    }
}

template <typename TKey, typename TValue, typename TLess>
TValue AvlTree<TKey, TValue, TLess>::Search(const TKey& key)
{
    auto comparing = _root;
    while (true)
    {
        if (comparing == nullptr)
        {
            return TValue();
        }

        if (TLess()(comparing->Key, key))
        {
            comparing = comparing->Right;
        }
        else if (TLess()(key, comparing->Key))
        {
            comparing = comparing->Left;
        }
        else
        {
            return comparing->Value;
        }
    }
}


template <typename TKey, typename TValue, typename TLess>
void AvlTree<TKey, TValue, TLess>::Remove(const TKey& key)
{
    _root = RemoveInTree(_root, key);
}

template <typename TKey, typename TValue, typename TLess>
void AvlTree<TKey, TValue, TLess>::InorderTraversal(
    const std::function<void(const TKey&, const TValue&)>& traversalFunction)
{
    if (_root != nullptr)
    {
        _root->InorderTraversal(traversalFunction);
    }
}

template <typename TKey, typename TValue, typename TLess>
AvlTree<TKey, TValue, TLess>::~AvlTree()
{
    delete _root;
}


template <typename TKey, typename TValue, typename TLess>
typename AvlTree<TKey, TValue, TLess>::AvlTreeNode* AvlTree<TKey, TValue, TLess>::RotateLeftLeft(AvlTreeNode* node)
{
    auto newRoot = node->Left;
    node->Left = newRoot->Right;
    newRoot->Right = node;

    // Update height.
    node->Height = std::max(AvlTreeNode::GetHeight(node->Left), AvlTreeNode::GetHeight(node->Right)) + 1;
    newRoot->Height = std::max(AvlTreeNode::GetHeight(newRoot->Left), AvlTreeNode::GetHeight(node)) + 1;

    return newRoot;
}


template <typename TKey, typename TValue, typename TLess>
typename AvlTree<TKey, TValue, TLess>::AvlTreeNode* AvlTree<TKey, TValue, TLess>::RotateRightRight(AvlTreeNode* node)
{
    auto newRoot = node->Right;
    node->Right = newRoot->Left;
    newRoot->Left = node;

    // Update height.
    node->Height = std::max(AvlTreeNode::GetHeight(node->Left), AvlTreeNode::GetHeight(node->Right)) + 1;
    newRoot->Height = std::max(AvlTreeNode::GetHeight(node), AvlTreeNode::GetHeight(newRoot->Right)) + 1;

    return newRoot;
}


template <typename TKey, typename TValue, typename TLess>
typename AvlTree<TKey, TValue, TLess>::AvlTreeNode* AvlTree<TKey, TValue, TLess>::RotateLeftRight(AvlTreeNode* node)
{
    node->Left = RotateRightRight(node->Left);
    return RotateLeftLeft(node);
}


template <typename TKey, typename TValue, typename TLess>
typename AvlTree<TKey, TValue, TLess>::AvlTreeNode* AvlTree<TKey, TValue, TLess>::RotateRightLeft(AvlTreeNode* node)
{
    node->Right = RotateLeftLeft(node->Right);
    return RotateRightRight(node);
}


template <typename TKey, typename TValue, typename TLess>
typename AvlTree<TKey, TValue, TLess>::AvlTreeNode*
AvlTree<TKey, TValue, TLess>::InsertToTree(AvlTreeNode* tree, const TKey& key, const TValue& value)
{
    if (tree == nullptr)
    {
        tree = new AvlTreeNode(key, value);
    }
    else if (TLess()(key, tree->Key))
    {
        tree->Left = InsertToTree(tree->Left, key, value);

        if (AvlTreeNode::GetHeight(tree->Left) - AvlTreeNode::GetHeight(tree->Right) == 2)
        {
            if (TLess()(key, tree->Left->Key))
            {
                tree = RotateLeftLeft(tree);
            }
            else
            {
                tree = RotateLeftRight(tree);
            }
        }
    }
    else if (TLess()(tree->Key, key))
    {
        tree->Right = InsertToTree(tree->Right, key, value);

        if (AvlTreeNode::GetHeight(tree->Left) - AvlTreeNode::GetHeight(tree->Right) == -2)
        {
            if (TLess()(tree->Right->Key, key))
            {
                tree = RotateRightRight(tree);
            }
            else
            {
                tree = RotateRightLeft(tree);
            }
        }
    }
    else
    {
        tree->Value = value;
    }

    tree->Height = std::max(AvlTreeNode::GetHeight(tree->Left), AvlTreeNode::GetHeight(tree->Right)) + 1;
    return tree;
}

template <typename TKey, typename TValue, typename TLess>
typename AvlTree<TKey, TValue, TLess>::AvlTreeNode*
AvlTree<TKey, TValue, TLess>::RemoveInTree(AvlTreeNode* tree, const TKey& key)
{
    if (tree == nullptr)
    {
        return nullptr;
    }

    if (TLess()(key, tree->Key))
    {
        tree->Left = RemoveInTree(tree->Left, key);

        if (AvlTreeNode::GetHeight(tree->Right) - AvlTreeNode::GetHeight(tree->Left) == 2)
        {
            auto rightTree = tree->Right;

            if (AvlTreeNode::GetHeight(rightTree->Left) > AvlTreeNode::GetHeight(rightTree->Right))
            {
                tree = RotateRightLeft(tree);
            }
            else
            {
                tree = RotateRightRight(tree);
            }
        }
    }
    else if (TLess()(tree->Key, key))
    {
        tree->Right = RemoveInTree(tree->Right, key);

        if (AvlTreeNode::GetHeight(tree->Left) - AvlTreeNode::GetHeight(tree->Right) == 2)
        {
            auto leftTree = tree->Left;

            if (AvlTreeNode::GetHeight(leftTree->Left) < AvlTreeNode::GetHeight(leftTree->Right))
            {
                tree = RotateLeftRight(tree);
            }
            else
            {
                tree = RotateLeftLeft(tree);
            }
        }
    }
    else
    {
        if (tree->Left != nullptr && tree->Right != nullptr)
        {
            if (AvlTreeNode::GetHeight(tree->Left) > AvlTreeNode::GetHeight(tree->Right))
            {
                auto leftTree = tree->Left;
                auto maxNode = leftTree;
                while (maxNode->Right != nullptr)
                {
                    maxNode = maxNode->Right;
                }

                tree->Key = maxNode->Key;
                tree->Value = std::move(maxNode->Value);

                tree->Left = RemoveInTree(leftTree, maxNode->Key);
            }
            else
            {
                auto rightTree = tree->Right;
                auto minNode = rightTree;
                while (minNode->Left != nullptr)
                {
                    minNode = minNode->Left;
                }

                tree->Key = minNode->Key;
                tree->Value = std::move(minNode->Value);

                tree->Right = RemoveInTree(rightTree, minNode->Key);
            }
        }
        else
        {
            auto temp = tree;
            tree = (tree->Left == nullptr ? tree->Right : tree->Left);
            temp->Left = nullptr;
            temp->Right = nullptr;
            delete temp;
        }
    }

    return tree;
}

template <typename TKey, typename TValue, typename TLess>
typename AvlTree<TKey, TValue, TLess>::ReadWriteIterator AvlTree<TKey, TValue, TLess>::Locate(const TKey& key)
{
    auto comparing = _root;
    while (true)
    {
        if (comparing == nullptr)
        {
            return end();
        }

        if (TLess()(comparing->Key, key))
        {
            comparing = comparing->Right;
        }
        else if (TLess()(key, comparing->Key))
        {
            comparing = comparing->Left;
        }
        else
        {
            return ReadWriteIterator(comparing);
        }
    }
}

template <typename TKey, typename TValue, typename TLess>
SortedList<std::pair<TKey, TValue>, typename AvlTree<TKey, TValue, TLess>::PairLess>
AvlTree<TKey, TValue, TLess>::ToSortedList()
{
    SortedList<std::pair<TKey, TValue>, typename AvlTree<TKey, TValue, TLess>::PairLess> ret;
    InorderTraversal(
        [&ret](const TKey& key,const TValue& value) -> void
        {
            ret.Insert(std::make_pair(key, value));
        }
    );

    return ret;
}

#endif //DATASTRUCTUREPROJECT_AVLTREE_HPP
