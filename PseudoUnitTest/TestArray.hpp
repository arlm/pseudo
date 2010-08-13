#include <Pseudo\Array.hpp>

using namespace Pseudo;

BEGIN_TEST_CLASS(TestArray)

    BEGIN_TEST_METHODS
        TEST_METHOD(Test)
        TEST_METHOD(TestCopyConstructor)
    END_TEST_METHODS

    void Test()
    {
        Array<Int> empty;
        
        TEST_ASSERT(empty.get_Count() == 0, L"Checking empty array count is zero");
        
        Array<Byte> data(10);

        TEST_ASSERT(data.get_Count() == 10, L"Length of array should be 10");
        
        for (Int i = 0; i < 10; i++)
        {
            data[i] = i;
        }
        
        data.set_Count(5);
        
        TEST_ASSERT(data.get_Count() == 5, L"Length of array should be 5");
        
        data.set_Count(20);
        
        TEST_ASSERT(data.get_Count() == 20, L"Length of array should be 20");
        TEST_ASSERT(data[15] == 0, L"Array value at index 15 should be 0");
    }

    Array<Single> ReturnArray(Array<Single> a)
    {
        for (Int i = 0; i < a.get_Count(); i++)
            a[i] = (Single)i;
            
        return a;
    }

    void TestCopyConstructor()
    {
        Array<Int> i1(10);
        
        Int* p1 = i1.get_Ptr();
        Int count1 = i1.get_Count();
        
        Array<Int> i2(i1);
        
        TEST_ASSERT(i2.get_Ptr() != p1, L"i2 should not point to i1");
        TEST_ASSERT(i1.get_Ptr() != NULL, L"i1 should not be NULL");
        TEST_ASSERT(i2.get_Count() == count1, L"Expected i2 to have count of count1");
        TEST_ASSERT(i1.get_Count() == 10, L"Expected i1 to have count of 10");

        Array<Single> i3(Array<Single>(3));
        
        TEST_ASSERT(i3.get_Count() == 3, L"Expected i3 to have count of 3");

        Array<Single> i4;
        
        i4 = i3;
        
        TEST_ASSERT(i4.get_Count() == 3, L"Expected i4 to have count of 3");
        TEST_ASSERT(i3.get_Count() == 3, L"Expected i3 to have count of 3");
        
        i4 = ReturnArray(i4);
        
        TEST_ASSERT(i4[1] == 1.0, L"Expected i4[1] == 1.0");
    }

END_TEST_CLASS
