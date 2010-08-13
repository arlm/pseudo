// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)
#include <windows.h>
#include <Pseudo/Timeout.hpp>
#include <Pseudo/Exception.hpp>

namespace Pseudo
{
    class EventWaitHandle
    {
        private:
            HANDLE handle;

        public:
            enum EventResetMode
            {
                AutoReset,
                ManualReset,
            };

            EventWaitHandle()
            {
                handle = INVALID_HANDLE_VALUE;
            }

            EventWaitHandle(HANDLE handle) : handle(handle)
            {
            }

			~EventWaitHandle()
			{
				Close();
			}

            EventWaitHandle(bool const initial, EventResetMode const mode, wchar_t const * const name, bool & createdNew)
            {
                if (name == NULL)
                    throw ArgumentNullException();

                handle = CreateEvent(NULL, mode == ManualReset, initial, name);
                if (handle == NULL)
                    throw Win32Exception(GetLastError());

                createdNew = GetLastError() != ERROR_ALREADY_EXISTS;
            }

            void Close()
            {
                if (handle != NULL)
                {
                    CloseHandle(handle);
                    handle = NULL;
                }
            }

            void Set() const
            {
                if (!::SetEvent(handle))
                    throw Win32Exception(GetLastError());
            }

            void Reset() const
            {
                if (!::ResetEvent(handle))
                    throw Win32Exception(GetLastError());
            }

            bool WaitOne(DWORD msec) const
            {
                return WAIT_OBJECT_0 == WaitForSingleObject(handle, msec);
            }

        private:
            EventWaitHandle(EventWaitHandle const & rhs);
            EventWaitHandle & operator=(EventWaitHandle const & rhs);

        public:
            static const unsigned int WaitTimeout = WAIT_TIMEOUT;

            template<int N>
            static unsigned int WaitAny(DWORD timeout, EventWaitHandle const * const (& events)[N])
            {
                HANDLE handles[N] = {};
                for (unsigned int i = 0; i < N; ++i)
                    handles[i] = events[i]->handle;

                DWORD ret = WaitForMultipleObjects(N, &handles[0], false, timeout);
                if (ret == WAIT_TIMEOUT)
                    return ret;
                else if (WAIT_OBJECT_0 <= ret && ret < WAIT_OBJECT_0 + N)
                    return ret - WAIT_OBJECT_0;
                else if (WAIT_ABANDONED_0 <= ret && ret < WAIT_ABANDONED_0 + N)
                    return ret - WAIT_OBJECT_0;
                else
                {
                    assert(false && "WaitForMultipleObjects returned unexpected code");
                    return WAIT_FAILED;
                }
            }
    };
}
