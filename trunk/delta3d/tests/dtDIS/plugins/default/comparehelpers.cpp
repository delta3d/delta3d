#include <UnitTest_Plugin_EntityStatePdu/CompareHelpers.h>

#include <dtGame/actorupdatemessage.h>
#include <cppunit/extensions/HelperMacros.h>


void dtTest::CompareParams(const std::string& name,
                           const dtGame::ActorUpdateMessage& should_be,
                           const dtGame::ActorUpdateMessage& truth)
{
   const dtGame::MessageParameter* mpsb = should_be.GetUpdateParameter(name);
   const dtGame::MessageParameter* mpis = truth.GetUpdateParameter(name);
   CPPUNIT_ASSERT_EQUAL( mpsb->ToString() , mpis->ToString() );
}
