#include <Pseudo\Math.hpp>
#include <Pseudo\ValueType.hpp>

BEGIN_TEST_CLASS(TestMath)

BEGIN_TEST_METHODS
	TEST_METHOD(TestMax)
	TEST_METHOD(TestMin)
	TEST_METHOD(TestAbs)
	TEST_METHOD(TestSqrt)
	TEST_METHOD(TestAlign)
	TEST_METHOD(TestSwap)
END_TEST_METHODS

void TestMax()
{
    TEST_ASSERT(Math::Max(0, 1) == 1, L"1 > 0");
    TEST_ASSERT(Math::Max(1, 0) == 1, L"1 > 0");
    TEST_ASSERT(Math::Max(0.0, 1.0) == 1.0, L"1.0 > 0.0");
    TEST_ASSERT(Math::Max(1.0, 0.0) == 1.0, L"1.0 > 0.0");
}

void TestMin()
{
    TEST_ASSERT(Math::Min(0, 1) == 0, L"0 < 1");
    TEST_ASSERT(Math::Min(1, 0) == 0, L"0 < 1");
    TEST_ASSERT(Math::Min(0.0, 1.0) == 0.0, L"0.0 < 1.0");
    TEST_ASSERT(Math::Min(1.0, 0.0) == 0.0, L"0.0 < 1.0");
}

void TestAbs()
{
    TEST_ASSERT(Math::Abs(1) == 1, L"|1| == 1");
    TEST_ASSERT(Math::Abs(-1) == 1, L"|-1| == 1");
    TEST_ASSERT(Math::Abs(1.0) == 1.0, L"|1.0| == 1.0");
    TEST_ASSERT(Math::Abs(-1.0) == 1.0, L"|-1.0| == 1.0");
    TEST_ASSERT(Math::Abs((IntPtr)0x10000000) == 0x10000000, L"|0x10000000| == 0x10000000");
}

void TestSqrt()
{
    TEST_ASSERT(Math::Sqrt(4.0) == 2.0, L"Sqrt(4.0) == 2.0");
}

void TestAlign()
{
    TEST_ASSERT(Math::Align<Int>(0, sizeof(Long)) == 0, L"Aligning 0 on sizeof(Long) boundary should be 0");    
    TEST_ASSERT(Math::Align<Int>(3, sizeof(Double)) == 8, L"Aligning 3 on sizeof(Double) boundary should be 8");    
    TEST_ASSERT(Math::Align<Int>(7, sizeof(Int)) == 8, L"Aligning 7 on sizeof(Int) boundary should be 8");
    TEST_ASSERT(Math::Align<UInt>(78, sizeof(UInt)) == 80, L"Aligning 78 on sizeof(UInt) boundary should be 80");
    TEST_ASSERT(Math::Align<UInt>(16, sizeof(UInt)) == 16, L"Aligning 16 on sizeof(UInt) boundary should be 16");
}

void TestSwap()
{
    Int i1 = 1;
    Int i2 = 100;
    
    Math::Swap(i1, i2);
    
    TEST_ASSERT(i1 == 100 && i2 == 1, L"Expected i1 == 100 and i2 == 1 after swap");
}

END_TEST_CLASS
