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

TestRecorder::TestRecorder(const std::string& config): BaseClass(config),
mRecorder(new CameraRecorder()), mRecordable(0), mMotionModel(0), mTerrain(new dtCore::InfiniteTerrain()), mFileHandle("recording.xml")
{
   mRecordable = new CameraRecorderAdaptor( GetCamera() );
   mRecorder->AddSource( mRecordable.get() );

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
   switch( key )
   {
   case 'r':  // start recording
      {
         if( mRecorder->GetState() == CameraRecorder::Recording )
         {
            mRecorder->Stop();  // should probably be called 'StopRecording'
            LOG_ALWAYS( "Stopped recording" )
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
         LOG_ALWAYS("Saving file " + mFileHandle )
         mRecorder->SaveFile( mFileHandle );
         LOG_ALWAYS("...Finished saving file " + mFileHandle )
         verdict = true;
      } break;

   case 'l':  // load from file
      {
         LOG_ALWAYS("Loading file: " + mFileHandle)
         mRecorder->LoadFile( mFileHandle );
         LOG_ALWAYS("...done loading file: " + mFileHandle)
         verdict = true;
      } break;

   case 'p':  // play loaded file
      {
         LOG_ALWAYS("Playing loaded data.")
         mRecorder->Play();
         verdict = true;
      }; break;

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
   mMotionModel = new dtCore::FlyMotionModel( GetKeyboard(), GetMouse() );

   dtCore::Transform xform(0.0f,0.0f,mTerrain->GetVerticalScale()+15.0f);

   GetCamera()->SetTransform( xform );

   // affect the camera
   mMotionModel->SetTarget( GetCamera() );
}

void TestRecorder::SetupScene()
{
   // use the infinite terrain class, add it to the scene
   AddDrawable( mTerrain.get() );
}

