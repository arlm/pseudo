// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)
#include <Pseudo\Compiler.hpp>
#include <Pseudo\Macros.hpp>
#include <Pseudo\String.hpp>
#include <Pseudo\ArrayList.hpp>
#include <Pseudo\Exception.hpp>
#include <Pseudo\AutoArrayPtr.hpp>
#include <Pseudo\CountedArrayPtr.hpp>

namespace Pseudo
{
    class RegistryKey
    {
        friend class Registry;

        public: enum ValueKind
        {
            Binary,
            DWord,
            ExpandString,
            MultiString,
            QWord,
            SimpleString,
            Unknown
        };

        public: enum PermissionCheck
        {
            Default,
            ReadSubTree,
            ReadWriteSubTree
        };
        
        public: enum ValueOptions
        {
            DoNotExpandEnvironmentNames,
            None
        };

        // Registry query information
        private: struct RegKeyQueryInfo
        {
            String className;		    
            UInt numSubKeys;				
            UInt maxSubKeys;				
            UInt maxValues;				
            UInt numValues;				
            UInt maxValueNames;			
            UInt maxValueData;			
            UInt securityDescriptorLen;	
            FILETIME lastWriteTime;		
        };
        
        private: RegistryKey() : hkey(NULL) 
        { 
        }

        private: RegistryKey(HKEY hkey)
        { 
            this->hkey = hkey;
        }

        public: ~RegistryKey()
        {
            Close();
        }

        // Check if the object contains a valid key Handle
        protected: Bool IsValid() const
        {
            return (this->hkey != NULL);
        }

        protected: Bool IsReservedHandle()
        {
            return 
                hkey == HKEY_CLASSES_ROOT ||
                hkey == HKEY_CURRENT_USER ||
                hkey == HKEY_LOCAL_MACHINE ||
                hkey == HKEY_USERS ||
                hkey == HKEY_PERFORMANCE_DATA ||    
                hkey == HKEY_PERFORMANCE_TEXT ||    
                hkey == HKEY_PERFORMANCE_NLSTEXT ||
                hkey == HKEY_CURRENT_CONFIG ||
                hkey == HKEY_DYN_DATA;
        }

        /// Close the internal Handle
        public: void Close()
        {
            if (!IsReservedHandle() && IsValid())
            {
                THROW_BAD_WINERROR(NativeMethods::RegCloseKey(this->hkey));
            }
        }

        public: RegistryKey* CreateSubKey(const Char* subKeyName) const
        {
            HKEY hKey;
            DWORD dwDisp;
            REGSAM sam = KEY_ALL_ACCESS;
            
            THROW_BAD_WINERROR(::RegCreateKeyEx(
                this->hkey,	
                subKeyName,
                0,
                NULL,
                REG_OPTION_NON_VOLATILE,
                sam,
                NULL,
                &hKey,
                &dwDisp));

            return PSEUDO_NEW RegistryKey(hKey);
        }

        public: RegistryKey* OpenSubKey(const Char* subKey, PermissionCheck check) const
        {
            DWORD openError = ERROR_SUCCESS;
            RegistryKey * key = TryOpenSubKey(subKey, check, &openError);
            if (NULL == key)
            {
                DEBUG_ASSERT(ERROR_SUCCESS != openError);
                THROW_BAD_WINERROR(openError);
            }
            return key;
        }

        public: RegistryKey* TryOpenSubKey(const Char* subKey, PermissionCheck check, DWORD * error = NULL) const
        {
            if (error != NULL)
                *error = ERROR_SUCCESS;

            HKEY hkey;
            REGSAM sam;
            
            switch (check)
            {
                case ReadSubTree:
                    sam = KEY_READ;
                    break;
                
                case ReadWriteSubTree:
                    sam = KEY_READ | KEY_WRITE;
                    break;    
                
                case Default:
                default:
                    sam = KEY_ALL_ACCESS;
            }
            
            DWORD openResult = NativeMethods::RegOpenKeyEx(this->hkey, subKey, 0, sam, &hkey);
            if (error != NULL)
                *error = openResult;

            return ERROR_SUCCESS != openResult ? NULL : PSEUDO_NEW RegistryKey(hkey);
        }

        public: void DeleteSubKey(Char const * szSubKey) const
        {
            THROW_BAD_WINERROR(NativeMethods::RegDeleteKey(this->hkey, szSubKey));
        }

        public: void DeleteValue(Char const * szValue) const
        {
            THROW_BAD_WINERROR(NativeMethods::RegDeleteValue(this->hkey, szValue));
        }

        public: ArrayList<String>* GetSubKeyNames()
        {
            Char keyName[256];
            FILETIME lastWrite;
            DWORD keyIndex = 0;
            ArrayList<String>* pKeys = PSEUDO_NEW ArrayList<String>();

            for(;;)
            {
                DWORD keyNameSize = ARRAY_SIZE(keyName);
                
                DWORD error = NativeMethods::RegEnumKeyEx(
                    this->hkey,
                    keyIndex,
                    keyName,
                    &keyNameSize,
                    NULL,
                    NULL,
                    NULL,
                    &lastWrite);

                if (ERROR_SUCCESS == error)
                {
                    keyIndex++;
                }
                else if (ERROR_NO_MORE_ITEMS == error)
                {
                    break;
                }
                else
                {
                    throw Win32Exception(error);
                }

                pKeys->Add(String(keyName));
            }
            
            return pKeys;
        }

        public: ArrayList<String>* GetValueNames()
        {
            const int maxValueNameSize = 256;
            Char valueName[maxValueNameSize];
            DWORD valueIndex = 0;
            ArrayList<String>* pValues = PSEUDO_NEW ArrayList<String>();
            
            for (;;)
            {
                DWORD valueNameSize = maxValueNameSize;
            
                DWORD error = ::RegEnumValue(
                    this->hkey,
                    valueIndex,
                    valueName,
                    &valueNameSize,
                    NULL,
                    NULL,
                    NULL,
                    NULL);

                if (ERROR_SUCCESS == error)
                {
                    valueIndex++;
                }
                else if (ERROR_MORE_DATA == error)
                {
                    // Make the data buffer match the bigger value and try again
                    valueNameSize = maxValueNameSize;

                    error = ::RegEnumValue(
                        this->hkey,
                        valueIndex,
                        valueName,
                        &valueNameSize,
                        NULL,
                        NULL,
                        NULL,
                        NULL);
                    
                    if (error != ERROR_SUCCESS)
                        throw Win32Exception(error);
                        
                    valueIndex++;
                }
                else if (ERROR_NO_MORE_ITEMS == error)
                {
                    break;
                }
                else
                {
                    throw Win32Exception(error);
                }
                
                pValues->Add(String(valueName));
            }
            
            return pValues;
        }

        public: void Flush()
        {
            THROW_BAD_WINERROR(::RegFlushKey(this->hkey));
        }

        public: void QueryValue(const Char* valueName)
        {
            return QueryValueInfo(valueName);
        }

        public: void QueryValue(Char const * valueName, PVOID pData, DWORD& dwDataLen)
        {
            DWORD valueType = 0;
            bool moreData;
            
            QueryValue(valueName, valueType, (LPBYTE)pData, dwDataLen, moreData); 

            if (REG_BINARY != valueType)
                throw Win32Exception(ERROR_INVALID_FUNCTION);
        }

        public: void QueryValue(
            const Char* valueName, String& stringValue, bool autoExpand = true)
        {
            DWORD value_size = 256;
            AutoArrayPtr<Char> pValue(PSEUDO_NEW Char[value_size + 1]);
            DWORD valueType = 0;
            bool moreData;
            
            QueryValue(valueName, valueType, (LPBYTE)pValue.get_Ptr(), value_size, moreData);

            if (REG_SZ != valueType && REG_EXPAND_SZ != valueType && REG_MULTI_SZ != valueType)
                throw Win32Exception(ERROR_INVALID_FUNCTION);

            // Some keys just keep growing...
            while (moreData)
            {
                pValue.Reset(PSEUDO_NEW Char[value_size]);
                
                QueryValue(valueName, valueType, (LPBYTE)pValue.get_Ptr(), value_size, moreData);
            }

            if (REG_EXPAND_SZ == valueType && autoExpand)
            {
                AutoArrayPtr<Char> pNewValue = PSEUDO_NEW Char[value_size];
                DWORD new_value_size = ::ExpandEnvironmentStrings(pValue.get_Ptr(), pNewValue.get_Ptr(), value_size);
                
                // Now we know how much space we really need
                if (new_value_size > value_size)
                {
                    pNewValue.Reset(PSEUDO_NEW Char[new_value_size]);
                    
                    ::ExpandEnvironmentStrings(pNewValue.get_Ptr(), pValue.get_Ptr(), new_value_size);
                }

                pValue = pNewValue;
            }

            //for REG_MULTI_SZ, replace null chars with \n
            if (REG_MULTI_SZ == valueType)
            {
                Char *pChar = pValue.get_Ptr();
                //value_size is in bytes, and we're iterating over WCHARs
                for(DWORD i=0; i<value_size/2-1; i++, pChar++)
                {
                    if((*pChar==L'\0') && !(*(pChar+1)==L'\0'))
                    {
                        *pChar = L'\n';
                    }
                }
            }
            //according to the documentation, "the application should ensure that the string is properly terminated before using it"
            pValue.get_Ptr()[value_size+1] = L'\0';
            stringValue = pValue.get_Ptr();
        }

        public: void QueryValue(Char const * valueName, DWORD& dwValue)
        {
            DWORD value_size = sizeof(DWORD);
            DWORD valueType = 0;
            bool moreData;

            QueryValue(valueName, valueType, (LPBYTE)&dwValue, value_size, moreData);

            if (REG_DWORD != valueType)
                throw Win32Exception(ERROR_INVALID_FUNCTION);
        }

        // Boolean
        public: void SetValue(Char const * valueName)
        {
            SetValue(valueName, REG_NONE, NULL, 0);
        }

        // Binary
        public: void SetValue(Char const * valueName, PVOID pData, DWORD dwDataLen)
        {
            SetValue(valueName, REG_BINARY, (LPBYTE)pData, dwDataLen);
        }

        // String
        public: void SetValue(const Char* pValueName, const Char* pValue)
        {
			Int length;

			HRESULT hr = StringCchLengthW(pValue, STRSAFE_MAX_CCH, (size_t*)&length);
			
			if (FAILED(hr))
				throw ArgumentException(); 
				
            // Lengths of String types must include the null terminator
            DWORD dataSize = ((DWORD)length + 1) * sizeof(Char);
            SetValue(pValueName, REG_SZ, (LPBYTE)pValue, dataSize);
        }

        // DWORD
        public: void SetValue(Char const * valueName, DWORD dwValue)
        {
            SetValue(valueName, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
        }

        public: Bool ValueExists(
            Char const * valueName)
        {
            return ERROR_SUCCESS == ::RegQueryValueEx(this->hkey,valueName,NULL,NULL,NULL,NULL);
        }

        private: void QueryKeyInfo()
        {
            RegKeyQueryInfo rqi;
            
            Char className[256];
            DWORD classNameSize = sizeof(className)/sizeof(Char);

            THROW_BAD_WINERROR(::RegQueryInfoKey(
                this->hkey,
                className,
                &classNameSize,
                NULL,
                (DWORD*)&rqi.numSubKeys,
                (DWORD*)&rqi.maxSubKeys,
                (DWORD*)&rqi.maxValues,
                (DWORD*)&rqi.numValues,
                (DWORD*)&rqi.maxValueNames,
                (DWORD*)&rqi.maxValueData,
                (DWORD*)&rqi.securityDescriptorLen,
                &rqi.lastWriteTime));

            rqi.className = className;
        }

        private: void QueryValueInfo(
            Char const * valueName, 
            DWORD* pdwValueType = NULL,
            DWORD* pdwValueByteSize = NULL)
        {
            THROW_BAD_WINERROR(::RegQueryValueEx(
                this->hkey,
                valueName,
                NULL,
                pdwValueType,
                NULL,
                pdwValueByteSize));
        }

        private: void QueryValue(
            Char const * valueName, 
            DWORD& dwValueType,
            _Out_cap_(dwByteSize) LPBYTE lpData,
            DWORD& dwByteSize,
            bool& moreData)
        {
            DWORD error = ::RegQueryValueEx(
                this->hkey,
                valueName,
                NULL,
                &dwValueType,
                lpData,
                &dwByteSize);
                
            if (error != ERROR_SUCCESS && error != ERROR_MORE_DATA)
                throw Win32Exception(error);
                
            moreData = (error == ERROR_MORE_DATA);
        }

        private: void SetValue(
            Char const * valueName, 
            DWORD valueType, 
            const BYTE* lpData,
            DWORD nDataSize)
        {
            THROW_BAD_WINERROR(::RegSetValueEx(
                this->hkey, valueName, NULL, 
                valueType, lpData, nDataSize));
        }

        // Data
        private: HKEY hkey;
    };
}

#pragma warning(pop)
