// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)
#include <Pseudo\Compiler.hpp>
#include <Pseudo\Exception.hpp>
#include <Pseudo\Trace.hpp>

#ifdef PSEUDO_OVERRIDE_NEW_AND_DELETE

inline void* _cdecl operator new(size_t n)
{
	void *p = ::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, n);
	
	if (p == NULL)
		throw Pseudo::OutOfMemoryException();
		
	return p;
}

inline void* _cdecl operator new[](size_t n)
{
	void *p = ::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, n);
	
	if (p == NULL)
		throw Pseudo::OutOfMemoryException();
	
	return p;
}

inline void _cdecl operator delete(void* p)
{
	DEBUG_ASSERT(HeapValidate(GetProcessHeap(), 0, p));

	::HeapFree(GetProcessHeap(), 0, p);
}

inline void _cdecl operator delete[](void* p)
{
    DEBUG_ASSERT(HeapValidate(GetProcessHeap(), 0, NULL));

	::HeapFree(GetProcessHeap(), 0, p);
}

#endif