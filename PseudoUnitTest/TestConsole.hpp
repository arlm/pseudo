#include <Pseudo\Console.hpp>

using namespace Pseudo;

BEGIN_TEST_CLASS(TestConsole)

BEGIN_TEST_METHODS
	TEST_METHOD(Test)
END_TEST_METHODS

void Test()
{
	String s(L"Hello world");
	
	//Console::WriteLine(s);
	//Console::WriteLine(L"Mighty fine day");
	//Console::WriteLine(String(L"Mighty fine day"));
}

END_TEST_CLASS
