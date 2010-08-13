#include <Pseudo\ArrayList.hpp>

using namespace Pseudo;

BEGIN_TEST_CLASS(TestArrayList)

BEGIN_TEST_METHODS
    TEST_METHOD(TestBasic)
    TEST_METHOD(TestCopyConstructor)
END_TEST_METHODS

void TestBasic()
{
    ArrayList<Byte> data(10);
    
    TEST_ASSERT(data.get_Count() == 0, L"Length of array should be 0");
    
    for (Int i = 0; i < 10; i++)
    {
        data.Add(i);
    }

    TEST_ASSERT(data.get_Count() == 10, L"Length of array should be 10");
}

void TestCopyConstructor()
{
    ArrayList<Char> a1(10);
    ArrayList<Char> a2(20);
    
    a1.Add('A'); 
    a2.Add('B');
    
    Int capacity1 = a1.get_Capacity();
    Int capacity2 = a2.get_Capacity();
    
    Math::Swap(a1, a2);
    
    TEST_ASSERT(a1[0] == 'B' && a2[0] == 'A', 
        L"Expected a1[0] == 'B' and a2[0] == 'A' after swap"); 
    TEST_ASSERT(a1.get_Capacity() == capacity2 && a2.get_Capacity() == capacity1,
        L"Expected a1.get_Capacity() and a2.get_Capacity() to be swapped");
}

END_TEST_CLASS
