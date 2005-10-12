// sound.cpp: Implementation of the Sound class.
//
//////////////////////////////////////////////////////////////////////

#include "dtAudio/sound.h"
#include "dtCore/scene.h"
#include "dtUtil/serializer.h"

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/util/XMLString.hpp>


// definitions
#if   defined(WIN32) | defined(_WIN32)
#pragma warning( disable : 4800 )
#endif



#if   !  defined(MAX_FLOAT)
#define  MAX_FLOAT   static_cast<float>(0xFFFFFFFF)
#endif



// namespaces
using namespace dtAudio;
XERCES_CPP_NAMESPACE_USE



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

IMPLEMENT_MANAGEMENT_LAYER(Sound)

Sound::FrameData::FrameData(): mGain(0.0f), mPitch(0.0f), mPlaying(false)
{
}

Sound::FrameData::FrameData(float gain, float pitch, bool playing): mGain(gain), mPitch(pitch), mPlaying(playing)
{
}

Sound::FrameData::~FrameData()
{
}


/********************************
 ** Protected Member Functions **
 ********************************/
/**
 * Constructor, user does not create directly
 * instead requests a sound from AudioManager
 */
Sound::Sound()
:  Transformable(),
   mFilename(""),
   mPlayCB(NULL),
   mPlayCBData(NULL),
   mStopCB(NULL),
   mStopCBData(NULL),
   mGain(1.0f),
   mPitch(1.0f),   
   mMinDist(1.0f),
   mMaxDist(static_cast<float>(MAX_FLOAT)),
   mRolloff(1.0f),
   mMinGain(0.0f),
   mMaxGain(1.0f)
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
void Sound::OnMessage( MessageData* data )
{
}



/*****************************
 ** Public Member Functions **
 *****************************/
/**
 * Loads the specified sound file.
 *
 * @param filename the name of the file to load
 */
void Sound::LoadFile( const char* file )
{
   mFilename   = file;
   SendMessage( kCommand[LOAD], this );
}



/**
 * Unloads the specified sound file.
 */
void Sound::UnloadFile( void )
{
   SendMessage( kCommand[UNLOAD], this );
}

/**
 * Set callback for when sound starts playing.
 *
 * @param callback function pointer
 * @param user data
 */
void Sound::SetPlayCallback( CallBack cb, void* param )
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
void Sound::SetStopCallback( CallBack cb, void* param )
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
void Sound::Play( void )
{
   SendMessage( kCommand[PLAY], this );
}



/**
 * Tell audio manager to pause this sound.
 */
void Sound::Pause( void )
{
   SendMessage( kCommand[PAUSE], this );
}



/**
 * Tell audio manager to stop this sound.
 */
void Sound::Stop( void )
{
   SendMessage( kCommand[STOP], this );
}



/**
 * Tell audio manager to rewind this sound.
 */
void Sound::Rewind( void )
{
   SendMessage( kCommand[REWIND], this );
}



/**
 * Sets whether or not to play the sound in a continuous loop.
 *
 * @param looping true to play the sound in a loop, false
 * otherwise
 */
void Sound::SetLooping( bool loop /*= true*/ )
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
void Sound::SetGain( float gain )
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
void Sound::SetPitch( float pitch )
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
void Sound::ListenerRelative( bool relative )
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
void Sound::SetTransform( dtCore::Transform* xform, dtCore::Transformable::CoordSysEnum cs )
{
   // properly set transform to transformable object
   dtCore::Transformable::SetTransform( xform, cs );

   // get new transform, and break up into
   // position and direction for sound object
   dtCore::Transform    transform;
   osg::Matrix          matrix;
   osg::Vec3            pos( 0.0f, 0.0f, 0.0f );
   osg::Vec3            dir( 0.0f, 1.0f, 0.0f );

   GetTransform( &transform, cs );

   transform.GetTranslation( pos );

   transform.Get( matrix );
   dir = osg::Matrix::transform3x3(dir, matrix);
   //sgXformVec3( dir, matrix );

   SetPosition( pos );
   SetDirection( dir );
}

/**
 * Set the position of sound.
 *
 * @param position to set
 */
void Sound::SetPosition( const osg::Vec3& position )
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
void Sound::GetPosition( osg::Vec3& position ) const
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
void Sound::SetDirection( const osg::Vec3& direction )
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
void Sound::GetDirection( osg::Vec3& direction ) const
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
void Sound::SetVelocity( const osg::Vec3& velocity )
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
void Sound::GetVelocity( osg::Vec3& velocity ) const
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
void Sound::SetMinDistance( float dist )
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
void Sound::SetMaxDistance( float dist )
{
   mMaxDist = MAX( 0.0f, dist );

   SendMessage( kCommand[MAX_DIST], this );
}



/**
 * Set the rolloff factor describing attenuation curve.
 *
 * @param rollff factor to set
 */
void Sound::SetRolloffFactor( float rolloff )
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

Sound::FrameData* Sound::CreateFrameData() const
{
   FrameData* fd = new FrameData(mGain,mPitch,this->IsPlaying());
   return fd;
}

void Sound::UseFrameData(const FrameData* fd)
{
   this->SetGain( fd->mGain );
   this->SetPitch( fd->mPitch );
   if( !this->IsPlaying() )
   {
      if( fd->mPlaying )
         this->Play();
   }
   else  // sound is playing
   {
      if( !fd->mPlaying )
         this->Stop();
   }
}

DOMElement* Sound::Serialize(const FrameData* d,XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc) const
{
   XMLCh* SOUND = XMLString::transcode( "Sound" );
   DOMElement* element = doc->createElement( SOUND );
   XMLString::release( &SOUND );

   XMLCh* MYNAME = XMLString::transcode( this->GetName().c_str() );
   XMLCh* NAME = XMLString::transcode( "Name" );
   element->setAttribute( NAME, MYNAME );
   XMLString::release( &NAME );
   XMLString::release( &MYNAME );

   DOMElement* gelement = dtUtil::Serializer::ToFloat(d->mGain,"Gain",doc);
   element->appendChild( gelement );

   DOMElement* pelement = dtUtil::Serializer::ToFloat(d->mPitch,"Pitch",doc);
   element->appendChild( pelement );

   DOMElement* playelement = dtUtil::Serializer::ToBool(d->mPlaying,"Playing",doc);
   element->appendChild( playelement );

   return element;
}

