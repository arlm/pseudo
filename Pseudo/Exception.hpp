// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)

#include <WinError.h>
#include <StrSafe.h>
#include <Pseudo\ValueType.hpp>
#include <Pseudo\AutoArrayPtr.hpp>
#include <Pseudo\Macros.hpp>

namespace Pseudo
{
    class Exception
    {
        protected: HRESULT hresult;
        // We can't use any dynamically allocated memory here as it causes 
        // circular references and is generally a bad idea.
        protected: Char message[256];

        public: Exception(HRESULT hresult = E_UNEXPECTED)
        {
            this->hresult = hresult;
        }

        public: Exception(const Char* pMessage, HRESULT hresult = E_UNEXPECTED)
        {
            this->hresult = hresult;
            set_Message(pMessage);
        }

        public: Exception(Exception const & rhs)
        {

            set_Message(rhs.get_Message());
        }

        public: virtual ~Exception()
        {
        }

        protected: void set_Message(const Char* pMessage)
        {
            if (pMessage == NULL)
                return;
                
            size_t length;
            HRESULT hr = StringCchLengthW(pMessage, INT_MAX, &length);

            if (FAILED(hr))
                ::RaiseException(EXCEPTION_NONCONTINUABLE_EXCEPTION, 0, 0, NULL);

            StringCchCopyW(message, ARRAY_SIZE(message), pMessage);
        }

        public: virtual const Char* get_Message() const
        {
            return message;
        }
    
        public: virtual HRESULT get_HResult() const
        {
            return hresult;
        }
    };

    class OutOfMemoryException : public Exception
    {
        public: OutOfMemoryException() : Exception(E_OUTOFMEMORY)
        {
        }

        public: OutOfMemoryException(const Char* pMessage) : Exception(pMessage, E_OUTOFMEMORY)
        {
        }

        public: OutOfMemoryException(OutOfMemoryException const & rhs) : Exception(rhs)
        {
        }

        public: virtual ~OutOfMemoryException()
        {
        }
    };

    class NotImplementedException : public Exception
    {
        public: NotImplementedException() : Exception(E_OUTOFMEMORY)
        {
        }

        public: NotImplementedException(const Char* pMessage) : Exception(pMessage, E_OUTOFMEMORY)
        {
        }

        public: NotImplementedException(NotImplementedException const & rhs) : Exception(rhs)
        {
        }

        public: virtual ~NotImplementedException()
        {
        }
    };

    class BadProgramException : public Exception
    {
        public: BadProgramException() : Exception()
        {
        }

        public: BadProgramException(const Char* pMessage) : Exception(pMessage)
        {
        }

        public: BadProgramException(BadProgramException const & rhs) : Exception(rhs)
        {
        }

        public: virtual ~BadProgramException()
        {
        }
    };

    class ArgumentNullException : public Exception
    {
        public: ArgumentNullException() : Exception(E_POINTER)
        {
        }

        public: ArgumentNullException(const Char* pMessage) : Exception(pMessage, E_POINTER)
        {
        }

        public: ArgumentNullException(ArgumentNullException const & rhs) : Exception(rhs)
        {
        }

        public: virtual ~ArgumentNullException()
        {
        }
    };

    class ArgumentException : public Exception
    {
        public: ArgumentException() : Exception(E_INVALIDARG)
        {
        }

        public: ArgumentException(const Char* pMessage) : Exception(pMessage, E_INVALIDARG)
        {
        }

        public: ArgumentException(ArgumentException const & rhs) : Exception(rhs)
        {
        }
        
        public: virtual ~ArgumentException()
        {
        }
    };

    class ArgumentOutOfRangeException : public Exception
    {
        public: ArgumentOutOfRangeException() : Exception(E_INVALIDARG)
        {
        }

        public: ArgumentOutOfRangeException(const Char* pMessage) : Exception(pMessage, E_INVALIDARG)
        {
        }

        public: ArgumentOutOfRangeException(ArgumentOutOfRangeException const & rhs) : Exception(rhs)
        {
        }

        public: virtual ~ArgumentOutOfRangeException()
        {
        }
    };

    class TimeoutException : public Exception
    {
        public: TimeoutException() : Exception(HRESULT_FROM_WIN32(WAIT_TIMEOUT))
        {
        }

        public: TimeoutException(const Char* pMessage) : Exception(pMessage, HRESULT_FROM_WIN32(WAIT_TIMEOUT))
        {
        }

        public: TimeoutException(TimeoutException const & rhs) : Exception(rhs)
        {
        }

        public: virtual ~TimeoutException()
        {
        }
    };

    class IOException : public Exception
    {
        public: IOException(HRESULT hresult = E_UNEXPECTED) : Exception(hresult)
        {
        }

        public: IOException(const Char* pMessage, HRESULT hresult = E_UNEXPECTED) : Exception(pMessage, hresult)
        {
        }

        public: IOException(IOException const & rhs) : Exception(rhs)
        {
        }

        public: virtual ~IOException()
        {
        }
    };

    class Win32Exception : public Exception
    {
        private: DWORD m_error;

        public: Win32Exception(DWORD error = ERROR_INVALID_FUNCTION) : Exception(HRESULT_FROM_WIN32(error)), m_error(error)
        {
        }
 
        public: Win32Exception(const Char* pMessage, DWORD error = ERROR_INVALID_FUNCTION)
            : Exception(pMessage, HRESULT_FROM_WIN32(error)), m_error(error)
        {
        }

        public: Win32Exception(Win32Exception const & rhs) : Exception(rhs), m_error(rhs.m_error)
        {
        }

        public: virtual ~Win32Exception()
        {
        }

        public: DWORD get_Error() const { return m_error; }

    };

    class ComException : public Exception
    {
        public: ComException(HRESULT hresult = E_FAIL)
        {
        }
 
        public: ComException(const Char* pMessage, HRESULT hresult = E_FAIL) : Exception(pMessage, hresult)
        {
        }

        public: ComException(ComException const & rhs) : Exception(rhs)
        {
        }

        public: virtual ~ComException()
        {
        }
    };
}

#pragma warning(pop)
