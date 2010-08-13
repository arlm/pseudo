#include <Pseudo\UnitTest.hpp>
#include "TestNew.hpp"
#include "TestMath.hpp"
#include "TestConsole.hpp"
#include "TestString.hpp"
#include "TestConvert.hpp"
#include "TestBitArray.hpp"
#include "TestDictionary.hpp"
#include "TestComPtr.hpp"
#include "TestArray.hpp"
#include "TestArrayList.hpp"
#include "TestStackWalk.hpp"
#include "TestPath.hpp"
#include "TestStringBuilder.hpp"
#include "TestRegistryKey.hpp"

using namespace Pseudo;

int _cdecl wmain(int argc, __in_ecount(argc) Char* argv[])
{
    TestClassRunner runner(Console::get_Out());

    runner.ADD_CLASS(TestNew);
    runner.ADD_CLASS(TestMath);
    runner.ADD_CLASS(TestConsole);
    runner.ADD_CLASS(TestString);
    runner.ADD_CLASS(TestStringBuilder);
    runner.ADD_CLASS(TestConvert);
    runner.ADD_CLASS(TestBitArray);
    runner.ADD_CLASS(TestDictionary);
    runner.ADD_CLASS(TestArray);
    runner.ADD_CLASS(TestArrayList);
    runner.ADD_CLASS(TestComPtr);
    runner.ADD_CLASS(TestPath);
    runner.ADD_CLASS(TestStackWalk);
    runner.ADD_CLASS(TestRegistryKey);

    size_t exitCode = runner.Run(); 

#ifdef DEBUG_WAIT_FOR_EXIT
    Console::WriteLine(L"\nPress <Enter> to exit...");
    Console::ReadKey();
#endif

    return (int)exitCode;
}

