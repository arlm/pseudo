#include <Pseudo\Convert.hpp>

using namespace Pseudo;

BEGIN_TEST_CLASS(TestConvert)

BEGIN_TEST_METHODS
    TEST_METHOD(Test)
END_TEST_METHODS

void Test()
{
    String s = Convert::ToString(123);
    
    TEST_ASSERT(s == L"123", L"Comparing converted integer 123 to string equivalent");
    
    GUID guid = 
    {
        0x12345678,
        0xABCD,
        0xEF01,
        0x23,
        0x45,
        0x67,
        0x89,
        0xAB,
        0xCD,
        0xEF,
        0x01
    };
    
    s = Convert::ToString(guid);
    
    TEST_ASSERT(s == L"{12345678-ABCD-EF01-2345-6789ABCDEF01}", L"Comparing GUID {12345678-ABCD-EF01-2345-6789ABCDEF01} to string equivalent.");
}

END_TEST_CLASS
