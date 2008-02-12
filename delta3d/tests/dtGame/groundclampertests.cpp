/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology, BMH Operation.
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
