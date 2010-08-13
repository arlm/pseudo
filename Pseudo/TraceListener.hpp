// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)

#include <Pseudo\Compiler.hpp>
#include <Pseudo\Macros.hpp>
#include <Pseudo\ValueType.hpp>

namespace Pseudo
{
	class TraceListener
	{
		public: TraceListener()
		{
		}
		
		public: virtual void Write(const Char *pMessage) = 0;
		public: virtual void WriteLine(const Char*pMessage) = 0;
	};

	class DefaultTraceListener : public TraceListener
	{
		public: DefaultTraceListener()
		{
		}
		
		public: virtual void Write(__in_z const Char *pMessage)
		{
			::OutputDebugStringW(pMessage);
		}

		public: virtual void WriteLine(__in_z const Char* pMessage)
		{
            Write(pMessage);
            // TODO-johnls-2/6/2008: Get this from Environment?
            Write(L"\r\n");
		}
	};
}

#pragma warning(pop)
