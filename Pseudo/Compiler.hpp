// Copyright (c) John Lyon-Smith All rights reserved.

#pragma once

// All the Win32 and CRT includes go here

#ifndef WINVER
    #define WINVER 0x501
#else
    #if WINVER < 0x501
        #error Only WINVER >= 0x501 supported.
    #endif
#endif

#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x501
#else
    #if _WIN32_WINNT < 0x501
        #error Only _WIN32_WINNT >= 0x501 supported
    #endif
#endif

#include <windows.h>
#include <sal.h>
#define STRSAFE_INLINE
#include <strsafe.h>
#include <limits.h>

/// <summary>
/// Disable various vetted warnings from Pseudo. This header pulls double duty as the compiler specific header file, as it is
/// included in everything.  Microsoft C++ can be a bit of a moving target, and it's important not to assume that the library
/// has been fully tested if this file does not specifically include an entry for it.
/// </summary>

// Global stuff
#pragma warning(disable:4482) // nonstandard extension used: enum used in qualified name

#if _MSC_VER==1400

// Whidbey compiler specific things here

#elif _MSC_VER==1500

// Orcas compiler specific things here

#elif _MSC_VER==1600

// Dev10 compiler specific things here

#else

#ifdef PSEUDO_ALLOW_UNSUPPORTED_COMPILER
	#pragma message(WARNING: This library has not been tested with this compiler!)
#else
	#error Unsupported compiler!
#endif

#endif

#include <Pseudo\NativeMethods.hpp>
