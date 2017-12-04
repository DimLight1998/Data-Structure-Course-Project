//
// Created on 2017/10/26 at 14:58.
//

#ifndef DATASTRUCTUREPROJECT_CHARSTRINGLIST_HPP
#define DATASTRUCTUREPROJECT_CHARSTRINGLIST_HPP

#include "LinkedList.hpp"
#include "CharString.hpp"

using CharStringList = LinkedList<CharString>;

CharStringList Split(const CharString& charString, wchar_t delimeter)
{
    auto length = charString.GetLength();
    CharStringList ret;

    auto slow = 0;

    while (slow < length && charString[slow] == delimeter)
    {
        slow++;
    }
    if (slow == length)
    {
        return ret;
    }

    auto fast = slow;

    while (true)
    {
        while (charString[fast] != delimeter && fast < length)
        {
            fast++;
        }

        auto slice = charString.GetSubstring(slow, fast);
        ret.Append(slice);

        if (fast == length)
        {
            return ret;
        }

        while (slow < length && charString[slow] != delimeter)
        {
            slow++;
        }
        while(slow < length &&charString[slow] == delimeter)
        {
            slow++;
        }
        if (slow == length)
        {
            return ret;
        }

        fast = slow;
    }
}

#endif //DATASTRUCTUREPROJECT_CHARSTRINGLIST_HPP
