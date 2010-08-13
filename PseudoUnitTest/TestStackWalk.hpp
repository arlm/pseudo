#include <Pseudo\StackWalk.hpp>

using namespace Pseudo;

BEGIN_TEST_CLASS(TestStackWalk)

BEGIN_TEST_METHODS
	TEST_METHOD(Test)
END_TEST_METHODS

void C()
{
// TODO-johnls-3/24/2008: Why doesn't this work in release builds...?
#if DEBUG
	ArrayList<StackFrame> stack;
	StackWalker::get_StackWalker().GetStack(stack);
	
	for (Int i = 0; i < stack.get_Count(); i++)
	{
	    TEST_ASSERT(
	        (i < 4 && *stack[i].methodName != 0) || (i >= 4), 
	        "Expect method names for at least first four methods on stack");
		//Console::WriteLine(stack[i].methodName);
	}
#endif
}

void B()
{
	C();
}

void A()
{
	B();
}

void Test()
{
	A();
}

END_TEST_CLASS
