// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#include <Pseudo\Compiler.hpp>
#include <Pseudo\ValueType.hpp>
#include <Pseudo\Exception.hpp>

namespace Pseudo
{
    class CharType
    {
        public: static Bool IsSpace(Char c)
        {
            WORD typeFlags;
            
            if (!GetStringTypeW(CT_CTYPE1, &c, 1, &typeFlags))
                throw ArgumentException();

            return ((typeFlags & C1_SPACE) != 0);
        }
    };
}