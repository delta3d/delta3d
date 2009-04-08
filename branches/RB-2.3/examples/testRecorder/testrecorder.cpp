//////////////////////////////////////////////////////////////////////////////
// NOTE - This test is provided for historical reference. The ability to
// record and playback is provided via the AAR Record and Playback
// components in dtGame. See testAAR for a example of using this behavior.
//////////////////////////////////////////////////////////////////////////////

/** \author John K. Grant
  * \date August 26, 2005
  * \file testRecorder.cpp
  */

#include "testrecorder.h"
#include <dtCore/flymotionmodel.h>
#include <dtCore/camera.h>
#include <dtCore/transform.h>

TestRecorder::TestRecorder(const std::string& config)
   : BaseClass(config)
   , mRecorder(new CameraRecorder()), mRecordable(0), mMotionModel(0), mTerrain(new dtCore::InfiniteTerrain())
   , mFileHandle("recording.xml")
{
   mRecordable = new CameraRecorderAdaptor(GetCamera());
   mRecorder->AddSource(mRecordable.get());

   SetupCamera();
   SetupScene();
}

TestRecorder::~TestRecorder()
{
}

// inherited functions
bool TestRecorder::KeyPressed(const dtCore::Keyboard* keyboard, int key)
{
   bool verdict(false);
   switch (key)
   {
   case 'r':  // start recording
      {
         if (mRecorder->GetState() == CameraRecorder::Recording)
         {
            mRecorder->Stop();  // should probably be called 'StopRecording'
            LOG_ALWAYS("Stopped recording")
         }

         else
         {
            LOG_ALWAYS("Recording now.")
            mRecorder->Record();
         }
         verdict = true;
      } break;

   case 'f':  // save to file
      {
         LOG_ALWAYS("Saving file " + mFileHandle)
         mRecorder->SaveFile(mFileHandle);
         LOG_ALWAYS("...Finished saving file " + mFileHandle)
         verdict = true;
      } break;

   case 'l':  // load from file
      {
         LOG_ALWAYS("Loading file: " + mFileHandle)
         mRecorder->LoadFile(mFileHandle);
         LOG_ALWAYS("...done loading file: " + mFileHandle)
         verdict = true;
      } break;

   case 'p':  // play loaded file
      {
         LOG_ALWAYS("Playing loaded data.")
         mRecorder->Play();
         verdict = true;
      }; break;

   case osgGA::GUIEventAdapter::KEY_F1:
      {
         mLabel->SetActive(!mLabel->GetActive());
      } break;

   default:   // don't care about the key
      {
         verdict = BaseClass::KeyPressed(keyboard,key);
      } break;
   }

   return verdict;
}

void TestRecorder::SetupCamera()
{
   // use a motion model
   mMotionModel = new dtCore::FlyMotionModel(GetKeyboard(), GetMouse());

   dtCore::Transform xform(0.0f, 0.0f, mTerrain->GetVerticalScale() + 15.0f);

   GetCamera()->SetTransform(xform);

   // affect the camera
   mMotionModel->SetTarget(GetCamera());
}

void TestRecorder::SetupScene()
{
   // use the infinite terrain class, add it to the scene
   AddDrawable(mTerrain.get());

   CreateHelpLabel();
}

void TestRecorder::CreateHelpLabel()
{
   mLabel = new dtABC::LabelActor();
   osg::Vec2 testSize(19.0f, 5.5f);
   mLabel->SetBackSize(testSize);
   mLabel->SetFontSize(0.8f);
   mLabel->SetTextAlignment(dtABC::LabelActor::AlignmentEnum::LEFT_CENTER);
   mLabel->SetText(CreateHelpLabelText());
   mLabel->SetEnableDepthTesting(false);
   mLabel->SetEnableLighting(false);

   GetCamera()->AddChild(mLabel.get());
   dtCore::Transform labelOffset(-17.0f, 50.0f, 10.5f, 0.0f, 90.0f, 0.0f);
   mLabel->SetTransform(labelOffset, dtCore::Transformable::REL_CS);
   AddDrawable(GetCamera());
}

std::string TestRecorder::CreateHelpLabelText()
{
   std::string testString("");
   testString += "F1: Toggle Help Screen\n";
   testString += "\n";
   testString += "r: Toggle recording\n";
   testString += "f: Save recording to file\n";
   testString += "l: Load recording from file\n";
   testString += "p: Play loaded recording\n";

   return testString;
}

