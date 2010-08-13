#include <windows.h>

#include "Pseudo\Exception.hpp"

#pragma once

namespace Pseudo
{
    class Stopwatch
    {
        public:
            Stopwatch() : m_startTicks(-1), m_stopTicks(-1), m_frequency(-1)
            {
            }

            void Start()
            {
                if(!QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_startTicks)))
                    throw Win32Exception(GetLastError());
            }

            void Stop()
            {
                if (!QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_stopTicks)))
                    throw Win32Exception(GetLastError());
            }

            double getElapsedMilliseconds()
            {
                if (m_startTicks == -1 || m_stopTicks == -1)
                    return 0;

                if (m_frequency == -1)
                    if (!QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_frequency)))
                        throw Win32Exception(GetLastError());

                return 1000. * (m_stopTicks - m_startTicks) / m_frequency;
            }

        private:
            __int64 m_startTicks;
            __int64 m_stopTicks;
            __int64 m_frequency;

    };
}
