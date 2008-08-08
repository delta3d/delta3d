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
* David Guthrie
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

#include <dtGame/basemessages.h>
#include <dtGame/gamemanager.h> 
#include <dtGame/messagefactory.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/groundclamper.h>

#include <dtDAL/actortype.h>

#include <dtActors/engineactorregistry.h>

#include <dtABC/application.h>
extern dtABC::Application& GetGlobalApplication();

namespace dtGame
{
   class GroundClamperTests : public CPPUNIT_NS::TestFixture 
   {
      CPPUNIT_TEST_SUITE(GroundClamperTests);
   
         CPPUNIT_TEST(TestTerrainProperty);
         CPPUNIT_TEST(TestEyePointProperty);
         CPPUNIT_TEST(TestIntermittentProperties);
         CPPUNIT_TEST(TestHighResClampProperty);
         CPPUNIT_TEST(TestLowResClampProperty);
         CPPUNIT_TEST(TestClampToNearest);
         
      CPPUNIT_TEST_SUITE_END();
   
      public:
   
         void setUp()
         {
            mGM = new dtGame::GameManager(*GetGlobalApplication().GetScene());
            mGM->SetApplication(GetGlobalApplication());
            mGroundClamper = new GroundClamper();
            mGM->LoadActorRegistry(mTestGameActorRegistry);

            mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, mTestGameActor);
            CPPUNIT_ASSERT(mTestGameActor.valid());
         }

         void tearDown()
         {
            if(mGM.valid())
            {
               mTestGameActor = NULL;
               mGM->DeleteAllActors(true);
               mGM->UnloadActorRegistry(mTestGameActorRegistry);
               mGM = NULL;
            }
         }

         void TestIntermittentProperties()
         {
            CPPUNIT_ASSERT_EQUAL(1.0f, mGroundClamper->GetIntermittentGroundClampingTimeDelta());
            mGroundClamper->SetIntermittentGroundClampingTimeDelta(40.0f);
            CPPUNIT_ASSERT_EQUAL(40.0f, mGroundClamper->GetIntermittentGroundClampingTimeDelta());

            CPPUNIT_ASSERT_EQUAL(0.25f, mGroundClamper->GetIntermittentGroundClampingSmoothingTime());
            mGroundClamper->SetIntermittentGroundClampingSmoothingTime(40.0f);
            CPPUNIT_ASSERT_EQUAL(40.0f, mGroundClamper->GetIntermittentGroundClampingSmoothingTime());
         }
         
         void TestTerrainProperty()
         {
            CPPUNIT_ASSERT(mGroundClamper->GetTerrainActor() == NULL);
            
            mGM->AddActor(*mTestGameActor, false, false);
            dtCore::Transformable* terrain = &mTestGameActor->GetGameActor();

            mGroundClamper->SetTerrainActor(terrain);

            CPPUNIT_ASSERT_MESSAGE("The terrain should be set.", 
                     mGroundClamper->GetTerrainActor() == terrain);
         }

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
         
         void TestHighResClampProperty()
         {
            CPPUNIT_ASSERT_EQUAL(0.0f, mGroundClamper->GetHighResGroundClampingRange());
            float value = 10.0f;
            mGroundClamper->SetHighResGroundClampingRange(value);
            CPPUNIT_ASSERT_EQUAL(value, mGroundClamper->GetHighResGroundClampingRange());
         }

         void TestLowResClampProperty()
         {
            CPPUNIT_ASSERT_EQUAL(0.0f, mGroundClamper->GetLowResGroundClampingRange());
            float value = 10.0f;
            mGroundClamper->SetLowResGroundClampingRange(value);
            CPPUNIT_ASSERT_EQUAL(value, mGroundClamper->GetLowResGroundClampingRange());
         }

         void TestClampToNearest()
         {
            dtCore::RefPtr<dtCore::BatchIsector::SingleISector> single = 
               new dtCore::BatchIsector::SingleISector(0);
            osgUtil::IntersectVisitor::HitList hitList;

            osg::Vec3 point, normal;

            CPPUNIT_ASSERT_MESSAGE("No nearest hit should have been found if no hits exist.", 
                     !mGroundClamper->GetClosestHit(*single, 0.03, point, normal));

            osgUtil::Hit hit;
            hit._intersectNormal = osg::Vec3(0.0f, 0.0f, 1.0f);
            hit._intersectPoint = osg::Vec3(3.0f, 4.0f, 1.0f);
            hitList.push_back(hit);

            hit._intersectNormal = osg::Vec3(0.0f, 1.0f, 0.0f);
            hit._intersectPoint = osg::Vec3(3.0f, 4.0f, 4.0f);
            hitList.push_back(hit);

            single->SetHitList(hitList);
            
            CPPUNIT_ASSERT_MESSAGE("A nearest hit should have been found.", 
                     mGroundClamper->GetClosestHit(*single, 0.03, point, normal));

            CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0f, point.x(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0f, point.y(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, point.z(), 0.001);
            
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, normal.x(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, normal.y(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, normal.z(), 0.001);
            
            CPPUNIT_ASSERT_MESSAGE("A nearest hit should have been found.", 
                     mGroundClamper->GetClosestHit(*single, 3.4, point, normal));

            CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0f, point.x(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0f, point.y(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0f, point.z(), 0.001);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, normal.x(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, normal.y(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, normal.z(), 0.001);

            CPPUNIT_ASSERT_MESSAGE("A nearest hit should have been found.", 
                     mGroundClamper->GetClosestHit(*single, 5.0, point, normal));

            CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0f, point.x(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0f, point.y(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0f, point.z(), 0.001);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, normal.x(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, normal.y(), 0.001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, normal.z(), 0.001);
         }

      private:

         dtCore::RefPtr<GameManager> mGM;
         dtCore::RefPtr<GroundClamper> mGroundClamper;
         dtCore::RefPtr<GameActorProxy> mTestGameActor;
         static const std::string mTestGameActorRegistry;
   };
   
   CPPUNIT_TEST_SUITE_REGISTRATION(GroundClamperTests);
   
   const std::string GroundClamperTests::mTestGameActorRegistry("testGameActorLibrary");
   
}
