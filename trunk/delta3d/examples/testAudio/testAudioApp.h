#ifndef  TEST_AUDIO
#define  TEST_AUDIO

#include <string>
#include <vector>
#include <queue>

#include "dt.h"
#include "dtaudio.h"
#include "dtabc.h"



class testAudioApp   :  public   dtABC::Application
{
   private:
      typedef  std::vector<dtAudio::Sound*>  SND_LST;
      typedef  SND_LST::iterator             SND_ITR;
      typedef  std::queue<dtAudio::Sound*>   SND_QUE;

   private:
      static   const char*       kDataPath;
      static   unsigned int      kNumSoundFiles;
      static   const char*       kSoundFile[];

      static   unsigned int      kNumGfxFiles;
      static   const char*       kGfxFile[];

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
      inline   void              LoadPlaySound( const char* fname );
      inline   void              StopAllSounds( void );
      inline   void              FreeAllStoppedSounds( bool forced = false );
      inline   void              FlushQueuedSounds( void );
      inline   void              ChangeSoundGain( float gain );
      inline   void              ChangeSoundPitch( float pitch );
      inline   void              ToggleSoundLooping( void );
      inline   void              PauseAllSounds( void );
      inline   void              RewindAllSounds( void );

      inline   dtCore::Object*   LoadGfxFile( const char* fname );

   private:
               SND_LST           mActive;
               SND_QUE           mQueued;
               float             mSndGain;
               float             mSndPitch;
               bool              mLooping;
};



#endif   // TEST_AUDIO