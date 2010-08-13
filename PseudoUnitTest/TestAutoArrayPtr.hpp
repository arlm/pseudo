#include <Pseudo\AutoArrayPtr.hpp>

using namespace Pseudo;

BEGIN_TEST_CLASS(TestAutoArrayPtr)

    BEGIN_TEST_METHODS
        TEST_METHOD(Test)
        TEST_METHOD(TestCopyConstructor)
    END_TEST_METHODS

    class ThingBase
    {
    public:
        ThingBase() { m_b = 1; }
        virtual ~ThingBase() {}
        
        int m_b;
    };

    class Thing : public ThingBase
    {
    public:
        Thing() : ThingBase() { m_t = 2; }
        
        int m_t;
    };

    void UseAThingByRef(const AutoArrayPtr<ThingBase>& rt)
    {
        rt[0].m_b = 60;
    }

    AutoArrayPtr<ThingBase> UseAThingByValue(AutoArrayPtr<ThingBase> t)
    {
        t[0].m_b = 60;
        
        return t;
    }

    void Test()
    {
        // initialization 	
        AutoArrayPtr<Thing> p(PSEUDO_NEW Thing[10]);
        AutoArrayPtr<Thing> p2(PSEUDO_NEW Thing[20]);

        Thing* pp = p.get_Ptr();
        Thing* pp2 = p2.get_Ptr();
        
        // move constructor
        AutoArrayPtr<Thing> p3(p2);

        TEST_ASSERT(p3.get_Ptr() == pp2, L"p3 should contain p2's previous value");
        TEST_ASSERT(p2.get_Ptr() == NULL, L"p2 should be null");

        Thing* pp3 = p3.get_Ptr();

        // assignment
        p = p3;

        TEST_ASSERT(p.get_Ptr() == pp3, L"p should contain p3's previous value");
        TEST_ASSERT(p3.get_Ptr() == NULL, L"p3 should be null");

        // explicit initialization
        AutoArrayPtr<ThingBase> q(p);

        TEST_ASSERT(q.get_Ptr() == pp3, L"q should contain p3's previous value");
        
        // accessors
        (*q).m_b = 15;
        q[1].m_b = 30;
        q.get_Ptr()->m_b = 45;
        
        // pass by value
        q = UseAThingByValue(q);

        // pass by constant reference
        UseAThingByRef(q);
        
        // straight temporary object
        int* pInts;
        AutoArrayPtr<int> w(AutoArrayPtr<int>(pInts = PSEUDO_NEW int[5]));
        
        TEST_ASSERT(w.get_Ptr() == pInts, L"w should point to pInts");
        
        // const AutoArrayPtr's
        Thing* pThings;
        const AutoArrayPtr<ThingBase> r(pThings = PSEUDO_NEW Thing[100]);
        
        TEST_ASSERT(r.get_Ptr() == pThings, L"r.get_Ptr() should point to pThings");
        
        #ifdef _PSEUDO_COMPILE_TIME_TEST
        // this is a "by design" compile time error.  Cannot assign an const object to a non-const one.
        q = r;
        #endif
    }

    void TestCopyConstructor()
    {
        AutoArrayPtr<Char> pa1(PSEUDO_NEW Char[16]);
        AutoArrayPtr<Char> pa2(PSEUDO_NEW Char[16]);
        
        *pa1 = 'A'; 
        *pa2 = 'B';
        
        Math::Swap(pa1, pa2);
        
        TEST_ASSERT(*pa1 == 'B' && *pa2 == 'A', L"Expected *pa1 == 'B' && *pa2 == 'A' after swap"); 
    }

END_TEST_CLASS
