#include <Pseudo\AutoPtr.hpp>

using namespace Pseudo;

BEGIN_TEST_CLASS(TestAutoPtr)

    BEGIN_TEST_METHODS
        TEST_METHOD(Test)
        TEST_METHOD(TestCopyConstructor)
    END_TEST_METHODS

    class ThingBase
    {
    public:
        ThingBase(int b) { m_b = b; }
        ThingBase() { m_b = 1; }
        virtual ~ThingBase() {}
        
        int m_b;
    };

    class Thing : public ThingBase
    {
    public:
        Thing(int t) : ThingBase(t) { m_t = t; }
        Thing() : ThingBase() { m_t = 2; }
        
        int m_t;
    };

    void UseAThingByRef(const AutoPtr<ThingBase>& rt)
    {
        rt->m_b = 60;
    }

    AutoPtr<ThingBase> UseAThingByValue(AutoPtr<ThingBase> t)
    {
        t->m_b = 60;
        
        return t;
    }

    void Test()
    {
        // initialization 	
        AutoPtr<Thing> p(PSEUDO_NEW Thing);
        AutoPtr<Thing> p2(PSEUDO_NEW Thing);
        
        Thing* pp = p.get_Ptr();
        Thing* pp2 = p2.get_Ptr();
        
        // move constructor
        AutoPtr<Thing> p3(p2);

        TEST_ASSERT(p3.get_Ptr() == pp2, L"p3 should contain p2's previous value");
        TEST_ASSERT(p2.get_Ptr() == NULL, L"p2 should be null");

        Thing* pp3 = p3.get_Ptr();

        // assignment
        p = p3;

        TEST_ASSERT(p.get_Ptr() == pp3, L"p should contain p3's previous value");
        TEST_ASSERT(p3.get_Ptr() == NULL, L"p3 should be null");

        // explicit initialization
        AutoPtr<ThingBase> q(p);

        TEST_ASSERT(q.get_Ptr() == pp3, L"q should contain p3's previous value");
        
        // accessors
        (*q).m_b = 15;
        q->m_b = 30;
        q.get_Ptr()->m_b = 45;
        
        // pass by value
        q = UseAThingByValue(q);

        // pass by constant reference
        UseAThingByRef(q);
        
        // straight temporary object
        int* pInt;
        AutoPtr<int> w(AutoPtr<int>(pInt = PSEUDO_NEW int(3)));
        
        TEST_ASSERT(w.get_Ptr() == pInt, L"w should point to pInt");
        
        // const AutoArrayPtr's
        Thing* pThing;
        const AutoPtr<ThingBase> r(pThing = PSEUDO_NEW Thing(11));
        
        TEST_ASSERT(r.get_Ptr() == pThing, L"r should point to pThing");
        
        #ifdef _PSEUDO_COMPILE_TIME_TEST
        // this is a "by design" compile time error.  Cannot assign an const object to a non-const one.
        q = r;
        #endif
    }

void TestCopyConstructor()
{
    AutoArrayPtr<Char> pa1(PSEUDO_NEW Char);
    AutoArrayPtr<Char> pa2(PSEUDO_NEW Char);
    
    *pa1 = 'A'; 
    *pa2 = 'B';
    
    Math::Swap(pa1, pa2);
    
    TEST_ASSERT(*pa1 == 'B' && *pa2 == 'A', L"Expected *pa1 == 'B' && *pa2 == 'A' after swap"); 
}

END_TEST_CLASS
