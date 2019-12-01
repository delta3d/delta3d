/* -*-c++-*-
* testAudio - testaudio (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
*/
#ifndef  DELTA_TEST_AUDIO
#define  DELTA_TEST_AUDIO

#include <string>
#include <vector>
#include <queue>

#include <dtAudio/sound.h>
#include <dtABC/application.h>
#include <dtABC/labelactor.h>
#include <dtCore/recorder.h>
#include <dtCore/refptr.h>

namespace dtAudio
{
   class Listener;
   class Sound;
   class SoundEffectBinder;
}

namespace dtCore
{
   class Camera;
   class EffectManager;
   class Object;
   class OrbitMotionModel;
   class ParticleSystem;
}

class TestAudioApp : public dtABC::Application
{
   DECLARE_MANAGEMENT_LAYER( TestAudioApp )
private:
   typedef  dtCore::Recorder<dtAudio::Sound,dtAudio::Sound::FrameData> SoundRecorder;

   typedef  std::vector<dtAudio::Sound*>                 SND_LST;
   typedef  SND_LST::iterator                            SND_ITR;
   typedef  std::queue<dtAudio::Sound*>                  SND_QUE;
   typedef  dtCore::RefPtr<dtCore::Object>               OBJ_PTR;
   typedef  dtCore::RefPtr<dtCore::EffectManager>        FXM_PTR;
   typedef  dtCore::RefPtr<dtCore::ParticleSystem>       PAR_PTR;
   typedef  dtCore::RefPtr<dtAudio::SoundEffectBinder>   BIN_PTR;
   typedef  dtCore::RefPtr<dtCore::OrbitMotionModel>     OMM_PTR;

private:
   static unsigned int kNumSoundFiles;
   static const char* kSoundFile[];

   enum GfxObjId
   {
      GROUND   = 0L,
      HELO,
      TRUCK,
      kNumGfx
   };

   static const char* kGfxFile[kNumGfx];

   enum FxObjId
   {
      EXPLODE  = 0L,
      SMOKE,
      kNumFx
   };

   static const char* kFxFile[kNumFx];
   static const char* kFxDetonationType[kNumFx];

public:
   TestAudioApp(const std::string& configFilename = "config.xml");
protected:
   virtual ~TestAudioApp();

public:
   virtual void PreFrame(const double deltaFrameTime);
   virtual void Frame(const double deltaFrameTime);
   virtual void PostFrame(const double deltaFrameTime);
   bool KeyPressed(const dtCore::Keyboard* keyboard, int key);

private:
   void CreateHelpLabel();
   std::string CreateHelpLabelText();

   inline void LoadPlaySound(const char* fname, unsigned int box = 0L );
   inline void StopAllSounds(void);
   inline void FreeAllStoppedSounds(bool forced = false);
   inline void FlushQueuedSounds(void);
   inline void ChangeSoundGain(float gain);
   inline void ChangeSoundPitch(float pitch);
   inline void ToggleSoundLooping(void);
   inline void PauseAllSounds(void);
   inline void RewindAllSounds(void);

   inline void                                     SetUpVisuals(void);
   inline dtCore::RefPtr<dtCore::Object>           LoadGfxFile(const char* fname);
   inline dtCore::RefPtr<dtCore::EffectManager>    LoadFxFile(const char* fname);
   inline dtCore::RefPtr<dtCore::ParticleSystem>   LoadPSFile(const char* fname);
   inline void                                     InitInputDevices(void);
   inline void                                     SetUpCamera(void);
   inline void MoveTheObject(unsigned int obj, const double deltaFrameTime);

   static void MakeSmoke(dtAudio::Sound* sound, void* param);
   static void StopSmoke(dtAudio::Sound* sound, void* param);

   void StartRecording();
   void StopRecording();

private:
   SND_LST                       mActive;
   SND_QUE                       mQueued;
   float                         mSndGain;
   float                         mSndPitch;
   bool                          mLooping;
   dtAudio::Listener*            mMic;

   OBJ_PTR                           mGfxObj[kNumGfx];
   PAR_PTR                           mPSysA;
   PAR_PTR                           mPSysC;
   FXM_PTR                           mFXMgr;
   BIN_PTR                           mSFXBinder;
   OMM_PTR                           mOribitMotionModel;
   unsigned int                      mSmokeCountA;
   unsigned int                      mSmokeCountC;
   dtCore::RefPtr<SoundRecorder>     mRecorder;
   dtCore::RefPtr<dtABC::LabelActor> mLabel;
};

#endif  // DELTA_TEST_AUDIO
