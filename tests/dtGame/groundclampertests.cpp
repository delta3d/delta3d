/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2007-2008, Alion Science and Technology Corporation
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
 * David Guthrie, Chris Rodgers
 */

#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <osg/Vec3>
#include <osg/Math>
#include <osg/Group>
#include <osg/Node>

#include <dtUtil/mathdefines.h>

#include <dtCore/transform.h>
#include <dtCore/transformable.h>
#include <dtCore/batchisector.h>
#include <dtCore/scene.h>
#include <dtCore/infiniteterrain.h>
#include <dtCore/system.h>

#include <dtGame/basemessages.h>
#include <dtGame/gamemanager.h>
#include <dtGame/messagefactory.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/defaultgroundclamper.h>

#include <dtDAL/actortype.h>

#include <dtActors/engineactorregistry.h>
#include <dtActors/infiniteterrainactorproxy.h>

#include <dtABC/application.h>
extern dtABC::Application& GetGlobalApplication();

namespace dtGame
{

   ///////////////////////////////////////////////////////////////////////
   // TESTABLE SUB-CLASS CODE
   ///////////////////////////////////////////////////////////////////////
   class TestClamper : public DefaultGroundClamper
   {
         typedef DefaultGroundClamper BaseClass;

      public:
         TestClamper()
            : BaseClass()
            , mOffset(0.0f)
         {
         }

         void SetPointOffset(float offset)
         {
            mOffset = offset;
         }

         float GetPointOffset() const
         {
            return mOffset;
         }

         /////////////////////////////////////////////////////////////////
         // Expose protected methods for public testing.
         /////////////////////////////////////////////////////////////////
         virtual void CalculateAndSetBoundingBox(osg::Vec3& modelDimensions,
            dtDAL::TransformableActorProxy& proxy, GroundClampingData& data)
         {
            BaseClass::CalculateAndSetBoundingBox(modelDimensions, proxy, data);
         }

         void OrientTransformToSurfacePoints(dtCore::Transform& xform,
            osg::Matrix& rotation, const osg::Vec3& location,
            const osg::Vec3 points[3])
         {
            BaseClass::OrientTransformToSurfacePoints(xform, rotation, location, points);
         }

         void OrientTransform(dtCore::Transform& xform,
            osg::Matrix& rotation, const osg::Vec3& location,
            const osg::Vec3& normal)
         {
            BaseClass::OrientTransform(xform, rotation, location, normal);
         }

         virtual void FinalizeSurfacePoints(dtDAL::TransformableActorProxy& proxy,
            GroundClampingData& data, osg::Vec3 inOutPoints[3])
         {
            BaseClass::FinalizeSurfacePoints(proxy, data, inOutPoints);

            if (mOffset != 0.0f)
            {
               inOutPoints[0].z() += mOffset;
               inOutPoints[1].z() += mOffset;
               inOutPoints[2].z() += mOffset;
            }
         }

         void ClampToGroundThreePoint(dtCore::Transform& xform,
            dtDAL::TransformableActorProxy& proxy, GroundClampingData& data,
            RuntimeData& runtimeData)
         {
            BaseClass::ClampToGroundThreePoint(xform, proxy, data, runtimeData);
         }

         void ClampToGroundIntermittent(double currentTime,
            dtCore::Transform& xform, dtDAL::TransformableActorProxy& proxy,
            GroundClampingData& data, RuntimeData& runtimeData)
         {
            BaseClass::ClampToGroundIntermittent(currentTime, xform, proxy, data, runtimeData);
         }

         virtual RuntimeData& GetOrCreateRuntimeData(dtGame::GroundClampingData& data)
         {
            return BaseClass::GetOrCreateRuntimeData(data);
         }

      protected:
         virtual ~TestClamper()
         {
         }

      private:
         float mOffset;
   };

   class GroundClamperTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(GroundClamperTests);

         CPPUNIT_TEST(TestTerrainProperty);
         CPPUNIT_TEST(TestEyePointProperty);
         CPPUNIT_TEST(TestIntermittentProperties);
         CPPUNIT_TEST(TestHighResClampProperty);
         CPPUNIT_TEST(TestLowResClampProperty);
         CPPUNIT_TEST(TestClampToNearest);
         CPPUNIT_TEST(TestRuntimeDataAccess);
         CPPUNIT_TEST(TestRuntimeDataProperties);
         CPPUNIT_TEST(TestOrientTransform);
         CPPUNIT_TEST(TestOrientTransformToSurfacePoints);
         CPPUNIT_TEST(TestMissingHit);
         CPPUNIT_TEST(TestActorDetectPoints);
         CPPUNIT_TEST(TestSurfacePointDetection);
         CPPUNIT_TEST(TestFinalizeSurfacePoints);
         CPPUNIT_TEST(TestClampThreePoint);
         CPPUNIT_TEST(TestClampIntermittent);
         CPPUNIT_TEST(TestClampTransformUnchanged);

      CPPUNIT_TEST_SUITE_END();

      public:

         ///////////////////////////////////////////////////////////////////////
         void setUp()
         {
            mGM = new dtGame::GameManager(*GetGlobalApplication().GetScene());
            mGM->SetApplication(GetGlobalApplication());
            mGroundClamper = new TestClamper();
            mGM->LoadActorRegistry(mTestGameActorRegistry);

            mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, mTestGameActor);
            CPPUNIT_ASSERT(mTestGameActor.valid());

            dtCore::System::GetInstance().Config();

            dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
            dtCore::System::GetInstance().Start();
            dtCore::System::GetInstance().Step();
         }

         ///////////////////////////////////////////////////////////////////////
         void tearDown()
         {
            dtCore::System::GetInstance().Stop();

            if (mGM.valid())
            {
               mTestGameActor = NULL;
               mGM->DeleteAllActors(true);
               mGM->UnloadActorRegistry(mTestGameActorRegistry);
               mGM = NULL;
            }
         }

         ///////////////////////////////////////////////////////////////////////
         bool CompareMatrices(const osg::Matrix& a, const osg::Matrix& b, float errorThreshold)
         {
            for (int i = 0; i < 16; ++i)
            {
               if (dtUtil::Abs(a.ptr()[i] - b.ptr()[i]) > errorThreshold)
               {
                  return false;
               }
            }
            return true;
         }

         ///////////////////////////////////////////////////////////////////////
         void CreateTestTerrain(dtCore::RefPtr<dtActors::InfiniteTerrainActorProxy>& outProxy,
            dtCore::InfiniteTerrain*& outTerrain)
         {
            mGM->CreateActor(*dtActors::EngineActorRegistry::INFINITE_TERRAIN_ACTOR_TYPE, outProxy);
            CPPUNIT_ASSERT_MESSAGE("Game Manager must be able create an Infinite Terrain Actor.",
               outProxy.valid());

            outProxy->GetActor(outTerrain);
            CPPUNIT_ASSERT_MESSAGE("Infinite Terrain Proxy must return a valid Infinite Terrain drawable.",
               outTerrain != NULL );

            outTerrain->SetBuildDistance(1000.0f);
            outTerrain->SetSegmentDivisions(64);

            mGM->AddActor(*outProxy);

            // --- WORK AROUND: Infinite terrain does not have geometry until it is rendered once.
            //     Step the system to cause the terrain to render and generate its geometry.
            //     This will make sure the involved Isectors can intersect with the terrain.
            dtCore::System::GetInstance().Step();
         }

         ///////////////////////////////////////////////////////////////////////
         bool IsEqual(const osg::Vec3& result, const osg::Vec3& testValue, float errorTolerance = 0.00001f)
         {
            return dtUtil::Abs(result.x() - testValue.x()) < errorTolerance
               &&  dtUtil::Abs(result.y() - testValue.y()) < errorTolerance
               &&  dtUtil::Abs(result.z() - testValue.z()) < errorTolerance;
         }

         ///////////////////////////////////////////////////////////////////////
         bool IsEqual(const osg::Matrix& result, const osg::Matrix& testValue, float errorTolerance = 0.00001f)
         {
            bool success = true;
            for (int i = 0; i < 16; ++i)
            {
               // Did the match fail?
               if (dtUtil::Abs(result.ptr()[i] - testValue.ptr()[i]) > errorTolerance)
               {
                  success = false;
                  break;
               }
            }
            return success;
         }

         ///////////////////////////////////////////////////////////////////////
         void TestIntermittentProperties()
         {
            CPPUNIT_ASSERT_EQUAL(1.0f, mGroundClamper->GetIntermittentGroundClampingTimeDelta());
            mGroundClamper->SetIntermittentGroundClampingTimeDelta(40.0f);
            CPPUNIT_ASSERT_EQUAL(40.0f, mGroundClamper->GetIntermittentGroundClampingTimeDelta());

            CPPUNIT_ASSERT_EQUAL(0.25f, mGroundClamper->GetIntermittentGroundClampingSmoothingTime());
            mGroundClamper->SetIntermittentGroundClampingSmoothingTime(40.0f);
            CPPUNIT_ASSERT_EQUAL(40.0f, mGroundClamper->GetIntermittentGroundClampingSmoothingTime());
         }

         ///////////////////////////////////////////////////////////////////////
         void TestTerrainProperty()
         {
            CPPUNIT_ASSERT(mGroundClamper->GetTerrainActor() == NULL);
            CPPUNIT_ASSERT(!mGroundClamper->HasValidSurface());

            mGM->AddActor(*mTestGameActor, false, false);
            dtCore::Transformable* terrain = &mTestGameActor->GetGameActor();

            mGroundClamper->SetTerrainActor(terrain);

            CPPUNIT_ASSERT_MESSAGE("The terrain should be set.",
                     mGroundClamper->GetTerrainActor() == terrain);
            CPPUNIT_ASSERT(mGroundClamper->HasValidSurface());
         }

         ///////////////////////////////////////////////////////////////////////
         void TestEyePointProperty()
         {
            CPPUNIT_ASSERT(mGroundClamper->GetEyePointActor() == NULL);

            mGM->AddActor(*mTestGameActor, false, false);

            dtCore::Transformable* eyePointActor = &mTestGameActor->GetGameActor();

            osg::Vec3 expectedEyePoint(3.3f, 3.2f, 97.2233f);
            dtCore::Transform xform;
            xform.SetTranslation(expectedEyePoint);
            eyePointActor->SetTransform(xform);

            mGroundClamper->SetEyePointActor(eyePointActor);

            CPPUNIT_ASSERT_MESSAGE("The eye point actor should be set.",
                     mGroundClamper->GetEyePointActor() == eyePointActor);

            mGroundClamper->UpdateEyePoint();
            CPPUNIT_ASSERT_EQUAL(expectedEyePoint,
                     mGroundClamper->GetLastEyePoint());
         }

         ///////////////////////////////////////////////////////////////////////
         void TestHighResClampProperty()
         {
            CPPUNIT_ASSERT_EQUAL(0.0f, mGroundClamper->GetHighResGroundClampingRange());
            float value = 10.0f;
            mGroundClamper->SetHighResGroundClampingRange(value);
            CPPUNIT_ASSERT_EQUAL(value, mGroundClamper->GetHighResGroundClampingRange());
         }

         ///////////////////////////////////////////////////////////////////////
         void TestLowResClampProperty()
         {
            CPPUNIT_ASSERT_EQUAL(0.0f, mGroundClamper->GetLowResGroundClampingRange());
            float value = 10.0f;
            mGroundClamper->SetLowResGroundClampingRange(value);
            CPPUNIT_ASSERT_EQUAL(value, mGroundClamper->GetLowResGroundClampingRange());
         }

         ///////////////////////////////////////////////////////////////////////
         void TestClampToNearest()
         {
            dtCore::RefPtr<dtCore::BatchIsector::SingleISector> single =
               new dtCore::BatchIsector::SingleISector(0);
            dtCore::BatchIsector::HitList hitList;

            osg::Vec3 point, normal;

            // Declare a Ground Clamp Data to satisfy the GetClosestHit method.
            GroundClampingData data;

            CPPUNIT_ASSERT_MESSAGE("No nearest hit should have been found if no hits exist.",
                     !mGroundClamper->GetClosestHit(*mTestGameActor, data, *single, 0.03, point, normal));

            dtCore::BatchIsector::Hit hit;
            hit._intersectNormal = osg::Vec3(0.0f, 0.0f, 1.0f);
            hit._intersectPoint  = osg::Vec3(3.0f, 4.0f, 1.0f);
            hitList.push_back(hit);

            hit._intersectNormal = osg::Vec3(0.0f, 1.0f, 0.0f);
            hit._intersectPoint  = osg::Vec3(3.0f, 4.0f, 4.0f);
            hitList.push_back(hit);

            single->SetHitList(hitList);

            CPPUNIT_ASSERT_MESSAGE("A nearest hit should have been found.",
                     mGroundClamper->GetClosestHit(*mTestGameActor, data, *single, 0.03, point, normal));

            CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0f, point.x(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0f, point.y(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, point.z(), 0.001);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, normal.x(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, normal.y(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, normal.z(), 0.001);

            CPPUNIT_ASSERT_MESSAGE("A nearest hit should have been found.",
                     mGroundClamper->GetClosestHit(*mTestGameActor, data, *single, 3.4, point, normal));

            CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0f, point.x(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0f, point.y(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0f, point.z(), 0.001);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, normal.x(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, normal.y(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, normal.z(), 0.001);

            CPPUNIT_ASSERT_MESSAGE("A nearest hit should have been found.",
                     mGroundClamper->GetClosestHit(*mTestGameActor, data, *single, 5.0, point, normal));

            CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0f, point.x(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0f, point.y(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0f, point.z(), 0.001);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, normal.x(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, normal.y(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, normal.z(), 0.001);
         }

         ///////////////////////////////////////////////////////////////////////
         void TestRuntimeDataAccess()
         {
            GroundClampingData clampData;
            dtCore::RefPtr<DefaultGroundClamper::RuntimeData> runtimeData
               = new DefaultGroundClamper::RuntimeData;

            // Verify that the current test Ground Clamping Data does not
            // contain any Runtime Data.
            CPPUNIT_ASSERT(clampData.GetUserData() == NULL);
            clampData.SetUserData(runtimeData.get());
            CPPUNIT_ASSERT(clampData.GetUserData() == runtimeData.get());

            // Ensure that the ground clamper can return the existing runtime data
            // contained within the ground clamp data.
            mGroundClamper->GetOrCreateRuntimeData(clampData);
            CPPUNIT_ASSERT(clampData.GetUserData() != NULL);
            CPPUNIT_ASSERT(clampData.GetUserData() == runtimeData.get());

            // Ensure that the ground clamper can create runtime data as need.
            clampData.SetUserData(NULL);
            CPPUNIT_ASSERT(clampData.GetUserData() == NULL);
            // --- Capture the new data.
            runtimeData = &mGroundClamper->GetOrCreateRuntimeData(clampData);
            CPPUNIT_ASSERT(clampData.GetUserData() != NULL);
            CPPUNIT_ASSERT(clampData.GetUserData() == runtimeData.get());

            // Test the clamper's ability to replace runtime clamp data with
            // runtime data class type expected of the Base Ground Clamper's
            // extending sub-class, such as Default Ground Clamper.
            dtCore::RefPtr<osg::Node> testUserData = new osg::Node();
            clampData.SetUserData(testUserData.get());
            CPPUNIT_ASSERT(clampData.GetUserData() == testUserData.get());
            // --- Capture the new data.
            runtimeData = &mGroundClamper->GetOrCreateRuntimeData(clampData);
            CPPUNIT_ASSERT(clampData.GetUserData() != NULL);
            CPPUNIT_ASSERT(clampData.GetUserData() != testUserData.get());
            CPPUNIT_ASSERT(clampData.GetUserData() == runtimeData.get());
         }

         ///////////////////////////////////////////////////////////////////////
         void TestRuntimeDataProperties()
         {
            dtCore::RefPtr<DefaultGroundClamper::RuntimeData> runtimeData
               = new DefaultGroundClamper::RuntimeData;

            // Time
            CPPUNIT_ASSERT(runtimeData->GetLastClampedTime() == 0.0f);
            runtimeData->SetLastClampedTime( 98.765f );
            CPPUNIT_ASSERT(runtimeData->GetLastClampedTime() == 98.765f);

            // Offset
            CPPUNIT_ASSERT(runtimeData->GetLastClampedOffset() == 0.0f);
            runtimeData->SetLastClampedOffset( 1.234f );
            CPPUNIT_ASSERT(runtimeData->GetLastClampedOffset() == 1.234f);

            // Rotation
            osg::Matrix mtx;
            mtx.set(
               1.0f, 2.0f, 3.0f, 4.0f,
               5.0f, 6.0f, 6.0f, 8.0f,
               9.0f, 1.0f, 2.0f, 3.0f,
               4.0f, 5.0f, 6.0f, 7.0f);
            CPPUNIT_ASSERT(runtimeData->GetLastClampedRotation() == osg::Matrix());
            runtimeData->SetLastClampedRotation( mtx );
            CPPUNIT_ASSERT(runtimeData->GetLastClampedRotation() == mtx);
         }

         ///////////////////////////////////////////////////////////////////////
         void TestOrientTransform()
         {
            osg::Vec3 testNormal(0.0f,0.0,1.0f);
            osg::Vec3 resultNormal;
            testNormal.normalize();

            osg::Vec3 testPosition(20.0f,-40.0f,10.0f);
            osg::Vec3 resultPosition;
            osg::Vec3 testHpr(-30.0f, 0.0f, 0.0f);
            osg::Vec3 resultHpr;
            osg::Matrix testRotation;
            osg::Matrix resultRotation;
            dtCore::Transform testXform;
            dtCore::Transform resultXform;

            // Prepare the test transform.
            testXform.SetRotation(testHpr);
            testXform.GetRotation(testRotation);
            testXform.SetTranslation(testPosition);

            mGroundClamper->OrientTransform(resultXform, testRotation, testPosition, testNormal);

            // Test the final transformation.
            resultXform.GetRotation(resultRotation);
            resultXform.GetRotation(resultHpr);
            resultXform.GetTranslation(resultPosition);

            CPPUNIT_ASSERT(IsEqual(resultPosition, testPosition));
            CPPUNIT_ASSERT(IsEqual(resultHpr, testHpr));
            CPPUNIT_ASSERT(IsEqual(resultRotation, testRotation));
         }

         ///////////////////////////////////////////////////////////////////////
         void TestOrientTransformToSurfacePoints()
         {
            // Predict the clamping normal.
            osg::Vec3 points[3];
            points[0].set(  0.0f,10.0f, 5.0f);
            points[1].set( 10.0f, 0.0f,-5.0f);
            points[2].set(-10.0f, 0.0f,-5.0f);
            osg::Vec3 ab = points[0] - points[2];
            osg::Vec3 ac = points[0] - points[1];
            osg::Vec3 testNormal = ab ^ ac;
            testNormal.normalize();

            // Prepare the transform position and orientation.
            osg::Vec3 testPosition(20.0f, -40.0f, 10.0f);
            osg::Vec3 resultPosition;
            osg::Vec3 testHpr(0.0f, 45.0f, 0.0f);
            osg::Vec3 resultHpr;
            osg::Matrix testRotation;
            osg::Matrix resultRotation;
            dtCore::Transform testXform;
            dtCore::Transform resultXform;

            // Prepare the test transform.
            testXform.SetRotation(testHpr);
            testXform.GetRotation(testRotation);
            testXform.SetTranslation(testPosition);

            mGroundClamper->OrientTransformToSurfacePoints(resultXform, testRotation, testPosition, points);

            // Test the final transformation.
            resultXform.GetRotation(resultRotation);
            resultXform.GetRotation(resultHpr);
            resultXform.GetTranslation(resultPosition);
            osg::Vec3 resultNormal(resultRotation.ptr()[8],resultRotation.ptr()[9],resultRotation.ptr()[10]);

            CPPUNIT_ASSERT(IsEqual(resultPosition, testPosition));
            CPPUNIT_ASSERT(IsEqual(resultNormal, testNormal));
            CPPUNIT_ASSERT(IsEqual(resultHpr, testHpr));
            CPPUNIT_ASSERT(IsEqual(resultRotation, testRotation));
         }

         ///////////////////////////////////////////////////////////////////////
         void TestMissingHit()
         {
            osg::Vec3 testPoint;
            osg::Vec3 testNormal(0.0f,0.0f,1.0f);
            osg::Vec3 resultPoint;
            osg::Vec3 resultNormal(testNormal);

            // Declare a Ground Clamp Data to satisfy the GetClosestHit method.
            GroundClampingData data;

            CPPUNIT_ASSERT_MESSAGE("Current Default Ground Clamper implementation should not return any improvised point & normal from GetMissingHit.",
               !mGroundClamper->GetMissingHit(*mTestGameActor, data, 3.0f, testPoint, testNormal));
            CPPUNIT_ASSERT(testPoint == resultPoint);
            CPPUNIT_ASSERT(testNormal == resultNormal);
         }

         ///////////////////////////////////////////////////////////////////////
         void TestActorDetectPoints()
         {
            osg::Vec3 testDimensions(1.23f, 4.56f, 7.89f);
            osg::Vec3 resultDimensions;
            osg::Vec3 testDetectPoints[3];
            osg::Vec3 resultDetectPoints[3];
            GroundClampingData clampData;

            clampData.SetUseModelDimensions(false);
            CPPUNIT_ASSERT(!clampData.UseModelDimensions());
            clampData.SetUseModelDimensions(true);
            CPPUNIT_ASSERT(clampData.UseModelDimensions());

            CPPUNIT_ASSERT(clampData.GetModelDimensions() == resultDimensions);
            clampData.SetModelDimensions(testDimensions);
            CPPUNIT_ASSERT(clampData.GetModelDimensions() == testDimensions);

            /* The GetActorDetectionPoints method's dimension calculation:
               outPoints[0].set(0.0f, modelDimensions[1] / 2, 0.0f);
               outPoints[1].set(modelDimensions[0] / 2, -(modelDimensions[1] / 2), 0.0f);
               outPoints[2].set(-(modelDimensions[0] / 2), -(modelDimensions[1] / 2), 0.0f);*/

            // Predict the detection points relative to the actor.
            resultDetectPoints[0].set(0.0f, testDimensions[1] / 2, 0.0f);
            resultDetectPoints[1].set(testDimensions[0] / 2, -(testDimensions[1] / 2), 0.0f);
            resultDetectPoints[2].set(-(testDimensions[0] / 2), -(testDimensions[1] / 2), 0.0f);

            // Position and orient the actor.
            dtCore::Transformable* actor = NULL;
            mTestGameActor->GetActor(actor);
            CPPUNIT_ASSERT(actor != NULL);

            osg::Vec3 testPosition(-98.7, 6.54f, -12.3f);
            osg::Vec3 testHPR(30.0f, -60.0f, 45.0f);
            osg::Vec3 resultPosition(testPosition);
            osg::Vec3 resultHPR(testHPR);

            dtCore::Transform xform;
            xform.SetRotation(testHPR);
            xform.SetTranslation(testPosition);
            actor->SetTransform(xform);

            // Test access to the actor's detection points.
            // --- With model dimensions already set.
            mGroundClamper->GetActorDetectionPoints(*mTestGameActor, clampData, testDetectPoints);

            // --- Ensure the set dimensions did not change.
            CPPUNIT_ASSERT(clampData.GetModelDimensions() == testDimensions);

            // --- Verify the points have been set as expected.
            CPPUNIT_ASSERT(resultDetectPoints[0] == testDetectPoints[0]);
            CPPUNIT_ASSERT(resultDetectPoints[1] == testDetectPoints[1]);
            CPPUNIT_ASSERT(resultDetectPoints[2] == testDetectPoints[2]);

            // --- Ensure the actor position and orientation
            //     have not been modified unexpectedly.
            actor->GetTransform(xform);
            xform.GetTranslation(testPosition);
            xform.GetRotation(testHPR);
            CPPUNIT_ASSERT(resultPosition == testPosition);
            CPPUNIT_ASSERT(IsEqual(resultHPR, testHPR));
         }

         ///////////////////////////////////////////////////////////////////////
         void TestSurfacePointDetection()
         {
            // Create terrain.
            dtCore::RefPtr<dtActors::InfiniteTerrainActorProxy> terrainProxy;
            dtCore::InfiniteTerrain* terrain = NULL;
            CreateTestTerrain(terrainProxy, terrain);
            mGroundClamper->SetTerrainActor(terrain);

            // Create surface detection points.
            osg::Vec3 testPoints[3];
            testPoints[0].set(-100.0f,-50.0f,0.0f);
            testPoints[1].set(100.0f,-50.0f,0.0f);
            testPoints[2].set(0.0f,100.0f,0.0f);

            osg::Vec3 resultPoints[3];
            resultPoints[0].set(testPoints[0]);
            resultPoints[1].set(testPoints[1]);
            resultPoints[2].set(testPoints[2]);

            // Determine the terrain heights at various points.
            resultPoints[0].z() = terrain->GetHeight(testPoints[0].x(), testPoints[0].y());
            resultPoints[1].z() = terrain->GetHeight(testPoints[1].x(), testPoints[1].y());
            resultPoints[2].z() = terrain->GetHeight(testPoints[2].x(), testPoints[2].y());

            // Ensure terrain height variation.
            CPPUNIT_ASSERT(resultPoints[0].z() != resultPoints[1].z());
            CPPUNIT_ASSERT(resultPoints[1].z() != resultPoints[2].z());
            CPPUNIT_ASSERT(resultPoints[2].z() != resultPoints[0].z());

            // Declare a Ground Clamp Data to satisfy the GetClosestHit method.
            GroundClampingData data;

            // Test the method.
            dtCore::Transform xform; // Satisfy the method parameters.
            mGroundClamper->GetSurfacePoints(*mTestGameActor, data, xform, testPoints);
            // --- Verify the resulting points.
            CPPUNIT_ASSERT(IsEqual(resultPoints[0], testPoints[0]));
            CPPUNIT_ASSERT(IsEqual(resultPoints[1], testPoints[1]));
            CPPUNIT_ASSERT(IsEqual(resultPoints[2], testPoints[2]));
         }

         ///////////////////////////////////////////////////////////////////////
         void TestFinalizeSurfacePoints()
         {
            // Setup test variables.
            GroundClampingData clampData;
            float testOffset = 3.0f;
            // --- Test Points
            osg::Vec3 testSurfacePoints[3];
            testSurfacePoints[0].set(1.5f, 24.3f, 0.6f);
            testSurfacePoints[1].set(-3.8f, 13.27f, -0.9f);
            testSurfacePoints[2].set(0.4f, -6.9f, 2.87f);
            // --- Result Points
            osg::Vec3 resultSurfacePoints[3];
            resultSurfacePoints[0] = testSurfacePoints[0];
            resultSurfacePoints[1] = testSurfacePoints[1];
            resultSurfacePoints[2] = testSurfacePoints[2];

            // Verify that the test sub-class of Default Ground Clamper has no
            // point offset value before performing the Finalize Surface Point tests.
            CPPUNIT_ASSERT(mGroundClamper->GetPointOffset() == 0.0f);

            // Test the default implementation. Nothing should happen.
            mGroundClamper->FinalizeSurfacePoints(
               *mTestGameActor, clampData, testSurfacePoints);
            CPPUNIT_ASSERT(resultSurfacePoints[0] == testSurfacePoints[0]);
            CPPUNIT_ASSERT(resultSurfacePoints[1] == testSurfacePoints[1]);
            CPPUNIT_ASSERT(resultSurfacePoints[2] == testSurfacePoints[2]);

            // Test testing sub-class implementation. Offset should be applied.
            resultSurfacePoints[0].z() += testOffset;
            resultSurfacePoints[1].z() += testOffset;
            resultSurfacePoints[2].z() += testOffset;

            mGroundClamper->SetPointOffset(testOffset);
            mGroundClamper->FinalizeSurfacePoints(
               *mTestGameActor, clampData, testSurfacePoints);

            CPPUNIT_ASSERT(resultSurfacePoints[0] == testSurfacePoints[0]);
            CPPUNIT_ASSERT(resultSurfacePoints[1] == testSurfacePoints[1]);
            CPPUNIT_ASSERT(resultSurfacePoints[2] == testSurfacePoints[2]);
         }

         ///////////////////////////////////////////////////////////////////////
         void TestClampThreePoint()
         {
            // Create terrain.
            dtCore::RefPtr<dtActors::InfiniteTerrainActorProxy> terrainProxy;
            dtCore::InfiniteTerrain* terrain = NULL;
            CreateTestTerrain(terrainProxy, terrain);
            mGroundClamper->SetTerrainActor(terrain);

            // Create Ground Clamping Data to work with.
            GroundClampingData clampData;
            CPPUNIT_ASSERT_MESSAGE("Ground Clamping Data should not have user data by default",
               clampData.GetUserData() == NULL);
            // --- Setup the data.
            osg::Vec3 modelDimensions(3.0f, 4.0f, 5.0f);
            clampData.SetModelDimensions(modelDimensions);
            clampData.SetUseModelDimensions(true);

            // Create runtime data to pass to the method.
            // This simulates the creation from the higher-level
            // calling method ClampToGround.
            DefaultGroundClamper::RuntimeData* runtimeData
               = &mGroundClamper->GetOrCreateRuntimeData(clampData);
            CPPUNIT_ASSERT_MESSAGE("Ground Clamping Data should have new runtime user data assigned to it.",
               clampData.GetUserData() != NULL);
            CPPUNIT_ASSERT(runtimeData != NULL);

            // Position the actor.
            osg::Vec3 positionA(34.21f, -65.78f, 0.0f);
            osg::Vec3 positionB(21.65f, 78.34f, 0.0f);
            osg::Vec3 resultPositionA;
            osg::Vec3 resultPositionB;
            dtCore::Transform xform;
            dtCore::Transformable* actor = NULL;
            mTestGameActor->GetActor(actor);
            actor->GetTransform(xform);

            // Track the clamping difference.
            float terrainHeight   = 0.0f;
            float clampDifference = 0.0f;
            float errorTolerance  = 0.0001f;
            CPPUNIT_ASSERT(runtimeData->GetLastClampedOffset() == 0.0f);

            // check clamping at 2 points.
            // --- Test at point A.
            xform.SetTranslation(positionA);
            actor->SetTransform(xform);
            terrainHeight = terrain->GetHeight(positionA.x(), positionA.y());
            mGroundClamper->ClampToGroundThreePoint(xform, *mTestGameActor, clampData, *runtimeData);
            xform.GetTranslation(resultPositionA);

            // --- Check the clamping difference.
            clampDifference = (resultPositionA - positionA).z();
            CPPUNIT_ASSERT(clampDifference != 0.0f);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(clampDifference, runtimeData->GetLastClampedOffset(), errorTolerance);

            // --- Test at point B.
            xform.SetTranslation(positionB);
            actor->SetTransform(xform);
            terrainHeight = terrain->GetHeight(positionB.x(), positionB.y());
            mGroundClamper->ClampToGroundThreePoint(xform, *mTestGameActor, clampData, *runtimeData);
            xform.GetTranslation(resultPositionB);

            // --- Check the clamping difference.
            clampDifference = (resultPositionB - positionB).z();
            CPPUNIT_ASSERT(clampDifference != 0.0f);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(clampDifference, runtimeData->GetLastClampedOffset(), errorTolerance);
         }

         ///////////////////////////////////////////////////////////////////////
         void TestClampIntermittent()
         {
            // Create time variables.
            float clampTimeStep = 0.5f;
            float currentTime   = 0.0f;
            mGroundClamper->SetIntermittentGroundClampingTimeDelta(clampTimeStep);

            // Create more than one test actor.
            dtCore::RefPtr<GameActorProxy> proxy1;
            dtCore::RefPtr<GameActorProxy> proxy2;
            mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, proxy1);
            mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, proxy2);
            CPPUNIT_ASSERT(proxy1.valid());
            CPPUNIT_ASSERT(proxy2.valid());
            // --- Get the newly created actors.
            dtCore::Transformable* actor1 = NULL;
            dtCore::Transformable* actor2 = NULL;
            proxy1->GetActor(actor1);
            proxy2->GetActor(actor2);

            // Create terrain.
            dtCore::RefPtr<dtActors::InfiniteTerrainActorProxy> terrainProxy;
            dtCore::InfiniteTerrain* terrain = NULL;
            CreateTestTerrain(terrainProxy, terrain);
            mGroundClamper->SetTerrainActor(terrain);

            // Create position variables
            osg::Vec3 pos1(10.0f, 10.0f, 0.0f);
            osg::Vec3 pos2(-10.0f, -10.0f, 0.0f);
            osg::Vec3 resultPos1(pos1);
            osg::Vec3 resultPos2(pos2);
            dtCore::Transform xform1;
            dtCore::Transform xform2;

            // Get the terrain heights.
            resultPos1.z() = terrain->GetHeight(resultPos1.x(), resultPos1.y(), true);
            resultPos2.z() = terrain->GetHeight(resultPos2.x(), resultPos2.y(), true);
            // --- Ensure variable heights.
            CPPUNIT_ASSERT(resultPos1.z() != resultPos2.z());

            // Set actor positions.
            xform1.SetTranslation(pos1);
            xform2.SetTranslation(pos2);
            actor1->SetTransform(xform1);
            actor2->SetTransform(xform2);

            // Create Ground Clamp Data.
            GroundClampingData clampData1;
            GroundClampingData clampData2;

            // Create User Runtime Data.
            DefaultGroundClamper::RuntimeData* runtimeData1
               = &mGroundClamper->GetOrCreateRuntimeData(clampData1);
            DefaultGroundClamper::RuntimeData* runtimeData2
               = &mGroundClamper->GetOrCreateRuntimeData(clampData2);
            runtimeData1->SetLastClampedTime(0.0f);
            runtimeData2->SetLastClampedTime(0.0f);

            // Test clamping on first call.
            mGroundClamper->ClampToGroundIntermittent(
               currentTime, xform1, *proxy1, clampData1, *runtimeData1);
            mGroundClamper->ClampToGroundIntermittent(
               currentTime, xform2, *proxy2, clampData2, *runtimeData2);
            // --- Get the actor positions.
            actor1->GetTransform(xform1);
            actor2->GetTransform(xform2);
            xform1.GetTranslation(pos1);
            xform2.GetTranslation(pos2);
            CPPUNIT_ASSERT(pos1.z() == 0.0f);
            CPPUNIT_ASSERT(pos2.z() == 0.0f);
            // --- Ensure nothing is in the batch.
            CPPUNIT_ASSERT(mGroundClamper->GetClampBatchSize() == 0);
            CPPUNIT_ASSERT(runtimeData1->GetLastClampedTime() == 0.0f);
            CPPUNIT_ASSERT(runtimeData2->GetLastClampedTime() == 0.0f);

            // Trigger batch insertion by a time change.
            currentTime += clampTimeStep;
            mGroundClamper->ClampToGroundIntermittent(
               currentTime, xform1, *proxy1, clampData1, *runtimeData1);
            mGroundClamper->ClampToGroundIntermittent(
               currentTime, xform2, *proxy2, clampData2, *runtimeData2);
            // --- Get the actor positions.
            actor1->GetTransform(xform1);
            actor2->GetTransform(xform2);
            xform1.GetTranslation(pos1);
            xform2.GetTranslation(pos2);
            // NOTE: The clamping should not have triggered yet.
            CPPUNIT_ASSERT(pos1.z() == 0.0f);
            CPPUNIT_ASSERT(pos2.z() == 0.0f);
            // --- Ensure both actors have been added to the batch.
            CPPUNIT_ASSERT(mGroundClamper->GetClampBatchSize() == 2);
            // NOTE: Makes sure modifications to time variables in this test do
            // not cause a bad time value comparison.
            CPPUNIT_ASSERT(currentTime != 0.0f);
            CPPUNIT_ASSERT(runtimeData1->GetLastClampedTime() == currentTime);
            CPPUNIT_ASSERT(runtimeData2->GetLastClampedTime() == currentTime);

            // Test batch clamping via a call to FinishUp, which subsequently
            // calls RunClampBatch. This method may be called from other components,
            // such as the Dead-Reckoning and Animation Components.
            mGroundClamper->FinishUp();
            // --- The batch should have been cleared after performing the whole
            //     clamp batch operation.
            CPPUNIT_ASSERT(mGroundClamper->GetClampBatchSize() == 0);
            // --- Get the actor positions.
            actor1->GetTransform(xform1);
            actor2->GetTransform(xform2);
            xform1.GetTranslation(pos1);
            xform2.GetTranslation(pos2);
            CPPUNIT_ASSERT(pos1.z() != 0.0f);
            CPPUNIT_ASSERT(pos2.z() != 0.0f);
            float errorTolerance = 0.1f;
            CPPUNIT_ASSERT_DOUBLES_EQUAL(resultPos1.z(), pos1.z(), errorTolerance);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(resultPos2.z(), pos2.z(), errorTolerance);
         }

         ///////////////////////////////////////////////////////////////////////
         void TestClampTransformUnchanged()
         {
            dtCore::RefPtr<DefaultGroundClamper::RuntimeData> runtimeData
               = new DefaultGroundClamper::RuntimeData;

            // Create the test actor.
            dtCore::Transform xform;
            dtCore::RefPtr<GameActorProxy> proxy;
            mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, proxy);
            CPPUNIT_ASSERT(proxy.valid());
            // --- Get the newly created actors.
            dtCore::Transformable* actor = NULL;
            proxy->GetActor(actor);

            // Create the terrain.
            dtCore::RefPtr<dtActors::InfiniteTerrainActorProxy> terrainProxy;
            dtCore::InfiniteTerrain* terrain = NULL;
            CreateTestTerrain(terrainProxy, terrain);
            mGroundClamper->SetTerrainActor(terrain);

            // Get the terrain height
            osg::Vec3 pos(12.34f, -54.76f, 10.9f);
            osg::Vec3 resultPos(pos);
            resultPos.z() = terrain->GetHeight(pos.x(), pos.y(), true);

            // Verify that the actor is not already at the height.
            xform.SetTranslation(pos);
            actor->SetTransform(xform);
            actor->GetTransform(xform);
            xform.GetTranslation(pos);
            CPPUNIT_ASSERT(pos != resultPos);

            // Clamp the actor, with transform flagged as changed for the first time.
            osg::Vec3 clampPos;
            osg::Matrix clampRotation;
            CPPUNIT_ASSERT(runtimeData->GetLastClampedRotation() == clampRotation);
            double curTime = 5.0;
            dtGame::GroundClampingData data;
            dtGame::BaseGroundClamper::GroundClampingType* clampType
               = &dtGame::BaseGroundClamper::GroundClampingType::INTERMITTENT_SAVE_OFFSET;
            data.SetUserData(runtimeData.get());
            data.SetAdjustRotationToGround(true);

            mGroundClamper->ClampToGround(*clampType, curTime,
               xform, *proxy, data, true);
            mGroundClamper->FinishUp();

            // Verify that the actor is now clamped.
            float errorThreshold = 0.1;
            osg::Matrix rotation;
            actor->GetTransform(xform);
            xform.GetTranslation(pos);
            xform.GetRotation(rotation);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(resultPos.x(), pos.x(), errorThreshold);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(resultPos.y(), pos.y(), errorThreshold);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(resultPos.z(), pos.z(), errorThreshold);
            CPPUNIT_ASSERT(CompareMatrices(runtimeData->GetLastClampedRotation(), rotation, errorThreshold));
            osg::Matrix lastClampRotation(rotation);

            // Clamp again without the transform flagged as changed.
            rotation.makeRotate(30.0f, osg::Vec3(0.0f,0.0f,1.0f));
            xform.SetRotation(rotation);
            actor->SetTransform(xform);
            osg::Matrix forcedRotation(rotation);
            curTime += 5.0;
            mGroundClamper->ClampToGround(*clampType, curTime,
               xform, *proxy, data, false);
            mGroundClamper->FinishUp();

            // Verify that the actor is now clamped, but the transform unchanged.
            actor->GetTransform(xform);
            xform.GetTranslation(pos);
            xform.GetRotation(rotation);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(resultPos.x(), pos.x(), errorThreshold);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(resultPos.y(), pos.y(), errorThreshold);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(resultPos.z(), pos.z(), errorThreshold);
            CPPUNIT_ASSERT(!CompareMatrices(runtimeData->GetLastClampedRotation(), forcedRotation, errorThreshold));
            CPPUNIT_ASSERT(CompareMatrices(runtimeData->GetLastClampedRotation(), lastClampRotation, errorThreshold));
            CPPUNIT_ASSERT(CompareMatrices(runtimeData->GetLastClampedRotation(), rotation, errorThreshold));
            lastClampRotation.set(rotation);

            // Clamp again with the transform flagged as changed, to ensure the clamper is not broken.
            curTime += 5.0;
            xform.SetRotation(forcedRotation);
            actor->SetTransform(xform);
            mGroundClamper->ClampToGround(*clampType, curTime,
               xform, *proxy, data, true);
            mGroundClamper->FinishUp();

            // Verify that the actor is now clamped, with the transform changed to the forced rotation.
            actor->GetTransform(xform);
            xform.GetTranslation(pos);
            xform.GetRotation(rotation);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(resultPos.x(), pos.x(), errorThreshold);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(resultPos.y(), pos.y(), errorThreshold);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(resultPos.z(), pos.z(), errorThreshold);
            CPPUNIT_ASSERT(!CompareMatrices(runtimeData->GetLastClampedRotation(), lastClampRotation, errorThreshold));
            CPPUNIT_ASSERT(CompareMatrices(runtimeData->GetLastClampedRotation(), rotation, errorThreshold));
         }

      private:

         dtCore::RefPtr<GameManager> mGM;
         dtCore::RefPtr<TestClamper> mGroundClamper;
         dtCore::RefPtr<GameActorProxy> mTestGameActor;
         static const std::string mTestGameActorRegistry;
   };

   CPPUNIT_TEST_SUITE_REGISTRATION(GroundClamperTests);

   const std::string GroundClamperTests::mTestGameActorRegistry("testGameActorLibrary");

}
