/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, MOVES Institute
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

//TODO this needs to be fixed.  The getter is not there on the trunk, nor can it be
//implemented the same as it was on the branch.
//   CPPUNIT_ASSERT( motionModelConst->GetUseSimTimeForSpeed());
//   motionModel->SetUseSimTimeForSpeed( false );
//   CPPUNIT_ASSERT( ! motionModelConst->GetUseSimTimeForSpeed() );
}
