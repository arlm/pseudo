// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)

#include <Pseudo\Macros.hpp>
#include <Pseudo\New.hpp>
#include <Pseudo\ValueType.hpp>
#include <Pseudo\AutoArrayPtr.hpp>

namespace Pseudo
{
	// One dimensional vector.  Use in place of the crappy C++ array.  Does not include capacity prediction, so use it 
	// when you know exactly how much data you'll be storing. Also, use this class instead of an AutoArrayPtr if you need to know the 
	// length of the memory later on, which is usually always the case.  If the array is grown, new values are zeroed out.
	//
	// This is the class to substitute where you want something close to a .NET array.  Usually, instead of returning an Array<T>* it's 
	// better to have the caller pass in an Array<T>& and just set it using set_Count().  This is cheap if the array has not already
	// been assigned to.
	template <class T> class Array
	{
		private: AutoArrayPtr<T> data;
		private: Int count;

		public: Array()
		{
			count = 0;
			// We don't do anything so as not to allocate any memory in the default constructor
		}

		public: Array(Int count)
		{
			this->count = 0;
			set_Count(count);
		}

        public: Array(const Array<T>& other)
        {
			this->count = 0;
            *this = other;
        }

		public: virtual ~Array()
		{
		}
		
		public: T& operator[](Int index)
		{
		    return GetValue(index);
		}
		
		public: const T& operator[](Int index) const
		{
			DEBUG_ASSERT(index < this->count);
				
			return data[index];
		}
		
        public: void operator=(const Array<T>& other)
        {
            set_Count(other.get_Count());
            other.CopyTo(*this, other.get_Count());
        }

		public: T& GetValue(Int index) const
		{
			DEBUG_ASSERT(index < this->count);
				
			return data[index];
		}
		
		public: void SetValue(Int index, const T& value)
		{
			DEBUG_ASSERT(index < this->count);
			DEBUG_ASSERT(count != 0);
				
			data[index] = value;
		}
		
		public: T* get_Ptr()
		{
			return data.get_Ptr();
		}
		
		public: void Clear()
		{
			data.Reset();
			count = 0;
		}
		
		public: void CopyTo(Array<T>& other, Int count) const
		{
			DEBUG_ASSERT(count >= 0);
			DEBUG_ASSERT(other.get_Count() >= this->count);
			
			for (Int i = 0; i < this->count; i++)
				other[i] = this->data[i];
		}
		
		public: void set_Count(Int newCount)
		{
			DEBUG_ASSERT(newCount >= 0);

			if (newCount == 0)
			{
				Clear();
			}
			else
			{
				AutoArrayPtr<T> newData(PSEUDO_NEW T[newCount]);
                Int i;

                if (newCount < count)
                {
                    for (i = 0; i < newCount; i++)
					    newData[i] = data[i];
                }
                else
                {
                    for (i = 0; i < this->count; i++)
					    newData[i] = data[i];
					
					// Zero out the new entries
					ZeroMemory(newData.get_Ptr() + this->count, (newCount - count) * sizeof(T));
			    }
				
				this->data = newData;
				this->count = newCount;
			}
		}
		
		public: Int get_Count() const
		{
			return this->count;
		}
		
		public: template<class TPredicate> Int CountIf(Int start, Int end, TPredicate pred)
		{
			Int count = 0;
			
			for (Int i = start; i < end; i++)
				count += pred(data[i]);
				
			return count;
		}

		public: template<class TFunctor1> Int ForEach(Int start, Int end, TFunctor1 functor)
		{
			Int count = 0;
			
			for (Int i = start; i < end; i++)
				functor(data[i]);
				
			return count;
		}
	};
}

#pragma warning(pop)
