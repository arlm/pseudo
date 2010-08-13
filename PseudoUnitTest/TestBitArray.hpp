#include <Pseudo\BitArray.hpp>

using namespace Pseudo;

BEGIN_TEST_CLASS(TestBitArray)

BEGIN_TEST_METHODS
    TEST_METHOD(Test)
END_TEST_METHODS

void Test()
{
    BitArray bits(12);

    TEST_ASSERT(bits[1] == false, L"Testing bit 1 of array is false");
    TEST_ASSERT(bits[2] == false, L"Testing bit 2 of array is false");
    TEST_ASSERT(bits[3] == false, L"Testing bit 3 of array is false");
    TEST_ASSERT(bits[4] == false, L"Testing bit 4 of array is false");
    
    bits.Set(3, true);
    
    TEST_ASSERT(bits[3] == true, L"Testing bit 3 of array is true");
    TEST_ASSERT(bits[2] == false, L"Testing bit 2 of array is false");
    TEST_ASSERT(bits[4] == false, L"Testing bit 4 of array is false");
}

END_TEST_CLASS
