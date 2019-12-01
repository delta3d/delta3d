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
#include <dtCore/stringactorproperty.h>

#include <dtGame/gameactorproxy.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/invokable.h>
#include <dtGame/machineinfo.h>
#include <dtGame/messagefactory.h>
#include <dtGame/messageparameter.h>
#include <dtGame/messagetype.h>
#include <dtGame/shaderactorcomponent.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/datastream.h>
#include <dtUtil/log.h>

#include <osg/Math>

#include "basegmtests.h"

#include <iostream>

class ActorComponentTests : public dtGame::BaseGMTestFixture
{
   CPPUNIT_TEST_SUITE(ActorComponentTests);

      CPPUNIT_TEST(TestAddActorComponent);
      CPPUNIT_TEST(TestActorComponentInitialized);
      CPPUNIT_TEST(TestGetAllActorComponents);
      CPPUNIT_TEST(TestPropertyAdding);
      CPPUNIT_TEST(TestPropertyRemoving);
      CPPUNIT_TEST(TestCloning);
      CPPUNIT_TEST(TestCopyPropertiesOnComponents);

   CPPUNIT_TEST_SUITE_END();

public:

   void AddTestProperty(dtGame::ActorComponent& comp)
   {
      using namespace dtCore;

      RefPtr<ActorProperty> prop = new StringActorProperty(
         "TestProp", "TestProp",
         StringActorProperty::SetFuncType(&comp, &dtGame::ActorComponent::SetName),
         StringActorProperty::GetFuncType(&comp, &dtGame::ActorComponent::GetName));

      comp.AddProperty(prop);
   }

   void TestAddActorComponent()
   {
      try
      {
         dtCore::RefPtr<const dtCore::ActorType> actorType = mGM->FindActorType("ExampleActors", "Test1Actor");
         dtCore::RefPtr<dtCore::BaseActorObject> baseActor = mGM->CreateActor(*actorType);
         dtCore::RefPtr<dtGame::GameActorProxy> actor = dynamic_cast<dtGame::GameActorProxy*>(baseActor.get());

         std::vector<dtGame::ActorComponent*> components = actor->GetComponents(TestActorComponent1::TYPE);
         bool notExists = components.empty();
         CPPUNIT_ASSERT_MESSAGE("Searching for an actor component not on the actor should return NULL.", notExists);

         dtCore::RefPtr<TestActorComponent1> component = new TestActorComponent1();

         // Add it twice.  We want to make sure it doesn't insert it twice.
         actor->AddComponent(*component);
         actor->AddComponent(*component);

         CPPUNIT_ASSERT_MESSAGE("Actor owner not set", component->GetOwner() == actor);

         bool hascomp = actor->HasComponent(TestActorComponent1::TYPE);
         CPPUNIT_ASSERT_MESSAGE("Actor component not found after it was added!", hascomp);

         actor->GetComponents(TestActorComponent1::TYPE, components);
         bool found = !components.empty();
         CPPUNIT_ASSERT_MESSAGE("Could not retrieve actor component after it was added!", found);
         CPPUNIT_ASSERT_MESSAGE("Adding the some component twice should not put it in the list twice.", components.size() == 1U);


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
         dtCore::RefPtr<dtCore::BaseActorObject> baseactor = mGM->CreateActor(*TestGameActorLibrary::TEST1_GAME_ACTOR_TYPE);
         dtCore::RefPtr<dtGame::GameActorProxy> actor = dynamic_cast<dtGame::GameActorProxy*>(baseactor.get());

         dtCore::RefPtr<TestActorComponent1> component1 = new TestActorComponent1();
         actor->AddComponent(*component1);
         CPPUNIT_ASSERT_EQUAL_MESSAGE("ActorComponent didn't get called when added to actor", true, component1->mWasAdded);


         mGM->AddActor(*actor, true, false);

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
         mGM->DeleteActor(*actor);

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
      dtCore::RefPtr<dtCore::BaseActorObject> baseActor = mGM->CreateActor(*TestGameActorLibrary::TEST1_GAME_ACTOR_TYPE);
      dtCore::RefPtr<dtGame::GameActorProxy> actor = dynamic_cast<dtGame::GameActorProxy*>(baseActor.get());
      mGM->AddActor(*actor, true, false);


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

   void TestPropertyAdding()
   {
      dtCore::RefPtr<dtGame::GameActorProxy> actor = 
         dynamic_cast<dtGame::GameActorProxy*>
         (mGM->CreateActor(*TestGameActorLibrary::TEST1_GAME_ACTOR_TYPE).get());
      dtCore::RefPtr<TestActorComponent1> comp = new TestActorComponent1();
      comp->BuildPropertyMap();

      const std::string propName("TestProp");

      CPPUNIT_ASSERT(comp->GetProperty(propName) == NULL);

      AddTestProperty(*comp);

      dtCore::ActorProperty* prop = comp->GetProperty(propName);
      CPPUNIT_ASSERT(prop != NULL);
      CPPUNIT_ASSERT(actor->GetProperty(propName) == NULL);

      actor->AddComponent(*comp);

      CPPUNIT_ASSERT(actor->GetProperty(propName) == NULL);

      actor->InvokeEnteredWorld();
    
      CPPUNIT_ASSERT(actor->GetProperty(propName) == prop);
   }

   void TestPropertyRemoving()
   {
      dtCore::RefPtr<dtGame::GameActorProxy> actor = 
         dynamic_cast<dtGame::GameActorProxy*>
         (mGM->CreateActor(*TestGameActorLibrary::TEST1_GAME_ACTOR_TYPE).get());
      dtCore::RefPtr<TestActorComponent1> comp = new TestActorComponent1();
      comp->BuildPropertyMap();

      const std::string propName("TestProp");
      AddTestProperty(*comp);
      actor->AddComponent(*comp);
      actor->InvokeEnteredWorld();

      dtCore::ActorProperty* prop = comp->GetProperty(propName);
      CPPUNIT_ASSERT(prop != NULL);
      CPPUNIT_ASSERT(actor->GetProperty(propName) == prop);

      actor->RemoveActorComponentProperties();

      CPPUNIT_ASSERT(comp->GetProperty(propName) == prop);
      CPPUNIT_ASSERT(actor->GetProperty(propName) == NULL);
   }

   void TestCloning()
   {
      // Game actors should have a defaul shader
      // component with a property of this name.
      const std::string propName("CurrentShader");

      dtCore::RefPtr<dtGame::GameActorProxy> actor;
      mGM->CreateActor(*TestGameActorLibrary::TEST1_GAME_ACTOR_TYPE, actor);
      actor->AddComponent(*new dtGame::ShaderActorComponent);
      dtGame::ShaderActorComponent* comp = actor->GetComponent<dtGame::ShaderActorComponent>();

      dtCore::ActorProperty* prop = comp->GetProperty(propName);
      CPPUNIT_ASSERT(prop != NULL);
      CPPUNIT_ASSERT(actor->GetProperty(propName) == NULL);

      // Ensure clone return valid references
      dtCore::RefPtr<dtGame::GameActorProxy> cloneActor
         = dynamic_cast<dtGame::GameActorProxy*>(actor->Clone().get());
      CPPUNIT_ASSERT(cloneActor.valid());
      dtGame::ShaderActorComponent* cloneComp = cloneActor->GetComponent<dtGame::ShaderActorComponent>();
      CPPUNIT_ASSERT(cloneComp != NULL);
      dtCore::ActorProperty* cloneProp = cloneComp->GetProperty(propName);
      CPPUNIT_ASSERT(cloneProp != NULL);

      // Determine reference distinction.
      CPPUNIT_ASSERT(cloneActor != actor);
      CPPUNIT_ASSERT(cloneComp != comp);
      CPPUNIT_ASSERT(cloneProp != prop);

      // Determine equality
      CPPUNIT_ASSERT(cloneActor->GetName() == actor->GetName());
      CPPUNIT_ASSERT(cloneActor->GetActorType() == actor->GetActorType());

      CPPUNIT_ASSERT(cloneComp->GetName() == comp->GetName());
      CPPUNIT_ASSERT(cloneComp->GetActorType() == comp->GetActorType());

      CPPUNIT_ASSERT(cloneProp->GetName() == propName);
      CPPUNIT_ASSERT(cloneProp->GetName() == prop->GetName());
      CPPUNIT_ASSERT(cloneProp->GetValueString() == prop->GetValueString());

      // Ensure the actor cannot access the component's property directly.
      CPPUNIT_ASSERT(cloneActor->GetProperty(propName) == NULL);
   }

   void TestCopyPropertiesOnComponents()
   {
      // Game actors should have a defaul shader
      // component with a property of this name.
      const std::string propName("CurrentShader");

      dtCore::RefPtr<dtGame::GameActorProxy> actor, actorCopyProp;
      mGM->CreateActor(*TestGameActorLibrary::TEST1_GAME_ACTOR_TYPE, actor);
      mGM->CreateActor(*TestGameActorLibrary::TEST1_GAME_ACTOR_TYPE, actorCopyProp);
      actor->AddComponent(*new dtGame::ShaderActorComponent);
      actorCopyProp->AddComponent(*new dtGame::ShaderActorComponent);

      dtCore::RefPtr<dtGame::ShaderActorComponent> extraComp, extraCompCopyProp;
      mGM->CreateActor(*dtGame::ShaderActorComponent::TYPE, extraComp);
      CPPUNIT_ASSERT(extraComp.valid());
      mGM->CreateActor(*dtGame::ShaderActorComponent::TYPE, extraCompCopyProp);
      CPPUNIT_ASSERT(extraCompCopyProp.valid());

      actor->AddComponent(*extraComp);
      actorCopyProp->AddComponent(*extraCompCopyProp);

      dtGame::ShaderActorComponent* comp1 = actor->GetComponent<dtGame::ShaderActorComponent>();
      CPPUNIT_ASSERT(comp1 != extraComp);
      dtGame::ShaderActorComponent* comp1CopyProp = actorCopyProp->GetComponent<dtGame::ShaderActorComponent>();
      CPPUNIT_ASSERT(comp1CopyProp != extraCompCopyProp);

      comp1->GetProperty(propName)->FromString("Shaders:Chicken.dtShader");
      extraComp->GetProperty(propName)->FromString("Shaders:Tofu.dtShader");

      actorCopyProp->CopyPropertiesFrom(*actor);

      CPPUNIT_ASSERT_EQUAL(comp1->GetProperty(propName)->ToString(), comp1CopyProp->GetProperty(propName)->ToString());
      CPPUNIT_ASSERT_EQUAL(extraComp->GetProperty(propName)->ToString(), extraCompCopyProp->GetProperty(propName)->ToString());
   }

private:
};


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(ActorComponentTests);





