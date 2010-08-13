////////////////////////////////////////////////////
// File:    StringComparer.hpp	
// Author:  Matthew Saffer 
// Date:    
//
// Description: Pseudo::Dictionary comparator for Pseudo::String. 
//              All comparisons and hashes are case-sensitive
//      
//
// Comments:    Made to be as close as possible to the BCL implementation
//
// Version:
//
// Revisions:	
////////////////////////////////////////////////////

#pragma once
#include <Pseudo\String.hpp>
#include <Pseudo\Dictionary.hpp>

//These magic numbers and shifts were taken directly from the NDP 
// string compare.
//This is a conversion from what's in the BCL.

#ifdef WIN32
    #define MAGIC1  (5381<<16)+5381;
#else
    #define MAGIC1  5381;
#endif
#define MAGIC2      0x5d588b65
#define SHIFTL      0x00000005
#define SHIFTR      0x0000001B

namespace Pseudo
{

    class StringComparer
    {

        private: StringComparer() { }
        public: static Bool Equals(Pseudo::String &x, Pseudo::String &y)
        {
            return (Pseudo::String::Compare(x,y) == 0);
        }

        public: static IntPtr GetHashCode(Pseudo::String &x)
        {
            const Char *chPtr = x.get_Ptr();
            Int hash1 = MAGIC1;
            Int hash2 = hash1;
            const Int *numPtr = (Int *)chPtr;
            Int c;
            while ((c = chPtr[0]) != 0 )
            {
                hash1 = ((hash1 << SHIFTL) + hash1) ^ c;
                c = chPtr[1];
                if (c == 0 )
                    break;
                hash2 = ((hash2 << SHIFTL) + hash2) ^ c;
                chPtr += 2;
            }

            return (hash1 + (hash2 * MAGIC2));
        }
    };

}
