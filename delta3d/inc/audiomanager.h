#ifndef  DELTA_AUDIOMANAGER
#define  DELTA_AUDIOMANAGER

#include <stddef.h>
#include <vector>
#include <queue>
#include <map>
#include <string>

#include <al.h>
#include <sg.h>

#include <base.h>
#include <transformable.h>
#include <listener.h>
#include <sound.h>



// forward references
struct   AudioConfigData;



namespace   dtAudio
{
   class DT_EXPORT AudioManager   :  public   dtCore::Base
   {
        DECLARE_MANAGEMENT_LAYER(AudioManager)

      private:
         /**
          * BufferData is an internal structure
          * used to identify an OpenAL buffer and
          * hold reference data associated with it
          */
         struct   BufferData
         {
            ALuint         buf;
            const char*    file;
            ALboolean      loop;
            unsigned int   use;

            BufferData()
            :  buf(0L),
               file(""),
               loop(AL_FALSE),
               use(0L)
            {}
         };

         /**
          * SoundObj is the concrete object associated
          * with Sound interface that users manipulate.
          *
          * The SoundObj is the mechanism that binds
          * an OpenAL buffer to an OpenAL source for
          * playing sounds.  Typically a SoundObj always
          * holds onto a buffer, but it's buffer could be
          * swapped for another.
          *
          * The SoundObj rarely holds onto a source.
          * Just before playing, the SoundObj gets a new-
          * recycled source and binds it with the buffer.
          * Immediately after finished playing, the source
          * is removed from SoundObj for recycling.
          */
         class SoundObj :  public   Sound
         {
            DECLARE_MANAGEMENT_LAYER(SoundObj)

            private:
               typedef  std::queue<const char*>   CMD_QUE;

            public:
                                       SoundObj();
               virtual                 ~SoundObj();

               /// update messages which set state flags and
               /// repositions sound if it's a child in scene-space
               virtual  void           OnMessage( MessageData* data );

               /// override method so sound knows when it's becomeing a child
               virtual  void           SetParent( dtCore::Transformable* parent );

               /// override methods for user to querry sound state
               virtual  bool           IsPlaying( void )          const;
               virtual  bool           IsPaused( void )           const;
               virtual  bool           IsStopped( void )          const;
               virtual  bool           IsLooping( void )          const;
               virtual  bool           IsListenerRelative( void ) const;

               /// set/get next command in this sound's command queue
                        void           Command( const char* cmd );
                        const char*    Command( void );

               /// set/get this sounds buffer id
                        void           Buffer( ALuint buffer );
                        ALuint         Buffer( void );

               /// set/get this sounds source id
                        void           Source( ALuint source );
                        ALuint         Source( void );

               /// set/reset/get various state flags
                        void           SetState( unsigned int flag );
                        void           ResetState( unsigned int flag );
                        bool           GetState( unsigned int flag ) const;

               /// clean up sound for recycling
                        void           Clear( void );

            private:
                        CMD_QUE        mCommand;
                        ALuint         mBuffer;
                        ALuint         mSource;
                        unsigned int   mState;
         };

         /**
          * ListenerObj is the concrete object associated
          * with Listener interface that users manipulate.
          * 
          * Function calls on the listener are processed
          * immediately, unlike the SoundObj functions.
          */
         class ListenerObj :  public   Listener
         {
            DECLARE_MANAGEMENT_LAYER(ListenerObj)

            public:
                                 ListenerObj();
               virtual           ~ListenerObj();

               /// set/get listener's velocity
               virtual  void     SetVelocity( const sgVec3& velocity );
               virtual  void     GetVelocity( sgVec3& velocity )  const;

               /// set/get listener's gain (master volume)
               virtual  void     SetGain( float gain );
               virtual  float    GetGain( void )   const;

               /// repositions listener if it's a child in scene-space
               virtual  void     OnMessage( MessageData* data );

               /// override method so listener knows when it's becomeing a child
               virtual  void     SetParent( dtCore::Transformable* parent );

               /// clean up listener for no apparent reason
               virtual  void     Clear( void );

            private:
                        ALfloat  mVelo[3L];
                        ALfloat  mGain;
         };

      private:
         typedef  osg::ref_ptr<AudioManager>          MOB_ptr;
         typedef  osg::ref_ptr<SoundObj>              SOB_PTR;
         typedef  osg::ref_ptr<ListenerObj>           LOB_PTR;

         typedef  std::map<std::string, BufferData*>  BUF_MAP;

         typedef  std::map<ALuint, SOB_PTR>           SRC_MAP;
         typedef  std::queue<ALuint>                  SRC_QUE;
         typedef  std::vector<ALuint>                 SRC_LST;

         typedef  std::queue<SOB_PTR>                 SND_QUE;
         typedef  std::vector<SOB_PTR>                SND_LST;

      private:
         static   MOB_ptr                 _Mgr;
         static   LOB_PTR                 _Mic;
         static   const char*             _EaxVer;
         static   const char*             _EaxSet;
         static   const char*             _EaxGet;
         static   const AudioConfigData   _DefCfg;

      private:
                                    AudioManager( std::string name = "audiomanager" );
         virtual                    ~AudioManager();

      public:
         static   void              Instantiate( void );
         static   void              Destroy( void );
         static   AudioManager*     GetManager( void );
         static   Listener*         GetListener( void );

         virtual  void              Config( const AudioConfigData& data = _DefCfg );
         virtual  void              OnMessage( MessageData* data );

                  Sound*            NewSound( void );
                  void              FreeSound( Sound*& sound );

                  bool              LoadWaveFile( const char* file );
                  bool              UnloadWaveFile( const char* file );

      private:
         inline   void              PreFrame( const double deltaFrameTime );
         inline   void              Frame( const double deltaFrameTime );
         inline   void              PostFrame( const double deltaFrameTime );
         inline   bool              Configured( void )   const;
         inline   bool              ConfigSources( unsigned int num );
         inline   bool              ConfigEAX( bool eax );
         inline   void              LoadSound( SoundObj* snd );
         inline   void              UnloadSound( SoundObj* snd );
         inline   void              PlaySound( SoundObj* snd );
         inline   void              PauseSound( SoundObj* snd );
         inline   void              StopSound( SoundObj* snd );
         inline   void              RewindSound( SoundObj* snd );
         inline   void              SetLoop( SoundObj* snd );
         inline   void              ResetLoop( SoundObj* snd );
         inline   void              SetRelative( SoundObj* snd );
         inline   void              SetAbsolute( SoundObj* snd );
         inline   void              SetGain( SoundObj* snd );
         inline   void              SetPitch( SoundObj* snd );
         inline   void              SetPosition( SoundObj* snd );
         inline   void              SetDirection( SoundObj* snd );
         inline   void              SetVelocity( SoundObj* snd );
         inline   void              SetReferenceDistance( SoundObj* snd );
         inline   void              SetMaximumDistance( SoundObj* snd );
         inline   void              SetRolloff( SoundObj* snd );
         inline   void              SetMinimumGain( SoundObj* snd );
         inline   void              SetMaximumGain( SoundObj* snd );
         inline   bool              GetSource( SoundObj* snd );
         inline   void              FreeSource( SoundObj* snd );

      private:
                  ALvoid*           mEAXSet;
                  ALvoid*           mEAXGet;

                  ALsizei           mNumSources;
                  ALuint*           mSource;

                  SRC_MAP           mSourceMap;
                  SRC_QUE           mAvailable;
                  SRC_QUE           mPlayQueue;
                  SRC_QUE           mPauseQueue;
                  SRC_QUE           mStopQueue;
                  SRC_QUE           mRewindQueue;
                  SRC_LST           mActiveList;

                  BUF_MAP           mBufferMap;

                  SND_QUE           mSoundCommand;
                  SND_QUE           mSoundRecycle;
                  SND_LST           mSoundList;
   };
};



// configuration data
struct DT_EXPORT AudioConfigData
{
   enum           DistanceModel
                  {
                     dmNONE      = AL_NONE,
                     dmINVERSE   = AL_INVERSE_DISTANCE,
                     dmINVCLAMP  = AL_INVERSE_DISTANCE_CLAMPED,
                  };

   unsigned int   numSources;
   bool           eax;
   unsigned int   distancemodel;

   AudioConfigData(  unsigned int   ns = 16L,
                     bool           ex = false,
                     unsigned int   dm = dmINVERSE )
   :  numSources(ns),
      eax(ex),
      distancemodel(dm)
   {}
};



#endif   // DELTA_AUDIOMANAGER
