#include <Pseudo\StringBuilder.hpp>

using namespace Pseudo;

BEGIN_TEST_CLASS(TestStringBuilder)

BEGIN_TEST_METHODS
	TEST_METHOD(Test)
END_TEST_METHODS

void Test()
{
    StringBuilder sb(10);

    *sb.get_Ptr() = '*';
    
    sb.set_Length(1);
    
    TEST_ASSERT(sb.get_Length() == 1, L"Length should be 1");
    TEST_ASSERT(sb.get_Capacity() >= 10, L"Capacity should be 10");
}

END_TEST_CLASS
