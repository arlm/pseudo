// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#include <Pseudo\String.hpp>

namespace Pseudo
{
    // Class for convert all sorts of types to other types
    class Convert
    {
        private: Convert() { }
    
        public: static String ToString(REFGUID rguid)
        {
            return String::Format(L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
                rguid.Data1, rguid.Data2, rguid.Data3,
                (int)rguid.Data4[0], (int)rguid.Data4[1], (int)rguid.Data4[2], (int)rguid.Data4[3],
                (int)rguid.Data4[4], (int)rguid.Data4[5], (int)rguid.Data4[6], (int)rguid.Data4[7]);
        }
        
        public: static String ToString(Int n)
        {
            return String::Format(L"%d", n);
        }
    };
}