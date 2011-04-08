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
 * David Guthrie, Curtiss Murphy
 */

#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <osg/Vec3>
#include <osg/Math>
#include <osg/Group>
#include <dtUtil/mathdefines.h>
#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtCore/transformable.h>
#include <dtCore/isector.h>
#include <dtCore/scene.h>
#include <dtCore/nodecollector.h>
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
         
         const osg::Vec3& GetLastUsedEyePoint() const
         {
            return GetLastEyePoint();
         }     

         bool ShouldForceClamp(DeadReckoningHelper& helper, float deltaRealTime, bool bTransformChanged)
         {
            return DeadReckoningComponent::ShouldForceClamp(helper, deltaRealTime, bTransformChanged);
         }     
         
         void InternalCalcTotSmoothingSteps(DeadReckoningHelper& helper, const dtCore::Transform& xform)
         {
            helper.CalculateSmoothingTimes(xform);
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
         CPPUNIT_TEST(TestSimpleBehaviorLocal);
         CPPUNIT_TEST(TestSimpleBehaviorRemote);
         CPPUNIT_TEST(TestHighResClampProperty);
         CPPUNIT_TEST(TestSmoothingStepsCalc);
         CPPUNIT_TEST(TestDoDRVelocityAccel);
         CPPUNIT_TEST(TestDoDRVelocityAccelNoMotion);
         CPPUNIT_TEST(TestDoDRStatic);
         CPPUNIT_TEST(TestDoDRNoDR);
         CPPUNIT_TEST(TestForceClampProperty);
         CPPUNIT_TEST(TestShouldForceClamp);
         
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

            mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, mTestGameActor);
            CPPUNIT_ASSERT(mTestGameActor.valid());
         }
         
         void tearDown()
         {
            dtCore::System::GetInstance().Stop();
            if(mGM.valid())
            {
               mTestGameActor = NULL;
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
            CPPUNIT_ASSERT_MESSAGE("The Update Mode should default to AUTO.", 
               helper->GetUpdateMode() == DeadReckoningHelper::UpdateMode::AUTO);
            CPPUNIT_ASSERT_MESSAGE("The Effective Update Mode for a local actor should default to CALCULATE_ONLY.", 
               helper->GetEffectiveUpdateMode(false) == DeadReckoningHelper::UpdateMode::CALCULATE_ONLY);
            CPPUNIT_ASSERT_MESSAGE("The Effective Update Mode for a remote actor should default to CALCULATE_AND_MOVE_ACTOR.", 
               helper->GetEffectiveUpdateMode(true) == DeadReckoningHelper::UpdateMode::CALCULATE_AND_MOVE_ACTOR);
            CPPUNIT_ASSERT_MESSAGE("Flying should default to false", !helper->IsFlying());
            
   	      osg::Vec3 vec(0.0f, 0.0f, 0.0f);
   	 
            CPPUNIT_ASSERT(helper->GetLastKnownTranslation() == vec);
            CPPUNIT_ASSERT(helper->GetLastKnownRotation() == vec);
            CPPUNIT_ASSERT(helper->GetVelocityVector() == vec);
            CPPUNIT_ASSERT(helper->GetAccelerationVector() == vec);
            CPPUNIT_ASSERT(helper->GetAngularVelocityVector() == vec);
            CPPUNIT_ASSERT(helper->GetGroundOffset() == 0.0f);
            CPPUNIT_ASSERT(helper->GetMaxRotationSmoothingTime() == 2.0f);
            CPPUNIT_ASSERT(helper->GetMaxTranslationSmoothingTime() == 8.0f);
            CPPUNIT_ASSERT(helper->GetTimeUntilForceClamp() == 0.0f);

            CPPUNIT_ASSERT(helper->GetNodeCollector() == NULL);
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
   
            helper->SetUpdateMode(DeadReckoningHelper::UpdateMode::CALCULATE_ONLY);
            CPPUNIT_ASSERT_MESSAGE("The Update Mode should now be CALCULATE_ONLY.", 
               helper->GetUpdateMode() == DeadReckoningHelper::UpdateMode::CALCULATE_ONLY);
            CPPUNIT_ASSERT_MESSAGE("The Effective Update Mode for remote should now be CALCULATE_ONLY.", 
               helper->GetEffectiveUpdateMode(true) == DeadReckoningHelper::UpdateMode::CALCULATE_ONLY);
            CPPUNIT_ASSERT_MESSAGE("The Effective Update Mode for local should now be CALCULATE_ONLY.", 
               helper->GetEffectiveUpdateMode(false) == DeadReckoningHelper::UpdateMode::CALCULATE_ONLY);
            
            helper->SetUpdateMode(DeadReckoningHelper::UpdateMode::CALCULATE_AND_MOVE_ACTOR);
            CPPUNIT_ASSERT_MESSAGE("The Update Mode should now be CALCULATE_AND_MOVE_ACTOR.", 
               helper->GetUpdateMode() == DeadReckoningHelper::UpdateMode::CALCULATE_AND_MOVE_ACTOR);
            CPPUNIT_ASSERT_MESSAGE("The Effective Update Mode for remote should now be CALCULATE_AND_MOVE_ACTOR.", 
               helper->GetEffectiveUpdateMode(true) == DeadReckoningHelper::UpdateMode::CALCULATE_AND_MOVE_ACTOR);
            CPPUNIT_ASSERT_MESSAGE("The Effective Update Mode for local should now be CALCULATE_AND_MOVE_ACTOR.", 
               helper->GetEffectiveUpdateMode(false) == DeadReckoningHelper::UpdateMode::CALCULATE_AND_MOVE_ACTOR);

            osg::Vec3 vec(3.1f, 9900.032f, 493.738f);
   	 
            helper->SetLastKnownTranslation(vec);
            CPPUNIT_ASSERT(helper->GetLastKnownTranslation() == vec);
            CPPUNIT_ASSERT(helper->GetLastKnownTranslationByCopy() == vec);
            CPPUNIT_ASSERT(helper->IsUpdated());
            helper->ClearUpdated();
   
            helper->SetLastKnownRotation(vec);
            CPPUNIT_ASSERT(helper->GetLastKnownRotation() == vec);
            CPPUNIT_ASSERT(helper->GetLastKnownRotationByCopy() == vec);
            CPPUNIT_ASSERT(helper->IsUpdated());
            helper->ClearUpdated();
   
            helper->SetVelocityVector(vec);
            CPPUNIT_ASSERT(helper->GetVelocityVector() == vec);
            CPPUNIT_ASSERT(helper->GetVelocityVectorByCopy() == vec);
            CPPUNIT_ASSERT(helper->IsUpdated());
            helper->ClearUpdated();
   
            helper->SetAccelerationVector(vec);
            CPPUNIT_ASSERT(helper->GetAccelerationVector() == vec);
            CPPUNIT_ASSERT(helper->GetAccelerationVectorByCopy() == vec);
            CPPUNIT_ASSERT(helper->IsUpdated());
            helper->ClearUpdated();
   
            helper->SetAngularVelocityVector(vec);
            CPPUNIT_ASSERT(helper->GetAngularVelocityVector() == vec);
            CPPUNIT_ASSERT(helper->GetAngularVelocityVectorByCopy() == vec);
            CPPUNIT_ASSERT(helper->IsUpdated());
            helper->ClearUpdated();
   
            helper->SetGroundOffset(43.4f);
            CPPUNIT_ASSERT(helper->GetGroundOffset() == 43.4f);
            CPPUNIT_ASSERT(helper->IsUpdated());
            helper->ClearUpdated();

            CPPUNIT_ASSERT_MESSAGE("Use the model dimensions should default to false.",
                  !helper->UseModelDimensions());
            helper->SetModelDimensions(vec);
            CPPUNIT_ASSERT_MESSAGE("Setting the model dimensions should set the use property to true", 
                  helper->UseModelDimensions());
            CPPUNIT_ASSERT(helper->GetModelDimensions() == vec);
            CPPUNIT_ASSERT(helper->GetModelDimensionsByCopy() == vec);

            helper->SetUseModelDimensions(false);
            CPPUNIT_ASSERT(!helper->UseModelDimensions());

            helper->SetMaxRotationSmoothingTime(5.3f);
            CPPUNIT_ASSERT(!helper->IsUpdated());
            CPPUNIT_ASSERT(helper->GetMaxRotationSmoothingTime() == 5.3f);

            helper->SetMaxTranslationSmoothingTime(4.8f);
            CPPUNIT_ASSERT(!helper->IsUpdated());
            CPPUNIT_ASSERT(helper->GetMaxTranslationSmoothingTime() == 4.8f);
            
            dtCore::RefPtr<dtCore::NodeCollector> nodeCollector = new dtCore::NodeCollector(new osg::Group()); 
            helper->SetNodeCollector(*nodeCollector);
            
            CPPUNIT_ASSERT(helper->GetNodeCollector() == nodeCollector.get());

            helper->SetTimeUntilForceClamp(6.7f);
            CPPUNIT_ASSERT_EQUAL(6.7f, helper->GetTimeUntilForceClamp());
         }
   
         void TestTerrainProperty()
         {
            CPPUNIT_ASSERT(mDeadReckoningComponent->GetTerrainActor() == NULL);
            
            mGM->AddActor(*mTestGameActor, false, false);
            dtCore::Transformable* terrain = &mTestGameActor->GetGameActor();
   
            mDeadReckoningComponent->SetTerrainActor(terrain);
          
            CPPUNIT_ASSERT_MESSAGE("The terrain should be set.", 
               mDeadReckoningComponent->GetTerrainActor() == terrain);
   
            mGM->DeleteActor(*mTestGameActor);
                              
            dtCore::System::GetInstance().Step();
   
            CPPUNIT_ASSERT_MESSAGE("The terrain should have been deleted.", 
               mDeadReckoningComponent->GetTerrainActor() == NULL);
         }
   
         void TestEyePointProperty()
         {
            CPPUNIT_ASSERT(mDeadReckoningComponent->GetEyePointActor() == NULL);
            
            mGM->AddActor(*mTestGameActor, false, false);
            
            dtCore::Transformable* eyePointActor = &mTestGameActor->GetGameActor();
            
            osg::Vec3 expectedEyePoint(3.3f, 3.2f, 97.2233f);
            dtCore::Transform xform;
            xform.SetTranslation(expectedEyePoint);
            eyePointActor->SetTransform(xform);
   
            mDeadReckoningComponent->SetEyePointActor(eyePointActor);
          
            CPPUNIT_ASSERT_MESSAGE("The eye point actor should be set.", 
               mDeadReckoningComponent->GetEyePointActor() == eyePointActor);

            dtCore::System::GetInstance().Step();
            CPPUNIT_ASSERT_EQUAL(expectedEyePoint, 
               mDeadReckoningComponent->GetLastUsedEyePoint());
   
            mGM->DeleteActor(*mTestGameActor);
                              
            dtCore::System::GetInstance().Step();
   
            CPPUNIT_ASSERT_MESSAGE("The eye point actor should have been deleted.",
               mDeadReckoningComponent->GetEyePointActor() == NULL);
         }

         void TestShouldForceClamp()
         {
            //set the interval to make sure the code respects it.
            mDeadReckoningComponent->SetForceClampInterval(8.0f);

            dtCore::RefPtr<DeadReckoningHelper> helper = new DeadReckoningHelper;
            helper->SetTimeUntilForceClamp(3.0);
            CPPUNIT_ASSERT(!mDeadReckoningComponent->ShouldForceClamp(*helper, 1.0f, false));
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The force clamp timer should not have expired, but it should decrement by 1.0 second.",
                  2.0f, helper->GetTimeUntilForceClamp(), 0.001f);
            CPPUNIT_ASSERT(!mDeadReckoningComponent->ShouldForceClamp(*helper, 3.0f, true));
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("If the actor hase been translated, the force clamp time should reset and no force should happe", 
                  mDeadReckoningComponent->GetForceClampInterval(), helper->GetTimeUntilForceClamp(), 0.001f);
            
            //Force the time to expire by just over the force clamp time so it will be guaranteed to return true.
            CPPUNIT_ASSERT(mDeadReckoningComponent->ShouldForceClamp(*helper, mDeadReckoningComponent->GetForceClampInterval() + 0.1, false));
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The time should reset on a force clamp.", 
                  mDeadReckoningComponent->GetForceClampInterval(), helper->GetTimeUntilForceClamp(), 0.001f);
         }

         void TestForceClampProperty()
         {
            CPPUNIT_ASSERT_EQUAL(3.0f, mDeadReckoningComponent->GetForceClampInterval());
            float value = 10.0f;
            mDeadReckoningComponent->SetForceClampInterval(value);
            CPPUNIT_ASSERT_EQUAL(value, mDeadReckoningComponent->GetForceClampInterval());
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
            dtCore::RefPtr<DeadReckoningHelper> helper = new DeadReckoningHelper;
            mDeadReckoningComponent->RegisterActor(*mTestGameActor, *helper);
            CPPUNIT_ASSERT(mDeadReckoningComponent->IsRegisteredActor(*mTestGameActor));
            mDeadReckoningComponent->UnregisterActor(*mTestGameActor);
            CPPUNIT_ASSERT(!mDeadReckoningComponent->IsRegisteredActor(*mTestGameActor));
         }
         
         void InitDoDRTestHelper(dtGame::DeadReckoningHelper& helper)
         {
            osg::Vec3 trans(1.1, 2.2, 3.3);
            osg::Vec3 rot(1.2, 2.3, 3.4);

            helper.SetLastKnownTranslation(trans);
            helper.SetLastKnownRotation(rot);
            helper.SetLastTranslationUpdatedTime(dtCore::System::GetInstance().GetSimulationTime());
            helper.SetLastRotationUpdatedTime(dtCore::System::GetInstance().GetSimulationTime());
         }
         
         void TestDoDRNoDR()
         {
            dtCore::RefPtr<DeadReckoningHelper> helper = new DeadReckoningHelper;
            
            InitDoDRTestHelper(*helper);
            
            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::NONE);

            dtCore::Transform xform;
                        
            bool shouldGroundClamp = false;
            bool wasTransformed = helper->DoDR(mTestGameActor->GetGameActor(), xform, 
                  &dtUtil::Log::GetInstance(), shouldGroundClamp);
            
            CPPUNIT_ASSERT(!wasTransformed);
         }

         void TestDoDRVelocityAccel()
         {
            TestDoDRVelocityAccel(true); 
            TestDoDRVelocityAccel(false); 
         }
         
         void TestDoDRVelocityAccelNoMotion()
         {
            dtCore::RefPtr<DeadReckoningHelper> helper = new DeadReckoningHelper;
            
            InitDoDRTestHelper(*helper);
            
            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION);

            dtCore::Transform xform;
            
            // we don't want this to update.
            xform.SetTranslation(helper->GetLastKnownTranslation());
            xform.SetRotation(helper->GetLastKnownRotation());
            helper->ClearUpdated();
            
            bool shouldGroundClamp = false;
            bool wasTransformed = helper->DoDR(mTestGameActor->GetGameActor(), xform, 
                  &dtUtil::Log::GetInstance(), shouldGroundClamp);
            
            CPPUNIT_ASSERT(!wasTransformed);
         }
         
         void TestDoDRStatic()
         {
            TestDoDRStatic(true);
            TestDoDRStatic(false);
         }
         
         void TestSimpleBehaviorLocal()
         {
            TestSimpleBehavior(false);
         }

         void TestSimpleBehaviorRemote()
         {
            TestSimpleBehavior(true);
         }

         void TestSimpleBehavior(bool updateActor)
         {
            dtCore::RefPtr<DeadReckoningHelper> helper = new DeadReckoningHelper;
   
            if (updateActor)
               helper->SetUpdateMode(DeadReckoningHelper::UpdateMode::CALCULATE_AND_MOVE_ACTOR);
            else
               helper->SetUpdateMode(DeadReckoningHelper::UpdateMode::CALCULATE_ONLY);
   
            mGM->AddActor(*mTestGameActor, true, false);
            mDeadReckoningComponent->RegisterActor(*mTestGameActor, *helper);
            CPPUNIT_ASSERT(mDeadReckoningComponent->IsRegisteredActor(*mTestGameActor));
            
            dtCore::Transform xform;
            mTestGameActor->GetGameActor().GetTransform(xform);
            osg::Vec3 vec;
            xform.GetTranslation(vec);
            CPPUNIT_ASSERT_MESSAGE("The translation should be 0,0,0 because nothing has changed yet.", 
               dtUtil::Equivalent(vec, osg::Vec3(0.0f,0.0f,0.0f), 3, 1e-2f));
            xform.GetRotation(vec);
            CPPUNIT_ASSERT_MESSAGE("The rotation should be 0,0,0 because nothing has changed yet.", 
               dtUtil::Equivalent(vec, osg::Vec3(0.0f,0.0f,0.0f), 3, 1e-2f));
                                 
            osg::Vec3 setVec = osg::Vec3(1.0, 1.2, 1.3);
            
            helper->SetLastKnownTranslation(setVec);
            helper->SetLastKnownRotation(setVec);
            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::NONE);
            helper->SetFlying(true);
            
            dtCore::System::GetInstance().Step();
         
            std::ostringstream ss;
            vec = helper->GetCurrentDeadReckonedTranslation();
            ss.str("");
            ss << "The position should be 0,0,0 but it is " << vec;
            CPPUNIT_ASSERT_MESSAGE(ss.str(), dtUtil::Equivalent(vec, osg::Vec3(0.0f, 0.0f, 0.0f), 3, 1e-2f));
   
            vec = helper->GetCurrentDeadReckonedRotation();
            ss.str("");
            ss << "The position should be 0,0,0 but it is " << vec;
            CPPUNIT_ASSERT_MESSAGE(ss.str(), dtUtil::Equivalent(vec, osg::Vec3(0.0f, 0.0f, 0.0f), 3, 1e-2f));
         
            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::STATIC);
            dtCore::System::GetInstance().Step();
            
            const osg::Vec3& currentPos = helper->GetCurrentDeadReckonedTranslation();
            ss.str("");
            ss << "The position should be " << setVec << " but it is " << currentPos;
            CPPUNIT_ASSERT_MESSAGE(ss.str(), dtUtil::Equivalent(setVec, currentPos, 3, 1e-2f));

            const osg::Vec3& currentHPR = helper->GetCurrentDeadReckonedRotation();
            ss.str("");
            ss << "The position should be " << setVec << " but it is " << currentHPR;
            CPPUNIT_ASSERT_MESSAGE(ss.str(), dtUtil::Equivalent(setVec, currentHPR, 3, 1e-2f));
                        
            mTestGameActor->GetGameActor().GetTransform(xform);

            if (updateActor)
            {
               xform.GetTranslation(vec);
               CPPUNIT_ASSERT(dtUtil::Equivalent(vec, currentPos, 3, 1e-2f));
   
               xform.GetRotation(vec);
               CPPUNIT_ASSERT(dtUtil::Equivalent(vec, currentHPR, 3, 1e-2f));
            }
            else
            {
               xform.GetTranslation(vec);
               CPPUNIT_ASSERT(dtUtil::Equivalent(vec, osg::Vec3(0.0f, 0.0f, 0.0f), 3, 1e-2f));
   
               xform.GetRotation(vec);
               CPPUNIT_ASSERT(dtUtil::Equivalent(vec, osg::Vec3(0.0f, 0.0f, 0.0f), 3, 1e-2f));
            }
            
            mDeadReckoningComponent->UnregisterActor(*mTestGameActor);
            CPPUNIT_ASSERT(!mDeadReckoningComponent->IsRegisteredActor(*mTestGameActor));
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

            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION);
            mDeadReckoningComponent->InternalCalcTotSmoothingSteps(*helper, xform);
            
            CPPUNIT_ASSERT_EQUAL_MESSAGE("The smoothing steps for translation should be 1.0 because it's too far for the velocity vector, so it should essentially warp.",
               1.0f, helper->GetTranslationEndSmoothingTime());
            
            CPPUNIT_ASSERT_EQUAL_MESSAGE("The smoothing steps for rotation should be 1.0 because the velocity vector is 0",
               1.0f, helper->GetRotationEndSmoothingTime());

            helper->SetVelocityVector(osg::Vec3(100.0f, 100.0f, 100.0f));
            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::VELOCITY_ONLY);

            mDeadReckoningComponent->InternalCalcTotSmoothingSteps(*helper, xform);
            
            CPPUNIT_ASSERT_EQUAL_MESSAGE("The translation smoothing steps should be set to the maximum because the actor can reach the new update point quickly given its velocity.",
               helper->GetTranslationEndSmoothingTime(), helper->GetMaxTranslationSmoothingTime());

            CPPUNIT_ASSERT_EQUAL_MESSAGE("The rotation smoothing steps should be set to the maximum because the actor is moving.",
               helper->GetRotationEndSmoothingTime(), helper->GetMaxRotationSmoothingTime());
         }
   
      private:
   
         void TestDoDRVelocityAccel(bool flying)
         {
            dtCore::RefPtr<DeadReckoningHelper> helper = new DeadReckoningHelper;
          
            InitDoDRTestHelper(*helper);
            
            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION);
            helper->SetFlying(flying);

            dtCore::Transform xform;
            bool shouldGroundClamp = false;
            bool wasTransformed = helper->DoDR(mTestGameActor->GetGameActor(), xform, 
                  &dtUtil::Log::GetInstance(), shouldGroundClamp);
            
            CPPUNIT_ASSERT(shouldGroundClamp == !flying);
            CPPUNIT_ASSERT(wasTransformed);
         }

         void TestDoDRStatic(bool flying)
         {
            dtCore::RefPtr<DeadReckoningHelper> helper = new DeadReckoningHelper;
            
            InitDoDRTestHelper(*helper);
            
            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::STATIC);
            helper->SetFlying(flying);

            dtCore::Transform xform;
            bool shouldGroundClamp = false;
            bool wasTransformed = helper->DoDR(mTestGameActor->GetGameActor(), xform, 
                  &dtUtil::Log::GetInstance(), shouldGroundClamp);
            
            CPPUNIT_ASSERT(shouldGroundClamp == !flying);
            CPPUNIT_ASSERT(wasTransformed);            

            std::ostringstream ss;
            ss << "The position should be " << helper->GetLastKnownTranslation() << " but it is " << xform.GetTranslation();

            CPPUNIT_ASSERT_MESSAGE(ss.str(), 
                  dtUtil::Equivalent(helper->GetLastKnownTranslation(), xform.GetTranslation(), 3, 1e-2f));

            ss.str("");
            osg::Vec3 hpr;
            xform.GetRotation(hpr);
            ss << "The rotation should be " << helper->GetLastKnownRotation() << " but it is " << hpr;

            CPPUNIT_ASSERT_MESSAGE(ss.str(), 
                  dtUtil::Equivalent(helper->GetLastKnownRotation(), hpr, 3, 1e-2f));
         }

         dtCore::RefPtr<GameManager> mGM;
         dtCore::RefPtr<TestDeadReckoningComponent> mDeadReckoningComponent;
         dtCore::RefPtr<GameActorProxy> mTestGameActor;
         static const std::string mTestGameActorRegistry;
   };
   
   CPPUNIT_TEST_SUITE_REGISTRATION(DeadReckoningComponentTests);
   
   const std::string DeadReckoningComponentTests::mTestGameActorRegistry("testGameActorLibrary");
   
}