#include <prefix/unittestprefix.h>

#include "comparehelpers.h"

#include <dtGame/actorupdatemessage.h>
#include <cppunit/extensions/HelperMacros.h>


void dtTest::CompareParams(const std::string& name,
                           const dtGame::ActorUpdateMessage& should_be,
                           const dtGame::ActorUpdateMessage& truth)
{
   const dtGame::MessageParameter* mpsb = should_be.GetUpdateParameter(name);
   const dtGame::MessageParameter* mpis = truth.GetUpdateParameter(name);

   std::string msg = "ActorUpdateMessage doesn't contain the parameter '" + name + "'";

   CPPUNIT_ASSERT_MESSAGE(msg, mpsb != NULL);
   CPPUNIT_ASSERT_MESSAGE(msg, mpis != NULL);
   CPPUNIT_ASSERT_EQUAL( mpsb->ToString() , mpis->ToString() );
}
