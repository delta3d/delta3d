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

#include <dtUtil/log.h>
#include <dtUtil/datapathutils.h>

#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>

#include <dtCore/datatype.h>

#include <dtGame/messageparameter.h>
#include <dtGame/machineinfo.h>
#include <dtGame/gameactor.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtGame/messagefactory.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/drpublishingactcomp.h>
#include <dtGame/defaultnetworkpublishingcomponent.h>
#include <dtGame/defaultmessageprocessor.h>

#include <dtActors/engineactorregistry.h>

#include "basegmtests.h"

#include <dtABC/application.h>

namespace dtGame
{
   class TestDRPublishingActComp : public DRPublishingActComp
   {
   public:
      TestDRPublishingActComp(): DRPublishingActComp(true) {}

      void CallVelCalc(float deltaTime, const osg::Vec3& pos, const osg::Vec3& rot)
      {
         CalculateCurrentVelocity(deltaTime, pos, rot);
      }
   };

   class DRPublishingActCompTests : public BaseGMTestFixture
   {
      typedef BaseGMTestFixture BaseClass;
      CPPUNIT_TEST_SUITE(DRPublishingActCompTests);

         CPPUNIT_TEST(TestBasics);
         CPPUNIT_TEST(TestCalculateVelocity);

      CPPUNIT_TEST_SUITE_END();

   public:
      /*override*/ void setUp();
      /*override*/ void tearDown();

      void TestBasics()
      {
         TestDRPublishingActComp* drp = mActor->GetComponent<TestDRPublishingActComp>();
         CPPUNIT_ASSERT(drp != NULL);
         CPPUNIT_ASSERT(drp->GetAccelSource() == NULL);
         CPPUNIT_ASSERT(drp->GetVelocitySource() == NULL);
         CPPUNIT_ASSERT(drp->GetAngVelSource() == NULL);
         CPPUNIT_ASSERT_EQUAL(0.0f, drp->GetVelocity().length());
         CPPUNIT_ASSERT_EQUAL(0.0f, drp->GetAcceleration().length());
         CPPUNIT_ASSERT_EQUAL(0.0f, drp->GetAngularVelocity().length());
         CPPUNIT_ASSERT_EQUAL(mActor->GetComponent<DeadReckoningActorComponent>(), drp->GetDeadReckoningActorComponent());
         CPPUNIT_ASSERT(drp->IsDeadReckoningActorComponentValid());
         CPPUNIT_ASSERT(drp->GetPublishAngularVelocity());
         CPPUNIT_ASSERT(drp->GetPublishLinearVelocity());
         CPPUNIT_ASSERT(drp->GetDrawable() == NULL);
         CPPUNIT_ASSERT(drp->GetMaxRotationError());

         CPPUNIT_ASSERT(drp->GetVelocityClampMagnitude() > FLT_EPSILON);
         CPPUNIT_ASSERT_EQUAL(1, drp->GetVelocityAverageFrameCount());

         CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0f, drp->GetMaxUpdateSendRate(), 0.001f);

      }

      void TestCalculateVelocity()
      {
         float stepTime = 1.0f/60.0f;
         TestDRPublishingActComp* drp = mActor->GetComponent<TestDRPublishingActComp>();
         CPPUNIT_ASSERT(drp != NULL);

         dtCore::Transform xform;
         drp->CallVelCalc(stepTime, xform.GetTranslation(), xform.GetRotation());
         xform.SetTranslation(0.0,0.0,0.001);
         mActor->GetDrawable<dtCore::Transformable>()->SetTransform(xform);
         drp->CallVelCalc(stepTime, xform.GetTranslation(), xform.GetRotation());
         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("This is smaller than the clamp threshold.", 0.00f, drp->GetVelocity().length(), 0.002f);


         // clamping makes tasking annoying
         drp->SetVelocityClampMagnitude(0.0f);

         float accumTime = stepTime;
         osg::Vec3 accel(0.2f, -0.2f, -0.01f);
         std::string idx;
         for (unsigned i = 0; i < 20; ++i)
         {
            idx = dtUtil::ToString(i);
            osg::Vec3 curvel = accel * accumTime;
            xform.SetTranslation(xform.GetTranslation() + (curvel * stepTime));
            drp->CallVelCalc(stepTime, xform.GetTranslation(), xform.GetRotation());

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(std::string("Updating velocity should keep up with motion. idx ") + idx, curvel.length(), drp->GetVelocity().length(), 0.002f);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(std::string("Updating accel should keep up with motion. idx ") + idx, accel.length(), drp->GetAcceleration().length(), 0.002f);
            accumTime += stepTime;
         }
      }


   private:

      dtCore::RefPtr<GameActorProxy> mActor;

   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION(DRPublishingActCompTests);


   //////////////////////////////////////////////////////////////////////////
   void DRPublishingActCompTests::setUp()
   {

      BaseClass::setUp();

      try
      {
         mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, mActor);

         mActor->AddComponent(*new DeadReckoningActorComponent);
         mActor->AddComponent(*new TestDRPublishingActComp);

         dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
         dtCore::System::GetInstance().Start();

         mTestComp->reset();
         //Publish the actor.
         mGM->AddActor(*mActor, false, false);
         dtCore::System::GetInstance().Step();
      }
      catch (const dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL((std::string("Error: ") + ex.ToString()).c_str());
      }

   }

   //////////////////////////////////////////////////////////////////////////
   void DRPublishingActCompTests::tearDown()
   {
      BaseClass::tearDown();
   }

}
