/** \author John K. Grant
  * \date August 26, 2005
  * \file testRecorder.cpp
  */

#include "testrecorder.h"
#include <dtCore/flymotionmodel.h>

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
void TestRecorder::KeyPressed(dtCore::Keyboard* keyboard,
                              Producer::KeyboardKey key,
                              Producer::KeyCharacter character)
{
   switch( key )
   {
   case Producer::Key_R:  // start recording
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
      } break;

   case Producer::Key_F:  // save to file
      {
         LOG_ALWAYS("Saving file " + mFileHandle )
         mRecorder->SaveFile( mFileHandle );
         LOG_ALWAYS("...Finished saving file " + mFileHandle )
      } break;

   case Producer::Key_L:  // load from file
      {
         LOG_ALWAYS("Loading file: " + mFileHandle)
         mRecorder->LoadFile( mFileHandle );
         LOG_ALWAYS("...done loading file: " + mFileHandle)
      } break;

   case Producer::Key_P:  // play loaded file
      {
         LOG_ALWAYS("Playing loaded data.")
         mRecorder->Play();
      };

   default:   // don't care about the key
      {
         BaseClass::KeyPressed(keyboard,key,character);
      } break;
   }
}

void TestRecorder::SetupCamera()
{
   // use a motion model
   mMotionModel = new dtCore::FlyMotionModel( GetKeyboard(), GetMouse() );

   dtCore::Transform xform(0.0f,0.0f,mTerrain->GetVerticalScale()+15.0f);

   GetCamera()->SetTransform( &xform );

   // affect the camera
   mMotionModel->SetTarget( GetCamera() );
}

void TestRecorder::SetupScene()
{
   // use the infinite terrain class, add it to the scene
   AddDrawable( mTerrain.get() );
}

