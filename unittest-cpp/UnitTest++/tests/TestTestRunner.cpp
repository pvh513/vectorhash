#include "../UnitTest++.h"
#include "RecordingReporter.h"
#include "../ReportAssert.h"
#include "../TestList.h"
#include "../TimeHelpers.h"
#include "../TimeConstraint.h"
#include "../ReportAssertImpl.h"

using namespace UnitTest;

namespace
{

   struct MockTest : public Test
   {
      MockTest(char const* testName, bool const success_, bool const assert_, int const count_ = 1)
         : Test(testName)
         , success(success_)
         , asserted(assert_)
         , count(count_)
      {}

      virtual void RunImpl() const
      {
         TestResults& testResults_ = *CurrentTest::Results();

         for (int i=0; i < count; ++i)
         {
            if (asserted)
            {
               ReportAssert("desc", "file", 0);
            }
            else if (!success)
            {
               testResults_.OnTestFailure(m_details, "message");
            }
         }
      }

      bool const success;
      bool const asserted;
      int const count;
   };

   struct FixtureBase
   {
      FixtureBase()
         : runner(reporter)
      {}

      template <class Predicate>
      int RunTestsIf(TestList const& list, char const* suiteName,
                     const Predicate& predicate, int maxTestTimeInMs)
      {
         TestResults* oldResults = CurrentTest::Results();
         const TestDetails* oldDetails = CurrentTest::Details();
         int result = runner.RunTestsIf(list, suiteName, predicate, maxTestTimeInMs);
         CurrentTest::Results() = oldResults;
         CurrentTest::Details() = oldDetails;
         return result;
      }

      TestRunner runner;
      RecordingReporter reporter;
   };

   struct TestRunnerFixture : public FixtureBase
   {
      TestList list;
   };

   TEST_FIXTURE(TestRunnerFixture, TestStartIsReportedCorrectly)
   {
      MockTest test("goodtest", true, false);
      list.Add(&test);

      RunTestsIf(list, NULL, True(), 0);
      CHECK_EQUAL(1, reporter.testRunCount);
      CHECK_EQUAL("goodtest", reporter.lastStartedTest);
   }

   TEST_FIXTURE(TestRunnerFixture, TestFinishIsReportedCorrectly)
   {
      MockTest test("goodtest", true, false);
      list.Add(&test);

      RunTestsIf(list, NULL, True(), 0);
      CHECK_EQUAL(1, reporter.testFinishedCount);
      CHECK_EQUAL("goodtest", reporter.lastFinishedTest);
   }

   class SlowTest : public Test
   {
   public:
      SlowTest()
         : Test("slow", "somesuite", "filename", 123)
      {}

      virtual void RunImpl() const
      {
         TimeHelpers::SleepMs(20);
      }
   };

   TEST_FIXTURE(TestRunnerFixture, TestFinishIsCalledWithCorrectTime)
   {
      SlowTest test;
      list.Add(&test);

      // Using UnitTest::Timer here is arguably a bit hokey and self-referential, but
      // it should guarantee that the test time recorded is less than that plus the
      // overhead of RunTestsIf -- the only thing we can reliably assert without
      // reworking the test to not use sleeps at all
      Timer actual;
      actual.Start();
      RunTestsIf(list, NULL, True(), 0);

      CHECK(reporter.lastFinishedTestTime >= 0.005f && reporter.lastFinishedTestTime <= actual.GetTimeInMs());
   }

   TEST_FIXTURE(TestRunnerFixture, FailureCountIsZeroWhenNoTestsAreRun)
   {
      CHECK_EQUAL(0, RunTestsIf(list, NULL, True(), 0));
      CHECK_EQUAL(0, reporter.testRunCount);
      CHECK_EQUAL(0, reporter.testFailedCount);
   }

   TEST_FIXTURE(TestRunnerFixture, CallsReportFailureOncePerFailingTest)
   {
      MockTest test1("test", false, false);
      list.Add(&test1);
      MockTest test2("test", true, false);
      list.Add(&test2);
      MockTest test3("test", false, false);
      list.Add(&test3);

      CHECK_EQUAL(2, RunTestsIf(list, NULL, True(), 0));
      CHECK_EQUAL(2, reporter.testFailedCount);
   }

   TEST_FIXTURE(TestRunnerFixture, TestsThatAssertAreReportedAsFailing)
   {
      MockTest test("test", true, true);
      list.Add(&test);

      RunTestsIf(list, NULL, True(), 0);
      CHECK_EQUAL(1, reporter.testFailedCount);
   }


   TEST_FIXTURE(TestRunnerFixture, ReporterNotifiedOfTestCount)
   {
      MockTest test1("test", true, false);
      MockTest test2("test", true, false);
      MockTest test3("test", true, false);
      list.Add(&test1);
      list.Add(&test2);
      list.Add(&test3);

      RunTestsIf(list, NULL, True(), 0);
      CHECK_EQUAL(3, reporter.summaryTotalTestCount);
   }

   TEST_FIXTURE(TestRunnerFixture, ReporterNotifiedOfFailedTests)
   {
      MockTest test1("test", false, false, 2);
      MockTest test2("test", true, false);
      MockTest test3("test", false, false, 3);
      list.Add(&test1);
      list.Add(&test2);
      list.Add(&test3);

      RunTestsIf(list, NULL, True(), 0);
      CHECK_EQUAL(2, reporter.summaryFailedTestCount);
   }

   TEST_FIXTURE(TestRunnerFixture, ReporterNotifiedOfFailures)
   {
      MockTest test1("test", false, false, 2);
      MockTest test2("test", true, false);
      MockTest test3("test", false, false, 3);
      list.Add(&test1);
      list.Add(&test2);
      list.Add(&test3);

      RunTestsIf(list, NULL, True(), 0);
      CHECK_EQUAL(5, reporter.summaryFailureCount);
   }

   TEST_FIXTURE(TestRunnerFixture, SlowTestPassesForHighTimeThreshold)
   {
      SlowTest test;
      list.Add(&test);

      RunTestsIf(list, NULL, True(), 0);
      CHECK_EQUAL(0, reporter.testFailedCount);
   }

   TEST_FIXTURE(TestRunnerFixture, SlowTestFailsForLowTimeThreshold)
   {
      SlowTest test;
      list.Add(&test);

      RunTestsIf(list, NULL, True(), 3);
      CHECK_EQUAL(1, reporter.testFailedCount);
   }

   TEST_FIXTURE(TestRunnerFixture, SlowTestHasCorrectFailureInformation)
   {
      SlowTest test;
      list.Add(&test);

      RunTestsIf(list, NULL, True(), 3);

      using namespace std;

      CHECK_EQUAL(test.m_details.testName, reporter.lastFailedTest);
      CHECK(strstr(test.m_details.filename, reporter.lastFailedFile));
      CHECK_EQUAL(test.m_details.lineNumber, reporter.lastFailedLine);

      CHECK(strstr(reporter.lastFailedMessage, "Global time constraint failed"));
      CHECK(strstr(reporter.lastFailedMessage, "3ms"));
   }

   TEST_FIXTURE(TestRunnerFixture, SlowTestWithTimeExemptionPasses)
   {
      class SlowExemptedTest : public Test
      {
      public:
         SlowExemptedTest() : Test("slowexempted", "", 0) {}
         virtual void RunImpl() const
         {
            UNITTEST_TIME_CONSTRAINT_EXEMPT();
            TimeHelpers::SleepMs(20);
         }
      };

      SlowExemptedTest test;
      list.Add(&test);

      RunTestsIf(list, NULL, True(), 3);
      CHECK_EQUAL(0, reporter.testFailedCount);
   }

   struct TestSuiteFixture : FixtureBase
   {
      TestSuiteFixture()
         : test1("TestInDefaultSuite")
         , test2("TestInOtherSuite", "OtherSuite")
         , test3("SecondTestInDefaultSuite")
      {
         list.Add(&test1);
         list.Add(&test2);
      }

      Test test1;
      Test test2;
      Test test3;
      TestList list;
   };

   TEST_FIXTURE(TestSuiteFixture, TestRunnerRunsAllSuitesIfNullSuiteIsPassed)
   {
      RunTestsIf(list, NULL, True(), 0);
      CHECK_EQUAL(2, reporter.summaryTotalTestCount);
   }

   TEST_FIXTURE(TestSuiteFixture,TestRunnerRunsOnlySpecifiedSuite)
   {
      RunTestsIf(list, "OtherSuite", True(), 0);
      CHECK_EQUAL(1, reporter.summaryTotalTestCount);
      CHECK_EQUAL("TestInOtherSuite", reporter.lastFinishedTest);
   }

   struct RunTestIfNameIs
   {
      RunTestIfNameIs(char const* name_)
         : name(name_)
      {}

      bool operator()(const Test* const test) const
      {
         using namespace std;
         return (0 == strcmp(test->m_details.testName, name));
      }

      char const* name;
   };

   TEST(TestMockPredicateBehavesCorrectly)
   {
      RunTestIfNameIs predicate("pass");

      Test pass("pass");
      Test fail("fail");

      CHECK(predicate(&pass));
      CHECK(!predicate(&fail));
   }

   TEST_FIXTURE(TestRunnerFixture, TestRunnerRunsTestsThatPassPredicate)
   {
      Test should_run("goodtest");
      list.Add(&should_run);

      Test should_not_run("badtest");
      list.Add(&should_not_run);

      RunTestsIf(list, NULL, RunTestIfNameIs("goodtest"), 0);
      CHECK_EQUAL(1, reporter.testRunCount);
      CHECK_EQUAL("goodtest", reporter.lastStartedTest);
   }

   TEST_FIXTURE(TestRunnerFixture, TestRunnerOnlyRunsTestsInSpecifiedSuiteAndThatPassPredicate)
   {
      Test runningTest1("goodtest", "suite");
      Test skippedTest2("goodtest");
      Test skippedTest3("badtest", "suite");
      Test skippedTest4("badtest");

      list.Add(&runningTest1);
      list.Add(&skippedTest2);
      list.Add(&skippedTest3);
      list.Add(&skippedTest4);

      RunTestsIf(list, "suite", RunTestIfNameIs("goodtest"), 0);

      CHECK_EQUAL(1, reporter.testRunCount);
      CHECK_EQUAL("goodtest", reporter.lastStartedTest);
      CHECK_EQUAL("suite", reporter.lastStartedSuite);
   }

}
