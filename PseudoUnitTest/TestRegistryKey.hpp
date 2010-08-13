#include <Pseudo\RegistryKey.hpp>
#include <Pseudo\Registry.hpp>

using namespace Pseudo;

BEGIN_TEST_CLASS(TestRegistryKey)

BEGIN_TEST_METHODS
	TEST_METHOD(Test)
END_TEST_METHODS

void Test()
{
    NativeMethods::set_TestMode(NativeMethods::TestMode::AlwaysSucceed);

    RegistryKey* pKey = Registry::get_LocalMachine().OpenSubKey(L"Anything", RegistryKey::PermissionCheck::ReadSubTree);
    
    TEST_ASSERT(pKey != NULL, L"Unable to open registry key");
    
    pKey->Close();

    NativeMethods::set_TestMode(NativeMethods::TestMode::Default);
}

END_TEST_CLASS
