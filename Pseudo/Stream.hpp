// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)

#include <Pseudo\Compiler.hpp>
#include <Pseudo\ValueType.hpp>

namespace Pseudo
{
	class Stream
	{
		public: Stream()
		{
		}
		
		public: virtual ~Stream()
		{
		}
		
		public: virtual void Read(Byte* pBuffer, Int offset, Int count) = 0;
		public: virtual void Write(Byte* pBuffer, Int offset, Int count) = 0;
	};
}

#pragma warning(pop)
