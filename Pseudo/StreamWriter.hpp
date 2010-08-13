// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)

#include <Pseudo\ValueType.hpp>
#include <Pseudo\TextWriter.hpp>
#include <Pseudo\Stream.hpp>
#include <Pseudo\Encoding.hpp>
#include <Pseudo\Environment.hpp>

namespace Pseudo
{
	/// <summary>
	/// StreamWriter for writing encoded text
	/// </summary>
	class StreamWriter : public TextWriter
	{
		private: Stream* pStream;
		private: Encoding* pEncoding;
		private: String newLine;
		private: Array<Byte> newLineBytes;
		
		public: StreamWriter(const Char* pFileName)
		{
			DEBUG_ASSERT(false);
		}

		public: StreamWriter(Stream* pStream)
		{
			this->pStream = pStream;
			newLine = Environment::get_NewLine();
			pEncoding = &Encoding::get_ASCII();
		}

		public: StreamWriter(Stream* pStream, Encoding& encoding)
		{
			this->pStream = pStream;
			newLine = Environment::get_NewLine();
			pEncoding = &encoding;
		}

		public: virtual ~StreamWriter()
		{
			delete pStream;
		}
		
		public: virtual void Write(Char c)
		{
		    Write(String(c, 1));
		} 

		public: virtual void Write(const String& s)
		{
			Array<Byte> bytes;
			
			pEncoding->GetBytes(s, bytes);
		
			pStream->Write(bytes.get_Ptr(), 0, bytes.get_Count());
		} 

		public: virtual void Write(const Char* p, ...)
		{
			va_list argList;
			va_start(argList, p);

			Write(String::Format(p, argList));
		} 
		
		public: virtual void WriteLine()
		{
			WriteNewLine();
		} 

		public: virtual void WriteLine(Char c)
		{
		    WriteLine(String(c, 1));
		} 

		public: virtual void WriteLine(const Char* p, ...)
		{
			va_list argList;
			va_start(argList, p);

			WriteLine(String::Format(p, argList));
		} 

		public: virtual void WriteLine(const String& s)
		{
            Write(s);
            WriteNewLine();
		}
		
		private: void WriteNewLine()
		{
			if (newLineBytes.get_Count() == 0)
			{
				pEncoding->GetBytes(newLine, newLineBytes);
			}
			
			pStream->Write(newLineBytes.get_Ptr(), 0, newLineBytes.get_Count());
		}
	};
}

#pragma warning(pop)
