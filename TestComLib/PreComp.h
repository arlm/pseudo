// Copyright (c) 2008 Microsoft Corporation. All rights reserved.

#pragma once

#define WIN32_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501	// Default to Windows 4.0 and higher
#endif

#ifndef STRICT
#define STRICT
#endif

#include <Pseudo\ComHelp.hpp>
#include <Pseudo\String.hpp>
#include <Pseudo\RegistryKey.hpp>
#include <Pseudo\Path.hpp>
