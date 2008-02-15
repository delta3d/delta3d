#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/globals.h>

namespace dtTest
{
   /// unit tests for dtCore::Axis
   class GlobalsTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( GlobalsTests );
      CPPUNIT_TEST( TestEnvironment );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp()
         {}
         void tearDown()
         {}

         /// tests handling and order of handling of multiple listeners for state changes.
         void TestEnvironment()
         {
            std::string result = dtCore::GetEnvironment("nothing");
            CPPUNIT_ASSERT_EQUAL_MESSAGE("An environment variable that doesn't exist should yield a result of ./.  "
                  "This is for historic reasons.", std::string("./"), result);
            dtCore::SetEnvironment("silly", "goose");
            result = dtCore::GetEnvironment("silly");
            CPPUNIT_ASSERT_EQUAL_MESSAGE("The environment variable \"silly\" should have the value \"goose\".", std::string("goose"), result);
         }
         
      private:
   };
}

CPPUNIT_TEST_SUITE_REGISTRATION( dtTest::GlobalsTests );
