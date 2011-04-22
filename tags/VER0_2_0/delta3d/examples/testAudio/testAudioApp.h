#ifndef  DELTA_TEST_AUDIO
#define  DELTA_TEST_AUDIO

#include <string>
#include <vector>
#include <queue>

#include "dt.h"
#include "dtaudio.h"
#include "dtabc.h"



class testAudioApp   :  public   dtABC::Application
{
   private:
      typedef  std::vector<dtAudio::Sound*>              SND_LST;
      typedef  SND_LST::iterator                         SND_ITR;
      typedef  std::queue<dtAudio::Sound*>               SND_QUE;
      typedef  osg::ref_ptr<dtCore::Object>              OBJ_PTR;
      typedef  osg::ref_ptr<dtCore::EffectManager>       FXM_PTR;
      typedef  osg::ref_ptr<dtCore::ParticleSystem>      PAR_PTR;
      typedef  osg::ref_ptr<dtAudio::SoundEffectBinder>  BIN_PTR;

   private:
      static   const char*       kDataPath;
      static   unsigned int      kNumSoundFiles;
      static   const char*       kSoundFile[];

               enum              GfxObjId
                                 {
                                    GROUND   = 0L,
                                    BOX_A,
                                    BOX_C,

                                    kNumGfx
                                 };
      static   const char*       kGfxFile[kNumGfx];

               enum              FxObjId
                                 {
                                    EXPLODE  = 0L,
                                    SMOKE,

                                    kNumFx
                                 };
      static   const char*       kFxFile[kNumFx];

   public:
                                 testAudioApp( std::string configFilename = "" );
      virtual                    ~testAudioApp();

      virtual  void              PreFrame( const double deltaFrameTime );
      virtual  void              Frame( const double deltaFrameTime );
      virtual  void              PostFrame( const double deltaFrameTime );

      virtual  void              KeyPressed( dtCore::Keyboard*       keyboard,
                                             Producer::KeyboardKey   key,
                                             Producer::KeyCharacter  character   );

   private:
      inline   void              LoadPlaySound( const char* fname, unsigned int box = 0L );
      inline   void              StopAllSounds( void );
      inline   void              FreeAllStoppedSounds( bool forced = false );
      inline   void              FlushQueuedSounds( void );
      inline   void              ChangeSoundGain( float gain );
      inline   void              ChangeSoundPitch( float pitch );
      inline   void              ToggleSoundLooping( void );
      inline   void              PauseAllSounds( void );
      inline   void              RewindAllSounds( void );

      inline   void                    SetUpVisuals( void );
      inline   dtCore::Object*         LoadGfxFile( const char* fname );
      inline   dtCore::EffectManager*  LoadFxFile( const char* fname );
      inline   dtCore::ParticleSystem* LoadPSFile( const char* fname );
      inline   void                    InitInputDevices( void );
      inline   void                    SetUpCamera( void );
      inline   void                    MoveTheStupidBox( unsigned int box );

      static   void              MakeSmoke( dtAudio::Sound* sound, void* param );
      static   void              StopSmoke( dtAudio::Sound* sound, void* param );

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
               dtCore::LogicalInputDevice*   mInputDevice;
               dtCore::MotionModel*          mMotionModel;
               unsigned int                  mSmokeCountA;
               unsigned int                  mSmokeCountC;
};



#endif   // DELTA_TEST_AUDIO