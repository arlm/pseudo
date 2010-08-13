// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#include <Pseudo\Compiler.hpp>
#include <Pseudo\ValueType.hpp>

// ATL redefines this into a function; if it's not defined, ATL has already done this.
#ifdef InterlockedExchangePointer 
#ifdef _M_IX86
#undef InterlockedExchangePointer
inline void* _stdcall InterlockedExchangePointer(void** ppOld, void* pNew) throw()
{
    // ATL does this macro and reinterpret_cast silliness too (just try it without)
    return (reinterpret_cast<void*>(static_cast<LONG_PTR>(
        ::InterlockedExchange(reinterpret_cast<LONG*>(ppOld), static_cast<__int32>(reinterpret_cast<LONG_PTR>(pNew))))) );
}
#endif
#endif

// WinXP/x86 does not have interlocked 64 bit operations so we define the intrinsic and 
// steal the rest of the definitions from winbase.h
#if defined(_M_IX86) && (_WIN32_WINNT < 0x0502)

extern "C" __int64 _InterlockedCompareExchange64(__int64 volatile *, __int64, __int64);

#define InterlockedCompareExchange64 _InterlockedCompareExchange64

__inline __int64 InterlockedIncrement64(__inout __int64 volatile* pAddend)
{
    __int64 old;

    do 
    {
        old = *pAddend;
    } 
    while (InterlockedCompareExchange64(pAddend, old + 1, old) != old);

    return old + 1;
}

__inline __int64 InterlockedDecrement64(__inout __int64 volatile *pAddend)
{
    __int64 old;

    do 
    {
        old = *pAddend;
    } 
    while (InterlockedCompareExchange64(pAddend, old - 1, old) != old);

    return old - 1;
}

__inline __int64 InterlockedExchange64(__inout __int64 volatile *pTarget, __in __int64 value)
{
    __int64 old;

    do 
    {
        old = *pTarget;
    } 
    while (InterlockedCompareExchange64(pTarget, value, old) != old);

    return old;
}

#endif // defined(_M_IX86) && (_WIN32_WINNT < 0x0502)

namespace Pseudo
{
    class Interlocked
    {
        // Can't use TDerived::T directly since TDerived is incomplete at point of instantiation
        private: template <typename TDerived, typename T> struct InterlockedBase 
        {
            static T Read(T volatile * p) { return TDerived::CompareExchange(p, 0, 0); }
        };

        // Template to differentiate interlocked operation based on operand size. Note, all pointers passed to the
        // member functions must be aligned at the respective 4 or 8 byte boundary.
        private: template <int TSize> struct _Interlocked { };

        // Explicit instantiations for __int32 and __int64
        private: template <> struct _Interlocked<4> : InterlockedBase<_Interlocked<4>, __int32>
        {
            typedef __int32 T;
            static T Increment(T volatile * p) { return ::InterlockedIncrement((LONG*)p); }
            static T Decrement(T volatile * p) { return ::InterlockedDecrement((LONG*)p); }
            static T Exchange(T volatile * p, T n) { return ::InterlockedExchange((LONG*)p, n); }
            static T CompareExchange(T volatile * p, T x, T y) { return ::InterlockedCompareExchange((LONG*)p, x, y); }
        };

        private: template <> struct _Interlocked<8> : InterlockedBase<_Interlocked<8> , __int64>
        {
            typedef __int64 T;
            static T Increment(T volatile * p) { return ::InterlockedIncrement64(p); }
            static T Decrement(T volatile * p) { return ::InterlockedDecrement64(p); }
            static T Exchange(T volatile * p, T n) { return ::InterlockedExchange64(p, n); }
            static T CompareExchange(T volatile * p, T x, T y) { return ::InterlockedCompareExchange64(p, x, y); }
        };

        public: template<typename T> static T _stdcall Increment(T volatile * p)
        {
            typedef _Interlocked<sizeof(T)> _InterlockedType;
            return static_cast<T>(_InterlockedType::Increment(reinterpret_cast<_InterlockedType::T volatile *>(p)));
        }

        public: template<typename T> static T _stdcall Decrement(T volatile * p)
        {
            typedef _Interlocked<sizeof(T)> _InterlockedType;
            return static_cast<T>(_InterlockedType::Decrement(reinterpret_cast<_InterlockedType::T volatile *>(p)));
        }

        public: template<typename T> static T _stdcall Exchange(T volatile * p, T y)
        {
            typedef _Interlocked<sizeof(T)> _InterlockedType;
            return static_cast<T>(_InterlockedType::Exchange(reinterpret_cast<_InterlockedType::T volatile *>(p), static_cast<_InterlockedType::T>(y)));
        }

        public: template<typename T> static T _stdcall CompareExchange(T volatile * p, T y, T z)
        {
            typedef _Interlocked<sizeof(T)> _InterlockedType;
            return static_cast<T>(_InterlockedType::CompareExchange(reinterpret_cast<_InterlockedType::T volatile *>(p), static_cast<_InterlockedType::T>(y), static_cast<_InterlockedType::T>(z)));
        }

        public: template<typename T> static T _stdcall Read(T volatile * p)
        {
            typedef _Interlocked<sizeof(T)> _InterlockedType;
            return static_cast<T>(_InterlockedType::Read(reinterpret_cast<_InterlockedType::T volatile *>(p)));
        }

        // Interlocked exchange.  Does a thread safe exchange of two values.   T must be the same size as void *.
        public: template<class T> static T _stdcall ExchangePtr(T volatile * p, T y) { return (T)InterlockedExchangePointer((PVOID*)p, (PVOID)y); }
    };
}
