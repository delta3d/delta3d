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
 * David Guthrie, Curtiss Murphy
 */

#include <prefix/unittestprefix.h>

#include <osg/Vec3>
#include <osg/Math>
#include <osg/Group>
#include <osg/Node>
#include <osgSim/DOFTransform>

#include <dtUtil/mathdefines.h>

#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtCore/transformable.h>
#include <dtCore/scene.h>
#include <dtUtil/nodecollector.h>
#include <dtCore/batchisector.h>


#include <dtGame/basemessages.h>
#include <dtGame/gamemanager.h>
#include <dtGame/messagefactory.h>
#include <dtGame/messagetype.h>
#include <dtGame/deadreckoningcomponent.h>
#include <dtGame/defaultgroundclamper.h>
#include <dtGame/gameactorproxy.h>
#include <dtGame/gameactor.h>

#include <dtABC/application.h>

#include <dtCore/actortype.h>

#include <dtActors/engineactorregistry.h>

#include "basegmtests.h"

namespace dtGame
{
   /////////////////////////////////////////////////////////////////////////////
   class TestDeadReckoningComponent : public DeadReckoningComponent
   {
      public:
         typedef DeadReckoningComponent BaseClass;

         TestDeadReckoningComponent(): DeadReckoningComponent() {}

         const osg::Vec3& GetLastUsedEyePoint() const
         {
            return GetGroundClamper().GetLastEyePoint();
         }

         void InternalCalcTotSmoothingSteps(DeadReckoningActorComponent& helper, const dtCore::Transform& xform)
         {
            helper.CalculateSmoothingTimes(xform);
         }

         /// Gets the ground clamping hit that is closest to the deadreckoned z value.
         bool DoGetClosestHit(dtGame::GameActorProxy& actor, GroundClampingData& data, dtCore::BatchIsector::SingleISector& single, float pointz,
                  osg::Vec3& hit, osg::Vec3& normal)
         {
            bool success = false;

            dtGame::DefaultGroundClamper* defaultClamper
               = dynamic_cast<DefaultGroundClamper*>(&GetGroundClamper());
            if( defaultClamper != NULL )
            {
               success = defaultClamper->GetClosestHit(actor, data, single, pointz, hit, normal);
            }

            return success;
         }

         void DoArticulationPublic(dtGame::DeadReckoningActorComponent& helper,
            const dtCore::Transformable& txable, const dtGame::TickMessage& tickMessage);

         void DoArticulationSmoothPublic(osgSim::DOFTransform& dofxform, const osg::Vec3& currLocation,
            const osg::Vec3& nextLocation, float currentTimeStep) const;

         void DoArticulationPredictionPublic(osgSim::DOFTransform& dofxform, const osg::Vec3& currLocation,
            const osg::Vec3& currentRate, float currentTimeStep) const;
   };

   void TestDeadReckoningComponent::DoArticulationPublic(
      dtGame::DeadReckoningActorComponent& helper, const dtCore::Transformable& txable,
      const dtGame::TickMessage& tickMessage)
   {
      // The following inherited function was not virtual, so this
      // function allows the programmer to call the inherited
      // function publicly for testing purposes.
      BaseClass::DoArticulation(helper, txable, tickMessage);
   }

   void TestDeadReckoningComponent::DoArticulationSmoothPublic(osgSim::DOFTransform& dofxform,
      const osg::Vec3& currLocation, const osg::Vec3& nextLocation, float currentTimeStep) const
   {
      // The following inherited function was not virtual, so this
      // function allows the programmer to call the inherited
      // function publicly for testing purposes.
      BaseClass::DoArticulationSmooth(dofxform, currLocation, nextLocation, currentTimeStep);
   }

   void TestDeadReckoningComponent::DoArticulationPredictionPublic(osgSim::DOFTransform& dofxform,
      const osg::Vec3& currLocation, const osg::Vec3& currentRate, float currentTimeStep) const
   {
      // The following inherited function was not virtual, so this
      // function allows the programmer to call the inherited
      // function publicly for testing purposes.
      BaseClass::DoArticulationPrediction(dofxform, currLocation, currentRate, currentTimeStep);
   }

   class DeadReckoningComponentTests : public BaseGMTestFixture
   {
      typedef BaseGMTestFixture BaseClass;

      CPPUNIT_TEST_SUITE(DeadReckoningComponentTests);

         CPPUNIT_TEST(TestDeadReckoningActorComponentDefaults);
         CPPUNIT_TEST(TestDeadReckoningActorComponentProperties);
         CPPUNIT_TEST(TestTerrainProperty);
         CPPUNIT_TEST(TestEyePointProperty);
         CPPUNIT_TEST(TestActorRegistration);
         CPPUNIT_TEST(TestSimpleBehaviorLocal);
         CPPUNIT_TEST(TestSimpleBehaviorRemote);
         CPPUNIT_TEST(TestSmoothingStepsCalc);
         CPPUNIT_TEST(TestSmoothingStepsCalcFastUpdate);
         CPPUNIT_TEST(TestDRArticulationStopCounts);
         CPPUNIT_TEST(TestDoDRArticulations);
         CPPUNIT_TEST(TestDoDRVelocityAccel);
         CPPUNIT_TEST(TestDoDRVelocityAccelNoMotion);
         CPPUNIT_TEST(TestDoDRStatic);
         CPPUNIT_TEST(TestDoDRStaticInitialConditions);
         CPPUNIT_TEST(TestDoDRNoDR);

      CPPUNIT_TEST_SUITE_END();

      public:

         /*override*/ void setUp()
         {
            BaseClass::setUp();

            mDeadReckoningComponent = new TestDeadReckoningComponent();
            mGM->AddComponent(*mDeadReckoningComponent, GameManager::ComponentPriority::NORMAL);

            mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, mTestGameActor);
            CPPUNIT_ASSERT(mTestGameActor.valid());
         }

         /*override*/ void tearDown()
         {
            mDeadReckoningComponent = NULL;
            BaseClass::tearDown();
         }

         void TestDeadReckoningActorComponentDefaults()
         {
            mGM->AddActor(*mTestGameActor, false, false);
            dtCore::RefPtr<DeadReckoningActorComponent> helper = new DeadReckoningActorComponent;
            CPPUNIT_ASSERT_MESSAGE("Updated flag should default to false", !helper->IsUpdated());
            CPPUNIT_ASSERT_MESSAGE("Updated flag should default to false", !helper->IsUpdated());
            CPPUNIT_ASSERT_MESSAGE("Updated flag should default to false", !helper->IsUpdated());
            CPPUNIT_ASSERT_MESSAGE("DeadReckoning algorithm should default to STATIC.",
               helper->GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::STATIC);
            CPPUNIT_ASSERT_MESSAGE("The Update Mode should default to AUTO.",
               helper->GetUpdateMode() == DeadReckoningActorComponent::UpdateMode::AUTO);
            CPPUNIT_ASSERT_MESSAGE("The Effective Update Mode for a local actor should default to CALCULATE_ONLY.",
               helper->GetEffectiveUpdateMode(false) == DeadReckoningActorComponent::UpdateMode::CALCULATE_ONLY);
            CPPUNIT_ASSERT_MESSAGE("The Effective Update Mode for a remote actor should default to CALCULATE_AND_MOVE_ACTOR.",
               helper->GetEffectiveUpdateMode(true) == DeadReckoningActorComponent::UpdateMode::CALCULATE_AND_MOVE_ACTOR);
            CPPUNIT_ASSERT_MESSAGE("Ground Clamp Type should default to Above Ground (previously flying false)", 
               helper->GetGroundClampType() == dtGame::GroundClampTypeEnum::KEEP_ABOVE );

            osg::Vec3 vec(0.0f, 0.0f, 0.0f);

            CPPUNIT_ASSERT(helper->GetLastKnownTranslation() == vec);
            CPPUNIT_ASSERT(helper->GetLastKnownRotation() == vec);
            CPPUNIT_ASSERT(helper->GetLastKnownVelocity() == vec);
            CPPUNIT_ASSERT(helper->GetLastKnownAcceleration() == vec);
            CPPUNIT_ASSERT(helper->GetLastKnownAngularVelocity() == vec);
            CPPUNIT_ASSERT(helper->GetCurrentInstantVelocity() == vec);
            CPPUNIT_ASSERT(helper->GetGroundOffset() == 0.0f);
            CPPUNIT_ASSERT(helper->GetMaxRotationSmoothingTime() ==
               dtGame::DeadReckoningActorComponent::DEFAULT_MAX_SMOOTHING_TIME_ROT);
            CPPUNIT_ASSERT(helper->GetMaxTranslationSmoothingTime() ==
               dtGame::DeadReckoningActorComponent::DEFAULT_MAX_SMOOTHING_TIME_POS);
            CPPUNIT_ASSERT(helper->GetAdjustRotationToGround());
            CPPUNIT_ASSERT(helper->GetNodeCollector() == NULL);

            dtCore::ActorProperty* prop = mTestGameActor->GetProperty(dtCore::TransformableActorProxy::PROPERTY_ROTATION);
            CPPUNIT_ASSERT(prop != NULL);
            CPPUNIT_ASSERT(prop->GetSendInFullUpdate());
            CPPUNIT_ASSERT(prop->GetSendInPartialUpdate());
            prop = mTestGameActor->GetProperty(dtCore::TransformableActorProxy::PROPERTY_TRANSLATION);
            CPPUNIT_ASSERT(prop != NULL);
            CPPUNIT_ASSERT(prop->GetSendInFullUpdate());
            CPPUNIT_ASSERT(prop->GetSendInPartialUpdate());

            mTestGameActor->AddComponent(*helper);
            CPPUNIT_ASSERT(mTestGameActor->GetProperty(dtGame::DeadReckoningActorComponent::PROPERTY_LAST_KNOWN_ROTATION) != NULL);

            prop = mTestGameActor->GetProperty(dtCore::TransformableActorProxy::PROPERTY_ROTATION);
            CPPUNIT_ASSERT(prop != NULL);
            CPPUNIT_ASSERT(!prop->GetSendInFullUpdate());
            CPPUNIT_ASSERT(!prop->GetSendInPartialUpdate());
            prop = mTestGameActor->GetProperty(dtCore::TransformableActorProxy::PROPERTY_TRANSLATION);
            CPPUNIT_ASSERT(prop != NULL);
            CPPUNIT_ASSERT(!prop->GetSendInFullUpdate());
            CPPUNIT_ASSERT(!prop->GetSendInPartialUpdate());

            std::vector<dtUtil::RefString> names;
            mTestGameActor->GetPartialUpdateProperties(names);
            CPPUNIT_ASSERT(!names.empty());
            CPPUNIT_ASSERT(std::find(names.begin(), names.end(), dtCore::TransformableActorProxy::PROPERTY_ROTATION) == names.end());
            CPPUNIT_ASSERT(std::find(names.begin(), names.end(), dtCore::TransformableActorProxy::PROPERTY_TRANSLATION) == names.end());
            CPPUNIT_ASSERT(std::find(names.begin(), names.end(), dtGame::DeadReckoningActorComponent::PROPERTY_LAST_KNOWN_ROTATION) != names.end());
            CPPUNIT_ASSERT(std::find(names.begin(), names.end(), dtGame::DeadReckoningActorComponent::PROPERTY_LAST_KNOWN_TRANSLATION) != names.end());
            CPPUNIT_ASSERT(std::find(names.begin(), names.end(), dtGame::DeadReckoningActorComponent::PROPERTY_VELOCITY_VECTOR) != names.end());
            CPPUNIT_ASSERT(std::find(names.begin(), names.end(), dtGame::DeadReckoningActorComponent::PROPERTY_ACCELERATION_VECTOR) != names.end());
            CPPUNIT_ASSERT(std::find(names.begin(), names.end(), dtGame::DeadReckoningActorComponent::PROPERTY_ANGULAR_VELOCITY_VECTOR) != names.end());
            CPPUNIT_ASSERT(std::find(names.begin(), names.end(), dtGame::DeadReckoningActorComponent::PROPERTY_DEAD_RECKONING_ALGORITHM) != names.end());

            mTestGameActor->RemoveComponent(*helper);
            CPPUNIT_ASSERT(mTestGameActor->GetProperty(dtGame::DeadReckoningActorComponent::PROPERTY_LAST_KNOWN_ROTATION) == NULL);
            prop = mTestGameActor->GetProperty(dtCore::TransformableActorProxy::PROPERTY_ROTATION);
            CPPUNIT_ASSERT(prop != NULL);
            CPPUNIT_ASSERT(prop->GetSendInFullUpdate());
            CPPUNIT_ASSERT(prop->GetSendInPartialUpdate());
            prop = mTestGameActor->GetProperty(dtCore::TransformableActorProxy::PROPERTY_TRANSLATION);
            CPPUNIT_ASSERT(prop != NULL);
            CPPUNIT_ASSERT(prop->GetSendInFullUpdate());
            CPPUNIT_ASSERT(prop->GetSendInPartialUpdate());
         }

         void TestDeadReckoningActorComponentProperties()
         {
            dtCore::RefPtr<DeadReckoningActorComponent> helper = new DeadReckoningActorComponent;
            helper->SetGroundClampType(dtGame::GroundClampTypeEnum::NONE); //helper->SetFlying(true);
            CPPUNIT_ASSERT(helper->IsUpdated());
            CPPUNIT_ASSERT(helper->GetGroundClampType() == 
               dtGame::GroundClampTypeEnum::NONE); //IsFlying()

            helper->ClearUpdated();

            CPPUNIT_ASSERT(!helper->IsUpdated());

            // Change the algorithm twice to make sure, regardless of the defaults changing, it will get marked as updated.
            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::NONE);
            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::STATIC);
            CPPUNIT_ASSERT_MESSAGE("DeadReckoning algorithm should be STATIC.",
            helper->GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::STATIC);
            CPPUNIT_ASSERT(helper->IsUpdated());
            helper->ClearUpdated();

            helper->SetUpdateMode(DeadReckoningActorComponent::UpdateMode::CALCULATE_ONLY);
            CPPUNIT_ASSERT_MESSAGE("The Update Mode should now be CALCULATE_ONLY.",
               helper->GetUpdateMode() == DeadReckoningActorComponent::UpdateMode::CALCULATE_ONLY);
            CPPUNIT_ASSERT_MESSAGE("The Effective Update Mode for remote should now be CALCULATE_ONLY.",
               helper->GetEffectiveUpdateMode(true) == DeadReckoningActorComponent::UpdateMode::CALCULATE_ONLY);
            CPPUNIT_ASSERT_MESSAGE("The Effective Update Mode for local should now be CALCULATE_ONLY.",
               helper->GetEffectiveUpdateMode(false) == DeadReckoningActorComponent::UpdateMode::CALCULATE_ONLY);

            helper->SetUpdateMode(DeadReckoningActorComponent::UpdateMode::CALCULATE_AND_MOVE_ACTOR);
            CPPUNIT_ASSERT_MESSAGE("The Update Mode should now be CALCULATE_AND_MOVE_ACTOR.",
               helper->GetUpdateMode() == DeadReckoningActorComponent::UpdateMode::CALCULATE_AND_MOVE_ACTOR);
            CPPUNIT_ASSERT_MESSAGE("The Effective Update Mode for remote should now be CALCULATE_AND_MOVE_ACTOR.",
               helper->GetEffectiveUpdateMode(true) == DeadReckoningActorComponent::UpdateMode::CALCULATE_AND_MOVE_ACTOR);
            CPPUNIT_ASSERT_MESSAGE("The Effective Update Mode for local should now be CALCULATE_AND_MOVE_ACTOR.",
               helper->GetEffectiveUpdateMode(false) == DeadReckoningActorComponent::UpdateMode::CALCULATE_AND_MOVE_ACTOR);

            osg::Vec3 vec(3.1f, 9900.032f, 493.738f);

            helper->SetLastKnownTranslation(vec);
            CPPUNIT_ASSERT(helper->GetLastKnownTranslation() == vec);
            CPPUNIT_ASSERT(helper->IsUpdated());
            helper->ClearUpdated();

            helper->SetLastKnownRotation(vec);
            CPPUNIT_ASSERT(helper->GetLastKnownRotation() == vec);
            CPPUNIT_ASSERT(helper->IsUpdated());
            helper->ClearUpdated();

            helper->SetLastKnownVelocity(vec);
            CPPUNIT_ASSERT(helper->GetLastKnownVelocity() == vec);
            CPPUNIT_ASSERT(helper->IsUpdated());
            helper->ClearUpdated();

            helper->SetLastKnownAcceleration(vec);
            CPPUNIT_ASSERT(helper->GetLastKnownAcceleration() == vec);
            CPPUNIT_ASSERT(helper->IsUpdated());
            helper->ClearUpdated();

            helper->SetLastKnownAngularVelocity(vec);
            CPPUNIT_ASSERT(helper->GetLastKnownAngularVelocity() == vec);
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

            helper->SetUseModelDimensions(false);
            CPPUNIT_ASSERT(!helper->UseModelDimensions());

            helper->SetMaxRotationSmoothingTime(5.3f);
            CPPUNIT_ASSERT(!helper->IsUpdated());
            CPPUNIT_ASSERT(helper->GetMaxRotationSmoothingTime() == 5.3f);

            helper->SetMaxTranslationSmoothingTime(4.8f);
            CPPUNIT_ASSERT(!helper->IsUpdated());
            CPPUNIT_ASSERT(helper->GetMaxTranslationSmoothingTime() == 4.8f);

            dtCore::RefPtr<dtUtil::NodeCollector> nodeCollector = new dtUtil::NodeCollector(new osg::Group(), dtUtil::NodeCollector::GroupFlag);
            helper->SetNodeCollector(*nodeCollector);

            CPPUNIT_ASSERT(helper->GetNodeCollector() == nodeCollector.get());

            helper->SetAdjustRotationToGround(false);
            CPPUNIT_ASSERT(!helper->GetAdjustRotationToGround());

            helper->SetUseFixedSmoothingTime(false);
            CPPUNIT_ASSERT(!helper->GetUseFixedSmoothingTime());
            helper->SetUseFixedSmoothingTime(true);
            CPPUNIT_ASSERT(helper->GetUseFixedSmoothingTime());

//            helper->SetUseCubicSplineTransBlend(false);
//            CPPUNIT_ASSERT(!helper->GetUseCubicSplineTransBlend());
//            helper->SetUseCubicSplineTransBlend(true);
//            CPPUNIT_ASSERT(helper->GetUseCubicSplineTransBlend());
            
         }

         void TestTerrainProperty()
         {
            CPPUNIT_ASSERT(mDeadReckoningComponent->GetTerrainActor() == NULL);

            mGM->AddActor(*mTestGameActor, false, false);
            dtCore::Transformable* terrain = mTestGameActor->GetDrawable<dtCore::Transformable>();

            mDeadReckoningComponent->SetTerrainActor(terrain);

            CPPUNIT_ASSERT_MESSAGE("The terrain should be set.",
               mDeadReckoningComponent->GetTerrainActor() == terrain);

            mGM->DeleteActor(*mTestGameActor);

            dtCore::System::GetInstance().Step();

            CPPUNIT_ASSERT_MESSAGE("The terrain should have been deleted.",
               mDeadReckoningComponent->GetTerrainActor() == NULL);

            mDeadReckoningComponent->SetTerrainActor(terrain);

            SimulateMapUnloaded();

            CPPUNIT_ASSERT_MESSAGE("The terrain should have been deleted on the map unload.",
               mDeadReckoningComponent->GetTerrainActor() == NULL);

         }

         void TestEyePointProperty()
         {
            CPPUNIT_ASSERT(mDeadReckoningComponent->GetEyePointActor() == NULL);

            mGM->AddActor(*mTestGameActor, false, false);

            dtCore::Transformable* eyePointActor = mTestGameActor->GetDrawable<dtCore::Transformable>();

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

            mDeadReckoningComponent->SetEyePointActor(eyePointActor);

            SimulateMapUnloaded();

            CPPUNIT_ASSERT_MESSAGE("The eye point actor should have been deleted on the map unload.",
               mDeadReckoningComponent->GetEyePointActor() == NULL);

         }

         void TestActorRegistration()
         {
            dtCore::RefPtr<DeadReckoningActorComponent> helper = new DeadReckoningActorComponent;
            mDeadReckoningComponent->RegisterActor(*mTestGameActor, *helper);
            CPPUNIT_ASSERT(mDeadReckoningComponent->IsRegisteredActor(*mTestGameActor));
            mDeadReckoningComponent->UnregisterActor(*mTestGameActor);
            CPPUNIT_ASSERT(!mDeadReckoningComponent->IsRegisteredActor(*mTestGameActor));

            mDeadReckoningComponent->RegisterActor(*mTestGameActor, *helper);
            CPPUNIT_ASSERT(mDeadReckoningComponent->IsRegisteredActor(*mTestGameActor));
            SimulateMapUnloaded();
            CPPUNIT_ASSERT_MESSAGE("Actor should be unregistered on map unloaded",
                     !mDeadReckoningComponent->IsRegisteredActor(*mTestGameActor));
         }

         void TickArticulations(DeadReckoningActorComponent& helper, const dtCore::Transformable& actor, float timeDelta)
         {
            // Setup a reusable tick message
            // The following message object will be used to test articulation updates
            // directly to avoid any complication caused by true circulation of messages
            // through higher level objects (which may compromise this test).
            dtCore::RefPtr<dtGame::TickMessage> tickMessage;
            mDeadReckoningComponent->GetGameManager()->GetMessageFactory()
               .CreateMessage(dtGame::MessageType::TICK_LOCAL,tickMessage);
            tickMessage->SetDeltaSimTime(timeDelta);

            mDeadReckoningComponent->DoArticulationPublic(helper, actor, *tickMessage);
         }

         void SubTestArticulationOrientation(osgSim::DOFTransform& dof,
            const osg::Vec3& expectedRotation, float errorTolerance)
         {
            osg::Vec3 dofRotation(dof.getCurrentHPR());
            //osg::Vec3 differences(dofRotation - expectedRotation);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedRotation.x(), dofRotation.x(), errorTolerance);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedRotation.y(), dofRotation.y(), errorTolerance);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedRotation.z(), dofRotation.z(), errorTolerance);
         }

         void TestDoDRArticulations()
         {
            // Setup game actor
            // The actor must be remote in order to apply dead reckoning
            // to its dofs.
            dtCore::RefPtr<dtGame::GameActorProxy> mRemoteProxy;
            mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, mRemoteProxy);
            dtCore::Transformable* remoteActor = mRemoteProxy->GetDrawable<dtCore::Transformable>();
            mGM->AddActor(*mRemoteProxy, true, false);

            // Setup the DOFs
            const std::string dofName1("TestDOF1");
            dtCore::RefPtr<osgSim::DOFTransform> dof1 = new osgSim::DOFTransform;
            dof1->setName(dofName1);

            const std::string dofName2("TestDOF2");
            dtCore::RefPtr<osgSim::DOFTransform> dof2 = new osgSim::DOFTransform;
            dof1->setName(dofName2);

            // Setup a node collector to reference the DOF
            dtCore::RefPtr<dtUtil::NodeCollector> nodeCollector = new dtUtil::NodeCollector;
            nodeCollector->AddDOFTransform(dofName1, *dof1);
            nodeCollector->AddDOFTransform(dofName2, *dof2);

            // Setup the helper
            dtCore::RefPtr<DeadReckoningActorComponent> helper = new DeadReckoningActorComponent;
            helper->SetLastKnownTranslation(osg::Vec3(0.0f,0.0f,0.0f));
            helper->SetLastKnownRotation(osg::Vec3(0.0f,0.0f,0.0f));
            helper->SetLastTranslationUpdatedTime(0.0);
            helper->SetLastRotationUpdatedTime(0.0);
            helper->SetNodeCollector(*nodeCollector);

            // Test dead reckoning
            float errorTolerance = 0.1f;
            float timeStep = 0.125f;
            float timePeriod = 1.0f;
            mDeadReckoningComponent->SetArticulationSmoothTime(1.0f);
            osg::Vec3 rotation;
            osg::Vec3 rotationRate1(
               osg::DegreesToRadians(90.0f),
               osg::DegreesToRadians(15.0f), 0.0f);
            osg::Vec3 rotationRate2(
               osg::DegreesToRadians(-30.0f),
               osg::DegreesToRadians(-45.0f), 0.0f);

            std::vector<osg::Vec3> predictedRotations1;
            std::vector<osg::Vec3> predictedRotations2;

            CalcPredictedRotationsArray(predictedRotations1, rotation, rotationRate1, timePeriod, timeStep);
            CalcPredictedRotationsArray(predictedRotations2, rotation, rotationRate2, timePeriod, timeStep);

            helper->AddToDeadReckonDOF(dofName1, rotation, rotationRate1);
            helper->AddToDeadReckonDOF(dofName1, osg::Vec3(rotation + rotationRate1), rotationRate1);

            helper->AddToDeadReckonDOF(dofName2, rotation, rotationRate2);
            helper->AddToDeadReckonDOF(dofName2, osg::Vec3(rotation + rotationRate2), rotationRate2);

            // Ensure both prediction arrays are the same size before proceeding.
            CPPUNIT_ASSERT(predictedRotations1.size() == predictedRotations2.size());

            // Tick articulations and ensure that the DOFs rotate as expected.
            unsigned limit = predictedRotations1.size();
            for (unsigned i = 0; i < limit; ++i)
            {
               TickArticulations(*helper, *remoteActor, timeStep);
               SubTestArticulationOrientation(*dof1, predictedRotations1[i], errorTolerance);
               SubTestArticulationOrientation(*dof2, predictedRotations2[i], errorTolerance);
            }
         }

         void TestDRArticulationStopCounts()
         {
            // Setup game actor
            // The actor must be remote in order to apply dead reckoning
            // to its dofs.
            dtCore::RefPtr<dtGame::GameActorProxy> remoteActor;
            mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, remoteActor);
            dtCore::Transformable& remoteTransformable = *remoteActor->GetDrawable<dtCore::Transformable>();
            mGM->AddActor(*remoteActor, true, false);

            // Setup the DOFs
            const std::string dofName("TestDOF");
            dtCore::RefPtr<osgSim::DOFTransform> dof = new osgSim::DOFTransform;
            dof->setName(dofName);

            // Setup a node collector to reference the DOF
            dtCore::RefPtr<dtUtil::NodeCollector> nodeCollector = new dtUtil::NodeCollector;
            nodeCollector->AddDOFTransform(dofName, *dof);

            // Setup the helper
            dtCore::RefPtr<DeadReckoningActorComponent> helper = new DeadReckoningActorComponent;
            helper->SetLastKnownTranslation(osg::Vec3(0.0f,0.0f,0.0f));
            helper->SetLastKnownRotation(osg::Vec3(0.0f,0.0f,0.0f));
            helper->SetLastTranslationUpdatedTime(0.0);
            helper->SetLastRotationUpdatedTime(0.0);
            helper->SetNodeCollector(*nodeCollector);

            // Declare other test variables
            float timeStep = 0.125f;
            osg::Vec3 rotation;
            osg::Vec3 rotationRate(
               osg::DegreesToRadians(90.0f),
               osg::DegreesToRadians(15.0f), 0.0f);

            // Test removing dead reckoning stops.
            // Stops are like key frames but the actual term comes from gradients (color stops);
            // this term used only for the sake of comments in this test.
            typedef std::list<dtCore::RefPtr<DeadReckoningActorComponent::DeadReckoningDOF> > DOFStopList;
            const DOFStopList& dofStops = helper->GetDeadReckoningDOFs();

            // --- Zero stops exist until a DOF is passed into the helper for DR.
            CPPUNIT_ASSERT(dofStops.size() == 0);

            // --- Add a couple DR stops and ensure they have been added to the helper.
            helper->AddToDeadReckonDOF(dofName, rotation, rotationRate);
            CPPUNIT_ASSERT(dofStops.size() == 1);
            helper->AddToDeadReckonDOF(dofName, rotation, rotationRate);
            CPPUNIT_ASSERT(dofStops.size() == 2);

            // --- Ensure it still exists after several ticks that would certainly
            //     cause the first stop to be removed under normal circumstances.
            TickArticulations(*helper, remoteTransformable, timeStep); // 0.0   to 0.125
            TickArticulations(*helper, remoteTransformable, timeStep); // 0.125 to 0.25
            TickArticulations(*helper, remoteTransformable, timeStep); // 0.25  to 0.375
            CPPUNIT_ASSERT(dofStops.size() == 2);
            TickArticulations(*helper, remoteTransformable, timeStep); // remove first DR DOF on 0.5 -> 0.0 to 0.125
            CPPUNIT_ASSERT(dofStops.size() == 1);

            DOFStopList::const_iterator iter = dofStops.begin();
            CPPUNIT_ASSERT((*iter)->mCurrentTime > 0.0f);

            // --- Tick articulation a few more time and make sure the DR DOF has updated properly.
            TickArticulations(*helper, remoteTransformable, timeStep); // 0.0   to 0.125
            TickArticulations(*helper, remoteTransformable, timeStep); // 0.125 to 0.25
            TickArticulations(*helper, remoteTransformable, timeStep); // 0.25  to 0.375
            TickArticulations(*helper, remoteTransformable, timeStep); // remove first DR DOF on 0.5 -> 0.0 to 0.125
            CPPUNIT_ASSERT((*iter)->mCurrentTime > 0.5f);

            // --- Add another stop to prove that stops can truly be added.
            helper->AddToDeadReckonDOF(dofName, rotation, rotationRate);
            // --- NEW add and causes both stops to have their times set back to zero
            CPPUNIT_ASSERT(dofStops.size() == 2);
            iter = dofStops.begin();
            CPPUNIT_ASSERT((*iter)->mCurrentTime == 0.0f);
            ++iter;
            CPPUNIT_ASSERT((*iter)->mCurrentTime == 0.0f);

            // --- The first stop was from the prior test and should now be over a half second old.
            //     The first stop should be removed here and the new one should remain.
            TickArticulations(*helper, remoteTransformable, timeStep);
            CPPUNIT_ASSERT(dofStops.size() == 2); // 2 stops are one step older (0.125)

            helper->AddToDeadReckonDOF(dofName, rotation, rotationRate);
            helper->AddToDeadReckonDOF(dofName, rotation, rotationRate);
            helper->AddToDeadReckonDOF(dofName, rotation, rotationRate);
            helper->AddToDeadReckonDOF(dofName, rotation, rotationRate);
            CPPUNIT_ASSERT(dofStops.size() == 6);

            // --- The component should only transition between 2 stops,
            //     thus all stops before the last two should be removed
            TickArticulations(*helper, remoteTransformable, timeStep);
            CPPUNIT_ASSERT(dofStops.size() == 2); // stops are one step older (0.125)
            iter = dofStops.begin();
            // --- The new first stop will have been set to 0 and then immediately
            //     ticked by one type step.
            CPPUNIT_ASSERT((*iter)->mCurrentTime == timeStep);

            TickArticulations(*helper, remoteTransformable, timeStep);
            CPPUNIT_ASSERT(dofStops.size() == 2); // stops are one step older (0.25)
            iter = dofStops.begin();
            CPPUNIT_ASSERT((*iter)->mCurrentTime == timeStep * 2.0f);

            TickArticulations(*helper, remoteTransformable, timeStep);
            TickArticulations(*helper, remoteTransformable, timeStep);
            // --- The first stop should have been removed since it was 1 second old on the tick.
            //     The last stop now becomes the first stop.
            CPPUNIT_ASSERT(dofStops.size() == 1); // stops are one step older (1.0)
         }

         void TestDoDRNoDR()
         {
            dtCore::RefPtr<DeadReckoningActorComponent> helper = new DeadReckoningActorComponent;

            InitDoDRTestHelper(*helper, dtCore::System::GetInstance().GetSimulationTime());

            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::NONE);

            dtCore::Transform xform;

            BaseGroundClamper::GroundClampRangeType* groundClampingType = &BaseGroundClamper::GroundClampRangeType::NONE;
            bool wasTransformed = helper->DoDR(*mTestGameActor->GetDrawable<dtCore::Transformable>(), xform,
                  &dtUtil::Log::GetInstance(), groundClampingType);

            CPPUNIT_ASSERT(!wasTransformed);
            CPPUNIT_ASSERT(*groundClampingType == BaseGroundClamper::GroundClampRangeType::NONE);
            CPPUNIT_ASSERT(xform.IsIdentity());
         }

         void TestDoDRVelocityAccel()
         {
            TestDoDRVelocityAccel(true);
            TestDoDRVelocityAccel(false);
         }

         void TestDoDRVelocityAccelNoMotion()
         {
            dtCore::RefPtr<DeadReckoningActorComponent> helper = new DeadReckoningActorComponent;

            InitDoDRTestHelper(*helper,dtCore::System::GetInstance().GetSimulationTime());

            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION);

            dtCore::Transform xform;

            // we don't want this to update.
            xform.SetTranslation(helper->GetLastKnownTranslation());
            xform.SetRotation(helper->GetLastKnownRotation());
            helper->ClearUpdated();

            BaseGroundClamper::GroundClampRangeType* groundClampingType = &BaseGroundClamper::GroundClampRangeType::NONE;
            bool wasTransformed = helper->DoDR(*mTestGameActor->GetDrawable<dtCore::Transformable>(), xform,
                  &dtUtil::Log::GetInstance(), groundClampingType);

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

         void TestSmoothingStepsCalc()
         {
            dtCore::RefPtr<DeadReckoningActorComponent> helper = new DeadReckoningActorComponent;
            dtCore::Transform xform(300.0f, 200.0f, 100.0f, 30.0f, 32.2f, 93.0f);

            helper->SetUseFixedSmoothingTime(false);
            helper->SetLastKnownTranslation(osg::Vec3(-0.4f, -0.3f, -2.7f));
            helper->SetLastKnownRotation(osg::Vec3(-0.4f, -0.3f, -2.7f));
            helper->SetLastKnownVelocity(osg::Vec3(0.0f, 0.0f, 0.0f));

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

            CPPUNIT_ASSERT_MESSAGE("The average time between updates is too low for the rest of the test to be valid",
               helper->GetAverageTimeBetweenRotationUpdates() > 10.0);

            helper->SetLastKnownVelocity(osg::Vec3(100.0f, 100.0f, 100.0f));
            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::VELOCITY_ONLY);

            mDeadReckoningComponent->InternalCalcTotSmoothingSteps(*helper, xform);

            CPPUNIT_ASSERT_EQUAL_MESSAGE("The translation smoothing steps should be set to the maximum because the actor can reach the new update point quickly given its velocity.",
               helper->GetTranslationEndSmoothingTime(), helper->GetMaxTranslationSmoothingTime());

            CPPUNIT_ASSERT_EQUAL_MESSAGE("The rotation smoothing steps should be set to the maximum because the actor is moving.",
               helper->GetRotationEndSmoothingTime(), helper->GetMaxRotationSmoothingTime());
         }

         void TestSmoothingStepsCalcFastUpdate()
         {
            dtCore::RefPtr<DeadReckoningActorComponent> helper = new DeadReckoningActorComponent;
            dtCore::Transform xform(300.0f, 200.0f, 100.0f, 30.0f, 32.2f, 93.0f);

            helper->SetUseFixedSmoothingTime(false);
            helper->SetLastKnownTranslation(osg::Vec3(-0.4f, -0.3f, -2.7f));
            helper->SetLastKnownRotation(osg::Vec3(-0.4f, -0.3f, -2.7f));
            helper->SetLastKnownVelocity(osg::Vec3(0.0f, 0.0f, 0.0f));

            //make sure the average update time is very low.
            helper->SetLastTranslationUpdatedTime(0.01);
            helper->SetLastTranslationUpdatedTime(0.01);

            helper->SetLastRotationUpdatedTime(0.01);
            helper->SetLastRotationUpdatedTime(0.01);

            CPPUNIT_ASSERT_MESSAGE("The average time between translation updates should be less than 1.0",
               helper->GetAverageTimeBetweenTranslationUpdates() < 1.0);

            CPPUNIT_ASSERT_MESSAGE("The average time between rotation updates should be less than 1.0",
               helper->GetAverageTimeBetweenRotationUpdates() < 1.0);


            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION);
            mDeadReckoningComponent->InternalCalcTotSmoothingSteps(*helper, xform);

            CPPUNIT_ASSERT_EQUAL_MESSAGE("The smoothing steps for translation should be less than "
                     "1.0 as it is essentially a warp, but with fast updates.",
               float(helper->GetAverageTimeBetweenTranslationUpdates()), helper->GetTranslationEndSmoothingTime());

            CPPUNIT_ASSERT_EQUAL_MESSAGE("The smoothing steps for rotation should bel less that 1.0 because "
                     "the velocity vector is 0 but with fast updates.",
               float(helper->GetAverageTimeBetweenRotationUpdates()), helper->GetRotationEndSmoothingTime());
         }

      private:

         void SimulateMapUnloaded()
         {
            dtGame::MessageFactory& msgFac = mGM->GetMessageFactory();

            dtCore::RefPtr<dtGame::MapMessage> mapMsg;
            msgFac.CreateMessage(dtGame::MessageType::INFO_MAP_UNLOAD_BEGIN, mapMsg);
            mGM->SendMessage(*mapMsg);
            dtCore::System::GetInstance().Step();
         }

         void InitDoDRTestHelper(dtGame::DeadReckoningActorComponent& helper, double simTime)
         {
            osg::Vec3 trans(1.1, 2.2, 3.3);
            osg::Vec3 rot(1.2, 2.3, 3.4);

            helper.SetLastKnownTranslation(trans);
            helper.SetLastKnownRotation(rot);
            helper.SetLastTranslationUpdatedTime(simTime);
            helper.SetLastRotationUpdatedTime(simTime);
         }

         osg::Vec3 CalcPredictedRotations(const osg::Vec3& rotationStart, const osg::Vec3& rotationEndOrRate, float deltaTime, bool smoothing)
         {
            dtCore::RefPtr<osgSim::DOFTransform> dof = new osgSim::DOFTransform;

            if (smoothing)
            {
               mDeadReckoningComponent->DoArticulationSmoothPublic(*dof,rotationStart,rotationEndOrRate,deltaTime);
            }
            else
            {
               mDeadReckoningComponent->DoArticulationPredictionPublic(*dof,rotationStart,rotationEndOrRate,deltaTime);
            }

            return dof->getCurrentHPR();
         }


         void CalcPredictedRotationsArray(std::vector<osg::Vec3>& outRotations,
            const osg::Vec3& rotationStart, const osg::Vec3& rotationRate,
            float timePeriod, float timeStep)
         {
            // Avoid division by zero.
            if (timeStep == 0.0f)
            {
               return;
            }

            // Determine the total predictions to be made.
            float totalTimeSteps = timePeriod / timeStep;
            unsigned limit = unsigned(totalTimeSteps) + 1;

            // Calculate all predictions
            osg::Vec3 rotationEndOrRate = rotationStart + rotationRate * timePeriod;
            osg::Vec3 newRotationStart(rotationStart);
            float runningTime = 0.0f;
            float smoothTime = mDeadReckoningComponent->GetArticulationSmoothTime();
            float timeAdvance = 0.0f;
            bool smoothingDisabled = false;
            for (unsigned i = 0; i < limit; ++i)
            {
               float smoothTimeReciprical = 1.0f;
               bool smoothingPerformed = runningTime+timeStep < smoothTime;
               if (!smoothingDisabled)
               {
                  if (smoothingPerformed) // DoDRArticulationSmooth will be called at this point
                  {
                     smoothTimeReciprical = 1.0f/(smoothTime * timePeriod);
                  }
                  else // DoDRArticulationPrediction will be called at this point
                  {
                     smoothingDisabled = true;
                  }
               }

               if (smoothingDisabled)
               {
                  // The next DR stop is assigned.
                  if (outRotations.size() > 0)
                  {
                     newRotationStart = outRotations[i-1];
                  }

                  // Advance the final rotation forward for another full time period.
                  rotationEndOrRate = rotationRate;

                  // No smoothing should occur so no time should be accumulated
                  runningTime = 0.0f;
               }

               // Advance time forward as normal.
               runningTime += timeStep;
               // Keep track of what time amount was last sent to CalcPredictedRotations.
               timeAdvance = runningTime * smoothTimeReciprical;
               outRotations.push_back( CalcPredictedRotations( newRotationStart, rotationEndOrRate, timeAdvance, !smoothingDisabled ) );
            }
         }

         void TestSimpleBehavior(bool updateActor)
         {
            dtCore::RefPtr<DeadReckoningActorComponent> helper = new DeadReckoningActorComponent;

            if (updateActor)
            {
               helper->SetUpdateMode(DeadReckoningActorComponent::UpdateMode::CALCULATE_AND_MOVE_ACTOR);
            }
            else
            {
               helper->SetUpdateMode(DeadReckoningActorComponent::UpdateMode::CALCULATE_ONLY);
            }

            mGM->AddActor(*mTestGameActor, true, false);
            mDeadReckoningComponent->RegisterActor(*mTestGameActor, *helper);
            CPPUNIT_ASSERT(mDeadReckoningComponent->IsRegisteredActor(*mTestGameActor));

            std::ostringstream ss;

            dtCore::Transform xform;
            mTestGameActor->GetDrawable<dtCore::Transformable>()->GetTransform(xform);
            osg::Vec3 vec;
            xform.GetTranslation(vec);
            ss.str("");
            ss << "The translation should be 0,0,0 because nothing has changed yet.  It is: " << vec << std::endl;
            CPPUNIT_ASSERT_MESSAGE(ss.str(),
               dtUtil::Equivalent(vec, osg::Vec3(0.0f,0.0f,0.0f), 1e-2f));
            xform.GetRotation(vec);
            ss.str("");
            ss << "The rotation should be 0,0,0 because nothing has changed yet.  It is: " << vec << std::endl;
            CPPUNIT_ASSERT_MESSAGE(ss.str(),
               dtUtil::Equivalent(vec, osg::Vec3(0.0f,0.0f,0.0f), 1e-2f));

            osg::Vec3 setVec = osg::Vec3(1.0, 1.2, 1.3);

            helper->SetLastKnownTranslation(setVec);
            helper->SetLastKnownRotation(setVec);
            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::NONE);
            helper->SetGroundClampType(dtGame::GroundClampTypeEnum::NONE);
            //helper->SetFlying(true);

            dtCore::System::GetInstance().Step();

            vec = helper->GetCurrentDeadReckonedTranslation();
            ss.str("");
            ss << "The position should be 0,0,0 but it is " << vec;
            CPPUNIT_ASSERT_MESSAGE(ss.str(), dtUtil::Equivalent(vec, osg::Vec3(0.0f, 0.0f, 0.0f), 1e-2f));

            vec = helper->GetCurrentDeadReckonedRotation();
            ss.str("");
            ss << "The position should be 0,0,0 but it is " << vec;
            CPPUNIT_ASSERT_MESSAGE(ss.str(), dtUtil::Equivalent(vec, osg::Vec3(0.0f, 0.0f, 0.0f), 1e-2f));

            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::STATIC);
            dtCore::System::GetInstance().Step();

            const osg::Vec3& currentPos = helper->GetCurrentDeadReckonedTranslation();
            ss.str("");
            ss << "The position should be " << setVec << " but it is " << currentPos;
            CPPUNIT_ASSERT_MESSAGE(ss.str(), dtUtil::Equivalent(setVec, currentPos, 1e-2f));

            const osg::Vec3& currentHPR = helper->GetCurrentDeadReckonedRotation();
            ss.str("");
            ss << "The position should be " << setVec << " but it is " << currentHPR;
            CPPUNIT_ASSERT_MESSAGE(ss.str(), dtUtil::Equivalent(setVec, currentHPR, 1e-2f));

            mTestGameActor->GetDrawable<dtCore::Transformable>()->GetTransform(xform);

            if (updateActor)
            {
               xform.GetTranslation(vec);
               CPPUNIT_ASSERT(dtUtil::Equivalent(vec, currentPos, 1e-2f));

               xform.GetRotation(vec);
               CPPUNIT_ASSERT(dtUtil::Equivalent(vec, currentHPR, 1e-2f));
            }
            else
            {
               xform.GetTranslation(vec);
               ss.str("");
               ss << "The translation should be 0,0,0 because updates are off.  It is: " << vec << std::endl;
               CPPUNIT_ASSERT_MESSAGE(ss.str(),
                  dtUtil::Equivalent(vec, osg::Vec3(0.0f,0.0f,0.0f), 1e-2f));
               xform.GetRotation(vec);
               ss.str("");
               ss << "The rotation should be 0,0,0 because updates are off.  It is: " << vec << std::endl;
               CPPUNIT_ASSERT_MESSAGE(ss.str(),
                  dtUtil::Equivalent(vec, osg::Vec3(0.0f,0.0f,0.0f), 1e-2f));
            }

            mDeadReckoningComponent->UnregisterActor(*mTestGameActor);
            CPPUNIT_ASSERT(!mDeadReckoningComponent->IsRegisteredActor(*mTestGameActor));
         }

         void TestDoDRVelocityAccel(bool flying)
         {
            dtCore::RefPtr<DeadReckoningActorComponent> helper = new DeadReckoningActorComponent;

            InitDoDRTestHelper(*helper, dtCore::System::GetInstance().GetSimulationTime());

            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION);
            helper->SetGroundClampType(dtGame::GroundClampTypeEnum::NONE);

            //helper->SetFlying(flying);
            if (flying) // we just use none or full for this test
            {
               helper->SetGroundClampType(dtGame::GroundClampTypeEnum::NONE);
            }
            else 
            {
               helper->SetGroundClampType(dtGame::GroundClampTypeEnum::FULL);
            }

            dtCore::Transform xform;
            BaseGroundClamper::GroundClampRangeType* groundClampingType = &BaseGroundClamper::GroundClampRangeType::NONE;
            bool wasTransformed = helper->DoDR(*mTestGameActor->GetDrawable<dtCore::Transformable>(), xform,
                  &dtUtil::Log::GetInstance(), groundClampingType);

            CPPUNIT_ASSERT((*groundClampingType == BaseGroundClamper::GroundClampRangeType::NONE) == flying);
            CPPUNIT_ASSERT(wasTransformed);
         }

         void TestDoDRStaticInitialConditions()
         {
            dtCore::RefPtr<DeadReckoningActorComponent> helper = new DeadReckoningActorComponent;
            osg::Vec3 zeroVec(0.0f,0.0f,0.0f);

            InitDoDRTestHelper(*helper, 0.016667);
            double simTime = 0.033333;

            helper->IncrementTimeSinceUpdate(0.016667f, simTime);

            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::STATIC);

            helper->SetGroundClampType(dtGame::GroundClampTypeEnum::NONE);

            dtCore::Transform xform;
            BaseGroundClamper::GroundClampRangeType* groundClampingType = &BaseGroundClamper::GroundClampRangeType::NONE;

            CPPUNIT_ASSERT(helper->GetCurrentInstantVelocity().length2() <= FLT_EPSILON);

            bool wasTransformed = helper->DoDR(*mTestGameActor->GetDrawable<dtCore::Transformable>(), xform,
                  &dtUtil::Log::GetInstance(), groundClampingType);

            CPPUNIT_ASSERT(wasTransformed);

            CPPUNIT_ASSERT_MESSAGE("The code should assume that the position is the initial position, and not calculate a velocity.", helper->GetCurrentInstantVelocity().length2() <= FLT_EPSILON);
         }

         void TestDoDRStatic(bool flying)
         {
            dtCore::RefPtr<DeadReckoningActorComponent> helper = new DeadReckoningActorComponent;
            osg::Vec3 zeroVec(0.0f,0.0f,0.0f);
            // Seed zero values to make sure it doesn't just have an initialized position.
            helper->SetLastKnownTranslation(zeroVec);
            helper->SetLastKnownRotation(zeroVec);
            float stepTime = 0.016667;
            // Set a weird init step number so that we know it uses the right time.
            InitDoDRTestHelper(*helper, stepTime * 1.03f);
            CPPUNIT_ASSERT(helper->IsUpdated());

            double simTime = stepTime * 1.03 + stepTime;

            helper->IncrementTimeSinceUpdate(stepTime, simTime);

            helper->SetDeadReckoningAlgorithm(DeadReckoningAlgorithm::STATIC);

            if (flying) // we just use none or full for this test
            {
               helper->SetGroundClampType(dtGame::GroundClampTypeEnum::NONE);
            }
            else 
            {
               helper->SetGroundClampType(dtGame::GroundClampTypeEnum::FULL);
            }

            dtCore::Transform xform;
            BaseGroundClamper::GroundClampRangeType* groundClampingType = &BaseGroundClamper::GroundClampRangeType::NONE;

            CPPUNIT_ASSERT(helper->GetCurrentInstantVelocity().length2() <= FLT_EPSILON);

            bool wasTransformed = helper->DoDR(*mTestGameActor->GetDrawable<dtCore::Transformable>(), xform,
                  &dtUtil::Log::GetInstance(), groundClampingType);
            helper->ClearUpdated();

            CPPUNIT_ASSERT((*groundClampingType == BaseGroundClamper::GroundClampRangeType::NONE) == flying);
            CPPUNIT_ASSERT(wasTransformed);

            osg::Vec3 trans;
            xform.GetTranslation(trans);
            std::ostringstream ss;
            ss << "The position should be " << helper->GetLastKnownTranslation() << " but it is " << trans;

            CPPUNIT_ASSERT_MESSAGE(ss.str(),
                  dtUtil::Equivalent(helper->GetLastKnownTranslation(), trans, 1e-2f));

            osg::Vec3 expectedVelocity = helper->GetLastKnownTranslation() / stepTime;
            ss.str("");
            ss << "The instant velocity should be " << expectedVelocity << " but it is " << helper->GetCurrentInstantVelocity();

            CPPUNIT_ASSERT_MESSAGE(ss.str(),
                  dtUtil::Equivalent(helper->GetCurrentInstantVelocity(), expectedVelocity, 1e-2f));

            ss.str("");
            osg::Vec3 hpr;
            xform.GetRotation(hpr);
            ss << "The rotation should be " << helper->GetLastKnownRotation() << " but it is " << hpr;

            CPPUNIT_ASSERT_MESSAGE(ss.str(),
                  dtUtil::Equivalent(helper->GetLastKnownRotation(), hpr, 1e-2f));

            wasTransformed = helper->DoDR(*mTestGameActor->GetDrawable<dtCore::Transformable>(), xform,
                  &dtUtil::Log::GetInstance(), groundClampingType);

            CPPUNIT_ASSERT(!wasTransformed);
            CPPUNIT_ASSERT(helper->GetCurrentInstantVelocity().length2() <= FLT_EPSILON);
         }

         dtCore::RefPtr<TestDeadReckoningComponent> mDeadReckoningComponent;
         dtCore::RefPtr<GameActorProxy> mTestGameActor;
   };

   CPPUNIT_TEST_SUITE_REGISTRATION(DeadReckoningComponentTests);

}
