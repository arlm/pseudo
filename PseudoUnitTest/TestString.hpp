#include <Pseudo\String.hpp>

using namespace Pseudo;

BEGIN_TEST_CLASS(TestString)

    BEGIN_TEST_METHODS
        TEST_METHOD(TestConcat)
        TEST_METHOD(TestSubString)
        TEST_METHOD(TestStartsWith)
        TEST_METHOD(TestEndsWith)
        TEST_METHOD(TestCompare)
        TEST_METHOD(TestArray)
        TEST_METHOD(TestTrim)
        TEST_METHOD(TestJoin)
        TEST_METHOD(TestIndexOf)
    END_TEST_METHODS

    void TestConcat()
    {
        String s = String::Format(L"Test %d, %d, %d", 1, 2, 3);
        
        TEST_ASSERT(s == L"Test 1, 2, 3", L"Strings don't match");

        String s1;
        String s2;	
        String s3;

        s1 = L"This";	
        s3 = s1 + String(L" and ") + L"that";
        
        TEST_ASSERT(s3 == L"This and that", L"Comparing s3 against \"This and that\""); 
        
        s1 = L"The quick";
        s2 = L"brown fox";
        
        s3 = s1 + (Char)' ' + s2;
        
        TEST_ASSERT(s3 == L"The quick brown fox", L"Comparing s3 against \"The quick brown fox\"");
        TEST_ASSERT(s2 == L"brown fox", L"Comparing s2 against \"brown fox\"");
        TEST_ASSERT(s1 == L"The quick", L"Comparing s1 against \"The quick\"");
        
        s2 = L"spotted cheetah";
        s3 = s1;
        s3 += L" " + s2;
        
        TEST_ASSERT(s3 == L"The quick spotted cheetah", L"Comparing s3 against \"The quick spotted cheetah\"");
        
        s3 = s1;
        s3 += String(L" ") + L"black cat";
        
        TEST_ASSERT(s3 == L"The quick black cat", L"Comparing s3 against \"The quick black cat\"");
    }

    void TestSubString()
    {
        String s(L"123456");
        
        TEST_ASSERT(s.SubString(0) == L"123456", L"Substring from 0 should be 123456"); 
        TEST_ASSERT(s.SubString(3) == L"456", L"Substring from 3 should be 456"); 
        TEST_ASSERT(s.SubString(6) == L"", L"Substring from 6 should be empty"); 
    }	

    void TestStartsWith()
    {
        String s(L"Beginning and end");
        String s2(L"begin");
        
        TEST_ASSERT(s.StartsWith(s2, true), L"Checking String shorter than target ignoring case"); 
        TEST_ASSERT(s.StartsWith(String(L"beginning")), L"Checking String shorter than target ignoring case"); 
        TEST_ASSERT(!s.StartsWith(String(L"This is longer so it won't even try")), L"Checking String longer than target ignoring case"); 
        
        TEST_ASSERT(s.StartsWith(L"Beginning", true), L"Checking Char* shorter than target ignoring case"); 
        TEST_ASSERT(!s.StartsWith(L"beginning", false), L"Checking Char* shorter than target not ignoring case"); 
        TEST_ASSERT(!s.StartsWith(L"Beginning and end of everything"), L"Checking Char* longer than target"); 
    }	

    void TestEndsWith()
    {
        String s(L"Beginning and end");
        String s2(L"end");
        
        TEST_ASSERT(s.EndsWith(s2, true), L"Checking String shorter than target ignoring case"); 
        TEST_ASSERT(s.EndsWith(String(L"end")), L"Checking String shorter than target ignoring case"); 
        TEST_ASSERT(!s.EndsWith(String(L"This is longer so it won't even try")), L"Checking String longer than target ignoring case"); 
        
        TEST_ASSERT(s.EndsWith(L"end", true), L"Checking Char* shorter than target ignoring case"); 
        TEST_ASSERT(!s.EndsWith(L"End", false), L"Checking Char* shorter than target not ignoring case"); 
        TEST_ASSERT(!s.EndsWith(L"Beginning and end of everything"), L"Checking Char* longer than target"); 
    }

    void TestTrim()
    {
        String bothEnds(L"   string   ");

        TEST_ASSERT(bothEnds.Trim() == L"string", L"String was not trimmed");

        String startOnly(L"   a");

        TEST_ASSERT(startOnly.Trim() == L"a", L"String with spaces at start was not trimmed");

        String endOnly(L"b   ");

        TEST_ASSERT(endOnly.Trim() == L"b", L"String with spaces at end was not trimmed");

        String empty(L"    ");
        
        TEST_ASSERT(empty.Trim() == String::get_Empty(), L"String is not empty");
    }	

    void TestCompare()
    {
        String blah1(L"Blah");
        String blah2(L"Blah");
        String blurp(L"Blurp");
        const Char* pBlah = L"Blah";
        const Char* pBlurp = L"Blurp";
        
        TEST_ASSERT(blah1 == blah2, L"Comparing (String)Blah == (String)Blah");
        TEST_ASSERT(blurp != blah1, L"Comparent (String)Blurp != (String)Blah");
        TEST_ASSERT(blah1 == pBlah, L"Comparing (String)Blah == (const Char*)Blah");
        TEST_ASSERT(blah1 != pBlurp, L"Comparing (String)Blah == (const Char*)Blurp");
    }

    void TestArray()
    {
        Array<String> output;
        String split1(L"abc;def;ghi;jkl");
        
        split1.Split(';', output);
        
        TEST_ASSERT(output.get_Count() == 4, L"Split string does not contain 4 entries");

        String split2(L";;ghi;");
        
        split2.Split(';', output);
        
        TEST_ASSERT(output.get_Count() == 4, L"Split string does not contain 4 entries");
    }

    void TestJoin()
    {
        Array<String> lines(3);

        lines[0] = L"abc";
        lines[1] = L"12345";
        lines[2] = L"xy";
        
        String result = String::Join(lines);

        TEST_ASSERT(result.get_Length() == 10, L"Should be of length 10");
        TEST_ASSERT(result[3] == L'1', L"Offset 3 should be '1'");
    }

    void TestIndexOf()
    {
        String s(L"abcdefg");

        TEST_ASSERT(s.IndexOf(L'b') == 1, L"'b' should be at index 1");
        TEST_ASSERT(s.IndexOf(L'b', 2) == -1, L"'b' should not be found after index 1");
        TEST_ASSERT(s.LastIndexOf(L'e') == 4, L"'e' should be at index 4");
        TEST_ASSERT(s.LastIndexOf(L'e', 3) == -1, L"'e' should not be found before index 4");
    }

END_TEST_CLASS
