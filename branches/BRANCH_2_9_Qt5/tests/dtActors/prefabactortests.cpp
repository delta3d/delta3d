#include "../dtGame/basegmtests.h"

class PrefabActorTests : public dtGame::BaseGMTestFixture
{
   CPPUNIT_TEST_SUITE(PrefabActorTests);
      CPPUNIT_TEST(TestCreateSaveAndLoad);
   CPPUNIT_TEST_SUITE_END();

public:
   void TestCreateSaveAndLoad()
   {

   }
private:

};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(PrefabActorTests);
