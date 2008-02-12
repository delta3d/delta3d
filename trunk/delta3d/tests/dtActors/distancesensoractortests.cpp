/*
* Delta3D Open Source Game and Simulation Engine
* Copyright 2008, Alion Science and Technology
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* David Guthrie
*/
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtActors/distancesensoractor.h>
#include <dtActors/gamemeshactor.h>
#include <dtActors/engineactorregistry.h>

#include <dtGame/gamemanager.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/basemessages.h>

#include <dtDAL/librarymanager.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/groupactorproperty.h>
#include <dtDAL/namedparameter.h>

#include <dtCore/globals.h>
#include <dtCore/system.h>
#include <dtCore/scene.h>

#include <dtUtil/stringutils.h>

#include <vector>
#include <dtABC/application.h>

extern dtABC::Application& GetGlobalApplication();

namespace dtActors
{
   /**
    * This test suite tests the base task actor proxy as well as the different
    * task subclasses.
    */
   class DistanceSensorActorTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(DistanceSensorActorTests);
         CPPUNIT_TEST(TestAttachToProperty);
         CPPUNIT_TEST(TestTriggerDistanceProperty);
         CPPUNIT_TEST(TestAttachTo);
         CPPUNIT_TEST(TestRegistration);
      CPPUNIT_TEST_SUITE_END();
   
      public:
         ///////////////////////////////////////////////////////////////////////////////
         void setUp()
         {
            try
            {
               mGameManager = new dtGame::GameManager(*GetGlobalApplication().GetScene());
               mGameManager->SetApplication(GetGlobalApplication());
               dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
               dtCore::System::GetInstance().Start();

               mGameManager->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, mParentProxy);
               mGameManager->CreateActor(*dtActors::EngineActorRegistry::DISTANCE_SENSOR_ACTOR_TYPE, mDSProxy1);
               
               CPPUNIT_ASSERT(mParentProxy.valid());
               CPPUNIT_ASSERT(mDSProxy1.valid());
            }
            catch (const dtUtil::Exception& e)
            {
               CPPUNIT_FAIL(e.ToString());
            }
         }

         ///////////////////////////////////////////////////////////////////////////////
         void tearDown()
         {
            dtCore::System::GetInstance().SetPause(false);
            dtCore::System::GetInstance().Stop();
            if(mGameManager.valid())
            {
               mGameManager->DeleteAllActors();
               mGameManager = NULL;
            }
         }

         ///////////////////////////////////////////////////////////////////////////////
         void TestAttachToProperty()
         {
            dtDAL::ActorActorProperty* aap;
            mDSProxy1->GetProperty(DistanceSensorActorProxy::PROPERTY_ATTACH_TO_ACTOR, aap);
            CPPUNIT_ASSERT(aap != NULL);
            CPPUNIT_ASSERT(aap->GetValue() == NULL);
            CPPUNIT_ASSERT(aap->GetRealActor() == NULL);

            aap->SetValue(mParentProxy.get());

            CPPUNIT_ASSERT(aap->GetValue() == mParentProxy.get());
            CPPUNIT_ASSERT(aap->GetRealActor() == mParentProxy->GetActor());

         }

         ///////////////////////////////////////////////////////////////////////////////
         void TestTriggerDistanceProperty()
         {
            dtDAL::FloatActorProperty* fap;
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
            dtDAL::ActorActorProperty* aap;
            mDSProxy1->GetProperty(DistanceSensorActorProxy::PROPERTY_ATTACH_TO_ACTOR, aap);
            CPPUNIT_ASSERT(aap != NULL);
            aap->SetValue(mParentProxy.get());
            //add the parent second
            mGameManager->AddActor(*mParentProxy, false, false);
            mGameManager->AddActor(*mDSProxy1, false, false);

            CPPUNIT_ASSERT(mDSProxy1->GetActor()->GetParent() == mParentProxy->GetActor());
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
            DistanceSensorActor* dsActor;
            mDSProxy1->GetActor(dsActor);
            const std::string TEST_NAME("jojo");
            dtCore::RefPtr<dtCore::Transformable> xformable = new dtCore::Transformable(TEST_NAME);

            CPPUNIT_ASSERT_MESSAGE("Registration has NOT yet been added.",
                     !dsActor->HasRegistration(TEST_NAME));

            CPPUNIT_ASSERT(dsActor->RegisterWithSensor(TEST_NAME, *xformable, TestCallbackFunction()));

            CPPUNIT_ASSERT_MESSAGE("Registration SHOULD have been added.",
                     dsActor->HasRegistration(TEST_NAME));

            CPPUNIT_ASSERT(!dsActor->RegisterWithSensor(TEST_NAME, *xformable, TestCallbackFunction()));

            dsActor->RemoveSensorRegistration(TEST_NAME);
            CPPUNIT_ASSERT_MESSAGE("Registration should have been removed.",
                     !dsActor->HasRegistration(TEST_NAME));
         }

      private:
         dtCore::RefPtr<dtGame::GameManager> mGameManager;
         dtCore::RefPtr<dtGame::GameActorProxy> mParentProxy;
         dtCore::RefPtr<DistanceSensorActorProxy> mDSProxy1;
   };
   
   //Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION(DistanceSensorActorTests);
   
   
}
