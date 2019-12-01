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
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/inputdevice.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/mouse.h>
#include <dtCore/keyboard.h>
#include <dtCore/transform.h>
#include <dtCore/transformable.h>
#include <dtCore/system.h>

using namespace dtCore;

class TestAxisHandler : public AxisHandler
{
   public:

      TestAxisHandler() :
         mAxisStateChanged(false)
      {
      }

      virtual bool HandleAxisStateChanged( const Axis*, double, double, double )
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
   CPPUNIT_TEST(TestOrbitMotionModelAxisStateClobbering);
   CPPUNIT_TEST(TestFlyMotionModelProperties);
   CPPUNIT_TEST(TestFlyMotionModelOptions);
   CPPUNIT_TEST(TestFlyMotionModelUpdate);
   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();

   void TestOrbitMotionModelAxisStateClobbering();
   void TestFlyMotionModelProperties();
   void TestFlyMotionModelOptions();
   void TestFlyMotionModelUpdate();
};

CPPUNIT_TEST_SUITE_REGISTRATION(MotionModelTests);

void MotionModelTests::setUp()
{
   dtCore::System::GetInstance().Start();
}

void MotionModelTests::tearDown()
{
   dtCore::System::GetInstance().Stop();
}

void MotionModelTests::TestOrbitMotionModelAxisStateClobbering()
{
   // This is a fun test. For a while, OribitMotionModel (with his
   // best intentions), was clobbering the ability of axis handlers
   // sharing the same axis with him to have their chance to be
   // notified of the axis state change. This test ensures this won't
   // ever happen again. Ever. -osb

   RefPtr<InputDevice> inputDevice( new InputDevice() );
   inputDevice->AddFeature( new Axis( inputDevice.get(), "Axis" ) );

   TestAxisHandler testAxisHandler;
   RefPtr<OrbitMotionModel> omm( new OrbitMotionModel() );

   // Since we are testing what happens when OrbitMotionModel's AxisStateChanged
   // does NOT handle the update, we need to make sure it is in a state that will
   // cause it to not handle the update (i.e. it is not enabled OR there is not
   // target set).
   omm->SetEnabled(false);
   omm->SetTarget(0);

   inputDevice->GetAxis(0)->AddAxisHandler( omm.get() );
   inputDevice->GetAxis(0)->AddAxisHandler( &testAxisHandler );

   CPPUNIT_ASSERT( !testAxisHandler.HasAxisStateChanged() );
   inputDevice->GetAxis( 0 )->SetState( 2.0 );
   inputDevice->GetAxis( 0 )->NotifyStateChange();
   CPPUNIT_ASSERT( testAxisHandler.HasAxisStateChanged() );
 }


void MotionModelTests::TestFlyMotionModelUpdate()
{
   RefPtr<Mouse> mouse = new Mouse();
   RefPtr<Keyboard> kb = new Keyboard();

   dtCore::RefPtr<dtCore::FlyMotionModel> motionModel = new dtCore::FlyMotionModel(kb, mouse);

   motionModel->SetEnabled(true);

   dtCore::RefPtr<dtCore::Transformable> parent, target;

   parent = new dtCore::Transformable("parent");
   target = new dtCore::Transformable("target");

   motionModel->SetTarget(target);

   parent->AddChild(target);

   dtCore::Transform xform;
   xform.SetTranslation(osg::Vec3(90.0, 5000.1, -34.76));
   xform.SetRotation(osg::Vec3(0.0, 0.0, 3.0));
   parent->SetTransform(xform, dtCore::Transformable::REL_CS);
   xform.MakeIdentity();
   target->SetTransform(xform, dtCore::Transformable::REL_CS);

   dtCore::System::GetInstance().Step();

   target->GetTransform(xform, dtCore::Transformable::REL_CS);
   osg::Vec3 hpr;
   xform.GetRotation(hpr);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, float(hpr[0]), 0.01f);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, float(hpr[1]), 0.01f);
   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The roll should be -2.0 because the fly motion model "
            "sets the absolute roll to 1 degree of error.  Setting it to -3.0 would no error, but it causes jerky behavior.",
            -2.0f, float(hpr[2]), 0.2f);

   xform.SetRotation(osg::Vec3(0.0, 0.0, -2.4f));
   target->SetTransform(xform, dtCore::Transformable::REL_CS);

   target->GetTransform(xform, dtCore::Transformable::REL_CS);
   xform.GetRotation(hpr);
   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The roll should not have changed because it uses a 1 degree margin of error.",
            -2.4f, float(hpr[2]), 0.2f);

   dtCore::System::GetInstance().Step();

   dtCore::Transform xform2;
   target->GetTransform(xform2, dtCore::Transformable::REL_CS);

   CPPUNIT_ASSERT_MESSAGE("The transform should not have changed AT ALL because"
            " none of the axes are different and the roll didn't change.", xform.EpsilonEquals(xform2, 0.0001));

   motionModel->GetFlyForwardBackwardAxis()->SetState(0.5, 0.5);
   motionModel->GetFlyLeftRightAxis()->SetState(0.5, 0.5);
   motionModel->GetFlyUpDownAxis()->SetState(0.5, 0.5);
   motionModel->GetTurnLeftRightAxis()->SetState(0.5, 0.5);
   motionModel->GetTurnUpDownAxis()->SetState(0.5, 0.5);

   motionModel->OnSystem(dtCore::System::MESSAGE_POST_EVENT_TRAVERSAL, 0.7, 0.7);

   target->GetTransform(xform2, dtCore::Transformable::REL_CS);

   CPPUNIT_ASSERT_MESSAGE("The transform should have changed quite a bit.", !xform.EpsilonEquals(xform2, 0.500));
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

void MotionModelTests::TestFlyMotionModelOptions()
{
   using namespace dtCore;

   RefPtr<Mouse> mouse = new Mouse();
   RefPtr<Keyboard> kb = new Keyboard();
   const unsigned int options = FlyMotionModel::OPTION_REQUIRE_MOUSE_DOWN |
                                FlyMotionModel::OPTION_USE_SIMTIME_FOR_SPEED;

   RefPtr<FlyMotionModel> motionModel = new FlyMotionModel(kb.get(),
                                                           mouse.get(),
                                                           options);


   CPPUNIT_ASSERT_EQUAL_MESSAGE("FlyMotionModel doesn't have the MOUSE_DOWN option",
      true, motionModel->HasOption(FlyMotionModel::OPTION_REQUIRE_MOUSE_DOWN));

   CPPUNIT_ASSERT_EQUAL_MESSAGE("FlyMotionModel doesn't have the USE_SIMTIME option",
      true, motionModel->HasOption(FlyMotionModel::OPTION_USE_SIMTIME_FOR_SPEED));

   CPPUNIT_ASSERT_EQUAL_MESSAGE("FlyMotionModel shouldn't have the RESET_MOUSE option",
      false, motionModel->HasOption(FlyMotionModel::OPTION_RESET_MOUSE_CURSOR));

   motionModel->SetUseSimTimeForSpeed(false);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("FlyMotionModel USE_SIMTIME option should be turned off",
      false, motionModel->HasOption(FlyMotionModel::OPTION_USE_SIMTIME_FOR_SPEED));
}
