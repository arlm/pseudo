// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)
#include <Pseudo\Compiler.hpp>
#include <Pseudo\ValueType.hpp>
#include <Pseudo\CharType.hpp>
#include <Pseudo\Exception.hpp>
#include <Pseudo\CountedArrayPtr.hpp>
#include <Pseudo\AutoArrayPtr.hpp>
#include <Pseudo\Array.hpp>
#include <Pseudo\New.hpp>

namespace Pseudo
{
    // Assert that STRSAFE_MAX_CCH is the same as INT_MAX so it's safe to cast StringCchLength result to Int
    C_ASSERT(STRSAFE_MAX_CCH <= INT_MAX);

    class String
    {
        private: CountedArrayPtr<Byte> data; // Pointer to length (Int), then string Char's then '\0' terminator Char
        public: String()
        {
            // TODO-johnls-2/7/2008: Perhaps don't use a C++ array here - call the memory allocator directly as we are storing the length twice
            this->data = CountedArrayPtr<Byte>(PSEUDO_NEW Byte[sizeof(Int) + sizeof(Char)]);
            *const_cast<Char*>(this->get_Ptr()) = 0;
            *((Int*)this->data.get_Ptr()) = 0;
        }

        public: String(const String& from)
        {
            this->data = from.data;
        }

        public: String(const Char * from)
        {
            CopyFrom(from);
        }

        public: String(const Char * from, Int length)
        {
            CopyFrom(from, length);
        }

        public: String(Char c, Int length)
        {
            this->data = CountedArrayPtr<Byte>(PSEUDO_NEW Byte[sizeof(Int) + (length + 1) * sizeof(Char)]);
            *((Int*)this->data.get_Ptr()) = length;

            // Copy the string
            Int i = get_Length();
            Char* p = const_cast<Char*>(get_Ptr());

            p[i] = 0;

            while(--i >= 0)
                p[i] = c;

        }

        private: String(CountedArrayPtr<Byte> other)
        {
            this->data = other;
        }

        private: void CopyFrom(const Char* from)
        {
            size_t length;

            HRESULT hr = StringCchLengthW(from, STRSAFE_MAX_CCH, &length);

            if (FAILED(hr))
                throw ArgumentException();

            CopyFrom(from, static_cast<Int>(length));
        }

        private: void CopyFrom(const Char * from, Int length)
        {
            this->data = CountedArrayPtr<Byte>(PSEUDO_NEW Byte[sizeof(Int) + (length + 1) * sizeof(Char)]);
            *((Int*)this->data.get_Ptr()) = length;

            // Copy the string
            Int i = get_Length();
            Char* p = const_cast<Char*>(get_Ptr());

            p[i] = 0;

            while(--i >= 0)
                p[i] = from[i];
        }

        public: ~String()
        {
        }

        public: const Char operator[](Int index) const
        {
            if (index > this->get_Length() - 1)
                throw ArgumentException();

            return get_Ptr()[index];
        }

        public: String& operator=(const String& other)
        {
            this->data = other.data;

            return *this;
        }

        public: String& operator=(const Char* p)
        {
            CopyFrom(p);

            return *this;
        }

        public: bool operator==(const String& s) const
        {
            return (String::Compare(*this, s, true) == 0);
        }

        public: bool operator!=(const String& s) const
        {
            return !(*this == s);
        }

        public: bool operator==(const Char* p) const
        {
            // Go with the pointer comparison, as taking the length is less work than creating a new String object
            return (String::Compare(this->get_Ptr(), p, true) == 0);
        }

        public: bool operator!=(const Char* p) const
        {
            return !(*this == p);
        }

        public: static Int Compare(const String& s1, const String& s2, Bool ignoreCase = true)
        {
            Int result = CompareStringW(
                LOCALE_USER_DEFAULT, ignoreCase ? NORM_IGNORECASE : 0,
                s1.get_Ptr(), s1.get_Length(), s2.get_Ptr(), s2.get_Length());

            if (result == 0)
                throw ArgumentException();

            // See help on CompareString for why this works
            return (result - 2);
        }

        public: static Int Compare(const Char* s1, const Char* s2, Bool ignoreCase = true)
        {
            HRESULT hr;
            size_t s1Length;
            size_t s2Length;

            hr = StringCchLengthW(s1, STRSAFE_MAX_CCH, &s1Length);

            if (FAILED(hr))
                throw ArgumentException();

            hr = StringCchLengthW(s2, STRSAFE_MAX_CCH, &s2Length);

            if (FAILED(hr))
                throw ArgumentException();

            Int result = CompareStringW(LOCALE_USER_DEFAULT, ignoreCase ? NORM_IGNORECASE : 0,
                s1, static_cast<int>(s1Length), s2, static_cast<int>(s2Length));

            if (result == 0)
                throw ArgumentException();

            // See help on CompareString for why this works
            return (result - 2);
        }

        public: int get_Length() const
        {
            return *(Int*)(this->data.get_Ptr());
        }

        public: const Char* get_Ptr() const
        {
            return (const Char*)(this->data.get_Ptr() + sizeof(Int));
        }

        public: operator const Char*() const
        {
            return this->get_Ptr();
        }

        public: static String get_Empty()
        {
            static String empty(L"");

            return empty;
        }

        public: static String Format(const Char* format, ...)
        {
            va_list argList;
            va_start(argList, format);

            return Format(format, argList);
        }

        public: static String Format(const Char* format, va_list argList)
        {
            size_t bufSize = 256;
            AutoArrayPtr<Char> buf(PSEUDO_NEW Char[bufSize]);

            while (true)
            {
                HRESULT hr = StringCchVPrintfW(buf.get_Ptr(), bufSize, format, argList);

                if (hr == STRSAFE_E_INSUFFICIENT_BUFFER)
                {
                    // Sigh.  Increase buffer size and try again.
                    bufSize *= 2;

                    if (bufSize >= STRSAFE_MAX_CCH)
                        throw ArgumentException();

                    buf.Reset(PSEUDO_NEW Char[bufSize]);
                    continue;
                }
                else if (FAILED(hr))
                {
                    throw ArgumentException();
                }
                else
                    break;
            }

            // TODO: Avoid this final copy by printing directly into the actual buffer
            return String(buf.get_Ptr());
        }

        public: static Bool IsNullOrEmpty(String const* p)
        {
            return (p == NULL || p->get_Length() == 0);
        }

        public: String ToLower() const
        {
            String val(*this);
            ::CharLower(const_cast<Char*>(val.get_Ptr()));
            return val;
        }

        public: String ToUpper() const
        {
            String val(*this);
            ::CharUpper(const_cast<Char*>(val.get_Ptr()));
            return val;
        }

        public: Int LastIndexOf(Char c) const
        {
            if (this->get_Length() == 0)
                return -1;

            return LastIndexOf(c, this->get_Length() - 1);
        }

        public: Int LastIndexOf(Char c, Int start) const
        {
            DEBUG_ASSERT(start >= 0 && start < this->get_Length());

            Int i = start;

            while (i >= 0)
            {
                if (this->get_Ptr()[i] == c)
                    break;
                else
                    i--;
            }

            return i;
        }

        public: Int IndexOf(Char c) const
        {
            return IndexOf(c, 0);
        }
        public: Int IndexOf(Char c, Int start) const
        {
            DEBUG_ASSERT(start >= 0 && start < this->get_Length());
            
            Int i = start;
            
            while (i < get_Length())
            {
                if (this->get_Ptr()[i] == c)
                    break;
                else
                    i++;
            }
            
            if (i == get_Length())
                i = -1;
            
            return i;
        }

        public: String Trim() const
        {
            if (get_Length() == 0)
                return *this; 
            
            Int start = 0;
            
            while (start != get_Length() && CharType::IsSpace((*this)[start]))
                start++;

            if (start == get_Length())
                return get_Empty();
        
            Int end = get_Length() - 1;
           
            while (CharType::IsSpace((*this)[end]))
                end--;
                
            return SubString(start, end - start + 1);
        }

        public: String SubString(Int start, Int length) const
        {
            DEBUG_ASSERT(start >= 0 && start <= this->get_Length());

            if (length == 0)
                return get_Empty();

            Int end = start + length - 1;

            DEBUG_ASSERT(end >= 0 && end < this->get_Length() && end >= start);

            return String(this->get_Ptr() + start, length);
        }

        public: String SubString(Int start) const
        {
            return SubString(start, this->get_Length() - start);
        }

        private: Bool StartsWith(const Char* pOther, Int otherLength, Bool ignoreCase = true) const
        {
            HRESULT hr;

            hr = StringCchLengthW(pOther, STRSAFE_MAX_CCH, (size_t*)&otherLength);

            if (FAILED(hr))
                throw ArgumentException();

            // Check if this string is too short to match
            if (get_Length() < otherLength)
            {
                return false;
            }

            Int result = CompareStringW(
                LOCALE_USER_DEFAULT, ignoreCase ? NORM_IGNORECASE : 0,
                this->get_Ptr(), otherLength, pOther, otherLength);

            return (result == CSTR_EQUAL);
        }

        public: Bool StartsWith(const Char* pOther, Bool ignoreCase = true) const
        {
            HRESULT hr;
            size_t otherLength;

            hr = StringCchLengthW(pOther, STRSAFE_MAX_CCH, &otherLength);

            if (FAILED(hr))
                throw ArgumentException();

            return StartsWith(pOther, static_cast<Int>(otherLength), ignoreCase);
        }

        public: template<int N>
        Bool StartsWith(Char const (& pOther)[N], Bool ignoreCase = true) const
        {
            return StartsWith(pOther, N-1, ignoreCase);
        }

        public: Bool StartsWith(const String& other, Bool ignoreCase = true) const
        {
            // Check if this string is too short to match
            if (get_Length() < other.get_Length())
            {
                return false;
            }

            Int result = CompareStringW(
                LOCALE_USER_DEFAULT, ignoreCase ? NORM_IGNORECASE : 0,
                this->get_Ptr(), other.get_Length(), other.get_Ptr(), other.get_Length());

            if (result == 0)
                throw ArgumentException();

            return (result == CSTR_EQUAL);
        }

        public: Bool EndsWith(const Char* pOther, Bool ignoreCase = true) const
        {
            HRESULT hr;
            size_t otherLength;

            hr = StringCchLengthW(pOther, STRSAFE_MAX_CCH, &otherLength);

            if (FAILED(hr))
                throw ArgumentException();

            Int otherLengthAsInt = static_cast<Int>(otherLength);
            // Check if this string is too short to match
            if (get_Length() < otherLengthAsInt)
            {
                return false;
            }

            Int result = CompareStringW(
                LOCALE_USER_DEFAULT, ignoreCase ? NORM_IGNORECASE : 0,
                this->get_Ptr() + this->get_Length() - otherLengthAsInt, otherLengthAsInt, pOther, otherLengthAsInt);

            return (result == CSTR_EQUAL);
        }

        public: Bool EndsWith(const String& other, Bool ignoreCase = true) const
        {
            // Check if this string is too short to match
            if (get_Length() < other.get_Length())
            {
                return false;
            }

            Int result = CompareStringW(
                LOCALE_USER_DEFAULT, ignoreCase ? NORM_IGNORECASE : 0,
                this->get_Ptr() + this->get_Length() - other.get_Length(), other.get_Length(), other.get_Ptr(), other.get_Length());

            if (result == 0)
                throw ArgumentException();

            return (result == CSTR_EQUAL);
        }

        public: Bool EndsWith(Char c) const
        {
            // Check if this string is too short to match
            if (get_Length() < 1)
            {
                return false;
            }

            return *(this->get_Ptr() + this->get_Length() - 1) == c;
        }

        public: void Split(Char separator, Array<String>& output) const
        {
	        output.Clear();
	        
	        if (get_Length() == 0)
	            return;
	        
	        // Run through the string and count the separator characters and set the array to one more than that number
	        Int count = 0;
            Int i;
	        
	        for (i = get_Length() - 1; i >= 0; i--)
	            if (get_Ptr()[i] == separator)
	                ++count;
	        
	        // TODO-johnls-9/20/2007: We could just save the offsets in an array...
	        
	        output.set_Count(count + 1);

            // Now run through the string and extract substrings for between all the separators
            Int base = 0;
            Int j = 0;
            
            i = 0;
            
            while(i < get_Length())
            {
                if (get_Ptr()[i] != separator)
                {
                    i++;
                }
                else
                {    
                    output[j++] = SubString(base, i - base);
                    
                    i++;

                    // This may be off the end of the string, but if so we will exit the loop on the next iteration
                    base = i;
                }
            }
            
            // Extract the string after the last separator
            if (base < get_Length())
                output[j] = SubString(base, i - base);
            else
                // This means there was a trailing separator at the end of the string
                output[j] = get_Empty();
        }

        public: static String Join(Array<String>& strings)
        {
            Int length = 0;
        
            for (Int i = 0; i < strings.get_Count(); i++)
                length += strings[i].get_Length();
                
            // Make new string string
            Byte* pNewData = PSEUDO_NEW Byte[sizeof(Int) + (length + 1) * sizeof(Char)];
            *((Int*)pNewData) = length;
            
            // Copy the strings in
            Char* p = (Char*)(pNewData + sizeof(Int));
            
            for (Int i = 0; i < strings.get_Count(); i++)
            {
                for (Int j = 0; j < strings[i].get_Length(); j++)
                    *p++ = strings[i].get_Ptr()[j];
            }

            ((Char*)(pNewData + sizeof(Int)))[length] = 0;
            
            return String(CountedArrayPtr<Byte>(pNewData));
        }

        // TODO-johnls-9/19/2007: We need a Concat that takes a const Char* as its second parameter for better performance
        public: static String Concat(const String& first, const String& second)
        {
            Int firstLength = first.get_Length();
            Int secondLength = second.get_Length();

            // Make the new string string bigger
            Byte* pNewData = PSEUDO_NEW Byte[sizeof(Int) + (firstLength + secondLength + 1) * sizeof(Char)];
            *((Int*)pNewData) = firstLength + secondLength;

            // Copy the first string
            for (Int i = 0; i < firstLength; i++)
                ((Char*)(pNewData + sizeof(Int)))[i] = first.get_Ptr()[i];

            // Copy the second string
            for (Int i = 0; i < secondLength; i++)
                ((Char*)(pNewData + sizeof(Int)))[i + firstLength] = second.get_Ptr()[i];

            ((Char*)(pNewData + sizeof(Int)))[firstLength + secondLength] = 0;

            return String(CountedArrayPtr<Byte>(pNewData));
        }

        public: static String Concat(const String& s, Char c)
        {
            Int length = s.get_Length();

            // Make the string bigger
            Byte* pNewData = PSEUDO_NEW Byte[sizeof(Int) + (length + 1 + 1) * sizeof(Char)];
            *((Int*)pNewData) = length + 1;

            // Copy the original string
            for (Int i = 0; i < length; i++)
                ((Char*)(pNewData + sizeof(Int)))[i] = s.get_Ptr()[i];

            // Copy the new character
            ((Char*)(pNewData + sizeof(Int)))[length] = c;

            // Zero terminate
            ((Char*)(pNewData + sizeof(Int)))[length + 1] = 0;

            return String(CountedArrayPtr<Byte>(pNewData));
        }

        public: String operator+(const String& other) const
        {
            return Concat(*this, other);
        }

        public: String operator+(Char c) const
        {
            return Concat(*this, c);
        }

        public: String operator+=(const Char* pOther)
        {
            // TODO-johnls-9/19/2007: Replace with Concat(String, Char*) when we have it
            return *this = Concat(*this, String(pOther));
        }

        public: String operator+=(const String& other)
        {
            return *this = Concat(*this, other);
        }

        public: String operator+=(Char c)
        {
            return *this = Concat(*this, c);
        }
    };

    inline static String operator+(const String& b, const Char* p)
    {
        // TODO-johnls-9/19/2007: Replace with Concat(String, Char*) when we have it
        return String::Concat(b, String(p));
    }

    inline static String operator+(const Char* p, const String& b)
    {
        return String::Concat(String(p), b);
    }
}

#pragma warning(pop)
