// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#include <Pseudo\AutoPtr.hpp>

namespace Pseudo
{
    // A class for counted pointer semantics, i.e. it deletes the object to which it 
    // refers when the last CountedPtr that refers to it is destroyed
	template <class T>  class CountedPtr 
	{
		private: T* p;        
		    // pointer to the value
		private: Int* pCount;   
		    // shared number of owners.  It's a pointer because everyone's got to be looking at the same reference!

		public: typedef T ElementType;

		public: template <class Y> friend class CountedPtr;	

		// Initialize pointer with existing pointer - requires that the pointer p is a return value of new
		public: explicit CountedPtr (T* pOther = 0) 
		{
			p = pOther;
			pCount = (pOther != NULL ? PSEUDO_NEW Int(1) : NULL);
		}

		// copy pointer (one more owner)
		public: CountedPtr (CountedPtr<T> const & pOther) : p(pOther.p), pCount(pOther.pCount) 
		{
			++*pCount;
		}

		public: template<class Y> CountedPtr(CountedPtr<Y> const & p) : p(pOther.p), pCount(p.pCount)
		{
			++*pCount;
		}

		public: template<class Y> CountedPtr(AutoPtr<Y> & pOther) : p(pOther.Release()), pCount(PSEUDO_NEW Int(1))
		{
		}

		// destructor (delete value if this was the last owner)
		public: ~CountedPtr () 
		{
			Dispose();
		}

		// assignment (unshare old and share new value)
		public: CountedPtr<T>& operator= (CountedPtr<T> const & pOther)
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

        public: bool operator==(CountedPtr<T> const & pOther)
        {
            return p == pOther.p;
        }

        public: bool operator!=(CountedPtr<T> const & pOther)
        {
            return !(this == pOther);
        }

		private: void Dispose() 
		{
			if (pCount != NULL && --*pCount == 0) 
			{
				delete pCount;
				delete p;
			}
		}
	};
}
