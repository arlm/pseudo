// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)

#include <Pseudo\Compiler.hpp>
#include <Pseudo\ValueType.hpp>
#include <Pseudo\StreamWriter.hpp>
#include <Pseudo\FileStream.hpp>
#include <limits.h>

namespace Pseudo
{
	class File
	{
		private: File()
		{
		}

		public: static StreamWriter* CreateText(const Char* path)
		{
			return PSEUDO_NEW StreamWriter(path);
		}
		
		public: static void ReadAllBytes(const Char* path, Array<Byte>& data)
		{
		    FileStream fs(path, FileMode::Open);
		    
		    Long length = fs.get_Length();

            if (length > INT_MAX)
                throw ArgumentException(L"File is too large to read into memory.");

            data.set_Count((Int)length);
		    
		    fs.Read(data.get_Ptr(), 0, (Int)length);
		}

		public: static bool Exists(String const & fileName)
		{
			// TODO-johnls-2/7/2008: This might not work in all circumstances; look at how the CLR does it for the full implementation
			return (::GetFileAttributes(fileName) != INVALID_FILE_ATTRIBUTES);
		}
	};
}

#pragma warning(pop)

