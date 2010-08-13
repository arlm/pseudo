// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)

#include <Pseudo\Macros.hpp>
#include <Pseudo\New.hpp>
#include <Pseudo\ValueType.hpp>
#include <Pseudo\Array.hpp>

namespace Pseudo
{
	template <class T> class ArrayList
	{
	private:
		Pseudo::AutoArrayPtr<T> data;
		Int capacity;
		Int count;

		enum
		{
			// TODO: This could be dynamic based on the size of the type
			capacityUnit = 16
		};

	public:
		ArrayList() : capacity(0), count(0)
		{
			set_Capacity(1);
		}

		ArrayList(Int capacity) : capacity(0), count(0)
		{
			set_Capacity(capacity);
		}

		virtual ~ArrayList()
		{
		}

		T& operator[](Int index)
		{
			DEBUG_ASSERT(index < this->count);

			return data[index];
		}

		const T& operator[](Int index) const
		{
			DEBUG_ASSERT(index < this->count);

			return data[index];
		}

		void Clear()
		{
			set_Capacity(0);
			count=0;
		}

		void CopyTo(ArrayList<T>& other) const
		{
			DEBUG_ASSERT(count >= 0);

			other.Clear();
			other.set_Capacity(this->count);

			for (Int i = 0; i < this->count; i++)
				other.Add(this->data[i]);
		}

		void set_Capacity(Int value)
		{
			DEBUG_ASSERT(value >= 0);

			// Now round up the capacity to the nearest whole unit
			value = ((value / capacityUnit) + 1) * capacityUnit;

			if(value==this->capacity)
				return;

			AutoArrayPtr<T> newData(PSEUDO_NEW T[value]);
			Int itemsToCopy = (capacity>value) ? value : this->capacity;

			for(int i=0;i<itemsToCopy;i++)
				newData[i] = data[i];

			this->data.Reset(newData.Release());
			this->capacity = value;
		}

		Int get_Capacity() const
		{
			return this->capacity;
		}

		Int get_Count() const
		{
			return this->count;
		}

		void Add(const T& item)
		{
			if (this->count + 1 >= this->capacity)
			{
				set_Capacity(count + 1);
			}

			data[this->count++] = item;
		}

		void Add(T const * range, Int count)
		{
			if (count < 0)
				throw ArgumentOutOfRangeException();

			if (this->count + count >= this->capacity)
				set_Capacity(this->count + count);

			for(Int i = 0; i < count; ++i)
				data[this->count + i] = range[i];

			this->count += count;
		}

		T Replace (T item, int position)
		{
			T oldItem = data[position];
			data[position] = item;
			return oldItem;
		}

		void InsertAt(T item, int position)
		{
			if (this->count + 1 >= this->capacity)
			{
				set_Capacity(count + 1);
			}

			for (Int i = this->count - 1; i >= position; i--)
				data[i + 1] = data[i];

			data[position] = item;
			this->count++;
		}

		T RemoveAt(int position)
		{
			DEBUG_ASSERT(count>=1);
			if(position<0 || position >= count)
				throw ArgumentOutOfRangeException();

			T result = data[position];
			if(count>1)
			{
				for(int i=position+1;i<count;i++)
					data[i-1] = data[i];
			}
			else
			{
				set_Capacity(0);
			}
			this->count--;
			return result;
		}

		bool Remove(T element)
		{
			for(int i=0;i<count;i++)
			{
				if(data[i]==element)
				{
					this->RemoveAt(i);
					return true;
				}
			}
			return false;
		}

		template<class TPredicate> Int CountIf(Int start, Int end, TPredicate pred)
		{
			Int count = 0;

			for (Int i = start; i <= end; i++)
				count += pred(data[i]);

			return count;
		}

		template<class TFunctor1> Int ForEach(Int start, Int end, TFunctor1 functor)
		{
			Int count = 0;

			for (Int i = start; i <= end; i++)
				functor(data[i]);

			return count;
		}


		template<typename T> void Swap(ArrayList<T>& lhs, ArrayList<T>& rhs)
		{
			Math::wap(lhs.data, rhs.data);
			Math::Swap(lhs.capacity, rhs.capacity);
			Math::Swap(lhs.count, rhs.count);
		}
	};
}

#pragma warning(pop)
