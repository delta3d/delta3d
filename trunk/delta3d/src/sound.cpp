// sound.cpp: Implementation of the Sound class.
//
//////////////////////////////////////////////////////////////////////

#include "sound.h"



// definitions
#if   defined(WIN32) | defined(_WIN32)
#pragma warning( disable : 4800 )
#endif



#if   !  defined(MAX_FLOAT)
#define  MAX_FLOAT   static_cast<float>(0xFFFFFFFF)
#endif



// namespaces
using namespace dtAudio;
using namespace std;



// helper template functions
template <class   T>
T  MIN( T a, T b )         {  return   (a<b)?a:b;        }

template <class   T>
T  MAX( T a, T b )         {  return   (a>b)?a:b;        }

template <class   T>
T  CLAMP( T x, T l, T h )  {  return   MAX(MIN(x,h),l);  }



// static member variables
const char* Sound::kCommand[kNumCommands]   =
            {
               "",            "load",        "unload",
               "play",        "pause",       "stop",
               "rewind",      "loop",        "unloop",
               "queue",       "gain",        "pitch",
               "position",    "direction",   "velocity",
               "absolute",    "relative",    "mindist",
               "maxdist",     "rolloff",     "mingain",
               "maxgain"
            };



/**
 * Represents a snapshot of the state of a sound.
 */
class SoundStateFrame : public dtCore::StateFrame
{
   /********************************************/
   /*
   /* WARNING FROM JPJ (Sep.23,2004)
   /*
   /* SoundStateFrame was a cut/past from
   /* the old dtCore::Sound,  I did not have
   /* time to implement the guts for the new
   /* dtAudio::Sound.  Whomever works on this
   /* code after me beware... and remove this
   /* comment when you fix this.
   /*
   /* So long and thanks for all the fish.
   /*
   /********************************************/
   public:

      enum Elements
      {
         Playing = 0x01,
         Gain = 0x02,
         Pitch = 0x04
      };

      SoundStateFrame(Sound* source, int validElements, 
                      bool playing, float gain)
         : StateFrame(source),
           mValidElements(validElements),
           mPlaying(playing),
           mGain(gain)
      {}

      virtual void ReapplyToSource()
      {
         Sound* source = (Sound*)GetSource();

         if(mValidElements & Playing)
         {
            if(mPlaying && !source->IsPlaying())
            {
               source->Play();
            }
            else if(!mPlaying && source->IsPlaying())
            {
               source->Stop();
            }
         }

         if(mValidElements & Gain)
         {
            source->SetGain(mGain);
         }
      }

      virtual TiXmlElement* Serialize() const
      {
         TiXmlElement* element = new TiXmlElement("SoundStateFrame");

         char buf[256];

         if(mValidElements & Playing)
         {
            element->SetAttribute(
               "playing",
               mPlaying ? "true" : "false"
            );
         }

         if(mValidElements & Gain)
         {
            sprintf(buf, "%g", mGain);

            element->SetAttribute("gain", buf);
         }

         return element;
      }

   private:

      int mValidElements;
      bool mPlaying;
      float mGain;
};



IMPLEMENT_MANAGEMENT_LAYER(Sound)



/********************************
/** Protected Member Functions **
/********************************
/**
 * Constructor, user does not create directly
 * instead requests a sound from AudioManager
 */
Sound::Sound()
:  Transformable(),
   mFilename(""),
   mGain(1.0f),
   mPitch(1.0f),
   mMinDist(1.0f),
   mMaxDist(static_cast<float>(MAX_FLOAT)),
   mRolloff(1.0f),
   mMinGain(0.0f),
   mMaxGain(1.0f),
   mPlayCB(NULL),
   mPlayCBData(NULL),
   mStopCB(NULL),
   mStopCBData(NULL)
{
   RegisterInstance( this );

   mPos[0L]       = 0.0f;
   mPos[1L]       = 0.0f;
   mPos[2L]       = 0.0f;

   mDir[0L]       = 0.0f;
   mDir[1L]       = 1.0f;
   mDir[2L]       = 0.0f;

   mVelo[0L]      = 0.0f;
   mVelo[1L]      = 0.0f;
   mVelo[2L]      = 0.0f;
}



/**
 * Destructor, user does not delete directly
 * instead frees sound to the AudioManager
 */
Sound::~Sound()
{
    DeregisterInstance(this);
}



/**
 * Message handler.
 *
 * @param data the received message
 */
void
Sound::OnMessage( MessageData* data )
{
}



/**
 * Loads the specified sound file.
 *
 * @param filename the name of the file to load
 */
void
Sound::LoadFile( const char* file )
{
   mFilename   = file;
   SendMessage( kCommand[LOAD], this );
}



/**
 * Unloads the specified sound file.
 */
void
Sound::UnloadFile( void )
{
   SendMessage( kCommand[UNLOAD], this );
}

/**
 * Set callback for when sound starts playing.
 *
 * @param callback function pointer
 * @param user data
 */
void
Sound::SetPlayCallback( SoundCB cb, void* param )
{
   mPlayCB  = cb;

   if( mPlayCB )
      mPlayCBData = param;
   else
      mPlayCBData = NULL;
}



/**
 * Set callback for when sound stops playing.
 *
 * @param callback function pointer
 * @param user data
 */
void
Sound::SetStopCallback( SoundCB cb, void* param )
{
   mStopCB  = cb;

   if( mStopCB )
      mStopCBData = param;
   else
      mStopCBData = NULL;
}



/**
 * Tell audio manager to play this sound.
 */
void
Sound::Play( void )
{
   SendMessage( kCommand[PLAY], this );
}



/**
 * Tell audio manager to pause this sound.
 */
void
Sound::Pause( void )
{
   SendMessage( kCommand[PAUSE], this );
}



/**
 * Tell audio manager to stop this sound.
 */
void
Sound::Stop( void )
{
   SendMessage( kCommand[STOP], this );
}



/**
 * Tell audio manager to rewind this sound.
 */
void
Sound::Rewind( void )
{
   SendMessage( kCommand[REWIND], this );
}



/**
 * Sets whether or not to play the sound in a continuous loop.
 *
 * @param looping true to play the sound in a loop, false
 * otherwise
 */
void
Sound::SetLooping( bool loop /*= true*/ )
{
   if( loop )
      SendMessage( kCommand[LOOP], this );
   else
      SendMessage( kCommand[UNLOOP], this );
}



/**
 * Sets the gain of the sound source.
 *
 * @param gain the new gain
 */
void
Sound::SetGain( float gain )
{
   // force gain to range from zero to one
   mGain    = CLAMP( gain, 0.0f, 1.0f );

   SendMessage( kCommand[GAIN], this );
}



/**
 * Sets the pitch multiplier of the sound source.
 *
 * @param pitch the new pitch
 */
void
Sound::SetPitch( float pitch )
{
   // force pitch to range from zero+ to two
   // for some reason openAL chokes on 2+
   // also, openAL states zero to be invalid
   mPitch   = CLAMP( pitch, 0.000001f, 2.0f );

   SendMessage( kCommand[PITCH], this );
}



/**
 * Flags sound to be relative to listener position.
 *
 * @param relative true uses distance modeling
 */
void
Sound::ListenerRelative( bool relative )
{
   if( relative )
      SendMessage( kCommand[REL], this );
   else
      SendMessage( kCommand[ABS], this );
}



/**
 * Set the transform position of sound.
 *
 * @param *xform : The new Transform to position this instance
 * @param cs : Optional parameter describing the coordinate system of xform
 *             Defaults to ABS_CS.
 */
void
Sound::SetTransform( dtCore::Transform* xform, dtCore::Transformable::CoordSysEnum cs )
{
   // properly set transform to transformable object
   dtCore::Transformable::SetTransform( xform, cs );

   // get new transform, and break up into
   // position and direction for sound object
   dtCore::Transform transform;
   sgMat4            matrix;
   sgVec3            pos   = { 0.0f, 0.0f, 0.0f };
   sgVec3            dir   = { 0.0f, 1.0f, 0.0f };

   GetTransform( &transform, cs );

   transform.GetTranslation( pos );

   transform.Get( matrix );
   sgXformVec3( dir, matrix );

   SetPosition( pos );
   SetDirection( dir );
}

/**
 * Set the position of sound.
 *
 * @param position to set
 */
void
Sound::SetPosition( const sgVec3& position )
{
   mPos[0L] = position[0L];
   mPos[1L] = position[1L];
   mPos[2L] = position[2L];

   SendMessage( kCommand[POSITION], this );
}



/**
 * Get the position of sound.
 *
 * @param position to get
 */
void
Sound::GetPosition( sgVec3& position ) const
{
   position[0L]   = mPos[0L];
   position[1L]   = mPos[1L];
   position[2L]   = mPos[2L];
}



/**
 * Set the direction of sound.
 *
 * @param direction to set
 */
void
Sound::SetDirection( const sgVec3& direction )
{
   mDir[0L] = direction[0L];
   mDir[1L] = direction[1L];
   mDir[2L] = direction[2L];

   SendMessage( kCommand[DIRECTION], this );
}



/**
 * Get the direction of sound.
 *
 * @param direction to get
 */
void
Sound::GetDirection( sgVec3& direction ) const
{
   direction[0L]  = mDir[0L];
   direction[1L]  = mDir[1L];
   direction[2L]  = mDir[2L];
}



/**
 * Set the velocity of sound.
 *
 * @param velocity to set
 */
void
Sound::SetVelocity( const sgVec3& velocity )
{
   mVelo[0L]   = velocity[0L];
   mVelo[1L]   = velocity[1L];
   mVelo[2L]   = velocity[2L];

   SendMessage( kCommand[VELOCITY], this );
}



/**
 * Get the velocity of sound.
 *
 * @param velocity to get
 */
void
Sound::GetVelocity( sgVec3& velocity ) const
{
   velocity[0L]   = mVelo[0L];
   velocity[1L]   = mVelo[1L];
   velocity[2L]   = mVelo[2L];
}



/**
 * Set the minimum distance that sound plays at max_gain.
 * Attenuation is not calculated below this distance
 *
 * @param distance set to minimum
 */
void
Sound::SetMinDistance( float dist )
{
   mMinDist = MAX( 0.0f, dist );

   SendMessage( kCommand[MIN_DIST], this );
}



/**
 * Set the maximum distance that sound plays at min_gain.
 * Attenuation is not calculated above this distance
 *
 * @param distance set to maximum
 */
void
Sound::SetMaxDistance( float dist )
{
   mMaxDist = MAX( 0.0f, dist );

   SendMessage( kCommand[MAX_DIST], this );
}



/**
 * Set the rolloff factor describing attenuation curve.
 *
 * @param rollff factor to set
 */
void
Sound::SetRolloffFactor( float rolloff )
{
   mRolloff = MAX( 0.0f, rolloff );

   SendMessage( kCommand[ROL_FACT], this );
}



/**
 * Set the minimum gain that sound plays at.
 * Attenuation is clamped to this gain
 *
 * @param gain set to minimum
 */
void
Sound::SetMinGain( float gain )
{
   mMinDist = CLAMP( gain, 0.0f, 1.0f );

   SendMessage( kCommand[MIN_DIST], this );
}



/**
 * Set the maximum gain that sound plays at.
 * Attenuation is clamped to this gain
 *
 * @param gain set to maximum
 */
void
Sound::SetMaxGain( float gain )
{
   mMaxDist = CLAMP( gain, 0.0f, 1.0f );

   SendMessage( kCommand[MAX_DIST], this );
}



/**
 * Generates and returns a key frame that represents the
 * complete recordable state of this object.
 *
 * @return a new key frame
 */
dtCore::StateFrame*
Sound::GenerateKeyFrame( void )
{
   /********************************************/
   /*
   /* WARNING FROM JPJ (Sep.23,2004)
   /*
   /* GenerateKeyFrame was a cut/past from
   /* the old dtCore::Sound,  I did not have
   /* time to implement the guts for the new
   /* dtAudio::Sound.  Whomever works on this
   /* code after me beware... and remove this
   /* comment when you fix this.
   /*
   /* So long and thanks for all the fish.
   /*
   /********************************************/
   return new SoundStateFrame(
      this, 
      SoundStateFrame::Playing | SoundStateFrame::Gain,
      IsPlaying(), 
      mGain
   );
}

/**
 * Deserializes an XML element representing a state frame, turning it
 * into a new StateFrame instance.
 *
 * @param element the element that represents the frame
 * @return a newly generated state frame corresponding to the element
 */
dtCore::StateFrame*
Sound::DeserializeFrame( TiXmlElement* element )
{
   /********************************************/
   /*
   /* WARNING FROM JPJ (Sep.23,2004)
   /*
   /* DeserializeFrame was a cut/past from
   /* the old dtCore::Sound,  I did not have
   /* time to implement the guts for the new
   /* dtAudio::Sound.  Whomever works on this
   /* code after me beware... and remove this
   /* comment when you fix this.
   /*
   /* So long and thanks for all the fish.
   /*
   /********************************************/
   int validElements = 0;
   bool playing = false;
   float gain = 1.0f;

   const char* buf = element->Attribute("playing");

   if(buf != NULL)
   {
      validElements |= SoundStateFrame::Playing;

      if(!strcmp(buf, "true"))
      {
         playing = true;
      }
   }

   buf = element->Attribute("gain");

   if(buf != NULL)
   {
      validElements |= SoundStateFrame::Gain;

      sscanf(buf, "%g", &gain);
   }

   return new SoundStateFrame(this, validElements, playing, gain);
}

