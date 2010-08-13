/*
This is a simple CLR hosting program.  It shows how Pseudo classes can be used to greatly simplify the work
required to write a simple shim program such as this.  The code is more readable and much smaller than 
it would otherwise be. johnls-5/9/2008
*/

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#include <Pseudo\ValueType.hpp>
#include <Pseudo\String.hpp>
#include <Pseudo\\StringBuilder.hpp>
#include <Pseudo\Environment.hpp>
#include <Pseudo\Path.hpp>
#include <Pseudo\File.hpp>
#include <Pseudo\Console.hpp>
#include <Pseudo\ComPtr.hpp>
#include <Pseudo\Exception.hpp>
#include <Pseudo\Trace.hpp>
#include <mscoree.h>
#include <corhlpr.h>
#include <ole2.h>
#include <oleauto.h>
#include <metahost.h>

#import <mscorlib.tlb> raw_interfaces_only high_property_prefixes("get_","put_","putref_") rename("ReportEvent", "ReportEvent2")

#pragma comment(lib, "mscoree.lib")

using namespace Pseudo;

Int _cdecl wmain(Int argc, Char* argv[])
{
    Trace::get_Listeners().Add(new DefaultTraceListener());

    const Char *configExt = L".config";
    Int ret = 0;
    
    // Check that we have a program to run
    if (argc < 5)
    {
        Console::WriteLine(L"usage: clrhost <assemblyName> <typeName> <methodName> <argument>]");
        return 0;
    }
    
    // Find the program to run based in the PATH.  When we find it we have
    // enough info to start the CLR and launch the program in it.
    
    String path;
    
    Environment::GetVariable(L"PATH", path);

    Array<String> dirs;
    
    path.Split(';', dirs);

    String assemblyName(argv[1]);
    String typeName(argv[2]);
    String methodName(argv[3]);
    String argument(argv[4]);
    Int i = 0;
    
    for (; i < dirs.get_Count(); i++)
    {
        if (File::Exists(assemblyName))
        {
            break;
        }
    }
    
    if (i == dirs.get_Count())
    {
        // We didn't find a .config file
        Console::WriteLine(L"error: Could not find '%s' in PATH", (const Char*)assemblyName);
    }

    String assemblyConfig(Path::Combine(dirs[i], configExt));
    
    Trace::WriteLine(String::Format(L"Running '%s'", assemblyName.get_Ptr()));
    Trace::WriteLine(String::Format(L"DEVPATH=%s", Environment::GetVariable(L"DEVPATH").get_Ptr()));

    try
    {
	    // Load the .config and create an IStream for it
	    Array<Byte> config;
	    
	    File::ReadAllBytes(assemblyConfig, config);
	    
	    ComPtr<IStream> piCfgStream;
        HGLOBAL hGlobal = GlobalAlloc(0, config.get_Count());
        
        memcpy((void*)hGlobal, config.get_Ptr(), config.get_Count());

	    ::CreateStreamOnHGlobal(hGlobal, TRUE, (LPSTREAM*)&piCfgStream);
	    
        typedef HRESULT (WINAPI *PFNCLRCREATEINSTANCE)(REFCLSID rclsid, REFIID riid, LPVOID *ppInterface);

        HMODULE hMscoree = LoadLibrary(L"mscoree.dll");

        // Check if any CLR is installed
        if (hMscoree == NULL || hMscoree == INVALID_HANDLE_VALUE)
        {
            Console::WriteLine(L"error: Unable to load mscoree.dll.  .NET must be installed.");
            return -1;
        }

        PFNCLRCREATEINSTANCE pfnCLRCreateInstance = (PFNCLRCREATEINSTANCE)(::GetProcAddress(hMscoree, "CLRCreateInstance"));
        
        if (pfnCLRCreateInstance == NULL)
        {
            Console::WriteLine(L"error: Unable to find correct entry point in mscoree.dll.  .NET 4.0 required.");
            return -1;
        }

        // CLR v4.0 must have been installed at one point.
        ComPtr<ICLRMetaHostPolicy> piClrMetaHostPolicy;
        HRESULT hr = pfnCLRCreateInstance(CLSID_CLRMetaHostPolicy, IID_ICLRMetaHostPolicy, (void**)&piClrMetaHostPolicy);
        
        if (FAILED(hr) || piClrMetaHostPolicy == NULL)
        {
            throw Pseudo::ComException(hr);
        }

        ComPtr<ICLRRuntimeInfo> piClrRuntimeInfo;
        StringBuilder version(128);
        DWORD versionLength = (DWORD)version.get_Capacity();
        StringBuilder imageVersion(128);
        DWORD imageVersionLength = (DWORD)imageVersion.get_Capacity();
        DWORD configFlags = 0;

        THROW_BAD_HRESULT(piClrMetaHostPolicy->GetRequestedRuntime(
            METAHOST_POLICY_HIGHCOMPAT, 
            assemblyName.get_Ptr(),
            piCfgStream, 
            version.get_Ptr(), 
            &versionLength, 
            imageVersion.get_Ptr(), 
            &imageVersionLength, 
            &configFlags,
            IID_ICLRRuntimeInfo, 
            (void**)&piClrRuntimeInfo));

        ComPtr<ICLRRuntimeHost> piClrRuntimeHost;
        
        THROW_BAD_HRESULT(piClrRuntimeInfo->GetInterface(CLSID_CLRRuntimeHost, IID_ICLRRuntimeHost, (void**)&piClrRuntimeHost));

        ComPtr<mscorlib::_AppDomain> piAppDomain;
        DWORD retVal;

        THROW_BAD_HRESULT(piClrRuntimeHost->ExecuteInDefaultAppDomain(
            assemblyName.get_Ptr(),
            typeName.get_Ptr(),
            methodName.get_Ptr(),
            argument.get_Ptr(), 
            &retVal));

        ret = (Int)retVal;
    }
    catch (ComException& e)
    {
        Console::WriteLine(L"error: Unable to start CLR and execute application (0x%x)", e.get_HResult());
        return -1;
    }
    
	return ret;
}