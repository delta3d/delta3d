/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2008, Alion Science and Technology Corporation
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
 * David Guthrie
 */

#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtActors/distancesensoractor.h>
#include <dtActors/engineactorregistry.h>
#include <dtActors/gamemeshactor.h>

#include <dtGame/basemessages.h>

#include <dtCore/actoractorproperty.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/actorfactory.h>
#include <dtCore/system.h>

#include <dtUtil/stringutils.h>

#include "../dtGame/basegmtests.h"

#include <vector>

namespace dtActors
{
   /**
    * This test suite tests the base task actor proxy as well as the different
    * task subclasses.
    */
   class DistanceSensorActorTests : public dtGame::BaseGMTestFixture
   {
      CPPUNIT_TEST_SUITE(DistanceSensorActorTests);
         CPPUNIT_TEST(TestAttachToProperty);
         CPPUNIT_TEST(TestTriggerDistanceProperty);
         CPPUNIT_TEST(TestAttachTo);
         CPPUNIT_TEST(TestRegistration);
      CPPUNIT_TEST_SUITE_END();

   public:
      ///////////////////////////////////////////////////////////////////////////////
      void setUp() override
      {
         dtGame::BaseGMTestFixture::setUp();
         try
         {
            mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, mParentProxy);
            mGM->CreateActor(*dtActors::EngineActorRegistry::DISTANCE_SENSOR_ACTOR_TYPE, mDSProxy1);

            CPPUNIT_ASSERT(mParentProxy.valid());
            CPPUNIT_ASSERT(mDSProxy1.valid());
         }
         catch (const dtUtil::Exception& e)
         {
            CPPUNIT_FAIL(e.ToString());
         }
      }

      ///////////////////////////////////////////////////////////////////////////////
      void tearDown() override
      {
         mParentProxy = NULL;
         mDSProxy1 = NULL;
         dtGame::BaseGMTestFixture::tearDown();
      }

      ///////////////////////////////////////////////////////////////////////////////
      void TestAttachToProperty()
      {
         mGM->AddActor(*mParentProxy, false, false);
         mGM->AddActor(*mDSProxy1, false, false);

         dtCore::ActorActorProperty* aap;
         mDSProxy1->GetProperty(DistanceSensorActorProxy::PROPERTY_ATTACH_TO_ACTOR, aap);
         CPPUNIT_ASSERT(aap != NULL);
         CPPUNIT_ASSERT(aap->GetValue() == NULL);

         aap->SetValue(mParentProxy.get());

         CPPUNIT_ASSERT(aap->GetValue() == mParentProxy.get());
      }

      ///////////////////////////////////////////////////////////////////////////////
      void TestTriggerDistanceProperty()
      {
         mGM->AddActor(*mParentProxy, false, false);
         mGM->AddActor(*mDSProxy1, false, false);

         dtCore::FloatActorProperty* fap;
         mDSProxy1->GetProperty(DistanceSensorActorProxy::PROPERTY_TRIGGER_DISTANCE, fap);
         CPPUNIT_ASSERT(fap != NULL);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, fap->GetValue(), 1e-3f);
         const float testValue = 25.125f;

         fap->SetValue(testValue);

         CPPUNIT_ASSERT_EQUAL(testValue, fap->GetValue());
      }

      ///////////////////////////////////////////////////////////////////////////////
      void TestAttachTo()
      {
         try
         {
            dtCore::ActorActorProperty* aap;
            mDSProxy1->GetProperty(DistanceSensorActorProxy::PROPERTY_ATTACH_TO_ACTOR, aap);
            CPPUNIT_ASSERT(aap != NULL);
            aap->SetValue(mParentProxy.get());

            mGM->AddActor(*mParentProxy, false, false);
            mGM->AddActor(*mDSProxy1, false, false);

            CPPUNIT_ASSERT(mDSProxy1->GetDrawable()->GetParent() == mParentProxy->GetDrawable());
         }
         catch (const dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(ex.ToString());
         }
      }

      struct TestCallbackFunction
      {
         void operator()(float distanceSquared)
         {
            mFiredValue = distanceSquared;
         }

         float mFiredValue;
      };

      void TestRegistration()
      {
         mGM->AddActor(*mParentProxy, false, false);
         mGM->AddActor(*mDSProxy1, false, false);

         DistanceSensorActor* dsActor;
         mDSProxy1->GetDrawable(dsActor);
         const std::string TEST_NAME("jojo");
         dtCore::RefPtr<dtCore::Transformable> xformable = new dtCore::Transformable(TEST_NAME);

         CPPUNIT_ASSERT_MESSAGE("Registration has NOT yet been added.",
                  !dsActor->HasRegistration(TEST_NAME));

         CPPUNIT_ASSERT(dsActor->RegisterWithSensor(TEST_NAME, *xformable, TestCallbackFunction()));

         CPPUNIT_ASSERT_MESSAGE("Registration SHOULD have been added.",
                  dsActor->HasRegistration(TEST_NAME));

         CPPUNIT_ASSERT(!dsActor->RegisterWithSensor(TEST_NAME, *xformable, TestCallbackFunction()));

         dsActor->RemoveSensorRegistration(TEST_NAME);
         dtCore::System::GetInstance().Step(0.016f);
         CPPUNIT_ASSERT_MESSAGE("Registration should have been removed.",
                  !dsActor->HasRegistration(TEST_NAME));
      }

   private:
      dtCore::RefPtr<dtGame::GameActorProxy> mParentProxy;
      dtCore::RefPtr<DistanceSensorActorProxy> mDSProxy1;
   };

   //Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION(DistanceSensorActorTests);
} // namespace dtActors
