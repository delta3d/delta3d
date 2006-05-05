#ifndef  DELTA_TEST_AUDIO
#define  DELTA_TEST_AUDIO

#include <string>
#include <vector>
#include <queue>

#include "dtCore/dt.h"
#include "dtAudio/dtaudio.h"
#include "dtABC/dtabc.h"
#include "dtCore/recorder.h"

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
   typedef  dtCore::RefPtr<dtCore::LogicalInputDevice>   LID_PTR;
   typedef  dtCore::RefPtr<dtCore::MotionModel>          MM_PTR;

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

public:
   TestAudioApp(const std::string& configFilename = "config.xml" );
   virtual ~TestAudioApp();

   virtual void PreFrame( const double deltaFrameTime );
   virtual void Frame( const double deltaFrameTime );
   virtual void PostFrame( const double deltaFrameTime );
   bool KeyPressed(const dtCore::Keyboard* keyboard, Producer::KeyboardKey key, Producer::KeyCharacter character);

private:
   inline void LoadPlaySound( const char* fname, unsigned int box = 0L );
   inline void StopAllSounds( void );
   inline void FreeAllStoppedSounds( bool forced = false );
   inline void FlushQueuedSounds( void );
   inline void ChangeSoundGain( float gain );
   inline void ChangeSoundPitch( float pitch );
   inline void ToggleSoundLooping( void );
   inline void PauseAllSounds( void );
   inline void RewindAllSounds( void );

   inline void                                     SetUpVisuals( void );
   inline dtCore::RefPtr<dtCore::Object>           LoadGfxFile( const char* fname );
   inline dtCore::RefPtr<dtCore::EffectManager>    LoadFxFile( const char* fname );
   inline dtCore::RefPtr<dtCore::ParticleSystem>   LoadPSFile( const char* fname );
   inline void                                     InitInputDevices( void );
   inline void                                     SetUpCamera( void );
   inline void                                     MoveTheObject( unsigned int obj );

   static void MakeSmoke( dtAudio::Sound* sound, void* param );
   static void StopSmoke( dtAudio::Sound* sound, void* param );

   void StartRecording();
   void StopRecording();

private:
   SND_LST                       mActive;
   SND_QUE                       mQueued;
   float                         mSndGain;
   float                         mSndPitch;
   bool                          mLooping;
   dtAudio::Listener*            mMic;

   OBJ_PTR                       mGfxObj[kNumGfx];
   PAR_PTR                       mPSysA;
   PAR_PTR                       mPSysC;
   FXM_PTR                       mFXMgr;
   BIN_PTR                       mSFXBinder;
   LID_PTR                       mInputDevice;
   MM_PTR                        mMotionModel;
   unsigned int                  mSmokeCountA;
   unsigned int                  mSmokeCountC;
   dtCore::RefPtr<SoundRecorder> mRecorder;
};

#endif  // DELTA_TEST_AUDIO
