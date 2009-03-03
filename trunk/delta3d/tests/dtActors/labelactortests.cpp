/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2006-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Chris Rodgers
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <vector>
#include <map>
#include <dtABC/application.h>
#include <dtABC/labelactor.h>
#include <dtActors/labelactorproxy.h>
#include <dtActors/engineactorregistry.h>
#include <dtCore/globals.h>
#include <dtCore/system.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/stringutils.h>

extern dtABC::Application& GetGlobalApplication();



////////////////////////////////////////////////////////////////////////////////
// TEST CLASS CODE
////////////////////////////////////////////////////////////////////////////////
class LabelActorTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(LabelActorTests);
      CPPUNIT_TEST(TestLabelActorProperties);
      CPPUNIT_TEST(TestLabelActorCreateActorProperties);
   CPPUNIT_TEST_SUITE_END();

   public:

      void setUp();
      void tearDown();

      // Test Methods
      void TestLabelActorProperties();
      void TestLabelActorCreateActorProperties();

   private:
      dtCore::RefPtr<dtActors::LabelActorProxy> mLabelProxy;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(LabelActorTests);

////////////////////////////////////////////////////////////////////////////////
void LabelActorTests::setUp()
{
   try
   {
      // Create the actor for testing.
      dtCore::RefPtr<dtDAL::ActorProxy> proxy = dtDAL::LibraryManager::GetInstance()
         .CreateActorProxy(*dtActors::EngineActorRegistry::LABEL_ACTOR_TYPE);
      mLabelProxy = dynamic_cast<dtActors::LabelActorProxy*>(proxy.get());

      // Validate the proxy.
      CPPUNIT_ASSERT_MESSAGE("Library Manager should be able to create a Label Actor Proxy",
         mLabelProxy.valid());

      // Start/restart the system.
      dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
      dtCore::System::GetInstance().Start();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

////////////////////////////////////////////////////////////////////////////////
void LabelActorTests::tearDown()
{
   // Shutdown the system.
   dtCore::System::GetInstance().SetPause(false);
   dtCore::System::GetInstance().Stop();

   // Clear all events
   mLabelProxy = NULL;
}

////////////////////////////////////////////////////////////////////////////////
void LabelActorTests::TestLabelActorProperties()
{
   using namespace dtActors;

   try
   {
      // Get the actor from the proxy.
      dtABC::LabelActor* actor = NULL;
      const dtABC::LabelActor* actorConst = NULL;
      mLabelProxy->GetActor(actor);
      actorConst = actor;

      // Test variables.
      std::string testString("Test Value");
      osg::Vec4 testTextColor(0.0f, 0.25f, 1.0f, 0.75f);
      osg::Vec4 testBackColor(1.0f, 0.5f, 0.75f, 0.25f);
      osg::Vec2 testSize(2.0f, 0.5f);

      // Test all the properties
      CPPUNIT_ASSERT(actorConst->GetText().empty());
      actor->SetText(testString);
      CPPUNIT_ASSERT(actorConst->GetText() == testString);

      CPPUNIT_ASSERT(actorConst->GetFontSize() == dtABC::LabelActor::DEFAULT_FONT_SIZE);
      actor->SetFontSize(5.0f);
      CPPUNIT_ASSERT(actorConst->GetFontSize() == 5.0f);

      CPPUNIT_ASSERT(actorConst->GetBackSize() == dtABC::LabelActor::DEFAULT_BACK_SIZE);
      CPPUNIT_ASSERT(actorConst->GetBackWidth() == dtABC::LabelActor::DEFAULT_BACK_SIZE.x());
      actor->SetBackWidth(testSize.x());
      CPPUNIT_ASSERT(actorConst->GetBackWidth() == testSize.x());

      CPPUNIT_ASSERT(actorConst->GetBackHeight() == dtABC::LabelActor::DEFAULT_BACK_SIZE.y());
      actor->SetBackHeight(testSize.y());
      CPPUNIT_ASSERT(actorConst->GetBackHeight() == testSize.y());

      CPPUNIT_ASSERT(actorConst->GetBackSize() == testSize);
      testSize *= 1.5f;
      actor->SetBackSize(testSize);
      CPPUNIT_ASSERT(actorConst->GetBackSize() == testSize);

      CPPUNIT_ASSERT(actorConst->GetTextColor() == dtABC::LabelActor::DEFAULT_COLOR_TEXT);
      actor->SetTextColor(testTextColor);
      CPPUNIT_ASSERT(actorConst->GetTextColor() == testTextColor);

      CPPUNIT_ASSERT(actorConst->GetBackColor() == dtABC::LabelActor::DEFAULT_COLOR_BACK);
      actor->SetBackColor(testBackColor);
      CPPUNIT_ASSERT(actorConst->GetBackColor() == testBackColor);

      CPPUNIT_ASSERT(actorConst->IsBackVisible());
      actor->SetBackVisible(false);
      CPPUNIT_ASSERT(!actorConst->IsBackVisible());

      CPPUNIT_ASSERT(actorConst->GetFont() == dtABC::LabelActor::DEFAULT_FONT.Get());
      actor->SetFont(testString);
      CPPUNIT_ASSERT(actorConst->GetFont() == testString);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

////////////////////////////////////////////////////////////////////////////////
void LabelActorTests::TestLabelActorCreateActorProperties()
{
   using namespace dtActors;

   try
   {
      // Get the actor from the proxy.
      dtABC::LabelActor* actor = NULL;
      mLabelProxy->GetActor(actor);

      dtABC::LabelActor::ActorPropertyArray propArray;
      actor->CreateActorProperties(propArray);

      typedef std::map<dtUtil::RefString, dtDAL::ActorProperty*> PropertyMap;
      PropertyMap propMap;

      // Convert the array to a map keyed on the property names.
      dtDAL::ActorProperty* curProp = NULL; // use this for code readability.
      dtABC::LabelActor::ActorPropertyArray::iterator curPropIter = propArray.begin();
      dtABC::LabelActor::ActorPropertyArray::iterator endPropArray = propArray.end();
      for (; curPropIter != endPropArray; ++curPropIter)
      {
         curProp = curPropIter->get(); // This is better for code readability.
         propMap.insert(std::make_pair(curProp->GetName(),curProp));
      }

      PropertyMap::iterator endMap = propMap.end(); // This is for better code readability.
      CPPUNIT_ASSERT(propMap.find(dtABC::LabelActor::PROPERTY_TEXT) != endMap);
      CPPUNIT_ASSERT(propMap.find(dtABC::LabelActor::PROPERTY_FONT) != endMap);
      CPPUNIT_ASSERT(propMap.find(dtABC::LabelActor::PROPERTY_FONT_SIZE) != endMap);
      CPPUNIT_ASSERT(propMap.find(dtABC::LabelActor::PROPERTY_BACK_SIZE) != endMap);
      CPPUNIT_ASSERT(propMap.find(dtABC::LabelActor::PROPERTY_TEXT_COLOR) != endMap);
      CPPUNIT_ASSERT(propMap.find(dtABC::LabelActor::PROPERTY_BACK_COLOR) != endMap);
      CPPUNIT_ASSERT(propMap.find(dtABC::LabelActor::PROPERTY_BACK_VISIBLE) != endMap);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}
