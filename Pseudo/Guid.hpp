// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#include <Pseudo\String.hpp>

namespace Pseudo
{
    class Guid
    {
        private: GUID guid;
        
        public: Guid(REFCLSID rclsid)
        {
            this->guid = rclsid;
        }
    
        public: String ToString()
        {
            return String::Format(L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
                guid.Data1, guid.Data2, guid.Data3,
                (int)guid.Data4[0], (int)guid.Data4[1], (int)guid.Data4[2], (int)guid.Data4[3],
                (int)guid.Data4[4], (int)guid.Data4[5], (int)guid.Data4[6], (int)guid.Data4[7]);
        }
    };
}