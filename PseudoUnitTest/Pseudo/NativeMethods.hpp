#pragma once

// Native method wrappers for use in the Pseudo unit tests.

namespace Pseudo
{
    class NativeMethods
    {
        public: enum TestMode
        {
            Default,
            AlwaysSucceed,
            AlwaysFail
        };
        
        private: static TestMode mode;
    
        public: static void set_TestMode(TestMode mode)
        {
            NativeMethods::mode = mode;
        }
        
        public: static LONG WINAPI RegOpenKeyEx(_In_ HKEY hKey, _In_ LPCTSTR lpSubKey, DWORD ulOptions, _In_ REGSAM samDesired, _Out_ PHKEY phkResult)
        {
            if (mode == AlwaysSucceed)
                return ERROR_SUCCESS;
                
            return ::RegOpenKeyEx(hKey, lpSubKey, ulOptions, samDesired, phkResult);
        }
        
        public: static LONG WINAPI RegDeleteKey(_In_ HKEY hKey, _In_ LPCTSTR lpSubKey)
        {
            if (mode == AlwaysSucceed)
                return ERROR_SUCCESS;
                
            return ::RegDeleteKey(hKey, lpSubKey);
        }

        public: static LONG WINAPI RegDeleteValue(_In_ HKEY hKey, _In_ LPCTSTR lpValueName)
        {
            if (mode == AlwaysSucceed)
                return ERROR_SUCCESS;
                
            return ::RegDeleteValue(hKey, lpValueName);
        }

        public: static LONG WINAPI RegEnumKeyEx(_In_ HKEY hKey, _In_ DWORD dwIndex, _Out_ LPTSTR lpName, _Inout_ LPDWORD lpcName, LPDWORD lpReserved, _Inout_ LPTSTR lpClass, _Inout_ LPDWORD lpcClass, _Out_ PFILETIME lpftLastWriteTime)
        {
            if (mode == AlwaysSucceed)
                return ERROR_SUCCESS;
                
            return ::RegEnumKeyEx(hKey, dwIndex, lpName, lpcName, lpReserved, lpClass, lpcClass, lpftLastWriteTime); 
        }
        
        public: static LONG WINAPI RegCloseKey(__in HKEY hKey)
        {
            if (mode == AlwaysSucceed)
                return ERROR_SUCCESS;
                
            return ::RegCloseKey(hKey);
        }
    };
    
    __declspec(selectany) NativeMethods::TestMode NativeMethods::mode;
}
