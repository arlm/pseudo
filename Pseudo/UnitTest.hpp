#pragma once

#pragma warning(push)
#include <Pseudo\Compiler.hpp>
#include <Pseudo\String.hpp>
#include <Pseudo\ArrayList.hpp>
#include <Pseudo\Macros.hpp>
#include <Pseudo\Exception.hpp>
#include <Pseudo\StackWalk.hpp>
#include <Pseudo\Encoding.hpp>
#include <Pseudo\StreamWriter.hpp>
#include <Pseudo\StringBuilder.hpp>

#pragma pack(8)

namespace Pseudo
{
    class TestException : public Exception
    {
        public: TestException()
        {
            StackWalker::get_StackWalker().GetStack(stack);
        }

        public: TestException(__in_z const Char* pMessage)
        {
            StackWalker::get_StackWalker().GetStack(stack);
            Exception::set_Message(pMessage);
        }

        public: TestException(TestException const & rhs)
        {
            set_Message(rhs.get_Message());

            if (rhs.stack.get_Count() > 0)
            {
                rhs.stack.CopyTo(this->stack);
            }
        }

        public: const ArrayList<StackFrame>& Stack() const
        {
            return this->stack;
        }

        private: ArrayList<StackFrame> stack;
    };

    /// <summary>
    /// The following is a simple unit testing framework that is used to provide unit tests for Pseudo
    /// </summary>

    /// Test the assertion and throw a TestException if not true
    #define TEST_ASSERT(x, m)	if (!(x)) { throw TestException(m); }
    /// Try something that might throw any exception and rethrow a TestException if it does
    #define TEST_CATCH_EXCEPTION(x, m)	try { (x); } catch (...) { throw TestException(m); }

    /// Use this to begin the definition of a test class
    #define BEGIN_TEST_CLASS(x) \
        class x : public TestClass \
        { \
            private: Char const * name; \
            public: x() : name(L#x) {} \
            public: Char const * GetName() { return name; }

    /// Use this to end the definition of a test class
    #define END_TEST_CLASS \
        };

    /// Use this to begin the definition of test case methods
    #define BEGIN_TEST_METHODS \
        public: virtual void RunAllTests(TestClassRunner *pRunner) \
            {

    /// Use this to end the definition of test case methods
    #define END_TEST_METHODS \
            }

    /// Use this to define a test case method
    #define TEST_METHOD(x) \
            pRunner->StartTestMethod(L#x); \
            try { x(); pRunner->EndTestMethod(); } \
            catch(TestException& e) { pRunner->EndTestMethod(&e); }

    /// Use this pseudo method macro to correctly add a class to a TestClassRunner
    #define ADD_CLASS(x) AddClass(PSEUDO_NEW x())

    class TestClassRunner;

    /// \brief Test class interface
    ///
    /// All test classes should derive from this interface.
    class TestClass
    {
        // TestClass interface.  Don't add any members to this or things will break - you'll
        // have to add a copy constructor.

        /// Get the name of the TestClass
        public: virtual Char const * GetName() = 0;
        /// \brief Run all the tests contained in the class
        ///
        /// \param pRunner the TestClassRunner that is running the tests
        public: virtual void RunAllTests(TestClassRunner* pRunner) = 0;
    };

    Char const tabChar = (Char)'\t';

    /// <summary>
    /// Runs a collection of test classes
    ///
    /// You add test classes to this class, then call the Run() method to execute the tests.
    /// </summary>
    class TestClassRunner
    {
        protected:	class RunClass
        {
            private: TestClassRunner *pRunner;

            public: RunClass(TestClassRunner *pRunner)
            {
                this->pRunner = pRunner;
            }

            void operator() (TestClass* pClass)
            {
                pRunner->StartClass();
                pClass->RunAllTests(pRunner);
                pRunner->EndClass();
            }
        };

        // The results of one test.
        protected: struct MethodResult
        {
            MethodResult()
            {
                time = 0;
            }

            MethodResult(const Char* pName)
            {
                name = pName;
                time = TestClassRunner::GetPerfCount();
            }

            MethodResult(MethodResult const &rhs)
            {
                operator=(rhs);
            }

            MethodResult& operator=(MethodResult const &rhs)
            {
                name = rhs.name;
                time = rhs.time;
                error = const_cast<MethodResult&>(rhs).error; // We need to remove the constness to make this work.

                return *this;
            }

            String name;
            Int64 time;
            AutoPtr<TestException> error;
        };

        // There is one ClassResult for each TestClass and they are at the
        // same ordinal position in their respective lists
        protected: struct ClassResult
        {
			ClassResult()
			{
				time = TestClassRunner::GetPerfCount();
			}
			ClassResult(ClassResult const & rhs) : time(rhs.time), methodResults(rhs.methodResults.get_Count())
			{
				rhs.methodResults.CopyTo(methodResults);
			}
			
			ClassResult& operator =(const ClassResult& other)
			{
          time = other.time;
          other.methodResults.CopyTo(methodResults);
          return *this;
			}

			Long time;
			ArrayList<MethodResult> methodResults;
        };

        protected: static void DeleteClass(TestClass* p)
        {
            delete p;
        }

        protected: static bool DidTestFail(const MethodResult &test)
        {
            return (test.error.get_Ptr() != 0);
        }

        // Methods for printing the Results as nicely formatted XML
        protected: static const Char* GetTabs(Int n)
        {
            static String tabs;

            if (tabs.get_Length() < n)
                tabs = String(tabChar, n);

            return tabs.get_Ptr() + tabs.get_Length() - n;
        }

        protected: static double AsSeconds(Int64 ticks)
        {
            static Int64 freq = 0;

            if (freq == 0)
                ::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&freq));

            double t = (double)ticks / (double)freq;

            return t < 0.001 ? 0.001 : t;
        }

        protected: static Char* GetTrueFalse(bool b)
        {
            return (b ? L"True" : L"False");
        }

        // Copies the characters and returns the end of the 'to' String
		protected: static Char * CopyChars(__out_z Char *to, __in_z const Char *from)
        {
            while (*from)
                *to++ = *from++;

            return to;
        }

        protected: static String XmlSafeString(__in_z Char const *pFrom)
        {
            const int MAX_SYMBOL_NAME_LEN_EXTRA = MAX_SYMBOL_NAME_LEN * 2;
            StringBuilder sb(MAX_SYMBOL_NAME_LEN_EXTRA);
            Char lt[] = L"&lt;";
            Char gt[] = L"&gt;";
            Char amp[] = L"&amp;";

            while (*pFrom)
            {
                if (*pFrom == '<')
                    sb.Append(lt);
                else if (*pFrom == '>')
                    sb.Append(gt);
                else if (*pFrom == '&')
                    sb.Append(amp);
                else
                    sb.Append(*pFrom);

                pFrom++;
            }

            return sb.ToString();
        }

        protected: void PrintTestFailure(Int& indent, TestException& e) const
        {
            pFile->WriteLine(L"%s<Failure>", GetTabs(indent++));

            pFile->WriteLine(L"%s<Message>%s</Message>"
                ,
                GetTabs(indent),
                (const Char *)XmlSafeString(e.get_Message()));

            pFile->WriteLine(L"%s<Stack>", GetTabs(indent++));

            for (Int i = 0; i < e.Stack().get_Count(); i++)
            {
                const StackFrame& frame = e.Stack()[i];

                pFile->WriteLine(
                    L"%s<StackFrame "
                    L"ModuleName=\"%s\" "
                    L"FileName=\"%s\" "
                    L"MethodName=\"%s\" "
                    L"Line=\"%d\" "
                    L"Address=\"%u\" "
                    L"Offset=\"%u\" "
                    L"/>"
                    ,
                    GetTabs(indent),
                    (const Char*)frame.moduleName,
                    (const Char*)frame.fileName,
                    XmlSafeString(frame.methodName.get_Ptr()).get_Ptr(),
                    frame.line,
                    (IntPtr)frame.address,
                    frame.offset);
            }

            pFile->WriteLine(L"%s</Stack>", GetTabs(--indent));
            pFile->WriteLine(L"%s</Failure>", GetTabs(--indent));
        }

        protected: Int PrintResults()
        {
            Int indent = 0;
            Int totalFailures = 0;
            Array<Int> classFailures(classResults.get_Count());

            for (Int i = 0; i < classResults.get_Count(); i++)
            {
                ClassResult& cr = classResults[i];

                Int n = cr.methodResults.CountIf(0, cr.methodResults.get_Count(), DidTestFail);

                classFailures[i] = n;
                totalFailures += n;
            }

            SYSTEMTIME st;

            ::GetLocalTime(&st);

            String testName = Path::GetFileName(Environment::get_ModuleFileName()).ToLower();

            pFile->WriteLine(
                L"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>\n"
                L"<!--This file represents the results of running a collection of test classes-->\n"
                L"%s<TestResults Name=\"%s\" Total=\"%u\" Failures=\"%u\" NotRun=\"%u\" Date=\"%u/%u/%u\" Time=\"%02u:%02u\">"
                ,
                GetTabs(indent++),
                (const Char*)testName,
                classes.get_Count(), // Total
                totalFailures, // Failures
                0, // NotRun
                st.wMonth, st.wDay, st.wYear, // Date
                st.wHour, st.wMinute); // Time

            for (Int i = 0; i < this->classes.get_Count(); i++)
            {
                ClassResult& cr = this->classResults[i];
                TestClass* tc = this->classes[i];

                pFile->WriteLine(
                    L"%s<TestClass Name=\"%s\" Success=\"%s\" Time=\"%.3f\">"
                    ,
                    GetTabs(indent++),
                    tc->GetName(),
                    GetTrueFalse(classFailures[i] == 0),
                    AsSeconds(cr.time));

                pFile->WriteLine(
                    L"%s<Results>",
                    GetTabs(indent++));

                // Print the test case Results
                for (Int test = 0; test < cr.methodResults.get_Count(); test++)
                {
                    MethodResult& mr = cr.methodResults[test];

                    if (mr.error.get_Ptr() == 0)
                    {
                        pFile->WriteLine(
                            L"%s<TestMethod Name=\"%s\" Executed=\"%s\" Success=\"%s\" Time=\"%.3f\"/>"
                            ,
                            GetTabs(indent),
                            (const Char*)mr.name,
                            GetTrueFalse(true),
                            GetTrueFalse(mr.error.get_Ptr() == 0),
                            AsSeconds(mr.time));
                    }
                    else
                    {
                        pFile->WriteLine(
                            L"%s<TestMethod Name='%s' Executed=\"%s\" Success=\"%s\" Time=\"%.3f\">"
                            ,
                            GetTabs(indent++),
                            (const Char*)mr.name,
                            GetTrueFalse(true),
                            GetTrueFalse(mr.error.get_Ptr() == 0),
                            AsSeconds(mr.time));

                        PrintTestFailure(indent, *mr.error.get_Ptr());

                        pFile->WriteLine(L"%s</TestMethod>", GetTabs(--indent));
                    }
                }

                pFile->WriteLine(
                    L"%s</Results>"
                    ,
                    GetTabs(--indent));

                pFile->WriteLine(
                    L"%s</TestClass>"
                    ,
                    GetTabs(--indent));
            }

            pFile->WriteLine(L"%s</TestResults>", GetTabs(--indent));

            return totalFailures;
        }

    // Data
        protected: TextWriter* pFile;
        protected: int activeClass;
        protected: ArrayList<TestClass*> classes;
        protected: bool autoDelete;
        protected: ArrayList<ClassResult> classResults;

        /// Constructor
        public: TestClassRunner(TextWriter* pFile, bool autoDelete = true)
        {
            this->pFile = pFile;
            this->autoDelete = autoDelete;
        }

        public: virtual ~TestClassRunner()
        {
            if (autoDelete)
                classes.ForEach(0, classes.get_Count() - 1, DeleteClass);
        }

        public: void AddClass(TestClass* pClass)
        {
            classes.Add(pClass);
        }

        /// Runs a set of test classes
        ///
        /// \return The number of failed tests
        public: Int Run()
        {
            DEBUG_ASSERT(pFile != NULL);

            activeClass = 0;
            ClassResult cr;
            while(classResults.get_Count() < classes.get_Count())
            {
                classResults.Add(cr);
            }

            classes.ForEach(0, classes.get_Count() - 1, RunClass(this));

            return PrintResults();
        }

        public: static Int64 GetPerfCount()
        {
            Int64 t;
            ::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&t));
            return t;
        }

        public: void StartClass()
        {
            classResults.Add(ClassResult());
        }

        public: void EndClass()
        {
            ClassResult& cr = classResults[activeClass];

            cr.time = GetPerfCount() - cr.time;

            activeClass++;
        }

        public: void StartTestMethod(Char const *pName)
        {
            ClassResult& cr = classResults[activeClass];

            cr.methodResults.Add(MethodResult(pName));;
        }

        void EndTestMethod(TestException* pError = NULL)
        {
            ClassResult& cr = classResults[activeClass];
            MethodResult& mr = cr.methodResults[cr.methodResults.get_Count() - 1];

            mr.time = GetPerfCount() - mr.time;

            if (pError != NULL)
                mr.error.Reset(PSEUDO_NEW TestException(*pError));
        }
    };
}

#pragma pack()
#pragma warning(pop)
