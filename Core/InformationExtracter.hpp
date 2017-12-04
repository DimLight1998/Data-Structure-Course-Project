//
// Created on 2017/10/26 at 23:59.
//

#ifndef DATASTRUCTUREPROJECT_INFORMATIONEXTRACTER_HPP
#define DATASTRUCTUREPROJECT_INFORMATIONEXTRACTER_HPP

#include "CharString.hpp"
#include "CharStringList.hpp"
#include "XmlParser.hpp"
#include "Spider.hpp"

/// \brief A class to extracte information from <a href="http://bbs.cehome.com/">cehome bbs</a>.
/// \note To use the class, you need to construct an instance with a HTML node. You can get the node with XmlParser.
class InformationExtracter
{
public:
    /// \brief Get the BigCategory of the thread in the instance.
    /// \return BigCategory of the thread.
    CharString GetBigCategory();

    /// \brief Get the SmallCategory of the thread in the instance.
    /// \return SmallCategory of the thread.
    CharString GetSmallCategory();

    /// \brief Get the PostTitle of the thread in the instance.
    /// \return PostTitle of the thread.
    CharString GetPostTitle();

    /// \brief Get the PostContent of the thread in the instance.
    /// \return PostContent of the thread.
    CharString GetPostContent();

    /// \brief Get the PostAuthor of the thread in the instance.
    /// \return PostAuthor of the thread.
    CharString GetPostAuthor();

    /// \brief Get the PostDate of the thread in the instance.
    /// \return PostDate of the thread.
    CharString GetPostDate();

    /// \brief Get the PostType of the thread in the instance.
    /// \return PostType of the thread.
    CharString GetPostType();

    explicit InformationExtracter(XmlNode* root);
private:
    /// \brief Pointer to the XmlNode.
    XmlNode* _root;
};


InformationExtracter::InformationExtracter(XmlNode* root)
    : _root(root)
{
}


CharString InformationExtracter::GetBigCategory()
{
    CharString attribution;
    attribution.FromStdWstring(std::wstring(L"id=\"pt\""));
    auto divPt = XmlParser::GetElementsByAttribution(_root, attribution).GetItemAt(0);

    LinkedList<int> path;
    path.Append(0);
    path.Append(4);
    auto anchor = XmlParser::NavigateFrom(divPt, path);

    return XmlParser::GetContent(anchor);
}


CharString InformationExtracter::GetSmallCategory()
{
    CharString attribution;
    attribution.FromStdWstring(std::wstring(L"id=\"pt\""));
    auto divPt = XmlParser::GetElementsByAttribution(_root, attribution).GetItemAt(0);

    LinkedList<int> path;
    path.Append(0);
    path.Append(6);
    auto anchor = XmlParser::NavigateFrom(divPt, path);

    return XmlParser::GetContent(anchor);
}


CharString InformationExtracter::GetPostTitle()
{
    CharString attribution;
    attribution.FromStdWstring(std::wstring(L"id=\"thread_subject\""));
    auto anchor = XmlParser::GetElementsByAttribution(_root, attribution).GetItemAt(0);

    LinkedList<int> path;
    path.Append(0);
    auto header = XmlParser::NavigateFrom(anchor, path);

    return XmlParser::GetContent(header);
}


CharString InformationExtracter::GetPostContent()
{
    CharString ignore;
    ignore.FromStdWstring(std::wstring(L"ignore_js_op"));

    CharStringList ignoringList;
    ignoringList.Append(ignore);

    CharString attribution;
    attribution.FromStdWstring(std::wstring(L"class=\"t_f\""));

    auto td = XmlParser::GetElementsByAttribution(_root, attribution).GetItemAt(0);

    // Some of the contents are encoded as HTML, need to decode them.
    auto decoded = Spider::GetDecodedHtml(XmlParser::GetContent(td, ignoringList).ToStdWstring());
    CharString ret;
    ret.FromStdWstring(decoded);

    return ret;
}


CharString InformationExtracter::GetPostAuthor()
{
    CharString attribution;
    attribution.FromStdWstring(std::wstring(L"class=\"authi\""));

    auto div = XmlParser::GetElementsByAttribution(_root, attribution).GetItemAt(0);

    LinkedList<int> path;
    path.Append(0);
    auto anchor = XmlParser::NavigateFrom(div, path);

    return XmlParser::GetContent(anchor);
}


CharString InformationExtracter::GetPostDate()
{
    CharString attribution;
    attribution.FromStdWstring(std::wstring(L"class=\"authi\""));

    auto div = XmlParser::GetElementsByAttribution(_root, attribution).GetItemAt(1);

    LinkedList<int> path;
    path.Append(0);
    auto em = XmlParser::NavigateFrom(div, path);

    return XmlParser::GetContent(em);
}


CharString InformationExtracter::GetPostType()
{
    CharString attribution;
    attribution.FromStdWstring(std::wstring(L"class=\"ts z h1\""));

    auto div = XmlParser::GetElementsByAttribution(_root, attribution).GetItemAt(0);

    LinkedList<int> path;
    path.Append(0);
    auto anchor = XmlParser::NavigateFrom(div, path);

    return XmlParser::GetContent(anchor);
}


#endif //DATASTRUCTUREPROJECT_INFORMATIONEXTRACTER_HPP
