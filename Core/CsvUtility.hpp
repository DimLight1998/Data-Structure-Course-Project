#pragma once

#include "CharString.hpp"
#include "CharStringList.hpp"


/// \brief A class to simplify CSV operations.
class CsvUtility
{
public:
    /// \brief Encode a <code>CharStringList</code> to a csv string.
    /// \param charStringList The list of string to be encoded.
    /// \param withQuotationMark Whether surround each items with quotation marks or not.
    /// \example When withQuotationMark == true, the list ["1", "2", "3"] will be encoded to "1","2","3";
    /// when withQuotationMark == false, the list will be encoded to 1,2,3.
    /// \return The encoded list.
    static CharString CsvEncode(const CharStringList& charStringList, bool withQuotationMark = true);

    /// \brief Decode a csv string.
    /// \param charString The string to be decode.
    /// \return A string list, each item of which is an item in the csv string.
    /// \note Each item in the csv can either be surrounded with quotation marks or not.
    static CharStringList CsvDecode(const CharString& charString);
};


CharString CsvUtility::CsvEncode(const CharStringList& charStringList, bool withQuotationMark)
{
    CharString ret;
    bool isFirst = true;

    std::function<void(const CharString&)> visitFunction = [&ret, &isFirst, withQuotationMark
        ](const CharString& charString) -> void
    {
        std::wstring quatationMarkStd = L"\"";
        auto quatationMark = CharString();
        quatationMark.FromStdWstring(quatationMarkStd);

        if (isFirst)
        {
            isFirst = false;
        }
        else
        {
            CharString c;
            c.FromStdWstring(L",");
            ret.Concat(c);
        }

        if (withQuotationMark)
        {
            ret.Concat(quatationMark);
        }

        ret.Concat(charString);

        if (withQuotationMark)
        {
            ret.Concat(quatationMark);
        }
    };

    const_cast<CharStringList&>(charStringList).Iterate(visitFunction);

    return ret;
}


CharStringList CsvUtility::CsvDecode(const CharString& charString)
{
    CharStringList list;

    auto reading = 0;
    auto start = 0;
    auto end = 0;

    while (reading < charString.GetLength())
    {
        while (charString[reading] == L' ' || charString[reading] == L'\t')
        {
            reading++;
        }

        start = reading;

        if (charString[reading] == L'\"')
        {
            reading++;
            while (charString[reading] != L'\"')
            {
                reading++;
            }
            reading++;
            if (reading == charString.GetLength())
            {
                break;
            }
        }
        else
        {
            while (charString[reading] != L' ' && charString[reading] != L',')
            {
                reading++;
                if (reading == charString.GetLength())
                {
                    break;
                }
            }
        }

        end = reading;
        list.Append(charString.GetSubstring(start, end));

        while (charString[reading] != L',')
        {
            reading++;
        }

        reading++;
    }

    end = charString.GetLength();
    list.Append(charString.GetSubstring(start, end));

    return list;
}
