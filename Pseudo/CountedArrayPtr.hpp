// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

// A class for counted pointer semantics, i.e. it deletes the object to which it 
// refers when the last CountedArrayPtr that refers to it is destroyed
#include <Pseudo\New.hpp>
#include <Pseudo\AutoArrayPtr.hpp>

namespace Pseudo
{
	template <class T>
	class CountedArrayPtr 
	{
		private: T* p;        // pointer to the value
		private: Int *pCount;   // shared number of owners.  It's a pointer because everyone's got to be looking at the same reference!

	    public: typedef T ElementType;

		public: template <class Y> friend class CountedArrayPtr;	

		// Initialize pointer with existing pointer - requires that the pointer p is a return value of new
		public: explicit CountedArrayPtr (T* pOther = 0) 
		{
			p = pOther;
			// Don't ref count the null pointer
			pCount = (pOther != NULL ? PSEUDO_NEW Int(1) : NULL);
		}

		// copy pointer (one more owner)
		public: CountedArrayPtr (CountedArrayPtr<T> const & pOther) 
		{
			p = pOther.p;
			pCount = p.pCount;
			++*pCount;
		}

		public: template<class Y> CountedArrayPtr(CountedArrayPtr<Y> const & pOther) : p(pOther.p), pCount(p.pCount)
		{
			++*pCount;
		}

		public: template<class Y> CountedArrayPtr(AutoArrayPtr<Y> & pOther) : p(pOther.Release()), pCount(PSEUDO_NEW Int(1))
		{
		}

		// destructor (delete value if this was the last owner)
		public: ~CountedArrayPtr () 
		{
			Dispose();
		}

		// assignment (unshare old and share new value)
		public: CountedArrayPtr<T>& operator= (CountedArrayPtr<T> const & pOther)
		{
			if (this != &pOther) 
			{
				Dispose();
				p = pOther.p;
				pCount = pOther.pCount;
				++*pCount;
			}
			return *this;
		}

		// access the value to which the pointer refers
		public: T* get_Ptr() const 
		{
			return p;
		}
		
		public: T& operator*() const 
		{
			return *p;
		}
		
		public: T* operator->() const 
		{
			return p;
		}

        // unchecked array access
        public: T& operator[](size_t i) const
        { 
			return p[i]; 
		}
        
        public: T& At(size_t i) const
        { 
			return p[i]; 
		}

		private: void Dispose() 
		{
			if (pCount != NULL && --*pCount == 0) 
			{
				delete pCount;
				delete [] p;
			}
		}
	};
}
