#pragma once

namespace Pseudo
{
    // This enables move construction and assignment of temporary AutoPtr objects (which are const)
	template<class T> struct AutoPtrAux
	{
		AutoPtrAux(T* pOther) : p(pOther) {}
		T* p;
	};

	// Wraps an object pointer to ensure destruction
	template<class T> class AutoPtr
	{
		private: T *p;	
		    // The wrapped object pointer

		// Construct from object pointer
		public: explicit AutoPtr(T *pOther = 0) throw() : p(pOther) {}

		// Construction by assuming pointer from right AutoPtr. Note non-const parameter.
		public: AutoPtr(AutoPtr<T>& right) throw() : p(right.Release()) {}

        // Special conversion with auxilliary type to enable move construction with temporary objects
		public: AutoPtr(AutoPtrAux<T> rhs) throw() : p(rhs.p) {}

		// Construct by assuming pointer from right
		public: template<class Y> AutoPtr(AutoPtr<Y>& right) throw() : p(right.Release()) {}

		// Destroy the object
		public: ~AutoPtr()
		{
			delete p;
		}

		// Return designated value
		public: T& operator*() const throw()
		{
			return (*p);
		}

		// Return pointer to class object
		public: T *operator->() const throw()
		{
			return (&**this);
		}

		// Return wrapped pointer
		public: T *get_Ptr() const throw()
		{
			return (p);
		}

		// Return wrapped pointer and give up ownership
		public: T *Release() throw()
		{
			T *pTemp = p;
			p = 0;
			return (pTemp);
		}

		// Destroy designated object and store new pointer
		public: void Reset(T* pOther = 0)
		{
			if (p != pOther)
				delete p;
			p = pOther;
		}

        // Casts AutoArray<T> to AutoPtrAux<Y>
        public: template<class Y> operator AutoPtrAux<Y>() throw() 
        {
            return AutoPtrAux<Y>(Release());
        }
        
		// Convert to compatible AutoPtr
		public: template<class Y> operator AutoPtr<Y>() throw()
		{
			return (AutoPtr<Y>(*this));
		}

        // Assignments (with implicit conversion). Note non-constant parameter 
		public: AutoPtr& operator=(AutoPtr& right) throw()
		{
			Reset(right.Release());
			return (*this);
		}

        // Special conversion with auxilliary type to enable move assignment with temporary objects
        public: AutoPtr& operator=(AutoPtrAux<T> rhs) throw()
        {
            Reset(rhs.p);
            return *this;
        }

		// Assign compatible right (assume pointer)
		public: template<class Y> AutoPtr<T>& operator=(AutoPtr<Y>& right) throw()
		{
			Reset(right.Release());
			return (*this);
		}

        // Implicit safe bool type
        private: typedef void (AutoPtr::* BoolType)() const;
	    private: void BoolTypeFunc() const {}

    	public:operator BoolType() const throw()
        {
            return p ? &AutoPtr::BoolTypeFunc : 0;
            // TODO-sgrankin-2008/02/20: This is slow, since it is never optimized; a better solution is to use pointer to
            // member; use a detail namespaced member class and member to avoid bloating the pointer class; use global
            // operator== overloads to prevent comparisons with implicitly converted value
        }

        bool operator== (AutoPtr const & rhs) throw() { return p == rhs.p; }
        bool operator!= (AutoPtr const & rhs) throw() { return p != rhs.p; }
        bool operator== (T const * const rhs) throw() { return p == rhs; }
        bool operator!= (T const * const rhs) throw() { return p != rhs; }
	};
}
