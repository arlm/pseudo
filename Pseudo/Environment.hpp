// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)

#include <Pseudo\Compiler.hpp>
#include <Pseudo\String.hpp>
#include <Pseudo\StringBuilder.hpp>

namespace Pseudo
{
	class Environment
	{
		private: Environment()
		{
		}

		public: static String& get_NewLine()
		{
			static String newLine(L"\r\n");
			
			return newLine;
		}
		
		private: static String Environment::newLine;
		
		public: static bool GetVariable(__in_z const Char* pVariable, String& value)
		{
            StringBuilder sb(1024);
			
			// Environment variables won't be longer than 32767 characters (see MSDN) so the cast is OK.
			Int length = (Int)::GetEnvironmentVariableW(pVariable, sb.get_Ptr(), sb.get_Capacity());
			
			if (length > sb.get_Capacity())
			{
				sb.set_Capacity(length);
				length = ::GetEnvironmentVariableW(pVariable, sb.get_Ptr(), sb.get_Capacity());
			}
			
			if (0 == length)
			{
				return false;
			}
			
			value = sb.get_Ptr();
			
			return true;
		}

        public: static String GetVariable(__in_z const Char* pVariable)
        {
            String value;
            
            GetVariable(pVariable, value);
            
            return value;
        }
		
        public: static void SetVariable(__in_z const Char* pVariable, __in_z const Char* pValue)
        {
            ::SetEnvironmentVariableW(pVariable, pValue);
        }
        
		public: static String get_CurrentDirectory()
		{
			StringBuilder sb(MAX_PATH);
		
			::GetCurrentDirectoryW(sb.get_Capacity(), sb.get_Ptr());
			
			return sb.ToString();
		}
		
		public: static String get_ModuleFileName()
		{
			StringBuilder sb(MAX_PATH);
		
			::GetModuleFileName(NULL, sb.get_Ptr(), sb.get_Capacity());
			
			return sb.ToString();
		}
	};
}

#pragma warning(pop)
