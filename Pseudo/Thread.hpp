// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)

#include <windows.h>

namespace Pseudo
{
    class Thread
    {
        private: Thread()
        {

        }

        public: static void SpinWait(int count)
        {
            for (int i = 0; i < count; ++i)
                YieldProcessor();
        }

        public: static void Sleep(int msec)
        {
            ::Sleep(msec);
        }
    };
}

#pragma warning(pop)
