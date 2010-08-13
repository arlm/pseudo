#include <Pseudo\Dictionary.hpp>

BEGIN_TEST_CLASS(TestDictionary)

BEGIN_TEST_METHODS
	TEST_METHOD(Test)
	TEST_METHOD(TestStress)
END_TEST_METHODS

void Test()
{
	Pseudo::Dictionary<int, const Char *> dict;

    const Char* pCarrot = L"Carrot";

    dict.Add(1, L"Apple");
    dict.Add(2, L"Banana");
    dict.Add(3, pCarrot);
    dict.Add(4, L"Peach");

    const Char* p;
    
    TEST_ASSERT(dict.TryGetValue(3, p) && p == pCarrot, L"Search for value 3/Carrot in dictionary");
}

void TestStress()
{
    Pseudo::Dictionary<int, int> dict;
    //const int numEntries = 0x6dda89;
    const int numEntries = 0xffff;

    for (int i = 0; i < numEntries; i++)
        dict.Add(i, i);
    
    TEST_ASSERT(dict.get_Count() == numEntries, L"Count of dictionary entries not equal to numEntries");
    
    for (int i = 0; i < numEntries; i++)
    {
        int key;
        int value;
        
        dict.get_EntryByOrd(i, &key, &value);
        
        TEST_ASSERT(key == i && key == value, L"Entry is not correct key/value pair");
    }
    
    for (int i = 0; i < numEntries; i++)
        dict.Remove(i);
    
    TEST_ASSERT(dict.get_Count() == 0, L"Count of dictionary entries not equal to 0");
}

END_TEST_CLASS
