// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)

#include <Pseudo\String.hpp>
#include <Pseudo\Array.hpp>
#include <Pseudo\StringBuilder.hpp>

namespace Pseudo
{
	class ASCIIEncoding;
	class UnicodeEncoding;

	class Encoding
	{
		public: Encoding()
		{
		}

		public: virtual String GetString(const Byte* pBytes, Int length) = 0;
		public: virtual void GetBytes(const String& s, Array<Byte>& bytes) = 0;
		public: virtual void GetBytesWithNull(const Char* p, Array<Byte>& bytes) = 0;
	
		public: static Encoding& get_ASCII()
		{
			return (Encoding&)asciiEncoding;
		}
		
		public: static Encoding& get_Unicode()
		{
			return (Encoding&)unicodeEncoding;
		}
		
		public: static ASCIIEncoding asciiEncoding;
		public: static UnicodeEncoding unicodeEncoding;
	};
	
	class ASCIIEncoding : public Encoding
	{
		public: virtual String GetString(const Byte* pBytes, Int length)
		{
			StringBuilder sb(length);
			
			Int i;
			
			for (i = 0; i < length; i++)
				sb.get_Ptr()[i] = pBytes[i];
			
			sb.get_Ptr()[i] = 0;
			
			return sb.ToString();
		}

		public: virtual void GetBytes(const String& s, Array<Byte>& bytes)
		{
			// TODO: Use a real Win32 API for this conversion?
			bytes.set_Count(s.get_Length());
			
			for (Int i = 0; i < s.get_Length(); i++)
				bytes[i] = (s[i] & 0x7F);
		}

		public: virtual void GetBytesWithNull(const Char* p, Array<Byte>& bytes)
		{
			size_t length;
			
			HRESULT hr = StringCchLengthW(p, STRSAFE_MAX_CCH, &length);
			
			if (FAILED(hr))
				throw ArgumentException();

			Int lengthAsInt = static_cast<Int>(length);
			bytes.set_Count(lengthAsInt + 1);
			
			Int i;
			
			for (i = 0; i < lengthAsInt; i++)
				bytes[i] = (p[i] & 0x7F);
			
			bytes[i] = 0;
		}
	};
	
	class UnicodeEncoding : public Encoding
	{
		public: virtual String GetString(const Byte* pBytes, Int length)
		{
			// TODO: Just copy the bytes into the string
			DEBUG_ASSERT(false);
			return String();
		}

		public: virtual void GetBytes(const String& s, Array<Byte>& bytes)
		{
			// Just copy the characters over
			bytes.set_Count(s.get_Length() * sizeof(Char));
			
			for (Int i = 0; i < s.get_Length(); i++)
				*((Char*)bytes.get_Ptr() + i) = s[i];
		}
		
		public: virtual void GetBytesWithNull(const Char* p, Array<Byte>& bytes)
		{
			// TODO: Just copy the characters over
			DEBUG_ASSERT(false);
		}
	};
	
	__declspec(selectany) UnicodeEncoding Encoding::unicodeEncoding;
	__declspec(selectany) ASCIIEncoding Encoding::asciiEncoding;
}

#pragma warning(pop)
