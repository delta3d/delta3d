/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 2.1 of the License, or (at your option) 
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
*/

#ifndef  DELTA_AUDIOMANAGER
#define  DELTA_AUDIOMANAGER

#include <stddef.h>
#include <vector>
#include <queue>
#include <map>
#include <string>

#ifdef __APPLE__
  #include <OpenAL/al.h>
#else
  #include <AL/al.h>
#endif

#include <dtCore/base.h>
#include <dtCore/transformable.h>
#include <dtAudio/listener.h>
#include <dtAudio/sound.h>

#include <osg/Vec3>

// forward references
struct   AudioConfigData;



namespace   dtAudio
{

   /** dtAudio::AudioManager 
    *
    * dtAudio::AudioManager is the interface to the underlying audio-
    * engine; OpenAL.
    *
    * Before using, the user must instantiate and configure the
    * AudioManager:
    *
    *    AudioManager::Instantiate();
    *    AudioManager::GetManager()->Config();
    *
    * Optionally the user can create an AudioConfigData structure
    * to pass to the AudioManager when configuring set some of the
    * base functionalit of the manager.  Currently only the number
    * of sources is set this way.  It is encouraged, but not required,
    * that the user know how many channels their audio hardware uses
    * and set the number of sources = the number of channels.
    *
    * After user is finished with all sound, the AudioManager should
    * be freed:
    *
    *    AudioManager::Destroy();
    *
    * FOR THE USER:
    * Sounds are not created by the user, but they are requested from
    * the AudioManager.  After getting a sound from the AudioManager
    * the user then calls the sound's functions.  When the user is
    * finished with the sound, the sound should be passed back to 
    * the AudioManager for free the resource.
    *
    * There is one global listener which the user also requests from
    * the AudioManager.  After getting the listener from the AudioManager
    * the usere then calls the listener's functions.  When finished
    * with the listener, the user does NOT free it.  The listener is
    * just an interface to AudioManager's protected object and the
    * AudioManager will handle it's resources.
    *
    * In many cases it's more efficeint to preload the sounds into
    * the AudioManager before loading them into the individual sounds.
    *
    * FOR THE DEVELOPER:
    * The AudioManager is a repository for all sounds objects, buffers,
    * sources (sound channels), listeners, etc.  The AudioManager 
    * hands out interfaces to the various objects for users to manipulate
    * but it holds all the resources for those objects.
    *
    * When a sound command is requested (play, stop, pitch, etc.) the
    * AudioManager receives the sound through the sig-slot messaging
    * system and pushes the sound onto a queue of sounds requesting
    * command processing.  At pre-frame, the AudioManager process all the
    * sounds waiting for command processing in the ordered they were queued.
    * State commands are commnads to change the state of the sound
    * (play, stop, pause, etc.), but not the value of any of the sounds
    * attributes (gain, pitch, etc.).  Value commands change the value of
    * a sound's attributes.  If the sound is currently active (has a source)
    * the value commands are processed immediately, else the value is saved
    * for when the sound becomes active.  State commands push the sound onto
    * a queue for further processing at the appropriate time.  For instance,
    * a play command will bind a sound's source to the sound's buffer and 
    * then will push the source onto the play queue awaiting to start playing.
    *
    * At frame time, AudioManager process all sources waiting for a state
    * change.  All sources waiting a stop command get stopped, waiting a
    * pause command get paused, waiting a rewind command get rewound, and
    * waiting a play command get played.  Then the AudioManager runs through
    * all sources in it's active list and removes sources that have finished
    * playing then puts them in a cleanup list for later cleanup.
    * 
    * At post-frame, the AudioManager takes all sources waiting for cleanup
    * and unbinds the source from it's associated buffer.  The source is then
    * put in a recycle queue for later use by new sounds.
    *
    *********************       WARNING       ********************
    ********************* JPJ (Sept. 23 2004) ********************
    * The SetRelative() and SetAbsolute() functions are not working properly.
    * The underlying sound engine (OpenAL) claims setting the
    * AL_SOURCE_RELATIVE flag to AL_TRUE will attenuate the sources with
    * respect to the gloabal listener's position, and resetting the
    * flag to AL_FALSE will not do any distance calculations.  This does
    * not  appear to be correct.  It appears that resetting the flag to
    * AL_FALSE does the distance calculations with respect to the
    * listener's position, and setting to AL_TRUE still does the
    * distance calculations with respect to the origin.  For now, we
    * are always resseting the flag to AL_FALSE.
    *
    */
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
               virtual  void     SetVelocity( const osg::Vec3& velocity );
               virtual  void     GetVelocity( osg::Vec3& velocity )  const;
               

               /// set/get listener's gain (master volume)
               virtual  void     SetGain( float gain );
               virtual  float    GetGain( void )   const;

               /// repositions listener if it's a child in scene-space
               virtual  void     OnMessage( MessageData* data );

               /// override method so listener knows when it's becomeing a child
               //virtual  void     SetParent( dtCore::Transformable* parent );

               /// clean up listener for no apparent reason
               virtual  void     Clear( void );

            private:
                        ALfloat  mVelo[3L];
                        ALfloat  mGain;
         };

      private:
         typedef  dtCore::RefPtr<AudioManager>          MOB_ptr;
         typedef  dtCore::RefPtr<SoundObj>              SOB_PTR;
         typedef  dtCore::RefPtr<ListenerObj>           LOB_PTR;

         typedef  std::map<std::string, BufferData*>  BUF_MAP;

         typedef  std::map<ALuint, SOB_PTR>           SRC_MAP;
         typedef  std::queue<ALuint>                  SRC_QUE;
         typedef  std::vector<ALuint>                 SRC_LST;

         typedef  std::queue<SOB_PTR>                 SND_QUE;
         typedef  std::vector<SOB_PTR>                SND_LST;

         enum SoundState
         {
            PAUSED,
            PLAYING,
            STOPPED
         };
      
         typedef std::map< SoundObj*, SoundState >     SoundObjectStateMap;

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
         /// create the singleton and initialize OpenAL
         static   void              Instantiate( void );

         /// destroy the singleton and shutdown OpenAL
         static   void              Destroy( void );

         /// access the AudioManager
         static   AudioManager*     GetManager( void );

         /// access the global Listener
         static   Listener*         GetListener( void );

         /// initialize AudioManager
         virtual  void              Config( const AudioConfigData& data = _DefCfg );

         /**
          * receive messages
          * handles the timeing messages (pre-post-frame) from the system
          * pushes sounds onto the command queue for later processing
          */
         virtual  void              OnMessage( MessageData* data );

         /// create or recycle a new sound for the user
                  Sound*            NewSound( void );

         /// free a sound (recycle it) that the user is finish with
                  void              FreeSound( Sound*& sound );

         /// pre-load a wave sound into a buffer
                  bool              LoadWaveFile( const char* file );

         /// un-load a wave sound from a buffer (if use-count is zero)
                  bool              UnloadWaveFile( const char* file );

      private:
         /// process commands of all sounds in the command queue
         inline   void              PreFrame( const double deltaFrameTime );

         /// process all sounds in the play, pause, stop, and rewind queues
         /// put active sounds on active list
         /// remove inactive sounds from active list
         inline   void              Frame( const double deltaFrameTime );

         /// unbind the source/buffers from all inactive sounds (stopped sounds)
         /// free the source
         /// stop sending AudioManager messages to sound
         inline   void              PostFrame( const double deltaFrameTime );

         inline   void              Pause( const double deltaFrameTime );
         /// check if manager has been configured
         inline   bool              Configured( void )   const;

         /// pre-create the specified number of sources
         inline   bool              ConfigSources( unsigned int num );

         /// get the eax function pointers from OpenAL (nothing done with them yet)
         inline   bool              ConfigEAX( bool eax );

         /// give a sound the pointer to the buffer it wants
         inline   void              LoadSound( SoundObj* snd );

         /// remove the buffer from a sound
         inline   void              UnloadSound( SoundObj* snd );

         /// bind a source to the sound's buffer
         /// initialize the source
         /// start sending AudioManager messages to sound
         /// push sound onto the play queue
         inline   void              PlaySound( SoundObj* snd );

         /// push sound onto pause queue
         inline   void              PauseSound( SoundObj* snd );

         /// push sound onto stopped queue
         inline   void              StopSound( SoundObj* snd );

         /// push sound onto rewind queue
         inline   void              RewindSound( SoundObj* snd );

         /// set sound's source to looping
         inline   void              SetLoop( SoundObj* snd );

         /// set sound's source to not looping
         inline   void              ResetLoop( SoundObj* snd );

         /// set sound's source to listener-relative
         inline   void              SetRelative( SoundObj* snd );

         /// set sound's source to not listener-relative
         inline   void              SetAbsolute( SoundObj* snd );

         /// set sound source's gain
         inline   void              SetGain( SoundObj* snd );

         /// set sound source's pitch
         inline   void              SetPitch( SoundObj* snd );

         /// set sound source's position
         inline   void              SetPosition( SoundObj* snd );

         /// set sound source's direction
         inline   void              SetDirection( SoundObj* snd );

         /// set sound source's velocity
         inline   void              SetVelocity( SoundObj* snd );

         /// set sound source's reference distance (minimum dist)
         inline   void              SetReferenceDistance( SoundObj* snd );

         /// set sound source's maximum distance
         inline   void              SetMaximumDistance( SoundObj* snd );

         /// set sound source's rolloff factor
         inline   void              SetRolloff( SoundObj* snd );

         /// set sound source's minimum gain
         inline   void              SetMinimumGain( SoundObj* snd );

         /// set sound source's maximum gain
         inline   void              SetMaximumGain( SoundObj* snd );

         /// give this sound a source
         inline   bool              GetSource( SoundObj* snd );

         /// take source away from this sound
         inline   void              FreeSource( SoundObj* snd );

      private:
                  ALvoid*             mEAXSet;
                  ALvoid*             mEAXGet;

                  ALsizei             mNumSources;
                  ALuint*             mSource;

                  SRC_MAP             mSourceMap;
                  SRC_QUE             mAvailable;
                  SRC_QUE             mPlayQueue;
                  SRC_QUE             mPauseQueue;
                  SRC_QUE             mStopQueue;
                  SRC_QUE             mRewindQueue;
                  SRC_LST             mActiveList;

                  BUF_MAP             mBufferMap;

                  SND_QUE             mSoundCommand;
                  SND_QUE             mSoundRecycle;
                  SND_LST             mSoundList;

                  SoundObjectStateMap mSoundStateMap; ///Maintains state of each SoundObject
                                                      ///prior to a system-wide pause message
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
