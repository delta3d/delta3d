/** \author John K. Grant
  * \date August 26, 2005
  * \file testRecorder.h
  */

#ifndef DELTA_TEST_RECORDER_INC
#define DELTA_TEST_RECORDER_INC

#include <dtCore/motionmodel.h>     // for member
#include <dtCore/infiniteterrain.h> // for member
#include <string>                   // for member

#include <dtABC/application.h>      // for base class
#include <dtCore/recorder.h>        // for member
#include "camerarecorderadaptor.h"  // for member


/** \brief Fly around terrain recording the camera position.
  * TestRecorder is an Application which will allow you to
  * fly around terrain and record the Camera data if desired.
  * The main point is to show how to use the Recorder class.
  * \todo make 'l' load from a file.
  * \todo print to the game screen Recorder: ON, or Recorder: OFF
  */
class TestRecorder : public dtABC::Application
{
public:
   typedef dtABC::Application BaseClass;

   TestRecorder(const std::string& config);

protected:
   virtual ~TestRecorder();

   // inherited functions
   virtual void KeyPressed(dtCore::Keyboard* keyboard, Producer::KeyboardKey key, Producer::KeyCharacter character);

   // initializations
   void SetupCamera();
   void SetupScene();

private:
   typedef dtCore::Recorder<CameraRecorderAdaptor,CameraRecorderAdaptor::FrameData> CameraRecorder;
   dtCore::RefPtr<CameraRecorder> mRecorder;
   dtCore::RefPtr<CameraRecorderAdaptor> mRecordable;
   dtCore::RefPtr<dtCore::MotionModel> mMotionModel;
   dtCore::RefPtr<dtCore::InfiniteTerrain> mTerrain;
   std::string mFileHandle;
};

#endif // DELTA_TEST_RECORDER_INC
