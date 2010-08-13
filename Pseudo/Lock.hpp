// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#include <Pseudo\Compiler.hpp>
#include <Pseudo\Exception.hpp>

namespace Pseudo
{
	class Lock
	{
	    public: Lock(int spinCount = 0)
		{
			if (!InitializeCriticalSectionAndSpinCount(&m_cs, (DWORD)spinCount))
			    throw Win32Exception(GetLastError());
		}
		
		public: ~Lock()
		{
			DeleteCriticalSection(&m_cs);
		}

		public: void Enter()
		{
			EnterCriticalSection(&m_cs);
		}

		public: bool TryEnter() throw()
		{
			return TryEnterCriticalSection(&m_cs) == TRUE;
		}

		public: void Leave() throw()
		{
			LeaveCriticalSection(&m_cs);
		}

		public: class Auto
		{
			public: Auto(Lock& cs) throw() 
			{ 
				m_pcs = &cs; m_pcs->Enter(); 
			}
			
			public: ~Auto() throw()
			{ 
				m_pcs->Leave(); 
			}
		
		    private: Lock *m_pcs;

            // Make non-copyable
            private: Auto(Auto const &);
            private: int operator=(Auto const &);
		};
		
	    protected: CRITICAL_SECTION m_cs;

        private: Lock(Lock const &);
        private: int operator=(Lock const &);
	};
}
