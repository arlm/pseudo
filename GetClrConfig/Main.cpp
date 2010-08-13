/*
This program tries to generate a .config file that targets the highest installed version of the CLR on a system.
*/

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#include <Pseudo\ValueType.hpp>
#include <Pseudo\String.hpp>
#include <Pseudo\Environment.hpp>
#include <Pseudo\Path.hpp>
#include <Pseudo\File.hpp>
#include <Pseudo\Console.hpp>
#include <Pseudo\Exception.hpp>
#include <Pseudo\Trace.hpp>
#include <Pseudo\ArrayList.hpp>
#include <Pseudo\ComPtr.hpp>
#include <ObjIdl.h>
#include <metahost.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "version.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "advapi32.lib")

using namespace Pseudo;

typedef HRESULT ( __stdcall *CallbackThreadSetFnPtr )( void);

typedef HRESULT ( __stdcall *CallbackThreadUnsetFnPtr )( void);

typedef void ( __stdcall *RuntimeLoadedCallbackFnPtr )( 
    ICLRRuntimeInfo *pRuntimeInfo,
    CallbackThreadSetFnPtr pfnCallbackThreadSet,
    CallbackThreadUnsetFnPtr pfnCallbackThreadUnset);

static String GetFileVersion(String filePath)
{
    VS_FIXEDFILEINFO* pFixedFileInfo;
    DWORD handle;
    DWORD infoSize = ::GetFileVersionInfoSize(filePath, &handle);

    if (0 == infoSize)
        throw ArgumentException();

    AutoArrayPtr<Byte> data(new Byte[infoSize]);

    if (!::GetFileVersionInfo(filePath, 0, infoSize, data.get_Ptr()))
        throw Exception();

    UInt bufLen;
        
    if (0 == VerQueryValue(data.get_Ptr(), L"\\", (void**)&pFixedFileInfo, &bufLen))
        throw Exception();

    if (0 == bufLen)
        throw Exception();

    String version;
    
    return String::Format(L"%d.%d.%d",
        HIWORD(pFixedFileInfo->dwFileVersionMS),
        LOWORD(pFixedFileInfo->dwFileVersionMS),
        HIWORD(pFixedFileInfo->dwFileVersionLS));
}

// Returns an array of version numbers of the installed runtimes.
static void GetInstalledRuntimes(ArrayList<String>& versions)
{
    typedef HRESULT (WINAPI *PFNCLRCREATEINSTANCE)(REFCLSID rclsid, REFIID riid, LPVOID *ppInterface);
    typedef HRESULT (WINAPI *PFNLOADLIBRARYSHIM)(LPCWSTR szDllName, LPCWSTR szVersion, LPVOID pvReserved, HMODULE *phModDll);
    HMODULE hMscoree = NULL;
    HRESULT hr;
    
    hMscoree = LoadLibrary(L"mscoree.dll");

    // Check if any CLR is installed
    if (hMscoree != NULL && hMscoree != INVALID_HANDLE_VALUE)
    {
        PFNCLRCREATEINSTANCE pfnCLRCreateInstance = (PFNCLRCREATEINSTANCE)(::GetProcAddress(hMscoree, "CLRCreateInstance"));
        
        if (pfnCLRCreateInstance != NULL)
        {
            // CLR v4.0 must have been installed at one point.  We can use it to enumerate the runtimes
            ComPtr<ICLRMetaHost> pMetaHost;
            
            if (SUCCEEDED(pfnCLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (void**)&pMetaHost)) && pMetaHost)
            {
                ComPtr<IEnumUnknown> pEnum;
                
                if (SUCCEEDED(pMetaHost->EnumerateInstalledRuntimes(&pEnum)) && pEnum)
                {
                    ComPtr<IUnknown> pUnk;
                
                    while (pEnum->Next(1, &pUnk, NULL) == S_OK)
                    {
                        ComPtr<ICLRRuntimeInfo> pRuntimeInfo(pUnk);
                        
                        if (pRuntimeInfo)
                        {
                            StringBuilder version(64);
                            DWORD count = version.get_Capacity();
                            
                            pRuntimeInfo->GetVersionString(version.get_Ptr(), &count);
                            
                            versions.Add(version.ToString());
                        }
                        
                        pUnk.Release();
                    }
                }
            }
        }            
        else
        {
            // We need to use the old LoadLibraryShim method 
            PFNLOADLIBRARYSHIM pfnLoadLibraryShim = (PFNLOADLIBRARYSHIM)(GetProcAddress(hMscoree, "LoadLibraryShim"));
            
            if (pfnLoadLibraryShim != NULL)
            {
                HINSTANCE hMscorjit = NULL;
                
                // Check for CLR v1.x and v2.0 jit assembly
                // TODO-johnls-1/26/2009: Do this for other runtime versions
                hr = pfnLoadLibraryShim(L"mscorjit.dll", NULL, NULL, &hMscorjit);
                
                if (SUCCEEDED(hr))
                {
                    StringBuilder fileNameBuilder(MAX_PATH);
                    
                    ::GetModuleFileName(hMscorjit, (Char*)fileNameBuilder, fileNameBuilder.get_Capacity());
                    versions.Add(GetFileVersion(fileNameBuilder.ToString()));
                    ::FreeLibrary(hMscorjit);
                }
            }
        }

        ::FreeLibrary(hMscoree);
    }
}

Int _cdecl wmain(Int argc, Char* argv[]) 
{
    if (argc != 2)
    {
        Console::get_Error()->WriteLine(L"GetClrConfig <Major.minor>");
        Console::get_Error()->WriteLine();
        Console::get_Error()->WriteLine(L"Generates a .exe.config file containing the full version number of the given Major.minor installation of the CLR, i.e. Major.minor.build");
        return -1;
    }
    
    String version(L"vM.m.b");
    String majorMinor(argv[1]);

    majorMinor = majorMinor.ToLower();
    
    if (!majorMinor.StartsWith(L"v"))
        majorMinor = L"v" + majorMinor;

    ArrayList<String> versions;
    
    GetInstalledRuntimes(versions);
    
    int i = 0;
    
    for (; i < versions.get_Count(); i++)
    {
        if (versions[i].StartsWith(majorMinor))
        {
            version = versions[i];
            break;
        }
    }

    if (i == versions.get_Count())
    {
        Console::WriteLine(L"ERROR: Unable to get version information for CLR %s", majorMinor.get_Ptr());
        return -1;
    }
    else
    {
        Console::WriteLine(
            L"<configuration>\r\n"
            L"  <startup>\r\n"
            L"    <supportedRuntime version=\"%s\"/>\r\n"
            L"  </startup>\r\n"
            L"</configuration>\r\n", 
                version.get_Ptr());

        return 0;
    }
}

