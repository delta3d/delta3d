#ifndef DELTA_SOUND
#define DELTA_SOUND

// sound.h: Declaration of the Sound class.
//
//////////////////////////////////////////////////////////////////////

#include "recorder.h"
#include "transformable.h"



namespace dtAudio
{
   class Sound;

   //! callback function type
   typedef  void  (*SoundCB)( Sound* sound, void* param );

   /**
    * A sound.
    */
   class DT_EXPORT   Sound :  public   dtCore::Transformable,
                              public   dtCore::Recordable
   {
      DECLARE_MANAGEMENT_LAYER(Sound)

      public:
                  enum  Command
                        {
                           NONE     = 0L,
                           LOAD,
                           UNLOAD,
                           PLAY,
                           PAUSE,
                           STOP,
                           REWIND,
                           LOOP,
                           UNLOOP,
                           QUEUE,
                           GAIN,
                           PITCH,
                           POSITION,
                           DIRECTION,
                           VELOCITY,
                           ABS,
                           REL,
                           MIN_DIST,
                           MAX_DIST,
                           ROL_FACT,
                           MIN_GAIN,
                           MAX_GAIN,

                           kNumCommands
                        };

      public:
         static   const char* kCommand[kNumCommands];

      protected:
         /**
          * Constructor, user does not create directly
          * instead requests a sound from AudioManager
          */
                              Sound();

         /**
          * Destructor, user does not delete directly
          * instead frees sound to the AudioManager
          */
         virtual              ~Sound();

         /**
          * Message handler.
          *
          * @param data the received message
          */
         virtual  void        OnMessage( MessageData* data );

      public:
         /**
          * Loads the specified sound file.
          *
          * @param filename the name of the file to load
          */
         virtual  void        LoadFile( const char* file );

         /**
          * Unloads the specified sound file.
          */
         virtual  void        UnloadFile( void );

         /**
          * Returns the name of the loaded sound file.
          *
          * @return the name of the loaded file
          */
         virtual  const char* GetFilename( void )              {  return   mFilename.c_str();   }

         /**
          * Set callback for when sound starts playing.
          *
          * @param callback function pointer
          * @param user data
          */
         virtual  void        SetPlayCallback( SoundCB cb, void* param );

         /**
          * Set callback for when sound stops playing.
          *
          * @param callback function pointer
          * @param user data
          */
         virtual  void        SetStopCallback( SoundCB cb, void* param );

         /**
          * Starts playing this sound.
          */
         virtual  void        Play( void );

         /**
          * Pauses playing this sound.
          */
         virtual  void        Pause( void );

         /**
          * Stops playing this sound.
          */
         virtual  void        Stop( void );

         /**
          * Rewinds to the beginning of this sound.
          */
         virtual  void        Rewind( void );

         /**
          * Is sound playing. (overloaded function)
          *
          * @param true if playing
          */
         virtual  bool        IsPlaying( void )                   const {  return   false;   }

         /**
          * Is sound paused. (overloaded function)
          *
          * @param true if playing
          */
         virtual  bool        IsPaused( void )                    const {  return   false;   }

         /**
          * Is sound stopped. (overloaded function)
          *
          * @param true if playing
          */
         virtual  bool        IsStopped( void )                   const {  return   true;    }

         /**
          * Sets whether or not to play the sound in a continuous loop.
          *
          * @param looping true to play the sound in a loop, false
          * otherwise
          */
         virtual  void        SetLooping( bool loop = true );

         /**
          * Checks whether or not the sound plays in a continuous loop.
          * (overloaded function)
          *
          * @return true if the sound plays in a loop, false otherwise
          */
         virtual  bool        IsLooping( void )                   const {  return   false;   }

         /**
          * Sets the gain of the sound source.
          *
          * @param gain the new gain
          */
         virtual  void        SetGain( float gain );

         /**
          * Returns the gain of the sound source.
          *
          * @return the current gain
          */
         virtual  float       GetGain( void )                     const {  return   mGain;    }

         /**
          * Sets the pitch multiplier of the sound source.
          *
          * @param pitch the new pitch
          */
         virtual  void        SetPitch( float pitch );

         /**
          * Returns the pitch multipier of the sound source.
          *
          * @return the current pitch
          */
         virtual  float       GetPitch( void )                    const {  return   mPitch;   }

         /**
          * Flags sound to be relative to listener position.
          *
          * @param relative true uses distance modeling
          */
         virtual  void        ListenerRelative( bool relative );

         /**
          * Get relative to listener position flag.
          * (overloaded function)
          *
          * @return relative true uses distance modeling
          */
         virtual  bool        IsListenerRelative( void )          const {  return   false;   }

         /**
          * Set the transform position of sound.
          *
          * @param *xform : The new Transform to position this instance
          * @param cs : Optional parameter describing the coordinate system of xform
          *             Defaults to ABS_CS.
          */
         virtual  void        SetTransform(  dtCore::Transform*                  xform,
                                             dtCore::Transformable::CoordSysEnum cs = dtCore::Transformable::ABS_CS );

         /**
          * Set the position of sound.
          *
          * @param position to set
          */
         virtual  void        SetPosition( const sgVec3& position );

         /**
          * Get the position of sound.
          *
          * @param position to get
          */
         virtual  void        GetPosition( sgVec3& position )     const;

         /**
          * Set the direction of sound.
          *
          * @param direction to set
          */
         virtual  void        SetDirection( const sgVec3& direction );

         /**
          * Get the direction of sound.
          *
          * @param direction to get
          */
         virtual  void        GetDirection( sgVec3& direction )   const;

         /**
          * Set the velocity of sound.
          *
          * @param velocity to set
          */
         virtual  void        SetVelocity( const sgVec3& velocity );

         /**
          * Get the velocity of sound.
          *
          * @param velocity to get
          */
         virtual  void        GetVelocity( sgVec3& velocity )     const;

         /**
          * Set the minimum distance that sound plays at max_gain.
          * Attenuation is not calculated below this distance
          *
          * @param distance set to minimum
          */
         virtual  void        SetMinDistance( float dist );

         /**
          * Get the minimum distance that sound plays at max_gain.
          *
          * @return distance minimum
          */
         virtual  float       GetMinDistance( void )              const {  return   mMinDist;   }

         /**
          * Set the maximum distance that sound plays at min_gain.
          * Attenuation is not calculated above this distance
          *
          * @param distance set to maximum
          */
         virtual  void        SetMaxDistance( float dist );

         /**
          * Get the maximum distance that sound plays at min_gain.
          *
          * @return distance maximum
          */
         virtual  float       GetMaxDistance( void )              const {  return   mMaxDist;   }

         /**
          * Set the rolloff factor describing attenuation curve.
          *
          * @param rollff factor to set
          */
         virtual  void        SetRolloffFactor( float rolloff );

         /**
          * Get the rolloff factor describing attenuation curve.
          *
          * @return rollff factor
          */
         virtual  float       GetRolloffFactor( void )            const {  return   mRolloff;   }

         /**
          * Set the minimum gain that sound plays at.
          * Attenuation is clamped to this gain
          *
          * @param gain set to minimum
          */
         virtual  void        SetMinGain( float gain );

         /**
          * Get the minimum gain that sound plays at.
          *
          * @return gain minimum
          */
         virtual  float       GetMinGain( void )                  const {  return   mMinGain;   }

         /**
          * Set the maximum gain that sound plays at.
          * Attenuation is clamped to this gain
          *
          * @param gain set to maximum
          */
         virtual  void        SetMaxGain( float gain );

         /**
          * Get the maximum gain that sound plays at.
          *
          * @return gain maximum
          */
         virtual  float       GetMaxGain( void )                  const {  return   mMaxGain;   }

         /**
          * Generates and returns a key frame that represents the
          * complete recordable state of this object.
          *
          * @return a new key frame
          */
         virtual dtCore::StateFrame*   GenerateKeyFrame( void );

         /**
          * Deserializes an XML element representing a state frame, turning it
          * into a new StateFrame instance.
          *
          * @param element the element that represents the frame
          * @return a newly generated state frame corresponding to the element
          */
         virtual dtCore::StateFrame*   DeserializeFrame( TiXmlElement* element );

      protected:
                  std::string mFilename;
                  SoundCB     mPlayCB;
                  void*       mPlayCBData;
                  SoundCB     mStopCB;
                  void*       mStopCBData;
                  float       mGain;
                  float       mPitch;
                  sgVec3      mPos;
                  sgVec3      mDir;
                  sgVec3      mVelo;
                  float       mMinDist;
                  float       mMaxDist;
                  float       mRolloff;
                  float       mMinGain;
                  float       mMaxGain;
   };
};


#endif // DELTA_SOUND
