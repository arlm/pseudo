// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)

#include <Pseudo\ValueType.hpp>
#include <Pseudo\AutoArrayPtr.hpp>

namespace Pseudo
{
	class BitArray
	{
		private: AutoArrayPtr<Byte> data;
		private: Int count; // Length of array in bits
		
		public: BitArray(Int count) : data(NULL), count(0)
		{
			set_Count(count);
		}

		public: ~BitArray()
		{
		}
		
		public: bool operator[](Int index)
		{
			return Get(index);
		}
		
		public: void set_Count(Int value)
		{
		    Int newByteCount = OffsetToByte(value) + 1;
		    Int oldByteCount = OffsetToByte(count) + 1;
            AutoArrayPtr<Byte> newData = AutoArrayPtr<Byte>(PSEUDO_NEW Byte[newByteCount]);			    
            
            // Zero out the array
            ZeroMemory(newData.get_Ptr(), newByteCount);
            
            if (data.get_Ptr() != NULL)
            {
                if (newByteCount > oldByteCount)
                {
                    for (Int i = 0; i < oldByteCount; i++)
                    {
                        this->data[i] = newData[i];
                    }
                }
                else
                {
                    for (Int i = 0; i < newByteCount; i++)
                    {
                        this->data[i] = newData[i];
                    }
                }
            }
            
		    this->count = value;
		    this->data = newData;
		}
		
		public: Int get_Count()
		{
			return count;
		}
		
		public: bool Get(Int index)
		{
			return (data[OffsetToByte(index)] & (0x80 >> OffsetInByte(index))) != 0;
		}
		
		public: void Set(Int index, bool val)
		{
			Int offset = OffsetToByte(index);
			Byte b = data[offset];
			
			if (val)
				b |= (0x80 >> OffsetInByte(index));
			else
				b &= ~(0x80 >> OffsetInByte(index));
				
			data[offset] = b;
		}
		
		private: Int OffsetInByte(Int index)
		{
			return index % 8;
		}
		
		private: Int OffsetToByte(Int index)
		{
			return index / 8;
		}
	};
}

#pragma warning(pop)
