// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)
#pragma warning(disable:4482) // nonstandard extension used: enum used in qualified name

#include <Pseudo\Compiler.hpp>
#include <Pseudo\Dictionary.hpp>
#include <Pseudo\Trace.hpp>
#include <Pseudo\Macros.hpp>
#include <Pseudo\Lock.hpp>
#include <Pseudo\RegistryKey.hpp>
#include <Pseudo\String.hpp>
#include <Pseudo\Interlocked.hpp>
#include <Pseudo\CountedPtr.hpp>
#include <Pseudo\Convert.hpp>
#include <Pseudo\Registry.hpp>
#include <unknwn.h>

//
// Macro to help with the laborious task of interface implementation
//
#define BEGIN_COM_IUNKNOWN_IMPL() \
    public: virtual ULONG _stdcall AddRef() { return ComUnknown::AddRef(); } \
    public: virtual ULONG _stdcall Release() { return ComUnknown::Release(); } \
    public: virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppInterface) \
    { \
        HRESULT hr = ComUnknown::QueryInterface(riid, ppInterface); \
        if (SUCCEEDED(hr)) \
            return hr;

#define COM_IUNKNOWN_INTERFACE(iid, iface) \
        else if (riid == iid) \
            *ppInterface = static_cast<iface *>(this);

#define COM_AGGREGATE_INTERFACE(iid, pUnk) \
        else if (riid == iid) \
            return pUnk->QueryInterface(riid, ppInterface);

#define END_COM_IUNKNOWN_IMPL() \
        else \
        { \
            *ppInterface = NULL; \
            return E_NOINTERFACE; \
        } \
        if (ppInterface) \
            static_cast<IUnknown *>(*ppInterface)->AddRef(); \
        return S_OK; \
    }

namespace Pseudo
{
    class ComUnknown : public IUnknown
    {
        public: ComUnknown()
        {
            m_refCount = 0;
        }
        
        public: virtual ~ComUnknown() {}
    
        // IUnknown
        public: virtual ULONG _stdcall AddRef()
        {
            return Interlocked::Increment(&m_refCount);
        }

        public: virtual ULONG _stdcall Release()
        {    
            long refCount;

            refCount = Interlocked::Decrement(&m_refCount);
            if (refCount == 0) 
                delete this;
            
            return refCount;
        }

        public: virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppInterface)
        {    
            #if (defined(_DEBUG) && defined(PSEUDO_DEBUG_QI_TRACE))
            Debug::Trace("ComUnknown: QI for %s", stringFromGuid(riid));
            #endif

            if (riid == IID_IUnknown)
            {
                this->AddRef();
                *ppInterface = static_cast<IUnknown *>(this);	
                return S_OK;
            }
            else
            {
                *ppInterface = NULL;                  
                return E_NOINTERFACE;
            }
        }

        private: long m_refCount;
    };

    //
    // Stuff for COM factory support
    //
    typedef HRESULT __stdcall PFN_DLLGETCLASSOBJECT(REFCLSID rclsid, REFIID riid, void **ppv);

    typedef HRESULT (* PFN_CREATEOBJECT)( REFIID riid, void **ppInterface );

    enum ComThreadingModel
    {
        ModelApartment,
        ModelFree,
        MainModel,
        ModelBoth,
        ModelNeutral	
    };

    struct ComClassRegister
    {	
        const CLSID *pClsid;
        ComThreadingModel model;
        const Char * szProgID;
        int iVersion;				
        const Char * szCoclassDesc;
        PFN_CREATEOBJECT pfnCreateObject;
        Bool bSingleton;
    };

    template <class T> class ComCreateObject
    {
    public:
        static HRESULT CreateObject(REFIID riid, void **ppInterface)
        {
            HRESULT hr = S_OK;
            T* pT = PSEUDO_NEW T();
            
            if (pT == NULL)
                return E_OUTOFMEMORY;
            
            hr = pT->QueryInterface(riid, ppInterface);
            
            if (FAILED(hr))
                delete pT;
            else
                hr = pT->ConstructObject();

            return hr;
        }
        
        STDMETHOD(ConstructObject)()
        {
            return S_OK;
        }
    };
    
    class ComClassFactory : 
        public ComUnknown,
        public IClassFactory
    {
        private: ComClassFactory() : m_punk(NULL) {}
        
        public: ComClassFactory(const ComClassRegister *pCoClass) :
            m_pCoClass(pCoClass), m_punk(NULL) {}

        public: virtual ~ComClassFactory() {}

        // IUnknown overrides
        public: virtual ULONG _stdcall AddRef() { return ComUnknown::AddRef(); }
        public: virtual ULONG _stdcall Release() { return ComUnknown::Release(); }
        public: virtual HRESULT _stdcall QueryInterface( REFIID riid, void **ppInterface)
        {    
            HRESULT hr = ComUnknown::QueryInterface(riid, ppInterface);
            
            if (SUCCEEDED(hr))
                return hr;
            else if ( riid == IID_IClassFactory )
                *ppInterface = static_cast<IClassFactory *>( this );
            else
            {
                *ppInterface = NULL;                  
                return E_NOINTERFACE;
            }
            
            reinterpret_cast<IUnknown *>(*ppInterface)->AddRef();
            
            return S_OK;
        }
        
        // IClassFactory 
        public: virtual HRESULT _stdcall LockServer(BOOL fLock)
        {    
            (void)fLock;
            // We are not required to hook any logic since this is always
            // an in-process server, we define the method for completeness
            return S_OK;
        }

        public: virtual HRESULT _stdcall CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppInterface)
        {	    
            // This class library does not support aggregation
            if (pUnkOuter != NULL)
                return CLASS_E_NOAGGREGATION;

            // If singleton object
            if (m_pCoClass->bSingleton)
            {
                if (m_punk)
                {
                    // Grab the existing one
                    m_punk->AddRef();
                    *ppInterface = (PVOID)m_punk;
                    return S_OK;
                }
                else
                {
                    // Create new object and add to singleton map
                    HRESULT hr = (*m_pCoClass->pfnCreateObject)(riid, ppInterface);
                    
                    if (SUCCEEDED(hr))
                    {
                        m_punk = (IUnknown*)*ppInterface;
                    }
                    
                    return hr;
                }
            }
            else
            {
                // Create new object
                return (*m_pCoClass->pfnCreateObject)(riid, ppInterface);
            }
        }
            
        private: const ComClassRegister *m_pCoClass;		
        private: IUnknown* m_punk;	 // Used for singleton objects
    };

    // Load a library from a CLSID
    inline HRESULT _stdcall LoadLibraryFromCLSID(
        REFCLSID rclsid,
        HINSTANCE* phDll)
    {
        HRESULT hr = S_OK;

        // Convert the clsid to a String so we can find the InprocServer32 in the registry.
        String guidString(Convert::ToString(rclsid));
        String keyString(L"CLSID\\" + guidString + L"\\InprocServer32");
        String str;
        RegistryKey* pKey = NULL;
        
        try
        {
            pKey = Registry::get_ClassesRoot().OpenSubKey(keyString, RegistryKey::PermissionCheck::ReadSubTree);

            pKey->QueryValue(NULL, str);
        }
        catch (Win32Exception& e)
        {
            return e.get_HResult();
        }	

        delete pKey;
        
        // We've got the name of the DLL to load, so load it.
        *phDll = ::LoadLibraryEx((const Char*)str, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
        
        if (*phDll == NULL)
            hr = HRESULT_FROM_WIN32(GetLastError());
        
        return hr;
    }

    // Use to create COM objects without invoking the wrath of COM
    inline HRESULT _stdcall FakeCoCreateInstance(
        REFCLSID rclsid,
        REFIID riid,
        void **ppv)
    {
        HINSTANCE hDll;
        HRESULT hr = LoadLibraryFromCLSID(rclsid, &hDll);

        if (FAILED(hr))
            return hr;
                    
        // We've loaded the DLL, so find the DllGetClassObject function.
        FARPROC func = GetProcAddress(hDll, "DllGetClassObject");

        if (func != NULL)
        {
            // Cool, lets call the function to get a class factory for the rclsid passed in.
            IClassFactory *classFactory;
            PFN_DLLGETCLASSOBJECT *pfnDllGetClassObject = (PFN_DLLGETCLASSOBJECT*)func;

            hr = pfnDllGetClassObject(rclsid, IID_IClassFactory, (void**)&classFactory);

            if (SUCCEEDED(hr))
            {
                // Ask the class factory to Create an instance of the necessary object.
                hr = classFactory->CreateInstance(NULL, riid, ppv);

                // Release that class factory!
                classFactory->Release();
            }
        }
        else
            hr = HRESULT_FROM_WIN32(GetLastError());

        return hr;
    }

    class ComHelp;
    
    class ComHelp
    {
        public: ComHelp() {}
        public: ~ComHelp() {}
        
        private: static Bool RegisterComClass( 
            REFCLSID rclsid,
            __in_z const Char* szDesc,
            __in_z const Char* szProgIDPrefix,
            __in_z const Char* szVersion,				
            __in_z const Char* szClassProgID,	
            __in_z const Char* szThreadingModel,
            __in_z const Char* szModule ) 
        {
            String sCLSID(L"CLSID\\");
            String sIndProgID(szProgIDPrefix);
            String sProgID;

            // format the prog ID values.
            sIndProgID += String(L".") + szClassProgID;
            sProgID = sIndProgID + L"." + szVersion;

            String sGuid(Convert::ToString(rclsid));
            
            sCLSID += sGuid;

            const RegistryKey& rkRoot = Registry::get_ClassesRoot();
            RegistryKey* prk1 = NULL;
            RegistryKey* prk2 = NULL;

            try 
            {
                // Create ProgID keys
                prk1 = rkRoot.CreateSubKey(sProgID);
                prk1->SetValue(NULL, szDesc);
                prk2 = prk1->CreateSubKey(L"CLSID");
                prk2->SetValue(NULL, sGuid);
                delete prk1;
                delete prk2;

                // Create VersionIndependentProgID keys.
                prk1 = rkRoot.CreateSubKey(sIndProgID);
                prk1->SetValue(NULL, szDesc);
                prk2 = prk1->CreateSubKey(L"CLSID");
                prk2->SetValue(NULL, sGuid);
                delete prk1;
                delete prk2;

                // Create entries under CLSID.
                prk1 = rkRoot.CreateSubKey(sCLSID);
                prk1->SetValue(NULL, szDesc);
                prk2 = prk1->CreateSubKey(L"ProgID");
                prk2->SetValue(NULL, sProgID);
                delete prk2;
                prk2 = prk1->CreateSubKey(L"VersionIndependentProgID");
                prk2->SetValue(NULL, sIndProgID);
                delete prk2;
                delete prk1->CreateSubKey(L"NotInsertable");
                prk2 = prk1->CreateSubKey(L"InprocServer32");
                prk2->SetValue(NULL, szModule);
                prk2->SetValue(L"ThreadingModel", szThreadingModel);
                delete prk2;
            }
            catch (Win32Exception& e)
            {
                // If anything goes wrong, clean-up
                (void)e;
                Trace::WriteLine(String::Format(L"error %u creating registry entries", (const Char*)e.get_Message()));
                
                UnregisterComClass(rclsid, szProgIDPrefix, szVersion, szClassProgID);
                return false;
            }
            
            return true;
        }
                
        public: static Bool UnregisterComClass( 
            REFCLSID rclsid,		   
            __in_z const Char* szProgIDPrefix,
            __in_z const Char* szVersion,			  
            __in_z const Char* szClassProgID)
        {
            String sCLSID(L"CLSID\\");
            String sIndProgID(szProgIDPrefix);
            String sProgID;

            // format the prog ID values.
            sIndProgID += String(L".") + szClassProgID;
            sProgID = sIndProgID + (Char)'.' + szVersion;

            String sGuid(Convert::ToString(rclsid));
            
            sCLSID += sGuid;

            const RegistryKey& rkRoot = Registry::get_ClassesRoot();
            RegistryKey* prk = NULL;

            try
            {
                // delete ProgID keys
                prk = rkRoot.OpenSubKey(sProgID, RegistryKey::PermissionCheck::ReadWriteSubTree);
                
                TRY_CATCH_WIN32EXCEPTION(prk->DeleteSubKey(L"CLSID"))
                
                delete prk;
                
                rkRoot.DeleteSubKey(sProgID);
            }
            catch (Win32Exception&) {}

            // delete VersionIndependentProgID keys.
            try
            {
                prk = rkRoot.OpenSubKey(sIndProgID, RegistryKey::PermissionCheck::ReadWriteSubTree);
                
                TRY_CATCH_WIN32EXCEPTION(prk->DeleteSubKey(L"CLSID"));
                
                delete prk;
                
                rkRoot.DeleteSubKey(sIndProgID);
            }
            catch (Win32Exception&) {}

            try
            {
                // delete entries under CLSID.
                prk = rkRoot.OpenSubKey(sCLSID, RegistryKey::PermissionCheck::ReadWriteSubTree);
            
                TRY_CATCH_WIN32EXCEPTION(prk->DeleteSubKey(L"ProgID"));
                TRY_CATCH_WIN32EXCEPTION(prk->DeleteSubKey(L"VersionIndependentProgID"));
                TRY_CATCH_WIN32EXCEPTION(prk->DeleteSubKey(L"NotInsertable"));
                TRY_CATCH_WIN32EXCEPTION(prk->DeleteSubKey(L"InprocServer32"));
                
                delete prk;
                
                rkRoot.DeleteSubKey(sCLSID);
            }
            catch (Win32Exception&){}
            
            return true;
        }
    
        public: void Initialize(
            HINSTANCE hinst,
            const Char * szProgIDPrefix,
            const ComClassRegister* pCoClasses)
        {
            this->hinst = hinst;
            this->progIDPrefix = szProgIDPrefix;
            this->pCoClasses = pCoClasses;
        }

        public: static HRESULT DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID FAR *ppv)
        {
            Lock::Auto lockAuto(comHelp.lock);

            // Search for the right class factory
            for (const ComClassRegister *pCoClass = ComHelp::comHelp.pCoClasses; 
                pCoClass->pClsid != NULL; pCoClass++)
            {
                if (*pCoClass->pClsid == rclsid)
                {
                    CountedPtr<ComClassFactory> classFactory;
                    
                    // If the class factory doesn't exist, add a new one
                    if (!comHelp.factory.TryGetValue((IntPtr)pCoClass, classFactory))
                    {
                        ComClassFactory* pClassFactory = PSEUDO_NEW ComClassFactory(pCoClass);
                        
                        classFactory = CountedPtr<ComClassFactory>(pClassFactory);
                        
                        comHelp.factory.set_Item((IntPtr)pCoClass, classFactory);
                                                
                        // Prevent object destruction until shutdown
                        reinterpret_cast<IUnknown *>(pClassFactory)->AddRef();
                    }

                    return classFactory->QueryInterface(riid, ppv);
                }
            }
                
            return CLASS_E_CLASSNOTAVAILABLE;
        }

        public: static HRESULT DllRegisterServer()
        {    
            static const Char * threadModels[] = 
            {
                L"Apartment",
                L"Free",
                L"Main",
                L"Both",
                L"Neutral"	
            };
            HRESULT hr = S_OK;
            Char szModule[_MAX_PATH];	
            
            hr = DllUnregisterServer();
            
            if (FAILED(hr))
            {
                return hr;
            }
            
            GetModuleFileName(comHelp.hinst, szModule, ARRAY_SIZE(szModule));

            Bool b = true;
            
            // for each item in the coclass ArrayList, register it
            for (const ComClassRegister *pCoClass = comHelp.pCoClasses; 
                (SUCCEEDED(hr) && (pCoClass->pClsid != NULL)); pCoClass++)
            {
                int model = (int)pCoClass->model;
                
                DEBUG_ASSERT(model >= 0 && model < ARRAY_SIZE(threadModels));
                
                if (model < 0 || model > ARRAY_SIZE(threadModels) - 1)
                    return E_FAIL;
                
                // register the class with default values
                b &= RegisterComClass( 
                    *pCoClass->pClsid, 
                    pCoClass->szCoclassDesc, 
                    comHelp.progIDPrefix,
                    (const Char*)Convert::ToString(pCoClass->iVersion), 
                    pCoClass->szProgID, 
                    threadModels[model], 
                    szModule);      	        
            }

            return b ? S_OK : E_FAIL;
        }
    
        public: static HRESULT DllUnregisterServer()
        {    
            Bool b = true;
            
            // for each item in the coclass ArrayList, unregister it
            for (const ComClassRegister *pCoClass = comHelp.pCoClasses; 
                pCoClass->pClsid != NULL; pCoClass++ )
            {
                b &= UnregisterComClass(
                    *pCoClass->pClsid, 
                    comHelp.progIDPrefix,
                    (const Char*)String::Format(L"%d", pCoClass->iVersion), 
                    pCoClass->szProgID );
            }
                
            return b ? S_OK : E_FAIL;
        }

        public: friend ComHelp& _stdcall GetComHelp();

        private: typedef Dictionary<IntPtr, CountedPtr<ComClassFactory> > FactoryCollection;

        private: static ComHelp comHelp;

        private: Lock lock;
        private: HINSTANCE hinst;
        private: String progIDPrefix;
        private: const ComClassRegister* pCoClasses;
        private: FactoryCollection factory; 
    };

    inline ComHelp& _stdcall GetComHelp()
    { 
        return ComHelp::comHelp; 
    }

    _declspec(selectany) ComHelp ComHelp::comHelp;
};

#pragma warning(pop)
