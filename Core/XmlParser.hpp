//
// Created on 2017/10/26 at 16:10.
//

#ifndef DATASTRUCTUREPROJECT_XMLPARSER_HPP
#define DATASTRUCTUREPROJECT_XMLPARSER_HPP

#include <functional>
#include "XmlNode.hpp"
#include "Stack.hpp"


/// \brief A set of method to parse a xml/html file.
class XmlParser
{
public:
    /// \brief Analyze a xml/html to a xml tree.
    /// \param xml Content of the xml/html.
    /// \return The pseudo root of the xml tree.
    static XmlNode* ParseXml(const CharString& xml);

    /// \brief Test if a tag is an inline tag.
    /// \param tag The name of the tag to be tested.
    /// \return True if the tag is an inline tag, otherwise false.
    static bool IsInlineTag(const CharString& tag);

    /// \brief Get all the texts in a xml node recursively.
    /// \param xmlNode The node whose texts need to be extracted.
    /// \return A string containing all the texts <code>xmlNode</code>.
    static CharString GetContent(XmlNode* xmlNode);

    /// \brief Get all the texts in a xml node recursively, but ignore some texts in specific sub tags.
    /// \param xmlNode The node whose texts need to be extracted.
    /// \param ignoringTags The tags need to be ignored.
    /// \return A string containing all the texts <code>xmlNode</code>,
    /// except those in any of the tag in <code>ignoringTags</code>.
    static CharString GetContent(XmlNode* xmlNode, const LinkedList<CharString>& ignoringTags);

    /// \brief Get a child xml node from a parent xml node.
    /// \param xmlNode The parent xml node.
    /// \param path The path to the child xml node. The indexes start from 0.
    /// \return The child node.
    /// \example By calling NavigateFrom(node, path) where path is {1, 2, 3},
    /// we can get the fourth child of the third child of the second child of the node <b>node</b>.
    static XmlNode* NavigateFrom(XmlNode* xmlNode, const LinkedList<int>& path);

    /// \brief Get all tags with a specific name from children of a parent node recursively.
    /// \param parent The parent node to be searched.
    /// \param name The name of the searching tags.
    /// \return A list of all tags with the name <code>name</code> whose parent is <code>parent</code>.
    static LinkedList<XmlNode *> GetElementsByName(XmlNode* parent, const CharString& name);

    /// \brief Get all tags with a specific name from a list of tags.
    /// \param list The list to filter.
    /// \param name The name of the searching tags.
    /// \return A list of all tags with the name <code>name</code> in the <code>list</code>.
    static LinkedList<XmlNode *> GetElementsByName(LinkedList<XmlNode *>& list, const CharString& name);

    /// \brief Get all tags with a specific attribute from children of a parent node recursively.
    /// \param parent The parent node to be searched.
    /// \param attribution The name of the attribute on the searching tags.
    /// \return A list of all tags with the attribute<code>attribution</code> whose parent is <code>parent</code>.
    static LinkedList<XmlNode *> GetElementsByAttribution(XmlNode* parent, const CharString& attribution);

    /// \brief Get all tags with a specific attribute from a list of tags.
    /// \param list The list to filter.
    /// \param attribution The name of the attribute on the searching tags.
    /// \return A list of all tags with the attribute <code>attribution</code> in the <code>list</code>.
    static LinkedList<XmlNode *> GetElementsByAttribution(LinkedList<XmlNode *>& list, const CharString& attribution);
};


XmlNode* XmlParser::ParseXml(const CharString& xml)
{
    auto root = new XmlNode();

    Stack<XmlNode *> stack;
    stack.Push(root);
    auto reading = 0;

    while (reading < xml.GetLength())
    {
        // It is a node of tag.
        if (xml[reading] == L'<')
        {
            reading++;
            while (CharString::IsSpace(xml[reading]))
            {
                reading++;
            }

            // Now the xml[reading] is pointing to the name of the node.

            if (xml[reading] == L'!' || xml[reading] == L'?') // Test if the node is an comment.
            {
                auto start = reading;
                auto isRealComment = false;

                // Check if is a real comment.
                // Because there are HTMLs like this:
                //
                // <!--[if lt IE 9]>
                // <script src = "https://static.zhihu.com/static/components/respond/dest/respond.min.js">< / script>
                // <link href = "https://static.zhihu.com/static/components/respond/cross-domain/respond-proxy.html" id = "respond-proxy" rel = "respond-proxy" / >
                // <link href = "/static/components/respond/cross-domain/respond.proxy.gif" id = "respond-redirect" rel = "respond-redirect" / >
                // <script src = "/static/components/respond/cross-domain/respond.proxy.js">< / script>
                // <![endif]-->
                //
                // They are all comments.
                if (xml.GetLength() > reading + 2)
                {
                    if (xml[reading + 1] == L'-' && xml[reading + 2] == L'-')
                    {
                        isRealComment = true;
                    }
                }

                while (xml[reading] != L'>')
                {
                    reading++;

                    if (isRealComment && xml[reading] == L'>')
                    {
                        if (xml[reading - 1] == L'-' && xml[reading - 2] == L'-')
                        {
                            break;
                        }
                        reading++;
                    }
                }

                auto end = reading;

                auto commentNode = new XmlNode();
                commentNode->IsCommentNode = true;
                commentNode->NameOrContent.Assign(xml.GetSubstring(start, end));
                stack.Top()->Children.Append(commentNode);

                reading++;
            }
            else if (xml[reading] == L'/') // Test if the node is closing.
            {
                reading++;
                while (CharString::IsSpace(xml[reading]))
                {
                    reading++;
                }

                auto nameStart = reading;

                while (xml[reading] != L'>' && xml[reading] != L' ')
                {
                    reading++;
                }

                auto nameEnd = reading;

                auto closingTagName = xml.GetSubstring(nameStart, nameEnd);

                while (xml[reading] != L'>')
                {
                    reading++;
                }

                reading++;

                // Some of the tags are not closed!
                auto top = stack.Top();
                while (top->NameOrContent != closingTagName)
                {
                    stack.Pop();
                    stack.Top()->Children.Append(top);
                    top = stack.Top();
                }

                stack.Pop();
                stack.Top()->Children.Append(top);
            }
            else // It should be an opening tag.
            {
                auto start = reading;

                // goto the first space or '>'
                while (!CharString::IsSpace(xml[reading]) && xml[reading] != L'>')
                {
                    reading++;
                }

                auto end = reading;

                auto top = new XmlNode();
                stack.Push(top);
                top->NameOrContent.Assign(xml.GetSubstring(start, end));

                // Read all attributes.
                while (xml[reading] != L'>')
                {
                    // goto the first letter or '>'
                    while (CharString::IsSpace(xml[reading]))
                    {
                        reading++;
                    }

                    if (xml[reading] == L'/') // The tag closes it self.
                    {
                        reading++;
                        continue;
                    }
                    if (xml[reading] == L'>')
                    {
                        break;
                    }
                    // This is an attribute, read it.
                    auto attributeStart = reading;

                    while (true)
                    {
                        reading++;
                        // The attribute may be in format of xxx or xxx="yyy".
                        // e.g. <input type="text" name="account" aria-label="xxxxx" placeholder="xxxxx" required>
                        // or <input type="text" name="account" aria-label="xxxxx" placeholder="xxxxx" required />

                        // For format xxx.
                        if (xml[reading] == L'>')
                        {
                            break;
                        }
                            // For format xxx="yyy" or xxx='yyy'.
                        else if (xml[reading] == L'=')
                        {
                            reading++;
                            while (CharString::IsSpace(xml[reading]))
                            {
                                reading++;
                            }

                            wchar_t quotationMark;

                            // Get the type of the quotation mark.
                            quotationMark = xml[reading];
                            reading++;
                            while (xml[reading] != quotationMark)
                            {
                                reading++;
                            }

                            reading++;
                            break;
                        }
                    }

                    auto attributeEnd = reading;
                    top->Attributes.Append(xml.GetSubstring(attributeStart, attributeEnd));
                }

                // Now the tag has been processed.
                // The element may be an inline element.

                if (IsInlineTag(top->NameOrContent))
                {
                    stack.Pop();
                    stack.Top()->Children.Append(top);
                }

                reading++;
            }
        }
        else if (CharString::IsSpace(xml[reading]))
        {
            reading++;
        }
        else // It is a node of text.
        {
            auto start = reading;
            while (xml[reading] != L'<')
            {
                reading++;
            }

            auto end = reading;

            auto textNode = new XmlNode();
            textNode->IsTextNode = true;
            textNode->NameOrContent.Assign(xml.GetSubstring(start, end));
            stack.Top()->Children.Append(textNode);
        }
    }

    // Some of the tags are not closed!
    while (stack.Top() != root)
    {
        auto top = stack.Top();
        stack.Pop();
        stack.Top()->Children.Append(top);
    }

    stack.Pop(); // Pop the root to avoid destruction.
    return root;
}


bool XmlParser::IsInlineTag(const CharString& tag)
{
    CharString inlineTag;

#define TEST_IF_IS_INLINE_TAG(x) \
    inlineTag.FromStdWstring(L##x); \
    if (tag == inlineTag) \
    { return true; }

    TEST_IF_IS_INLINE_TAG("area")
    TEST_IF_IS_INLINE_TAG("base")
    TEST_IF_IS_INLINE_TAG("br")
    TEST_IF_IS_INLINE_TAG("col")
    TEST_IF_IS_INLINE_TAG("command")
    TEST_IF_IS_INLINE_TAG("embed")
    TEST_IF_IS_INLINE_TAG("hr")
    TEST_IF_IS_INLINE_TAG("img")
    TEST_IF_IS_INLINE_TAG("input")
    TEST_IF_IS_INLINE_TAG("keygen")
    TEST_IF_IS_INLINE_TAG("link")
    TEST_IF_IS_INLINE_TAG("meta")
    TEST_IF_IS_INLINE_TAG("param")
    TEST_IF_IS_INLINE_TAG("source")
    TEST_IF_IS_INLINE_TAG("track")
    TEST_IF_IS_INLINE_TAG("wbr")

#undef TEST_IF_IS_INLINE_TAG

    return false;
}


CharString XmlParser::GetContent(XmlNode* xmlNode)
{
    LinkedList<CharString> ignoringTags;
    return GetContent(xmlNode, ignoringTags);
}


CharString XmlParser::GetContent(XmlNode* xmlNode, const LinkedList<CharString>& ignoringTags)
{
    if (xmlNode->IsTextNode)
    {
        return xmlNode->NameOrContent;
    }
    if (xmlNode->IsCommentNode)
    {
        return CharString();
    }
    CharString charString;

    using PointerToNode = XmlNode *;
    std::function<void(const PointerToNode&)> visitingFunction = [&charString, &visitingFunction, &ignoringTags
        ](const PointerToNode& node) -> void
    {
        if (node->IsTextNode)
        {
            charString.Concat(node->NameOrContent);
        }
        else if (node->IsCommentNode)
        {
            return;
        }
        else
        {
            if (ignoringTags.IndexOf(node->NameOrContent) == -1)
            {
                node->Children.Iterate(visitingFunction);
            }
        }
    };

    // Iteration
    if (ignoringTags.IndexOf(xmlNode->NameOrContent) == -1)
    {
        xmlNode->Children.Iterate(visitingFunction);
    }

    return charString;
}


XmlNode* XmlParser::NavigateFrom(XmlNode* xmlNode, const LinkedList<int>& path)
{
    // The length of the path won't be too long, the time wasted by a slow visiting function can be ignored.
    auto length = path.GetLength();
    auto currentNode = xmlNode;

    for (auto i = 0; i < length; i++)
    {
        auto index = path.GetItemAt(i);
        currentNode = currentNode->Children.GetItemAt(index);
    }

    return const_cast<XmlNode *>(currentNode);
}


LinkedList<XmlNode *> XmlParser::GetElementsByName(XmlNode* parent, const CharString& name)
{
    LinkedList<XmlNode *> list;

    using PointerToNode = XmlNode *;
    std::function<void(const PointerToNode&)> visitingFunction = [&name, &visitingFunction, &list
        ](const PointerToNode& node) -> void
    {
        if (node->IsCommentNode || node->IsTextNode)
        {
            return;
        }
        else
        {
            if (node->NameOrContent == name)
            {
                list.Append(node);
            }

            node->Children.Iterate(visitingFunction);
        }
    };

    visitingFunction(parent);

    return list;
}


LinkedList<XmlNode *> XmlParser::GetElementsByName(LinkedList<XmlNode *>& list, const CharString& name)
{
    LinkedList<XmlNode *> ret;

    using PointerToNode = XmlNode *;
    std::function<void(const PointerToNode&)> visitingFunction = [&ret, &name](const PointerToNode& node) -> void
    {
        if (!(node->IsCommentNode) && !(node->IsTextNode) && (node->NameOrContent == name))
        {
            ret.Append(node);
        }
    };

    list.Iterate(visitingFunction);

    return ret;
}


LinkedList<XmlNode *> XmlParser::GetElementsByAttribution(XmlNode* parent, const CharString& attribution)
{
    LinkedList<XmlNode *> list;

    using PointerToNode = XmlNode *;
    std::function<void(const PointerToNode&)> visitingFunction = [&attribution, &visitingFunction, &list
        ](const PointerToNode& node) -> void
    {
        if (node->IsCommentNode || node->IsTextNode)
        {
            return;
        }
        else
        {
            if (node->Attributes.IndexOf(attribution) != -1)
            {
                list.Append(node);
            }

            node->Children.Iterate(visitingFunction);
        }
    };

    visitingFunction(parent);

    return list;
}


LinkedList<XmlNode *> XmlParser::GetElementsByAttribution(LinkedList<XmlNode *>& list, const CharString& attribution)
{
    LinkedList<XmlNode *> ret;

    using PointerToNode = XmlNode *;
    std::function<void(const PointerToNode&)> visitingFunction = [&ret, &attribution](const PointerToNode& node) -> void
    {
        if (!(node->IsCommentNode) && !(node->IsTextNode) && (node->Attributes.IndexOf(attribution) != -1))
        {
            ret.Append(node);
        }
    };

    list.Iterate(visitingFunction);

    return ret;
}


#endif //DATASTRUCTUREPROJECT_XMLPARSER_HPP
