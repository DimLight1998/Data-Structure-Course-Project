//
// Created on 2017/10/26 at 15:55.
//

#ifndef DATASTRUCTUREPROJECT_XMLNODE_HPP
#define DATASTRUCTUREPROJECT_XMLNODE_HPP

#include "LinkedList.hpp"
#include "CharString.hpp"


/// \brief A class representing a node in a xml/html file.
class XmlNode
{
public:
    /// \brief If the node is a comment or a text, this field is the content of it.<br>
    /// Else this field is the name of the tag.
    CharString NameOrContent;

    /// \brief The list of all the attributes of the instance if the instance is a tag.
    LinkedList<CharString> Attributes;

    /// \brief The list of all the children nodes of the instance if the instance is a tag.
    LinkedList<XmlNode *> Children;

    /// \brief Whether the node is a comment.
    bool IsCommentNode = false;

    /// \brief Whether the node is a text.
    bool IsTextNode = false;

    ~XmlNode()
    {
        for(auto i:Children)
        {
            delete i;
        }
    }
};


#endif //DATASTRUCTUREPROJECT_XMLNODE_HPP
