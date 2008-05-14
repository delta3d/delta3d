/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2006 MOVES Institute 
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
 * @author Chris Osborn
 */ 
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/inputdevice.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/flymotionmodel.h>

using namespace dtCore;

class TestAxisListener : public AxisListener
{
   public:

      TestAxisListener() :
         mAxisStateChanged(false)
      {
      }

      virtual bool AxisStateChanged( const Axis*, double, double, double )
      {
         mAxisStateChanged = true;

         // We really don't care about the return value in this context.
         // The tests will use this class as the last object in the input
         // chain.
         return false; 
      }

      bool HasAxisStateChanged() const { return mAxisStateChanged; }
      void Reset() { mAxisStateChanged = false; }

   private:

      bool mAxisStateChanged;
};

class MotionModelTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(MotionModelTests);
   CPPUNIT_TEST(TestOribitMotionModelAxisStateClobbering);
   CPPUNIT_TEST(TestFlyMotionModelProperties);
   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();

   void TestOribitMotionModelAxisStateClobbering();
   void TestFlyMotionModelProperties();
};

CPPUNIT_TEST_SUITE_REGISTRATION(MotionModelTests);

void MotionModelTests::setUp()
{
}

void MotionModelTests::tearDown()
{
}

void MotionModelTests::TestOribitMotionModelAxisStateClobbering()
{
   // This is a fun test. For a while, OribitMotionModel (with his
   // best intentions), was clobbering the ability of axis listeners
   // sharing the same axis with him to have their chance to be
   // notified of the axis state change. This test ensures this won't
   // ever happen again. Ever. -osb

   RefPtr<InputDevice> inputDevice( new InputDevice() );
   inputDevice->AddFeature( new Axis( inputDevice.get(), "Axis" ) );

   TestAxisListener testAxisListener;
   RefPtr<OrbitMotionModel> omm( new OrbitMotionModel() );

   // Since we are testing what happens when OrbitMotionModel's AxisStateChanged
   // does NOT handle the update, we need to make sure it is in a state that will
   // cause it to not handle the update (i.e. it is not enabled OR there is not
   // target set).
   omm->SetEnabled(false); 
   omm->SetTarget(0);

   inputDevice->GetAxis(0)->AddAxisListener( omm.get() );
   inputDevice->GetAxis(0)->AddAxisListener( &testAxisListener );
   
   CPPUNIT_ASSERT( !testAxisListener.HasAxisStateChanged() );
   inputDevice->GetAxis( 0 )->SetState( 2.0 );
   CPPUNIT_ASSERT( testAxisListener.HasAxisStateChanged() );
 }

void MotionModelTests::TestFlyMotionModelProperties()
{
   dtCore::RefPtr<dtCore::FlyMotionModel> motionModel = new dtCore::FlyMotionModel();
   const dtCore::FlyMotionModel* motionModelConst = motionModel.get();

   float flySpeed = 123.456f;
   float turnSpeed = 7.89f;
   dtCore::RefPtr<dtCore::Axis> testAxis_FB = new dtCore::Axis(NULL,"Test Axis Forward/Back");
   dtCore::RefPtr<dtCore::Axis> testAxis_LR = new dtCore::Axis(NULL,"Test Axis Left/Right");
   dtCore::RefPtr<dtCore::Axis> testAxis_UD = new dtCore::Axis(NULL,"Test Axis Up/Down");
   dtCore::RefPtr<dtCore::Axis> testAxis_Turn_LR = new dtCore::Axis(NULL,"Test Axis Turn Left/Right");
   dtCore::RefPtr<dtCore::Axis> testAxis_Turn_UD = new dtCore::Axis(NULL,"Test Axis Turn Up/Down");

   motionModel->SetMaximumFlySpeed( flySpeed );
   motionModel->SetMaximumTurnSpeed( turnSpeed );
   motionModel->SetFlyForwardBackwardAxis( testAxis_FB.get() );
   motionModel->SetFlyLeftRightAxis( testAxis_LR.get() );
   motionModel->SetFlyUpDownAxis( testAxis_UD.get() );
   motionModel->SetTurnLeftRightAxis( testAxis_Turn_LR.get() );
   motionModel->SetTurnUpDownAxis( testAxis_Turn_UD.get() );

   CPPUNIT_ASSERT( motionModelConst->GetMaximumFlySpeed() == flySpeed );
   CPPUNIT_ASSERT( motionModelConst->GetMaximumTurnSpeed() == turnSpeed );
   CPPUNIT_ASSERT( motionModelConst->GetFlyForwardBackwardAxis() == testAxis_FB.get() );
   CPPUNIT_ASSERT( motionModelConst->GetFlyLeftRightAxis() == testAxis_LR.get() );
   CPPUNIT_ASSERT( motionModelConst->GetFlyUpDownAxis() == testAxis_UD.get() );
   CPPUNIT_ASSERT( motionModelConst->GetTurnLeftRightAxis() == testAxis_Turn_LR.get() );
   CPPUNIT_ASSERT( motionModelConst->GetTurnUpDownAxis() == testAxis_Turn_UD.get() );

   CPPUNIT_ASSERT( motionModelConst->GetUseSimTimeForSpeed());
   motionModel->SetUseSimTimeForSpeed( false );
   CPPUNIT_ASSERT( ! motionModelConst->GetUseSimTimeForSpeed() );
}
