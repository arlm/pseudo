#include <Pseudo\ComPtr.hpp>

using namespace Pseudo;

interface __declspec(uuid("b4bc601d-e15c-404c-bb71-2eb2fe080498")) IOne : IUnknown
{
    HRESULT One();
};

interface __declspec(uuid("241c5782-8fd2-469a-b1fe-2acdff8153eb")) ITwo : IUnknown
{
    HRESULT Two();
};

BEGIN_TEST_CLASS(TestComPtr)

#pragma warning(push)
#pragma warning(disable:4584)  // base-class 'IUnknown' is already a base-class of 'IOne'

    class OneTwo : public IOne, public ITwo, public IUnknown
    {
        public: OneTwo() 
        { 
            ref = 0; 
            bogus = 0xdeadbeef;
        }
        
        // These are public so they can be checked/manipulated as part of a test
        public: ULONG ref; 
        public: ULONG bogus;
        
        public: HRESULT _stdcall QueryInterface(REFIID riid, void** ppvObject) 
        { 
            if (riid == IID_IUnknown)
            {
                this->AddRef();
               *ppvObject = static_cast<IUnknown*>(this);	
                return S_OK;
            }
            else if (riid == __uuidof(IOne))
            {
                this->AddRef();
               *ppvObject = static_cast<IOne*>(this);	
                return S_OK;
            }
            else if (riid == __uuidof(ITwo))
            {
                this->AddRef();
               *ppvObject = static_cast<ITwo*>(this);	
                return S_OK;
            }
            else
            {
               *ppvObject = NULL;                  
                return E_NOINTERFACE;
            }
        }
        public: ULONG _stdcall AddRef(void) { return ++ref; }
        public: ULONG _stdcall Release(void) { return --ref; }
        public: HRESULT _stdcall One() { return S_OK; }
        public: HRESULT _stdcall Two() { return S_OK; }
    };

#pragma warning(pop)

    BEGIN_TEST_METHODS
	    TEST_METHOD(Test)
	    TEST_METHOD(Test2)
    END_TEST_METHODS

    void Test()
    {
        ComPtr<IUnknown> pUnk(NULL);
        
        TEST_ASSERT(pUnk == NULL, L"pUnk == NULL");
        
        IOne* piOne;
        ITwo* piTwo;
        IUnknown* piUnk1;
        IUnknown* piUnk2;
        IUnknown* piUnk3;
        ComPtr<IOne> pOne;
        ComPtr<ITwo> pTwo;
        ComPtr<IUnknown> pUnk1;
        ComPtr<IUnknown> pUnk2;
        ComPtr<IUnknown> pUnk3;
        OneTwo oneTwo;
        
        piOne = (IOne*)&oneTwo;
        pOne = piOne;
        piTwo = &oneTwo; // No explicit cast
        pTwo = piTwo;
        piUnk1 = (IUnknown*)(IOne*)&oneTwo;
        pUnk1 = piUnk1;
        piUnk2 = (IUnknown*)(ITwo*)&oneTwo;
        pUnk2 = piUnk2;
        piUnk3 = &oneTwo;
        pUnk3 = piUnk3;

        // Test that the IUnknown's are different so the test is valid
        TEST_ASSERT(piUnk1 != piUnk2, L"piUnk1 != piUnk2");
        
        // Even though the IUnkowns can be different, QI will always return the same one
        TEST_ASSERT(pUnk1.Equals(pUnk2), L"pUnk1.Equals(pUnk2)");
        TEST_ASSERT(pUnk2.Equals(pUnk3), L"pUnk2.Equals(pUnk3)");
        TEST_ASSERT(pUnk3.Equals(pUnk1), L"pUnk3.Equals(pUnk1)");
        
        TEST_ASSERT(piOne == pOne, L"piOne == pOne");
        TEST_ASSERT(piUnk1 == pOne, L"piTwo == pTwo");
        TEST_ASSERT(piTwo == pTwo, L"piTwo == pTwo");
        TEST_ASSERT(piUnk2 == pTwo, L"piUnk2 == pTwo");
    }
    
    void Test2()
    {
        OneTwo oneTwo;
        
        oneTwo.ref = 1;  // Pretend there is one reference already
        
        // This should call the copy constructor using an interface pointer
        ComPtr<IOne> piOne = (IOne*)&oneTwo;

        // Ensure that the copy constructor was called        
        TEST_ASSERT(oneTwo.ref == 2, L"oneTwo.ref == 2");
        
        // This should call the copy constructor using another ComPtr
        ComPtr<IOne> piOneT;
        
        piOneT = piOne;

        // Ensure that the copy constructor was called        
        TEST_ASSERT(oneTwo.ref == 3, L"oneTwo.ref == 3");

        ComPtr<IOne> piOneNull;
        ComPtr<IOne> piOneQ(piOneNull);
        
        // Ensure that piOneQ is NULL
        TEST_ASSERT(piOneQ == NULL, L"piOneQ == NULL");
    }

END_TEST_CLASS
