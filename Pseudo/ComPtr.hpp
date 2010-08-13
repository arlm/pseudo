// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)
#include <unknwn.h>
#include <Pseudo\Compiler.hpp>
#include <Pseudo\Exception.hpp>

namespace Pseudo
{
    // This is used to hide AddRef and Release on interfaces that ComPtr wraps
    template <class T>class NoAddRefReleaseOnComPtr : public T
    {
	    private: virtual ULONG _stdcall AddRef() = 0;
		private: virtual ULONG _stdcall Release() = 0;
    };

    template <class T> class ComPtr
    {
        public: ComPtr()
        {
            p = NULL;
        }
        
        public: ComPtr(ComPtr<T> const &other) 
        {
            // A simplification of Assign()
            p = other.p;
            
            if (p != NULL)
                p->AddRef();
        }

        // Construction from ComPtr<TOther> which implements interface T
        public: template <typename TOther> ComPtr(ComPtr<TOther> const &other) 
        {
            // A simplification of AssignWithQI()
	        if (other)
		        other.QueryInterface(&p);
		    else
		        p = NULL;
        }
    
        public: ComPtr(T* p)
        {
            this->p = p;
            
            if (this->p != NULL)
                this->p->AddRef();
        }

        public: ~ComPtr()
        {
            if (p)
                p->Release();
        }
        
        public: operator T*() const
        {
            return p;
        }
        
        public: T& operator*() const
        {
            if (p == NULL)
                throw ArgumentException();
                
            return *p;
        }
        
        public: T** operator&()
        {
            // If this assert fires you should be taking the address of the p member explicitly
            DEBUG_ASSERT(p == NULL);
            
            return &p;
        }

	    NoAddRefReleaseOnComPtr<T>* operator->() const
	    {
		    DEBUG_ASSERT(p != NULL);
		    return (NoAddRefReleaseOnComPtr<T>*)p;
	    }
        
        public: Bool operator!() const
        {
            return (p == NULL);
        }
        
        public: Bool operator<(__in_opt T* pT) const
        {
            return p < pT;
        }
        
        public: Bool operator!=(__in_opt T* pT) const
        {
            return !operator==(pT);
        }
        
        public: Bool operator==(__in_opt T* pT) const
        {
            return p == pT;
        }

        // Assignment from interface T
        public: T* operator=(__in_opt T* lp)
        {
            if (*this!=lp)
            {
                return static_cast<T*>(Assign((IUnknown**)&p, lp));
            }
            else
            {
                return *this;
            }
        }

        // Assignment from another ComPtr<T>         
        public: T* operator=(__in const ComPtr<T>& lp)
        {
            if (*this != lp)
            {
                return static_cast<T*>(Assign((IUnknown**)&p, lp));
            }
            else
            {
                return *this;
            }
        }

        // Asignment from a ComPtr<Q> which implements interface T
        public: template <typename Q> T* operator=(__in const ComPtr<Q>& lp)
        {
            if (!Equals(lp))
            {
                return static_cast<T*>(AssignWithQI((IUnknown**)&p, lp, __uuidof(T)));
            }
            else
            {
                return *this;
            }
        }

        // Release the interface and set to NULL
        public: void Release()
        {
            T* pTemp = p;
            
            if (pTemp)
            {
                p = NULL;
                pTemp->Release();
            }
        }
        
        // Compare two objects for equivalence
        public: Bool Equals(__in_opt IUnknown* pOther)
        {
            if (p == NULL && pOther == NULL)
                return true;	// They are both NULL objects

            if (p == NULL || pOther == NULL)
                return false;	// One is NULL the other is not

            ComPtr<IUnknown> punk1;
            ComPtr<IUnknown> punk2;
            p->QueryInterface(__uuidof(IUnknown), (void**)&punk1);
            pOther->QueryInterface(__uuidof(IUnknown), (void**)&punk2);
            
            return punk1 == punk2;
        }
        
        // Attach to an existing interface (does not AddRef)
        public: void Attach(__in_opt T* p2)
        {
            if (p)
                p->Release();
            
            p = p2;
        }
        
        // Detach the interface (does not Release)
        public: T* Detach()
        {
            T* pt = p;
            
            p = NULL;
            
            return pt;
        }
        
        public: __checkReturn HRESULT CopyTo(__deref_out_opt T** ppT)
        {
            DEBUG_ASSERT(ppT != NULL);
            
            if (ppT == NULL)
                return E_POINTER;
                
            *ppT = p;
            
            if (p)
                p->AddRef();
                
            return S_OK;
        }
        
        public: __checkReturn HRESULT CoCreateInstance(__in REFCLSID rclsid, __in_opt LPUNKNOWN pUnkOuter = NULL, __in DWORD dwClsContext = CLSCTX_ALL)
        {
            DEBUG_ASSERT(p == NULL);
            return ::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, __uuidof(T), (void**)&p);
        }
        
        public: __checkReturn HRESULT CoCreateInstance(__in LPCOLESTR szProgID, __in_opt LPUNKNOWN pUnkOuter = NULL, __in DWORD dwClsContext = CLSCTX_ALL)
        {
            CLSID clsid;
            HRESULT hr = ::CLSIDFromProgID(szProgID, &clsid);
            
            DEBUG_ASSERT(p == NULL);
            
            if (SUCCEEDED(hr))
                hr = ::CoCreateInstance(clsid, pUnkOuter, dwClsContext, __uuidof(T), (void**)&p);
                
            return hr;
        }
        
        public: template <class Q> __checkReturn HRESULT QueryInterface(__deref_out_opt Q** pp) const
        {
            DEBUG_ASSERT(pp != NULL);
            return p->QueryInterface(__uuidof(Q), (void**)pp);
        }

        public: static IUnknown* Assign(__deref_out_opt IUnknown** pp, __in_opt IUnknown* lp)
        {
	        if (pp == NULL)
		        return NULL;
        		
	        if (lp != NULL)
		        lp->AddRef();
    	    
	        if (*pp)
		        (*pp)->Release();
    	    
	        *pp = lp;
    	    
	        return lp;
        }

        public: static IUnknown* _stdcall AssignWithQI(__deref_out_opt IUnknown** pp, __in_opt IUnknown* lp, REFIID riid)
        {
	        if (pp == NULL)
		        return NULL;

	        IUnknown* pTemp = *pp;
	        *pp = NULL;
    	    
	        if (lp != NULL)
		        lp->QueryInterface(riid, (void**)pp);
    	    
	        if (pTemp)
		        pTemp->Release();
    	    
	        return *pp;
        }
        
        private: T* p;
    };
}

#pragma warning(pop)
