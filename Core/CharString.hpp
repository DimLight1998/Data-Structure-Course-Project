//
// Created on 2017/10/26 at 11:38.
//

#ifndef DATASTRUCTUREPROJECT_CHARSTRING_HPP
#define DATASTRUCTUREPROJECT_CHARSTRING_HPP

#include <string>
#include <stdexcept>
#include <algorithm>

/// \brief A string of <code>wchar_t</code>.
class CharString
{
public:
    /// \brief Get the index of the first occurrence of the target string in the instance.
    /// \param targetString The string to locate in the instance.
    /// \return The index of the first occurrence, -1 if the <code>targetString</code> not found.
    /// \throw std::logic_error if the instance represents an empty string.
    int IndexOf(const CharString& targetString) const;

    /// \brief Get the substring in the instance.
    /// \param left Starting character position of the substring in the instance.
    /// \param right Ending character position of the substring in the instance.
    /// \return The substring with characters indexed with [<code>left</code>,<code>right</code>) in the instance.
    /// \throw std::out_of_range if the indexes are invalid.
    CharString GetSubstring(int left, int right) const;

    /// \brief Concatenate another string to the end of the instance.
    /// \param rhs The string to concatenate.
    void Concat(const CharString& rhs);

    /// \brief Assign the instance.
    /// \param rhs The assigning string.
    void Assign(const CharString& rhs);

    /// \brief Assign the instance.
    /// \param rhs The assigning string.
    /// \return Reference to the newly assigned string.
    CharString& operator=(const CharString& rhs);

    /// \brief Get the hash code of the instance. The hash code will be in [0, 100018].
    /// \return The hash code of the instance.
    unsigned int GetHashCode() const;

    /// \brief Assign the instance from a std::wstring.
    /// \param rhs The assigning std::wstring.
    void FromStdWstring(const std::wstring& rhs);

    /// \brief Translate the instance to a std::wstring.
    /// \return The translated std::wstring.
    std::wstring ToStdWstring() const;

    /// \brief Compare if two instances are equal.
    /// \param rhs The instance to be compared with.
    /// \return True if their content are the same, otherwise false.
    bool operator==(const CharString& rhs) const;

    /// \brief Compare if two instances are not equal.
    /// \param rhs The instance to be compared with.
    /// \return False if their content are the same, otherwise true.
    bool operator!=(const CharString& rhs) const;

    bool operator<(const CharString& rhs) const;
    bool operator>(const CharString& rhs) const;
    bool operator<=(const CharString& rhs) const;
    bool operator>=(const CharString& rhs) const;

    /// \brief Get the character at a specific index of the instance.
    /// \param index The position of the character need to get.
    /// \return The character on the position <code>index</code>.
    const wchar_t operator[](int index) const;

    /// \brief Test if the instance represents an empth string.
    /// \return True if the string is empty, otherwise false.
    bool IsEmpty() const;

    /// \brief Get the length of the string in the instance.
    /// \return The length of the string in the instance.
    int GetLength() const;

    virtual ~CharString();

    /// \brief Test if the character is a white-space character.
    /// \param character The character to be tested.
    /// \return True if the character in one of: space, tab, linefeed, carriage-return, or null character,
    /// otherwise false.
    static bool IsSpace(wchar_t character);

    CharString();
    CharString(const CharString& rhs);
    CharString(CharString&& rhs) noexcept;
    explicit CharString(const std::wstring& rhs);

    /// \brief Replace all the specific characters in the instance to another one.
    /// \param from The replaced character.
    /// \param to The replacing character.
    void Replace(wchar_t from, wchar_t to);
private:
    /// \brief Pointer to the raw data.
    wchar_t* _data = nullptr;

    /// \brief The cached length.
    int _length = 0;

    /// \brief The cached hash code.
    unsigned int _hashCode = 0;

    /// \brief Update the hash code of the instance.
    /// \note Call this method when the content of the instance changed.
    void UpdateHashCode();

public:
    class Hasher
    {
        int operator()(const CharString& charString) const
        {
            return charString.GetHashCode();
        }
    };

    static const int HashMin = 0;
    static const int HashMax = 100019;
};


int CharString::IndexOf(const CharString& targetString) const
{
    // Calculate Next[j].
    if (targetString._length == 0)
    {
        throw std::logic_error("Target string is Empty in CharString::IndexOf()");
    }

    auto next = new int[targetString._length];
    next[0] = -1;

    for (auto i = 1; i < targetString._length; i++)
    {
        if (next[i - 1] == -1)
        {
            next[i] = 0;
        }
        else
        {
            auto matching = targetString._data[i - 1];
            auto nextIndex = next[i - 1];

            while (matching != targetString._data[nextIndex])
            {
                nextIndex = next[nextIndex];
                if (nextIndex == -1)
                {
                    next[i] = 0;
                    break;
                }
            }

            next[i] = nextIndex + 1;
        }
    }

    // Optimize Next[j].
    for (auto i = 1; i < targetString._length; i++)
    {
        if (targetString._data[i] == targetString._data[next[i]])
        {
            next[i] = next[next[i]];
        }
    }

    // Start matching.
    auto i = 0;
    auto j = 0;

    while (true)
    {
        while (_data[i] == targetString._data[j])
        {
            i++;
            j++;

            if (j == targetString._length)
            {
                delete[] next;

                return i - targetString._length;
            }
        }

        while (_data[i] != targetString._data[j])
        {
            j = next[j];
            if (j == -1)
            {
                i++;
                j = 0;
            }
        }

        if (i == _length)
        {
            delete[] next;
            return -1;
        }
    }
}


CharString CharString::GetSubstring(int left, int right) const
{
    if (left >= 0 && left < _length && right >= 0 && right <= _length && left <= right)
    {
        CharString ret;
        if (left == right)
        {
            return ret;
        }
        ret._data = new wchar_t[right - left];
        ret._length = right - left;

        for (auto i = 0; i < ret._length; i++)
        {
            ret._data[i] = _data[i + left];
        }

        return ret;
    }
    throw std::out_of_range("Index out of range in CharString::SubString");
}


void CharString::Concat(const CharString& rhs)
{
    if (rhs._length == 0)
    {
        return;
    }

    auto newData = new wchar_t[_length + rhs._length];

    for (auto i = 0; i < _length; i++)
    {
        newData[i] = _data[i];
    }

    for (auto i = _length; i < _length + rhs._length; i++)
    {
        newData[i] = rhs._data[i - _length];
    }

    _length = _length + rhs._length;
    delete[] _data;
    _data = newData;

    UpdateHashCode();
}


void CharString::Assign(const CharString& rhs)
{
    delete[] _data;
    _data = nullptr;
    _length = rhs._length;

    if (_length > 0)
    {
        _data = new wchar_t[_length];

        for (auto i = 0; i < _length; i++)
        {
            _data[i] = rhs._data[i];
        }
    }

    UpdateHashCode();
}


unsigned CharString::GetHashCode() const
{
    return _hashCode;
}


CharString& CharString::operator=(const CharString& rhs)
{
    Assign(rhs);
    return (*this);
}


void CharString::FromStdWstring(const std::wstring& rhs)
{
    delete[] _data;
    _data = nullptr;
    _length = rhs.size();

    if (_length > 0)
    {
        _data = new wchar_t[_length];

        for (auto i = 0; i < _length; i++)
        {
            _data[i] = rhs[i];
        }
    }

    UpdateHashCode();
}


std::wstring CharString::ToStdWstring() const
{
    return std::wstring(_data, _length);
}


bool CharString::operator==(const CharString& rhs) const
{
    if (_data == nullptr && rhs._data == nullptr)
    {
        return true;
    }

    if (_data == nullptr || rhs._data == nullptr)
    {
        return false;
    }

    if (_length != rhs._length)
    {
        return false;
    }

    for (auto i = 0; i < _length; i++)
    {
        if (_data[i] != rhs._data[i])
        {
            return false;
        }
    }

    return true;
}


bool CharString::operator!=(const CharString& rhs) const
{
    return !((*this) == rhs);
}

inline bool CharString::operator<(const CharString& rhs) const
{
    if (_length == 0)
    {
        return rhs._length != 0;
    }

    if (rhs._length == 0)
    {
        return false;
    }

    auto index = 0;
    const auto minLength = std::min(_length, rhs._length);

    while (_data[index] == rhs._data[index])
    {
        index++;
        if (index == minLength)
        {
            return minLength != rhs._length;
        }
    }

    return _data[index] < rhs._data[index];
}

inline bool CharString::operator>(const CharString& rhs) const
{
    return rhs < *this;
}

inline bool CharString::operator<=(const CharString& rhs) const
{
    return !(*this > rhs);
}

inline bool CharString::operator>=(const CharString& rhs) const
{
    return !(*this < rhs);
}


CharString::~CharString()
{
    delete[] _data;
}


const wchar_t CharString::operator[](int index) const
{
    return _data[index];
}


bool CharString::IsEmpty() const
{
    return _data == nullptr;
}


int CharString::GetLength() const
{
    return _length;
}


bool CharString::IsSpace(wchar_t character)
{
    return (character == L' ' || character == L'\t' || character == L'\n' || character == L'\r' || character == L'\0');
}


CharString::CharString()
{
    UpdateHashCode();
}


CharString::CharString(const CharString& rhs)
{
    Assign(rhs);
}


CharString::CharString(CharString&& rhs) noexcept
{
    _length = rhs._length;
    _data = rhs._data;
    rhs._data = nullptr;

    UpdateHashCode();
}


void CharString::Replace(wchar_t from, wchar_t to)
{
    for (auto i = 0; i < _length; i++)
    {
        if (_data[i] == from)
        {
            _data[i] = to;
        }
    }

    UpdateHashCode();
}


void CharString::UpdateHashCode()
{
    auto sum = 0u;
    for (auto i = 0; i < _length; i++)
    {
        sum += sum * 7 + _data[i];
    }

    _hashCode = HashMin + sum % (HashMax - HashMin);
}


CharString::CharString(const std::wstring& rhs)
{
    FromStdWstring(rhs);
}


#endif //DATASTRUCTUREPROJECT_CHARSTRING_HPP
