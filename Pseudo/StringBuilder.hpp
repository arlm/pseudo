// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)
#include <strsafe.h>
#include <limits.h>

#include <Pseudo\Exception.hpp>
#include <Pseudo\ValueType.hpp>
#include <Pseudo\CountedArrayPtr.hpp>
#include <Pseudo\AutoArrayPtr.hpp>
#include <Pseudo\Exception.hpp>
#include <Pseudo\Trace.hpp>
#include <Pseudo\String.hpp>

namespace Pseudo
{
    // Assert that STRSAFE_MAX_CCH is the same as INT_MAX so it's safe to cast StringCchLength result to Int
    C_ASSERT(STRSAFE_MAX_CCH <= INT_MAX);

    /// <summary>
    /// A growable string object.  Note, the length of the string is always dynamically calculated by looking for the terminator.
    /// This allows the object to be used as a generic buffer for functions that take a buffer pointer and fill it in.
    /// </summary>
    class StringBuilder
    {
        private: Int capacity;
            // Actual string capacity
        private: AutoArrayPtr<Char> data; 
            // Pointer to '\0' terminated string
        private: static Int capacityUnit;
        
        public: StringBuilder() : capacity(0)
        {
            set_Capacity(capacity);
        }
       
        public: StringBuilder(Int capacity) : capacity(0)
        {
            set_Capacity(capacity);
        }

        public: StringBuilder(const Char* from) : capacity(0)
        {
            size_t length;

            HRESULT hr = StringCchLengthW(from, STRSAFE_MAX_CCH, &length);
            
            if (FAILED(hr))
                throw ArgumentException(); 

            // Copy the string
            Int i = static_cast<Int>(length);
            set_Capacity(i);
            Char* p = this->data.get_Ptr();
            
            p[i] = 0;
            
            while(--i >= 0)
                p[i] = from[i];
        }

        public: StringBuilder(Char c, Int length)
        {
            set_Capacity(length);
            
            // Copy the string
            Int i = length;
            Char* p = this->data.get_Ptr();
            
            // Terminate the string
            p[i] = 0;
            
            while(--i >= 0)
                p[i] = c;
        }

        public: virtual ~StringBuilder()
        {
        }

        public: int get_Length()
        {
            size_t length;

            HRESULT hr = ::StringCchLengthW(this->get_Ptr(), STRSAFE_MAX_CCH, &length);
            
            if (FAILED(hr))
                throw ArgumentException(); 

            return static_cast<int>(length);
        }
        
        public: void set_Length(Int value)
        {
            Int length = get_Length();
            
            if (value < length)
            {
                this->data.get_Ptr()[value] = 0;    
            }
            else if (value > length) 
            {
                // TODO: Extend the capacity if necessary and fill with spaces
                DEBUG_ASSERT(false);    
            }
        }
        
        public: Char* get_Ptr()
        {
            return this->data.get_Ptr();
        }
        
        public: operator Char*()
        {
            return this->get_Ptr();
        }

        public: String ToString()
        {
            return String(this->data.get_Ptr());
        }

        public: void AppendFormat(const Char* format, ...)
        {
            va_list argList;
            va_start(argList, format);

            return AppendFormat(format, argList);
        }
        
        public: void AppendFormat(const Char* format, va_list argList)
        {
            size_t bufSize = 256;
            AutoArrayPtr<Char> buf(PSEUDO_NEW Char[bufSize]);

            while (true)
            {           
                HRESULT hr = StringCchVPrintfW(buf.get_Ptr(), bufSize, format, argList);
                
                if (hr == STRSAFE_E_INSUFFICIENT_BUFFER)
                {
                    // Sigh.  Wish the function would tell us how much bigger the buffer needs to be.  Increase buffer size and try again.
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
            
            // TODO: Need to copy the buffer to this 
            DEBUG_ASSERT(false);
        }
        
        public: void Append(Char c)
        {
            Int length = this->get_Length();
            
            if (length + 1 >= capacity - 1)
            {
                set_Capacity(length + 1);
            }
            
            this->data[length++] = c;
            this->data[length] = 0;
        }
        
        public: void Append(const Char* pOther)
        {
            Int otherLength;

            HRESULT hr = StringCchLengthW(pOther, STRSAFE_MAX_CCH, (size_t*)&otherLength);
            
            if (FAILED(hr))
                throw ArgumentException(); 

            Int length = this->get_Length();
            Int otherLengthAsInt = static_cast<Int>(otherLength);
            
            if (length + otherLengthAsInt >= capacity - 1)
            {
                set_Capacity(length + otherLengthAsInt);
            }
            
            for (Int i = 0; i < otherLengthAsInt; i++)
            {
                this->data.get_Ptr()[i + length] = pOther[i];
            }           
            
            this->data[length + otherLengthAsInt] = 0;
        }
        
        public: void set_Capacity(Int value)
        {
            if (value < 0)
                throw ArgumentOutOfRangeException();

            // Round up to the nearest whole allocation unit
            Int newCapacity = ((value / capacityUnit) + 1) * capacityUnit;
            
            // If this is the first time the buffer is being set...
            if (this->data.get_Ptr() == NULL)
            {
                this->data = AutoArrayPtr<Char>(PSEUDO_NEW Char[newCapacity]);
                
                // Zero terminate
                *this->data.get_Ptr() = 0;
            }
            else 
            {
                Int length = this->get_Length();
                
                // If the caller supplied new capacity is less than the length, reduce the length
                if (value < length)
                {
                    this->data.get_Ptr()[value] = 0;
                }
            
                // If necessary, reallocate the array and copy the data
                if (value > capacity - 1)
                {
                    AutoArrayPtr<Char> newData(PSEUDO_NEW Char[newCapacity]);

                    Int i;
                                    
                    for (i = 0; i < length; i++)
                        newData[i] = data[i];
                    
                    // Terminate    
                    newData[i] = 0;
                
                    this->data = newData;
                }
            }
            
            this->capacity = newCapacity;
        }

        
        // The capacity of the StringBuilder, including the space reserved for the null terminator.
        public: int get_Capacity() const
        {
            return capacity;
        }
    };
    
    __declspec(selectany) Int StringBuilder::capacityUnit = 32;
}

#pragma warning(pop)
