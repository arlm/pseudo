// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#include <stdlib.h>
#include <math.h>
#include <limits.h>

namespace Pseudo
{
    class Math
    {
        public: static const UInt UIntMax = UINT_MAX;
        public: static const UInt UIntMin = 0;
    
        public: template<class T> 
        static T Align(T n, Int size)
        {
            return T(n + ((size - n) & (size - 1)));
        }

        public: template<typename T>
        static T Min(T lhs, T rhs)
        {
            if (lhs < rhs)
                return lhs;
            else
                return rhs;
        }

        public: template<typename T>
        static T Max(T lhs, T rhs)
        {
            if (lhs > rhs)
                return lhs;
        else
            return rhs;
        }
        
        public: static Int Abs(Int n)
        {
            return abs(n);
        }
        
        public: static Long Abs(Long n)
        {
            return _abs64(n);
        }
        
        public: static Double Abs(Double n)
        {    
            return abs(n);
        }
        
        public: static Single Abs(Single n)
        {    
            return abs(n);
        }
        
        public: template<typename T>
        static T Sqrt(T n)
        {
            return sqrt(n);
        }
        
        public: template<typename T>
        static void Swap(T& a, T& b)
        {
            T t = b;
            b = a;
            a = t;
        }
    };
}
