/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2014, David Guthrie
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
 * @author David Guthrie
 */

#include <prefix/unittestprefix.h>
#include <dtGame/testcomponent.h>

#include <testGameActorLibrary/testgameactorlibrary.h>
#include <testGameActorLibrary/testgameenvironmentactor.h>
#include <testGameActorLibrary/testgamepropertyactor.h>
#include <testGameActorLibrary/testgameactor.h>

#include <dtABC/application.h>

#include <dtCore/camera.h>
#include <dtCore/observerptr.h>
#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/timer.h>

#include <dtCore/actortype.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/datatype.h>
#include <dtCore/intactorproperty.h>
#include <dtCore/project.h>
#include <dtCore/resourcedescriptor.h>

#include <dtGame/gameactorproxy.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/invokable.h>
#include <dtGame/machineinfo.h>
#include <dtGame/messagefactory.h>
#include <dtGame/messageparameter.h>
#include <dtGame/messagetype.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/datastream.h>
#include <dtUtil/log.h>

#include <osg/Math>

#include <cppunit/extensions/HelperMacros.h>

#include <iostream>

extern dtABC::Application& GetGlobalApplication();

class ActorComponentTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(ActorComponentTests);

      CPPUNIT_TEST(TestAddActorComponent);
      CPPUNIT_TEST(TestActorComponentInitialized);
      CPPUNIT_TEST(TestGetAllActorComponents);

   CPPUNIT_TEST_SUITE_END();

public:
   ////////////////////////////////////////////////////////////////////////
   void setUp()
   {
      try
      {
         dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
         dtCore::System::GetInstance().Start();
         dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList());

         mManager = new dtGame::GameManager(*GetGlobalApplication().GetScene());
         mManager->SetApplication(GetGlobalApplication());
         mManager->LoadActorRegistry(mTestGameActorLibrary);
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
      }
      catch (const std::exception& ex)
      {
         CPPUNIT_FAIL(std::string("Error: ") + ex.what());
      }
   }

   ////////////////////////////////////////////////////////////////////////
   void tearDown()
   {
      dtCore::System::GetInstance().Stop();
      if (mManager.valid())
      {
         mManager->DeleteAllActors(true);
         mManager->UnloadActorRegistry(mTestGameActorLibrary);
         mManager = NULL;
      }
   }

   void TestAddActorComponent()
   {
      try
      {
         dtCore::RefPtr<const dtCore::ActorType> actorType = mManager->FindActorType("ExampleActors", "Test1Actor");
         dtCore::RefPtr<dtCore::BaseActorObject> baseActor = mManager->CreateActor(*actorType);
         dtCore::RefPtr<dtGame::GameActorProxy> actor = dynamic_cast<dtGame::GameActorProxy*>(baseActor.get());

         std::vector<dtGame::ActorComponent*> components = actor->GetComponents(TestActorComponent1::TYPE);
         bool notExists = components.empty();
         CPPUNIT_ASSERT_MESSAGE("Searching for an actor component not on the actor should return NULL.", notExists);

         dtCore::RefPtr<TestActorComponent1> component = new TestActorComponent1();
         actor->AddComponent(*component);

         CPPUNIT_ASSERT_MESSAGE("Actor owner not set", component->GetOwner() == actor);

         bool hascomp = actor->HasComponent(TestActorComponent1::TYPE);
         CPPUNIT_ASSERT_MESSAGE("Actor component not found after it was added!", hascomp);

         components = actor->GetComponents(TestActorComponent1::TYPE);
         bool found = !components.empty();
         CPPUNIT_ASSERT_MESSAGE("Could not retrieve actor component after it was added!", found);

         dtCore::RefPtr<TestActorComponent1> component2 = new TestActorComponent1();
         actor->AddComponent(*component2);

         components = actor->GetComponents(TestActorComponent1::TYPE);
         bool foundTwo = components.size() == 2;
         CPPUNIT_ASSERT_MESSAGE("Could not retrieve both actor components after they were added!", foundTwo);

         TestActorComponent1* compare;
         if(!actor->GetComponent(compare))
         {
            CPPUNIT_FAIL("Could not retrieve actor component after it was added");
         }
         bool foundtemplate = (compare == component);
         CPPUNIT_ASSERT_MESSAGE("Could not retrieve actor component after it was added!", foundtemplate);

         actor->RemoveAllComponentsOfType(TestActorComponent1::TYPE);

         components = actor->GetComponents(TestActorComponent1::TYPE);
         bool notfound = components.empty();
         CPPUNIT_ASSERT_MESSAGE("Searching for removed actor component should return NULL.", notfound);
      }
      catch(const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(e.What());
      }
   }



   //////////////////////////////////////////////////////
   void TestActorComponentInitialized()
   {
      try
      {
         dtCore::RefPtr<dtCore::BaseActorObject> baseactor = mManager->CreateActor(*TestGameActorLibrary::TEST1_GAME_ACTOR_TYPE);
         dtCore::RefPtr<dtGame::GameActorProxy> actor = dynamic_cast<dtGame::GameActorProxy*>(baseactor.get());

         dtCore::RefPtr<TestActorComponent1> component1 = new TestActorComponent1();
         actor->AddComponent(*component1);
         CPPUNIT_ASSERT_EQUAL_MESSAGE("ActorComponent didn't get called when added to actor", true, component1->mWasAdded);


         mManager->AddActor(*actor, true, false);

         //component 1 should have entered the world now
         CPPUNIT_ASSERT_EQUAL_MESSAGE("ActorComponent didn't enter the world, after being added to the GM", true, component1->mEnteredWorld);

         dtCore::RefPtr<TestActorComponent2> component2 = new TestActorComponent2();
         CPPUNIT_ASSERT_EQUAL_MESSAGE("ActorComponent shouldn't have been added to Actor yet", false, component2->mWasAdded);

         actor->AddComponent(*component2);
         CPPUNIT_ASSERT_EQUAL_MESSAGE("ActorComponent didn't get added to an actor which is already in the world",true, component2->mWasAdded);
         CPPUNIT_ASSERT_EQUAL_MESSAGE("ActorComponent didn't enter the world, after being added to an Actor already in the world", true, component2->mEnteredWorld);

         actor->RemoveComponent(*component2);
         CPPUNIT_ASSERT_MESSAGE("Actor component2 should have been removed from world when removed from actor!", component2->mLeftWorld);
         CPPUNIT_ASSERT_MESSAGE("Actor component2 should be de-initialized when removed from actor!", component2->mWasRemoved);

         CPPUNIT_ASSERT_MESSAGE("Actor component should not be removed yet!", !component1->mWasRemoved);
         mManager->DeleteActor(*actor);

         dtCore::System::GetInstance().Step();

         CPPUNIT_ASSERT_MESSAGE("Actor component should have left the world!", component1->mLeftWorld);

         // Make sure the actor is deleted
         actor = NULL;
         baseactor = NULL;
         // Actor should be removed by now.
         CPPUNIT_ASSERT_MESSAGE("Actor component should have been removed when the actor is deleted from memory!", component1->mWasRemoved);
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(e.What());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void TestGetAllActorComponents()
   {
      dtCore::RefPtr<dtCore::BaseActorObject> baseActor = mManager->CreateActor(*TestGameActorLibrary::TEST1_GAME_ACTOR_TYPE);
      dtCore::RefPtr<dtGame::GameActorProxy> actor = dynamic_cast<dtGame::GameActorProxy*>(baseActor.get());
      mManager->AddActor(*actor, true, false);


      std::vector<dtGame::ActorComponent*> components;
      actor->GetAllComponents(components);
      const size_t startingSize = components.size();


      dtCore::RefPtr<TestActorComponent1> component1 = new TestActorComponent1();
      dtCore::RefPtr<TestActorComponent2> component2 = new TestActorComponent2();
      actor->AddComponent(*component1);
      actor->AddComponent(*component2);

      //wipe out any old remnants
      components = std::vector<dtGame::ActorComponent*>();
      actor->GetAllComponents(components);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Actor didn't return back the number of added ActorComponents",
                                   startingSize + 2, components.size());
   }

private:
   static const std::string mTestGameActorLibrary;
   static const std::string mTestActorLibrary;
   dtCore::RefPtr<dtGame::GameManager> mManager;
};


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(ActorComponentTests);

const std::string ActorComponentTests::mTestGameActorLibrary = "testGameActorLibrary";
const std::string ActorComponentTests::mTestActorLibrary     = "testActorLibrary";





