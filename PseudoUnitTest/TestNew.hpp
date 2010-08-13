#include <Pseudo\New.hpp>

using namespace Pseudo;

BEGIN_TEST_CLASS(TestNew)

BEGIN_TEST_METHODS
	TEST_METHOD(Test)
END_TEST_METHODS

class A
{
    public: A(int a, int b)
    {
        this->a = a;
        this->b = b;
    }

    public: ~A()
    {
        a = b = 0;
    }
    
    public: int a;
    public: int b;
};

void Test()
{
    A* pA = PSEUDO_NEW A(1, 2);
    
    TEST_ASSERT(pA->a == 1 && pA->b == 2, L"A() constructor not called correctly");
    // TODO-johnls-2/7/2008: Check allocator to see if one new object is around
    
    delete pA;
    // TODO-johnls-2/7/2008: Check allocator to see if one new object is gone
}

END_TEST_CLASS
