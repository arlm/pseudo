// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)

#include <Pseudo\ValueType.hpp>
#include <Pseudo\Exception.hpp>
#include <Pseudo\Stream.hpp>
#include <winerror.h>

namespace Pseudo
{
	enum FileMode
	{
		Append,
		Create,
		CreateNew,
		Open,
		OpenOrCreate,
		Truncate
	};

	class FileStream : public Stream
	{
		private: HANDLE handle;

		public: FileStream(HANDLE handle, FileMode mode)
		{
			this->handle = handle;
		}
		
		public: FileStream(const Char* path, FileMode mode) : handle(INVALID_HANDLE_VALUE)
		{
			DWORD attrFlags = FILE_ATTRIBUTE_NORMAL;
			DWORD modeFlags;
			
			switch (mode)
			{
			    case Create:
			        modeFlags = CREATE_ALWAYS;
			        break;
			        
			    case CreateNew:
			        modeFlags = CREATE_NEW;
			        break;

			    case Open:
			        modeFlags = OPEN_EXISTING;
			        break;
			        
			    case OpenOrCreate:
			        modeFlags = OPEN_ALWAYS;
			        break;

			    case Truncate:
			        modeFlags = TRUNCATE_EXISTING;
			        break;

			    case Append:
			        DEBUG_ASSERT(false); // Not implemented;
			        break;
			}
		
			HANDLE handle = ::CreateFile(
				path, GENERIC_READ | GENERIC_WRITE, 0, NULL, modeFlags, attrFlags, NULL);

			if (handle != INVALID_HANDLE_VALUE)
			{
				this->handle = handle;
			}
			else
			{
				throw IOException(String::Format(L"File error (0x%08X)", GetLastError()));
			}
		}
		
		public: FileStream(const FileStream& from)
		{
			// TODO-johnls-2/7/2008: Should be done with DuplicateHandle!
			this->handle = from.handle;
		}

		public: virtual ~FileStream()
		{
		    if (get_IsValid())
		    {
		        ::CloseHandle(this->handle);
		        this->handle = INVALID_HANDLE_VALUE;
		    }
		}

        public: bool get_IsValid()
        {
            return (this->handle != INVALID_HANDLE_VALUE);
        }

        public: Long get_Length()
        {
            DEBUG_ASSERT(get_IsValid());
            
            Long fileSize;
            
            if (!::GetFileSizeEx(handle, (PLARGE_INTEGER)&fileSize))
                throw Win32Exception(L"Unable to get file size", GetLastError());
            
            return fileSize;
        }

        public: virtual void Read(Byte* pBuffer, Int offset, Int count)
        {
            DEBUG_ASSERT(get_IsValid());
            
            DWORD read;
            
            ::ReadFile(handle, pBuffer + offset, count, &read, NULL);
        }

		public: virtual void Write(Byte* pBuffer, Int offset, Int count)
		{
            DEBUG_ASSERT(get_IsValid());
            
			DWORD written;
			
			::WriteFile(handle, pBuffer + offset, count, &written, NULL);
		}
	};
}

#pragma warning(pop)
