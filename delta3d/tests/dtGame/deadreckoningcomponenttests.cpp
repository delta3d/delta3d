/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
#include <osg/Vec3>
#include <osg/Math>
#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtCore/transformable.h>
#include <dtCore/isector.h>
#include <dtCore/scene.h>
#include <dtGame/gamemanager.h> 
#include <dtGame/exceptionenum.h>
#include <dtGame/deadreckoningcomponent.h>
#include <dtDAL/actortype.h>
#include <dtActors/engineactorregistry.h>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #include <Windows.h>
   #define SLEEP(milliseconds) Sleep((milliseconds))
#else
   #include <unistd.h>
   #define SLEEP(milliseconds) usleep(((milliseconds) * 1000))
#endif

namespace dtGame
{
   
   class TestDeadReckoningComponent : public DeadReckoningComponent
   {
      public:
         TestDeadReckoningComponent(): DeadReckoningComponent("DeadReckoningComponent") {}
         
         dtCore::Isector& GetInternalIsector()
         {
            return GetGroundClampIsector();
         }     
         
         void InternalCalcTotSmoothingSteps(DeadReckoningHelper& helper, const dtCore::Transform& xform)
         {
            CalculateTotalSmoothingSteps(helper, xform);
         }      
   };
   
   class DeadReckoningComponentTests : public CPPUNIT_NS::TestFixture 
   {
      CPPUNIT_TEST_SUITE(DeadReckoningComponentTests);
   
         CPPUNIT_TEST(TestDeadReckoningHelperDefaults);
         CPPUNIT_TEST(TestDeadReckoningHelperProperties);
         CPPUNIT_TEST(TestTerrainProperty);
         CPPUNIT_TEST(TestEyePointProperty);
         CPPUNIT_TEST(TestActorRegistration);
         CPPUNIT_TEST(TestSimpleBehavior);
         CPPUNIT_TEST(TestHighResClampProperty);
         CPPUNIT_TEST(TestSmoothingStepsCalc);
   
      CPPUNIT_TEST_SUITE_END();
   
      public:
   
         void setUp()
         {
            dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
            dtCore::System::GetInstance().Start();
            mGM = new GameManager(*new dtCore::Scene());
            mDeadReckoningComponent = new TestDeadReckoningComponent();
            mGM->AddComponent(*mDeadReckoningComponent, GameManager::ComponentPriority::NORMAL);
            mGM->LoadActorRegistry(mTestGameActorRegistry);
         }
         
         void tearDown()
         {
            dtCore::System::GetInstance().Stop();
            if(mGM.valid())
            {
               mGM->DeleteAllActors(true);
               mGM->UnloadActorRegistry(mTestGameActorRegistry);
               mGM = NULL;
            }
            mDeadReckoningComponent = NULL;
         }
   
         void TestDeadReckoningHelperDefaults()
         {
            dtCore::RefPtr<DeadReckoningHelper> helper = new DeadReckoningHelper;
   	      CPPUNIT_ASSERT_MESSAGE("Updated flag should default to false", !helper->IsUpdated());
            CPPUNIT_ASSERT_MESSAGE("Updated flag should default to false", !helper->IsUpdated());
            CPPUNIT_ASSERT_MESSAGE("Updated flag should default to false", !helper->IsUpdated());
   	      CPPUNIT_ASSERT_MESSAGE("DeadReckoning algorithm should default to NONE.", 
   	      helper->GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::NONE);
            CPPUNIT_ASSERT_MESSAGE("Flying should default to false", !helper->IsFlying());
            
   	      osg::Vec3 vec(0.0f, 0.0f, 0.0f);
   	 
            CPPUNIT_ASSERT(helper->GetLastKnownTranslation() == vec);
            CPPUNIT_ASSERT(helper->GetLastKnownRotation() == vec);
            CPPUNIT_ASSERT(helper->GetVelocityVector() == vec);
            CPPUNIT_ASSERT(helper->GetAccelerationVector() == vec);
            CPPUNIT_ASSERT(helper->GetAngularVelocityVector() == vec);
            CPPUNIT_ASSERT(helper->GetGroundOffset() == 0.0f);
            CPPUNIT_ASSERT(helper->GetMaxRotationSmoothingSteps() == 2.0f);
            CPPUNIT_ASSERT(helper->GetMaxTranslationSmoothingSteps() == 8.0f);
         }
   
         void TestDeadReckoningHelperProperties()
         {
            dtCore::RefPtr<DeadReckoningHelper> helper = new DeadReckoningHelper;
         	helper->SetFlying(true);
            CPPUNIT_ASSERT(helper->IsUpdated());
            CPPUNIT_ASSERT(helper->IsFlying());
   	 
            helper->ClearUpdated();
   
            CPPUNIT_ASSERT(!helper->IsUpdated());
    
            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::STATIC);
            CPPUNIT_ASSERT_MESSAGE("DeadReckoning algorithm should be STATIC.",
            helper->GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::STATIC);
            CPPUNIT_ASSERT(helper->IsUpdated());
            helper->ClearUpdated();
   
            
            osg::Vec3 vec(3.1f, 9900.032f, 493.738f);
   	 
            helper->SetLastKnownTranslation(vec);
            CPPUNIT_ASSERT(helper->GetLastKnownTranslation() == vec);
            CPPUNIT_ASSERT(helper->IsUpdated());
            helper->ClearUpdated();
   
            helper->SetLastKnownRotation(vec);
            CPPUNIT_ASSERT(helper->GetLastKnownRotation() == vec);
            CPPUNIT_ASSERT(helper->IsUpdated());
            helper->ClearUpdated();
   
            helper->SetVelocityVector(vec);
            CPPUNIT_ASSERT(helper->GetVelocityVector() == vec);
            CPPUNIT_ASSERT(helper->IsUpdated());
            helper->ClearUpdated();
   
            helper->SetAccelerationVector(vec);
            CPPUNIT_ASSERT(helper->GetAccelerationVector() == vec);
            CPPUNIT_ASSERT(helper->IsUpdated());
            helper->ClearUpdated();
   
            helper->SetAngularVelocityVector(vec);
            CPPUNIT_ASSERT(helper->GetAngularVelocityVector() == vec);
            CPPUNIT_ASSERT(helper->IsUpdated());
            helper->ClearUpdated();
   
            helper->SetGroundOffset(43.4f);
            CPPUNIT_ASSERT(helper->GetGroundOffset() == 43.4f);
            CPPUNIT_ASSERT(helper->IsUpdated());
            helper->ClearUpdated();

            helper->SetMaxRotationSmoothingSteps(5.3f);
            CPPUNIT_ASSERT(!helper->IsUpdated());
            CPPUNIT_ASSERT(helper->GetMaxRotationSmoothingSteps() == 5.3f);

            helper->SetMaxTranslationSmoothingSteps(4.8f);
            CPPUNIT_ASSERT(!helper->IsUpdated());
            CPPUNIT_ASSERT(helper->GetMaxTranslationSmoothingSteps() == 4.8f);
         }
   
         void TestTerrainProperty()
         {
            CPPUNIT_ASSERT(mDeadReckoningComponent->GetTerrainActor() == NULL);
            
            dtCore::RefPtr<dtGame::GameActorProxy> terrainProxy;
            mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, terrainProxy);
            mGM->AddActor(*terrainProxy, false, false);
            dtCore::Transformable* terrain = &terrainProxy->GetGameActor();
   
            mDeadReckoningComponent->SetTerrainActor(terrain);
          
            CPPUNIT_ASSERT_MESSAGE("The terrain should be set.", 
               mDeadReckoningComponent->GetTerrainActor() == terrain);
   
            mGM->DeleteActor(*terrainProxy);
                              
            dtCore::System::GetInstance().Step();
   
            CPPUNIT_ASSERT_MESSAGE("The terrain should have been deleted.", mDeadReckoningComponent->GetTerrainActor() == NULL);
         }
   
         void TestEyePointProperty()
         {
            CPPUNIT_ASSERT(mDeadReckoningComponent->GetEyePointActor() == NULL);
            
            dtCore::RefPtr<dtGame::GameActorProxy> eyePointActorProxy;
            mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, eyePointActorProxy);
            mGM->AddActor(*eyePointActorProxy, false, false);
            
            dtCore::Transformable* eyePointActor = &eyePointActorProxy->GetGameActor();
            
            osg::Vec3 expectedEyePoint(3.3f, 3.2f, 97.2233f);
            dtCore::Transform xform;
            xform.SetTranslation(expectedEyePoint);
            eyePointActor->SetTransform(xform);
   
            mDeadReckoningComponent->SetEyePointActor(eyePointActor);
          
            CPPUNIT_ASSERT_MESSAGE("The eye point actor should be set.", 
               mDeadReckoningComponent->GetEyePointActor() == eyePointActor);

            dtCore::System::GetInstance().Step();
            CPPUNIT_ASSERT_EQUAL(expectedEyePoint, mDeadReckoningComponent->GetInternalIsector().GetEyePoint());
   
            mGM->DeleteActor(*eyePointActorProxy);
                              
            dtCore::System::GetInstance().Step();
   
            CPPUNIT_ASSERT_MESSAGE("The eye point actor should have been deleted.", mDeadReckoningComponent->GetEyePointActor() == NULL);
         }
   
         void TestHighResClampProperty()
         {
            CPPUNIT_ASSERT_EQUAL(0.0f, mDeadReckoningComponent->GetHighResGroundClampingRange());
            float value = 10.0f;
            mDeadReckoningComponent->SetHighResGroundClampingRange(value);
            CPPUNIT_ASSERT_EQUAL(value, mDeadReckoningComponent->GetHighResGroundClampingRange());
         }
   
         void TestActorRegistration()
         {
            dtCore::RefPtr<dtDAL::ActorType> type;
            dtCore::RefPtr<GameActorProxy> actor;
            type = mGM->FindActorType("ExampleActors", "Test1Actor");
            CPPUNIT_ASSERT(type.valid());
            mGM->CreateActor(*type, actor);
            CPPUNIT_ASSERT(actor.valid());
            dtCore::RefPtr<DeadReckoningHelper> helper = new DeadReckoningHelper;
            mDeadReckoningComponent->RegisterActor(*actor, *helper);
            CPPUNIT_ASSERT(mDeadReckoningComponent->IsRegisteredActor(*actor));
            mDeadReckoningComponent->UnregisterActor(*actor);
            CPPUNIT_ASSERT(!mDeadReckoningComponent->IsRegisteredActor(*actor));
         }
         
         void TestSimpleBehavior()
         {
            dtCore::RefPtr<dtDAL::ActorType> type;
            dtCore::RefPtr<GameActorProxy> actor;
            dtCore::RefPtr<DeadReckoningHelper> helper = new DeadReckoningHelper;
   
            type = mGM->FindActorType("ExampleActors", "Test1Actor");
            CPPUNIT_ASSERT(type.valid());
            mGM->CreateActor(*type, actor);
            CPPUNIT_ASSERT(actor.valid());
            
            mGM->CreateActor(*type, actor);
            CPPUNIT_ASSERT(actor.valid());
            mGM->AddActor(*actor, true, false);
            mDeadReckoningComponent->RegisterActor(*actor, *helper);
            CPPUNIT_ASSERT(mDeadReckoningComponent->IsRegisteredActor(*actor));
            
            dtCore::Transform xform;
            actor->GetGameActor().GetTransform(xform);
            osg::Vec3 vec;
            xform.GetTranslation(vec);
            CPPUNIT_ASSERT(osg::equivalent(vec.x(), 0.0f, 1e-2f) &&
                           osg::equivalent(vec.y(), 0.0f, 1e-2f) &&
                           osg::equivalent(vec.z(), 0.0f, 1e-2f)
                           );
            xform.GetRotation(vec);
            CPPUNIT_ASSERT(osg::equivalent(vec.x(), 0.0f, 1e-2f) &&
                           osg::equivalent(vec.y(), 0.0f, 1e-2f) &&
                           osg::equivalent(vec.z(), 0.0f, 1e-2f)
                           );
                                 
            osg::Vec3 setVec = osg::Vec3(1.0, 1.2, 1.3);
            
            helper->SetLastKnownTranslation(setVec);
            helper->SetLastKnownRotation(setVec);
            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::NONE);
            helper->SetFlying(true);
            
            dtCore::System::GetInstance().Step();
         
            actor->GetGameActor().GetTransform(xform);
            xform.GetTranslation(vec);
            CPPUNIT_ASSERT(osg::equivalent(vec.x(), 0.0f, 1e-2f) &&
                           osg::equivalent(vec.y(), 0.0f, 1e-2f) &&
                           osg::equivalent(vec.z(), 0.0f, 1e-2f)
                           );
            xform.GetRotation(vec);
            CPPUNIT_ASSERT(osg::equivalent(vec.x(), 0.0f, 1e-2f) &&
                           osg::equivalent(vec.y(), 0.0f, 1e-2f) &&
                           osg::equivalent(vec.z(), 0.0f, 1e-2f)
                           );
         
            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::STATIC);
            dtCore::System::GetInstance().Step();
         
            actor->GetGameActor().GetTransform(xform);
            xform.GetTranslation(vec);
            CPPUNIT_ASSERT(osg::equivalent(vec.x(), setVec.x(), 1e-2f) &&
                           osg::equivalent(vec.y(), setVec.y(), 1e-2f) &&
                           osg::equivalent(vec.z(), setVec.z(), 1e-2f)
                           );
            xform.GetRotation(vec);
            CPPUNIT_ASSERT(osg::equivalent(vec.x(), setVec.x(), 1e-2f) &&
                           osg::equivalent(vec.y(), setVec.y(), 1e-2f) &&
                           osg::equivalent(vec.z(), setVec.z(), 1e-2f)
                           );
         
            
            mDeadReckoningComponent->UnregisterActor(*actor);
            CPPUNIT_ASSERT(!mDeadReckoningComponent->IsRegisteredActor(*actor));
         }
   
         void TestSmoothingStepsCalc()
         {
            dtCore::RefPtr<DeadReckoningHelper> helper = new DeadReckoningHelper;            
            dtCore::Transform xform(300.0f, 200.0f, 100.0f, 30.0f, 32.2f, 93.0f);

            helper->SetLastKnownTranslation(osg::Vec3(-0.4f, -0.3f, -2.7f));
            helper->SetLastKnownRotation(osg::Vec3(-0.4f, -0.3f, -2.7f));
            helper->SetVelocityVector(osg::Vec3(0.0f, 0.0f, 0.0f));

            //make sure the average update time is high.
            helper->SetLastTranslationUpdatedTime(20.0);
            helper->SetLastTranslationUpdatedTime(40.0);

            CPPUNIT_ASSERT_MESSAGE("The average time between updates is too low for the rest of the test to be valid", 
               helper->GetAverageTimeBetweenTranslationUpdates() > 10.0);

            //make sure the average update time is high.
            helper->SetLastRotationUpdatedTime(20.0);
            helper->SetLastRotationUpdatedTime(40.0);

            CPPUNIT_ASSERT_MESSAGE("The average time between updates is too low for the rest of the test to be valid", 
               helper->GetAverageTimeBetweenRotationUpdates() > 10.0);

            mDeadReckoningComponent->InternalCalcTotSmoothingSteps(*helper, xform);
            
            CPPUNIT_ASSERT_EQUAL_MESSAGE("The smoothing steps for translation should be 1.0 because it's too far for the velocity vector, so it should essentially warp.",
               1.0f, helper->GetCurrentTotalTranslationSmoothingSteps());
            
            CPPUNIT_ASSERT_EQUAL_MESSAGE("The smoothing steps for rotation should be 1.0 because the velocity vector is 0",
               1.0f, helper->GetCurrentTotalRotationSmoothingSteps());

            helper->SetVelocityVector(osg::Vec3(100.0f, 100.0f, 100.0f));

            mDeadReckoningComponent->InternalCalcTotSmoothingSteps(*helper, xform);
            
            CPPUNIT_ASSERT_EQUAL_MESSAGE("The translation smoothing steps should be set to the maximum because the actor can reach the new update point quickly given its velocity.",
               helper->GetCurrentTotalTranslationSmoothingSteps(), helper->GetCurrentTotalTranslationSmoothingSteps());

            CPPUNIT_ASSERT_EQUAL_MESSAGE("The rotation smoothing steps should be set to the maximum because the actor is moving.",
               helper->GetCurrentTotalRotationSmoothingSteps(), helper->GetCurrentTotalRotationSmoothingSteps());
            
         }
   
      private:
   
         dtCore::RefPtr<GameManager> mGM;
         dtCore::RefPtr<TestDeadReckoningComponent> mDeadReckoningComponent;
         static const std::string mTestGameActorRegistry;
   };
   
   CPPUNIT_TEST_SUITE_REGISTRATION(DeadReckoningComponentTests);
   
   const std::string DeadReckoningComponentTests::mTestGameActorRegistry("testGameActorLibrary");
   
}
