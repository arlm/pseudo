#include <Pseudo\CharType.hpp>

using namespace Pseudo;

BEGIN_TEST_CLASS(TestCharType)

BEGIN_TEST_METHODS
    TEST_METHOD(Test)
END_TEST_METHODS

void Test()
{
    TEST_ASSERT(CharType::IsSpace(' '), L"IsSpace(' ') should be true");
    TEST_ASSERT(!CharType::IsSpace('A'), L"IsSpace('A') should be false");
}

END_TEST_CLASS
