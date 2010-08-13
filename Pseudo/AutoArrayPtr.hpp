#pragma once

namespace Pseudo
{
    // This enables move construction and assignment of temporary AutoArrayPtr objects (which are const)
    template<class T> struct AutoArrayPtrAux 
    {
        AutoArrayPtrAux(T* pOther) : p(pOther) {}
        T* p;
    };
    
    // The template class describes an object that stores a pointer to an allocated object of type T*[] 
    // that ensures that the object to which it points gets destroyed automatically when control leaves a block.
    template<class T> class AutoArrayPtr
    {
        private: T* p;	// refers to the actual owned object (if any)
    
        // Constructor 
        public: explicit AutoArrayPtr(T* pOther = 0) throw() : p(pOther) {}
        
        // Copy constructor (with implicit conversion). Note non-constant parameter 
        public: AutoArrayPtr(AutoArrayPtr& rhs) throw() : p(rhs.Release()) {}
        
        // Copy constructor (with implicit conversion). Note non-constant parameter 
        public: template<class Y> AutoArrayPtr(AutoArrayPtr<Y>& rhs) throw() : p(rhs.Release()) {}

         // Special constructory with auxilliary type to enable move construction with temporary objects
        public: AutoArrayPtr(AutoArrayPtrAux<T> rhs) throw() : p(rhs.p) {}
        
        // Destructor 
        public: ~AutoArrayPtr() throw()
        {
            delete [] p;
        }
        
        // Value access 
        public: T* get_Ptr() const throw() 
        {
            return p;
        }
        
        // Pointer access
        public: T& operator*() const throw()
        {
            return *p;
        }

        // Unchecked array access 
        public: T& operator[](size_t i) const throw() 
        { 
            return p[i]; 
        }
        
        // Unchecked array access 
        public: T& At(size_t i) const throw()
        { 
            return p[i]; 
        }
        
        // Explicitly release ownership 
        public: T* Release() throw()
        {
            T* t(p);
            p = 0;
            return t;
        }
        
        // Reset value 
        public: void Reset(T* pOther=0) throw()
        {
            if (p != pOther)
            {
                delete [] p;
                p = pOther;
            }
        }
       
        // Assignments (with implicit conversion). Note non-constant parameter 
        public: AutoArrayPtr& operator=(AutoArrayPtr& rhs) throw()
        {
            Reset(rhs.Release());
            return *this;
        }

        // Special conversion with auxilliary type to enable move assignment with temporary objects
        public: AutoArrayPtr& operator=(AutoArrayPtrAux<T> rhs) throw()
        {
            Reset(rhs.p);
            return *this;
        }

        // Casts AutoArrayPtr<T> to AutoArrayPtrAux<Y>
        public: template<class Y> operator AutoArrayPtrAux<Y>() throw() 
        {
            return AutoArrayPtrAux<Y>(Release());
        }
        
        // Casts AutoArrayPtr<T> to AutoArrayPtr<Y>   		
        public: template<class Y> operator AutoArrayPtr<Y>() throw()
        {
            return AutoArrayPtr<Y>(Release());
        }
        
        // Implicit safe-bool implementation
        private: typedef void (AutoArrayPtr::* BoolType)() const;
        private: void BoolTypeFunc() const {}

        public: operator BoolType() const throw()
        {
            return p ? &AutoArrayPtr::BoolTypeFunc : 0;
        }

        public: bool operator== (AutoArrayPtr const & rhs) throw() { return p == rhs.p; }
        public: bool operator!= (AutoArrayPtr const & rhs) throw() { return p != rhs.p; }
    };
}
