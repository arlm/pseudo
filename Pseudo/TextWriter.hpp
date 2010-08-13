// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#ifndef __PSEUDO_TEXT_WRITER_HPP__
#define __PSEUDO_TEXT_WRITER_HPP__

#pragma warning(push)

#include <Pseudo\ValueType.hpp>
#include <Pseudo\String.hpp>

namespace Pseudo
{
	/// <summary>
	/// TextWriter for writing encoded text
	/// </summary>
	class TextWriter
	{
		private: 
		
		public: TextWriter()
		{
		}

		public: virtual ~TextWriter()
		{
		}
		
		public: virtual void Write(Char c) = 0;
		public: virtual void Write(const String& s) = 0;
		public: virtual void Write(const Char* p, ...) = 0;
		public: virtual void WriteLine() = 0;
		public: virtual void WriteLine(Char c) = 0;
		public: virtual void WriteLine(const String& s) = 0;
		public: virtual void WriteLine(const Char* p, ...) = 0;
	};
}

#pragma warning(pop)

#endif // __PSEUDO_TEXT_WRITER_HPP__
