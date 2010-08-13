// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#include <Pseudo\String.hpp>

namespace Pseudo
{
    class FileVersionInfo
    {
        private: AutoArrayPtr<Byte> _data;
        private: VS_FIXEDFILEINFO* _pFixedFileInfo;

        public: static FileVersionInfo GetVersionInfo(String filePath)
        {
            VS_FIXEDFILEINFO* pFixedFileInfo;
            DWORD handle;
            DWORD infoSize = ::GetFileVersionInfoSize(filePath, &handle);

            if (0 == infoSize)
                throw ArgumentException();

            AutoArrayPtr<Byte> data(PSEUDO_NEW Byte[infoSize]);

            if (!::GetFileVersionInfo(filePath, 0, infoSize, data.get_Ptr()))
                throw Exception();

            UInt bufLen;
                
            if (0 == VerQueryValue(data.get_Ptr(), L"\\", (void**)&pFixedFileInfo, &bufLen))
                throw Exception();

            if (0 == bufLen)
                throw Exception();

            String version;
            
            return String::Format(L"%d.%d.%d",
                HIWORD(pFixedFileInfo->dwFileVersionMS),
                LOWORD(pFixedFileInfo->dwFileVersionMS),
                HIWORD(pFixedFileInfo->dwFileVersionLS));
        }
        
        public: get_FileMajorPart()
        {
            return pFixedFileInfo
        }
    };
}