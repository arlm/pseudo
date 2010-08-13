#include <Pseudo\Path.hpp>

using namespace Pseudo;

BEGIN_TEST_CLASS(TestPath)

BEGIN_TEST_METHODS
	TEST_METHOD(Test)
END_TEST_METHODS

void Test()
{
    String path(L"c:\\blah\\blah\\abc.txt");
    
    TEST_ASSERT(Path::GetDirectoryName(path) == L"c:\\blah\\blah", L"Directory name does not match c:\blah\blah");
    TEST_ASSERT(Path::GetFileName(path) == L"abc.txt", L"File name does not match abc.txt");
    TEST_ASSERT(Path::GetFileNameWithoutExtension(path) == L"abc", L"File name without extension does not match abc");
    
    path = L".txt";
    
    TEST_ASSERT(Path::GetDirectoryName(path) == L"", L"Directory name should be empty");
    TEST_ASSERT(Path::GetFileName(path) == L".txt", L"File name should be .txt");
    TEST_ASSERT(Path::GetFileNameWithoutExtension(path) == L"", L"File name without extension should be empty");
}

END_TEST_CLASS
