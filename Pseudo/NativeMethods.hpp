#pragma once

namespace Pseudo
{
    class NativeMethods
    {
        public: static LONG WINAPI RegOpenKeyEx(_In_ HKEY hKey, _In_ LPCTSTR lpSubKey, DWORD ulOptions, _In_ REGSAM samDesired, _Out_ PHKEY phkResult)
        {
            return ::RegOpenKeyEx(hKey, lpSubKey, ulOptions, samDesired, phkResult);
        }
        
        public: static LONG WINAPI RegDeleteKey(_In_ HKEY hKey, _In_ LPCTSTR lpSubKey)
        {
            return ::RegDeleteKey(hKey, lpSubKey);
        }

        public: static LONG WINAPI RegDeleteValue(_In_ HKEY hKey, _In_ LPCTSTR lpValueName)
        {
            return ::RegDeleteValue(hKey, lpValueName);
        }

        public: static LONG WINAPI RegEnumKeyEx(_In_ HKEY hKey, _In_ DWORD dwIndex, _Out_ LPTSTR lpName, _Inout_ LPDWORD lpcName, LPDWORD lpReserved, _Inout_ LPTSTR lpClass, _Inout_ LPDWORD lpcClass, _Out_ PFILETIME lpftLastWriteTime)
        {
            return ::RegEnumKeyEx(hKey, dwIndex, lpName, lpcName, lpReserved, lpClass, lpcClass, lpftLastWriteTime); 
        }
        
        public: static LONG WINAPI RegCloseKey(__in HKEY hKey)
        {
            return ::RegCloseKey(hKey);
        }
    };
}
