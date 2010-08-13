// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#include <malloc.h>

// Helper macros for stringizing and quoting, one for ANSI one for Unicode string constants
// Note that it is necessary for QUOTE to call STRINGIZE to expand the argument to QUOTE.
// The stringizing operator alone does not expand the argument to the macro.
#define STRINGIZE(s) #s
#define LSTRINGIZE(s) L#s

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

#define QUOTE(s) STRINGIZE(s)
#define LQUOTE(s) LSTRINGIZE(s)

#ifndef PASTE
#define PASTE2(x,y) x##y
#define PASTE(x,y) PASTE2(x,y)
#endif

#ifdef ARRAY_SIZE
#undef ARRAY_SIZE
#endif
#define ARRAY_SIZE(x)  (sizeof(x)/sizeof(x[0]))

#ifdef COUNT_OF
#undef COUNT_OF
#endif
#define COUNT_OF(x)  ARRAY_SIZE(x)

#ifdef STACK_ARRAY
#undef STACK_ARRAY
#endif
// A very useful macro for allocating a typed array on the stack
#define STACK_ARRAY(type, size) (type*)_alloca((size) * sizeof(type))

#ifdef STACK_ALLOC
#undef STACK_ALLOC
#endif
// A macro for allocating an untyped array of bytes on the stack
#define STACK_ALLOC(size) (void*)_alloca(size)

// Error detection helpers
#define THROW_BAD_HRESULT(x)	{ register HRESULT _hr = (x); if (FAILED(_hr)) throw Pseudo::ComException(_hr); }
#define THROW_BAD_WINERROR(x)	{ register LONG _ret = (x); if (_ret != ERROR_SUCCESS) throw Pseudo::Win32Exception(_ret); }

#define CLEANUP_BAD_HRESULT(x, y)	{ register HRESULT _hr = (x); if (FAILED(_hr)) goto y; }

#define RETURN_BAD_HRESULT(x)	{ register HRESULT _hr = (x); if (FAILED(_hr)) return _hr; }
#define RETURN_GOOD_HRESULT(x)	{ register HRESULT _hr = (x); if (SUCCEEDED(_hr)) return _hr; }

#define RETURN_IF_BAD_HRESULT(x, y)	{ if (FAILED(x)) return y; }

#define TRY_CATCH_WIN32EXCEPTION(x)	try { (x); } catch (Win32Exception&) {}

#define ASSERT_GOOD_HRESULT(x)	DEBUG_ASSERT(SUCCEEDED(x))

#ifdef _DEBUG
	#define VERIFY_GOOD_HR(x)	Debug.Assert(SUCCEEDED(x))
#else // _DEBUG
	#define VERIFY_GOOD_HR(x)	(x)
#endif // _DEBUG

#if !defined(DEBUG_ASSERT)
	#if defined(_DEBUG)
		#if defined(PSEUDO_NO_BREAK_ON_ASSERT)
			#define DEBUG_ASSERT(exp) (void)( (exp) || (Pseudo::Trace::WriteLine(LSTRINGIZE(__FILE__) L"(" LQUOTE(__LINE__) L"): Assertion failed: " LQUOTE(exp)), 0) )
		#else
			#define DEBUG_ASSERT(exp) (void)( (exp) || (Pseudo::Trace::WriteLine(LSTRINGIZE(__FILE__) L"(" LQUOTE(__LINE__) L"): Assertion failed: " LQUOTE(exp)), 0) || (__debugbreak(), 0) )
		#endif
	#else
		#define DEBUG_ASSERT(exp) ((void)0)
	#endif
#endif

#if !defined(PSEUDO_NEW)
  #ifdef _DEBUG
        #include <crtdbg.h>
        #define PSEUDO_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__ )
  #else
        #define PSEUDO_NEW new
  #endif
#endif