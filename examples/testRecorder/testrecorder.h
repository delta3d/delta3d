/* -*-c++-*-
* testRecorder - testrecorder (.h & .cpp) - Using 'The MIT License'
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
* author John K. Grant
*/

#ifndef DELTA_TEST_RECORDER_INC
#define DELTA_TEST_RECORDER_INC

#include <dtCore/motionmodel.h>     // for member
#include <dtCore/infiniteterrain.h> // for member
#include <string>                   // for member

#include <dtABC/application.h>      // for base class
#include <dtABC/labelactor.h>       // for member
#include <dtCore/recorder.h>        // for member
#include "camerarecorderadaptor.h"  // for member


//////////////////////////////////////////////////////////////////////////////
// NOTE - This test is provided for historical reference. The ability to 
// record and playback is provided via the AAR Record and Playback 
// components in dtGame. See testAAR for a example of using this behavior.
//////////////////////////////////////////////////////////////////////////////

/** \brief Fly around terrain recording the camera position.
  * TestRecorder is an Application which will allow you to
  * fly around terrain and record the Camera data if desired.
  * The main point is to show how to use the Recorder class.
  */
class TestRecorder : public dtABC::Application
{
public:
   typedef dtABC::Application BaseClass;

   TestRecorder(const std::string& config);

protected:
   virtual ~TestRecorder();

   // inherited functions
   virtual bool KeyPressed(const dtCore::Keyboard* keyboard, int key);

   // initializations
   void SetupCamera();
   void SetupScene();

private:
   void CreateHelpLabel();
   std::string CreateHelpLabelText();

   typedef dtCore::Recorder<CameraRecorderAdaptor,CameraRecorderAdaptor::FrameData> CameraRecorder;
   dtCore::RefPtr<CameraRecorder> mRecorder;
   dtCore::RefPtr<CameraRecorderAdaptor> mRecordable;
   dtCore::RefPtr<dtCore::MotionModel> mMotionModel;
   dtCore::RefPtr<dtCore::InfiniteTerrain> mTerrain;
   dtCore::RefPtr<dtABC::LabelActor> mLabel;
   std::string mFileHandle;
};

#endif // DELTA_TEST_RECORDER_INC
