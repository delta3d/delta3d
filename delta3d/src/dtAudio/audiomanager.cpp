#include <assert.h>
#include <stack>

#include <osgDB/FileUtils>

#include <alut.h>

#include "dtCore/system.h"
#include "dtCore/notify.h"
#include "dtAudio/audiomanager.h"



#if   defined(WIN32) | defined(_WIN32)
#pragma warning( disable : 4800 )
#endif



// definitions
#if   !  defined(BIT)
#define  BIT(a)      (1L<<a)
#endif

#if   !  defined(MAX_FLOAT)
#define  MAX_FLOAT   static_cast<float>(0xFFFFFFFF)
#endif



// name spaces
using namespace   dtAudio;
using namespace   std;



// helper template functions
template <class   T>
T  MIN( T a, T b )         {  return   (a<b)?a:b;        }

template <class   T>
T  MAX( T a, T b )         {  return   (a>b)?a:b;        }

template <class   T>
T  CLAMP( T x, T l, T h )  {  return   MAX(MIN(x,h),l);  }



AudioManager::MOB_ptr   AudioManager::_Mgr(NULL);
AudioManager::LOB_PTR   AudioManager::_Mic(NULL);
const char*             AudioManager::_EaxVer   = "EAX2.0";
const char*             AudioManager::_EaxSet   = "EAXSet";
const char*             AudioManager::_EaxGet   = "EAXGet";
const AudioConfigData   AudioManager::_DefCfg;



IMPLEMENT_MANAGEMENT_LAYER(AudioManager::SoundObj)
IMPLEMENT_MANAGEMENT_LAYER(AudioManager::ListenerObj)
IMPLEMENT_MANAGEMENT_LAYER(AudioManager)



// public member functions
// default consructor
AudioManager::AudioManager( std::string name /*= "audiomanager"*/ )
:  Base(name),
   mNumSources(0L),
   mSource(NULL),
   mEAXSet(NULL),
   mEAXGet(NULL)
{
   RegisterInstance( this );

   mSourceMap.clear();
   mActiveList.clear();
   mBufferMap.clear();
   mSoundList.clear();

   while( mAvailable.size() )
      mAvailable.pop();

   while( mPlayQueue.size() )
      mPlayQueue.pop();

   while( mPauseQueue.size() )
      mPauseQueue.pop();

   while( mStopQueue.size() )
      mStopQueue.pop();

   while( mRewindQueue.size() )
      mRewindQueue.pop();

   while( mSoundCommand.size() )
      mSoundCommand.pop();

   while( mSoundRecycle.size() )
      mSoundRecycle.pop();

   AddSender( dtCore::System::GetSystem() );

   alutInit( 0L, NULL );
}



// desructor
AudioManager::~AudioManager()
{
   DeregisterInstance( this );

   // stop all sources
   for( unsigned int ii(0L); ii < mNumSources; ii++ )
   {
      alSourceStop( mSource[ii] );
      alSourcei( mSource[ii], AL_BUFFER, AL_NONE );
   }

   // delete the sources
   if( mSource != NULL )
   {
      alDeleteSources( mNumSources, mSource );
      delete   mSource;
   }

   mSourceMap.clear();
   mActiveList.clear();

   while( mAvailable.size() )
      mAvailable.pop();

   while( mPlayQueue.size() )
      mPlayQueue.pop();

   while( mPauseQueue.size() )
      mPauseQueue.pop();

   while( mStopQueue.size() )
      mStopQueue.pop();

   while( mRewindQueue.size() )
      mRewindQueue.pop();

   // delete the buffers
   BufferData* bd(NULL);
   for( BUF_MAP::iterator iter(mBufferMap.begin()); iter != mBufferMap.end(); iter++ )
   {
      if( ( bd = iter->second ) == NULL )
         continue;

      iter->second   = NULL;
      alDeleteBuffers( 1L, &bd->buf );
      delete   bd;
   }
   mBufferMap.clear();

//   SOB_PTR  snd(NULL);
   for( SND_LST::iterator iter(mSoundList.begin()); iter != mSoundList.end(); iter++ )
   {
//      snd   = *iter;
      *iter = NULL;

//      if( snd == NULL )
//         continue;

//      delete   snd;
//      snd   = NULL;
   }
   mSoundList.clear();

   while( mSoundCommand.size() )
      mSoundCommand.pop();

   while( mSoundRecycle.size() )
      mSoundRecycle.pop();

   alutExit();

   RemoveSender( dtCore::System::GetSystem() );
}



// create the singleton manager
void
AudioManager::Instantiate( void )
{
   if( _Mgr.get() )
      return;

   _Mgr  = new AudioManager;
   assert( _Mgr.get() );

   _Mic  = new ListenerObj;
   assert( _Mic.get() );
}



// destroy the singleton manager
void
AudioManager::Destroy( void )
{
   _Mic  = NULL;
   _Mgr  = NULL;
}



// static instance accessor
AudioManager*
AudioManager::GetManager( void )
{
   return   _Mgr.get();
}



// static listener accessor
Listener*
AudioManager::GetListener( void )
{
   return   static_cast<Listener*>(_Mic.get());
}



// manager configuration
void
AudioManager::Config( const AudioConfigData& data /*= _DefCfg*/ )
{
   if( mSource )
   {
      // already configured
      assert( false );
      return;
   }

   // set up the distance model
   switch( data.distancemodel )
   {
      case  AL_NONE:
         alDistanceModel( AL_NONE );
         break;

      case  AL_INVERSE_DISTANCE_CLAMPED:
         alDistanceModel( AL_INVERSE_DISTANCE_CLAMPED );
         break;

      case  AL_INVERSE_DISTANCE:
      default:
         alDistanceModel( AL_INVERSE_DISTANCE );
         break;
   }


   // set up the sources
   if( ! ConfigSources( data.numSources ) )
      return;


   // set up EAX
   ConfigEAX( data.eax );
}



// message receiver
void
AudioManager::OnMessage( MessageData* data )
{
   assert( data );

   // system messages
   if( data->message == "preframe" )
   {
      PreFrame( *static_cast<const double*>(data->userData) );
      return;
   }

   if( data->message == "frame" )
   {
      Frame( *static_cast<const double*>(data->userData) );
      return;
   }

   if( data->message == "postframe" )
   {
      PostFrame( *static_cast<const double*>(data->userData) );
      return;
   }


   // sound commands
   if( data->message == Sound::kCommand[Sound::POSITION] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::POSITION] );
      mSoundCommand.push( snd );
      return;
   }

   if( data->message == Sound::kCommand[Sound::DIRECTION] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::DIRECTION] );
      mSoundCommand.push( snd );
      return;
   }

   if( data->message == Sound::kCommand[Sound::VELOCITY] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::VELOCITY] );
      mSoundCommand.push( snd );
      return;
   }

   if( data->message == Sound::kCommand[Sound::PLAY] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::PLAY] );
      mSoundCommand.push( snd );
      return;
   }

   if( data->message == Sound::kCommand[Sound::STOP] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::STOP] );
      mSoundCommand.push( snd );
      return;
   }

   if( data->message == Sound::kCommand[Sound::PAUSE] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::PAUSE] );
      mSoundCommand.push( snd );
      return;
   }

   if( data->message == Sound::kCommand[Sound::LOAD] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::LOAD] );
      mSoundCommand.push( snd );
      return;
   }

   if( data->message == Sound::kCommand[Sound::UNLOAD] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::UNLOAD] );
      mSoundCommand.push( snd );
      return;
   }

   if( data->message == Sound::kCommand[Sound::LOOP] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::LOOP] );
      mSoundCommand.push( snd );
      return;
   }

   if( data->message == Sound::kCommand[Sound::UNLOOP] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::UNLOOP] );
      mSoundCommand.push( snd );
      return;
   }

   if( data->message == Sound::kCommand[Sound::GAIN] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::GAIN] );
      mSoundCommand.push( snd );
      return;
   }

   if( data->message == Sound::kCommand[Sound::PITCH] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::PITCH] );
      mSoundCommand.push( snd );
      return;
   }

   if( data->message == Sound::kCommand[Sound::REWIND] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::REWIND] );
      mSoundCommand.push( snd );
      return;
   }

   if( data->message == Sound::kCommand[Sound::REL] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::REL] );
      mSoundCommand.push( snd );
      return;
   }

   if( data->message == Sound::kCommand[Sound::ABS] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::ABS] );
      mSoundCommand.push( snd );
      return;
   }

   if( data->message == Sound::kCommand[Sound::MIN_DIST] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::MIN_DIST] );
      mSoundCommand.push( snd );
      return;
   }

   if( data->message == Sound::kCommand[Sound::MAX_DIST] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::MAX_DIST] );
      mSoundCommand.push( snd );
      return;
   }

   if( data->message == Sound::kCommand[Sound::ROL_FACT] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::ROL_FACT] );
      mSoundCommand.push( snd );
      return;
   }

   if( data->message == Sound::kCommand[Sound::MIN_GAIN] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::MIN_GAIN] );
      mSoundCommand.push( snd );
      return;
   }

   if( data->message == Sound::kCommand[Sound::MAX_GAIN] )
   {
      assert( data->userData );
      SoundObj*   snd(static_cast<SoundObj*>(data->userData));
      snd->Command( Sound::kCommand[Sound::MAX_GAIN] );
      mSoundCommand.push( snd );
      return;
   }
}



Sound*
AudioManager::NewSound( void )
{
   SOB_PTR  snd(NULL);

   // first look if we can recycle a sound
   if( mSoundRecycle.size() )
   {
      snd   = mSoundRecycle.front();
      assert( snd.get() );

      snd->Clear();
      mSoundRecycle.pop();
   }

   // create a new sound object if we don't have one
   if( snd.get() == NULL )
   {
      snd   = new SoundObj;
      assert( snd.get() );
   }

   // listen to messages from this guy
   AddSender( snd.get() );

   // save the sound
   mSoundList.push_back( snd );

   // hand out the interface to the sound
   return   static_cast<Sound*>(snd.get());
}



void
AudioManager::FreeSound( Sound*& sound )
{
   SOB_PTR  snd   = static_cast<SoundObj*>(sound);

   // remove user's copy of pointer
   sound = NULL;

   if( snd.get() == NULL )
      return;

   // remove sound from list
   SND_LST::iterator iter(NULL);
   for( iter = mSoundList.begin(); iter != mSoundList.end(); iter++ )
   {
      if( snd != *iter )
         continue;

      mSoundList.erase( iter );
      break;
   }

   // stop listening to this guys messages
   snd->RemoveSender( this );
   snd->RemoveSender( dtCore::System::GetSystem() );
   RemoveSender( snd.get() );

   // free the sound's source and buffer
   FreeSource( snd.get() );
   UnloadSound( snd.get() );
   snd->Clear();

   // recycle this sound
   mSoundRecycle.push( snd );
}



bool
AudioManager::LoadWaveFile( const char* file )
{
   if( file == NULL )
      // no file name, bail...
      return   false;

   std::string filename = osgDB::findDataFile( file );
   if( filename == "" )
   {
      // still no file name, bail...
      dtCore::Notify( dtCore::WARN, "AudioManager: can't load file %s", file );
      return   false;
   }

   BufferData* bd = mBufferMap[file];
   if( bd != NULL )
      // file already loaded, bail...
      return   false;

   bd = new BufferData;
   assert( bd );

   ALenum      err(alGetError());
   ALenum      format(ALenum(0L));
   ALvoid*     data(NULL);
   ALsizei     size(0L);
   ALsizei     freq(0L);
   ALbyte      fname[256L];


   // create buffer for the wave file
   alGenBuffers( 1L, &bd->buf );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      // somethings wrong with buffers, bail...
      dtCore::Notify( dtCore::WARN, "AudioManager: alGenBuffers error %d", err );
      delete   bd;
      return   false;
   }


   // load the wave file
   unsigned int   len = MIN( filename.size(), size_t(255L) );
   memcpy( fname, filename.c_str(), len );
   fname[len]  = 0L;
   alutLoadWAVFile( fname, &format, &data, &size, &freq, &bd->loop );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      // can't load the wave file, bail...
      dtCore::Notify( dtCore::WARN, "AudioManager: alutLoadWAVFile error %d on %s", err, file );
      alDeleteBuffers( 1L, &bd->buf );
      delete   bd;
      return   false;
   }


   // copy wave file to the buffer
   alBufferData( bd->buf, format, data, size, freq );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      // can't copy the data??? bail like you've never bailed before...
      dtCore::Notify( dtCore::WARN, "AudioManager: alBufferData error %d on %s", err, file );
      alDeleteBuffers( 1L, &bd->buf );
      alutUnloadWAV( format, data, size, freq );
      delete   bd;
      return   false;
   }


   // unload the wave file
   alutUnloadWAV( format, data, size, freq );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      // can't unload the wave file?
      // well, ok we just leaked but can continue
      dtCore::Notify( dtCore::WARN, "AudioManager: alutUnloadWAV error %d on %s", err, file );
   }


   // store this buffer in the map by name
   mBufferMap[file]  = bd;
   bd->file          = mBufferMap.find( file )->first.c_str();
   return   true;
}



bool
AudioManager::UnloadWaveFile( const char* file )
{
   if( file == NULL )
      // no file name, bail...
      return   false;

   BUF_MAP::iterator iter  = mBufferMap.find( file );
   if( iter == mBufferMap.end() )
      // file is not loaded, bail...
      return   false;

   BufferData* bd = iter->second;
   if( bd == NULL )
   {
      // bd should never be NULL
      // this code should never run
      mBufferMap.erase( iter );
      return   false;
   }

   if( bd->use )
      // buffer still in use, don't remove buffer
      return   false;

   alDeleteBuffers( 1L, &bd->buf );
   delete   bd;

   mBufferMap.erase( iter );
   return   true;
}



// private member functions
void
AudioManager::PreFrame( const double deltaFrameTime )
{
   SOB_PTR     snd(NULL);
   const char* cmd(NULL);

   // flush all the sound commands
   while( mSoundCommand.size() )
   {
      snd   = mSoundCommand.front();
      mSoundCommand.pop();

      if( snd.get() == NULL )
         continue;

      cmd   = snd->Command();

      // set sound position
      if( cmd == Sound::kCommand[Sound::POSITION] )
      {
         SetRelative( snd.get() );
         SetPosition( snd.get() );
         continue;
      }

      // set sound direction
      if( cmd == Sound::kCommand[Sound::DIRECTION] )
      {
         SetRelative( snd.get() );
         SetDirection( snd.get() );
         continue;
      }

      // set sound velocity
      if( cmd == Sound::kCommand[Sound::VELOCITY] )
      {
         SetRelative( snd.get() );
         SetVelocity( snd.get() );
         continue;
      }

      // set sound to play
      if( cmd == Sound::kCommand[Sound::PLAY] )
      {
         PlaySound( snd.get() );
         continue;
      }

      // set sound to stop
      if( cmd == Sound::kCommand[Sound::STOP] )
      {
         StopSound( snd.get() );
         continue;
      }

      // set sound to pause
      if( cmd == Sound::kCommand[Sound::PAUSE] )
      {
         PauseSound( snd.get() );
         continue;
      }

      // loading a new sound
      if( cmd == Sound::kCommand[Sound::LOAD] )
      {
         LoadSound( snd.get() );
         continue;
      }

      // unloading an old sound
      if( cmd == Sound::kCommand[Sound::UNLOAD] )
      {
         UnloadSound( snd.get() );
         continue;
      }

      // setting the loop flag
      if( cmd == Sound::kCommand[Sound::LOOP] )
      {
         SetLoop( snd.get() );
         continue;
      }

      // un-setting the loop flag
      if( cmd == Sound::kCommand[Sound::UNLOOP] )
      {
         ResetLoop( snd.get() );
         continue;
      }

      // setting the gain
      if( cmd == Sound::kCommand[Sound::GAIN] )
      {
         SetGain( snd.get() );
         continue;
      }

      // setting the pitch
      if( cmd == Sound::kCommand[Sound::PITCH] )
      {
         SetPitch( snd.get() );
         continue;
      }

      // rewind the sound
      if( cmd == Sound::kCommand[Sound::REWIND] )
      {
         RewindSound( snd.get() );
         continue;
      }

      // set sound relative to listener
      if( cmd == Sound::kCommand[Sound::REL] )
      {
         SetRelative( snd.get() );
         continue;
      }

      // set sound absolute (not relative to listener)
      if( cmd == Sound::kCommand[Sound::ABS] )
      {
         SetAbsolute( snd.get() );
         continue;
      }

      // set minimum distance for attenuation
      if( cmd == Sound::kCommand[Sound::MIN_DIST] )
      {
         SetReferenceDistance( snd.get() );
         continue;
      }

      // set maximum distance for attenuation
      if( cmd == Sound::kCommand[Sound::MAX_DIST] )
      {
         SetMaximumDistance( snd.get() );
         continue;
      }

      // set rolloff factor for attenuation
      if( cmd == Sound::kCommand[Sound::MIN_GAIN] )
      {
         SetRolloff( snd.get() );
         continue;
      }

      // set minimum gain for attenuation
      if( cmd == Sound::kCommand[Sound::MIN_GAIN] )
      {
         SetMinimumGain( snd.get() );
         continue;
      }

      // set maximum gain for attenuation
      if( cmd == Sound::kCommand[Sound::MAX_GAIN] )
      {
         SetMaximumGain( snd.get() );
         continue;
      }

      // set the roll off attenutation factor
      if ( cmd == Sound::kCommand[Sound::ROL_FACT] )
      {
         SetRolloff( snd.get() );
         continue;
      }

      Notify( dtCore::WARN, "AudioManager: Unknown command:%s", cmd);
   }
}



void
AudioManager::Frame( const double deltaFrameTime )
{
   SRC_LST::iterator             iter(NULL);
   std::stack<SRC_LST::iterator> stk;
   ALuint                        src(0L);
   ALint                         state(AL_STOPPED);
   ALenum                        err(alGetError());
   SOB_PTR                       snd(NULL);


   // signal any sources commanded to stop
   while( mStopQueue.size() )
   {
      src   = mStopQueue.front();
      mStopQueue.pop();

      assert( alIsSource( src ) );

      alSourceStop( src );
   }


   // push the new sources onto the active list
   while( mPlayQueue.size() )
   {
      src   = mPlayQueue.front();
      mPlayQueue.pop();

      assert( alIsSource( src ) );

      mActiveList.push_back( src );
   }


   // start any new sounds and
   // remove any sounds that have stopped
   for( iter = mActiveList.begin(); iter != mActiveList.end(); iter++ )
   {
      src   = *iter;
      assert( alIsSource( src ) == AL_TRUE );

      alGetSourcei( src, AL_SOURCE_STATE, &state );
      if( ( err = alGetError() ) != AL_NO_ERROR )
      {
         dtCore::Notify( dtCore::WARN, "AudioManager: alGetSourcei(AL_SOURCE_STATE) error %d", err );
         continue;
      }

      switch( state )
      {
         case  AL_PLAYING:
         case  AL_PAUSED:
            // don't need to do anything
            break;

         case  AL_INITIAL:
            {
               // start any new sources
               alSourcePlay( src );

               // send play message
               snd   = mSourceMap[src];
               if( snd.get() )
               {
                  SendMessage( Sound::kCommand[Sound::PLAY], snd.get() );
               }
            }
            break;

         case  AL_STOPPED:
            {
               // send stopped message
               snd   = mSourceMap[src];
               if( snd.get() )
               {
                  SendMessage( Sound::kCommand[Sound::STOP], snd.get() );
               }

               // save stopped sound iterator for later removal
               stk.push( iter );
            }
            break;

         default:
            break;
      }
   }


   // signal any sources commanded to pause
   while( mPauseQueue.size() )
   {
      src   = mPauseQueue.front();
      mPauseQueue.pop();

      assert( alIsSource( src ) );

      alGetSourcei( src, AL_SOURCE_STATE, &state );
      if( ( err = alGetError() ) != AL_NO_ERROR )
      {
         dtCore::Notify( dtCore::WARN, "AudioManager: alGetSourcei(AL_SOURCE_STATE) error %d", err );
         continue;
      }

      switch( state )
      {
         case  AL_PLAYING:
            {
               alSourcePause( src );

               // send pause message
               snd   = mSourceMap[src];
               if( snd.get() )
               {
                  SendMessage( Sound::kCommand[Sound::PAUSE], snd.get() );
               }
            }
            break;

         case  AL_PAUSED:
            {
               alSourcePlay( src );

               // send pause message
               snd   = mSourceMap[src];
               if( snd.get() )
               {
                  SendMessage( Sound::kCommand[Sound::PLAY], snd.get() );
               }
            }
            break;

         default:
            break;
      }
   }


   // signal any sources commanded to rewind
   while( mRewindQueue.size() )
   {
      src   = mRewindQueue.front();
      mRewindQueue.pop();

      assert( alIsSource( src ) );

      alSourceRewind( src );

      // send rewind message
      snd   = mSourceMap[src];
      if( snd.get() )
      {
         SendMessage( Sound::kCommand[Sound::REWIND], snd.get() );
      }
   }


   // remove stopped sounds from the active list
   while( stk.size() )
   {
      iter  = stk.top();
      stk.pop();

      src   = *iter;
      mStopQueue.push( src );
      mActiveList.erase( iter );
   }
}



void
AudioManager::PostFrame( const double deltaFrameTime )
{
   SOB_PTR     snd(NULL);
   ALuint      src(0L);
   ALenum      err(alGetError());

   // for all sounds that have stopped
   while( mStopQueue.size() )
   {
      // free the source for later use
      src   = mStopQueue.front();
      mStopQueue.pop();

      snd   = mSourceMap[src];
      snd->RemoveSender( this );
      snd->RemoveSender( dtCore::System::GetSystem() );

      FreeSource( snd.get() );
   }
}



bool
AudioManager::Configured( void ) const
{
   return   mSource != NULL;
}



bool
AudioManager::ConfigSources( unsigned int num )
{
   if( num == 0L )
      return   false;

   mNumSources = ALsizei(num);

   mSource  = new ALuint[mNumSources];
   assert( mSource );

   ALenum   error(alGetError());
   alGenSources( mNumSources, mSource );
   if( ( error = alGetError() ) != AL_NO_ERROR )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: alGenSources Error %d", error );
      if( mSource )
      {
         delete   mSource;
         mSource  = NULL;
      }
      return   false;
   }


   for( unsigned int ii(0L); ii < mNumSources; ii++ )
   {
      assert( alIsSource( mSource[ii] ) == AL_TRUE );
      mAvailable.push( mSource[ii] );
   }

   return   true;
}



bool
AudioManager::ConfigEAX( bool eax )
{
   if( ! eax )
      return   false;

   // copy strings because
   // funcs don't take const char
   ALubyte  buf[32L];
   memset( buf, 0L, 32L );
   memcpy( buf, _EaxVer, MIN( strlen( _EaxVer ), size_t(32L) ) );


   // check for EAX support
   if( alIsExtensionPresent( buf ) == AL_FALSE )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: %s is not available", _EaxVer );
      return   false;
   }


   // get the eax-set function
   memset( buf, 0L, 32L );
   memcpy( buf, _EaxSet, MIN( strlen( _EaxSet ), size_t(32L) ) );

   mEAXSet  = alGetProcAddress( buf );
   if( mEAXSet == NULL )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: %s is not available", _EaxVer );
      return   false;
   }


   // get the eax-get function
   memset( buf, 0L, 32L );
   memcpy( buf, _EaxGet, MIN( strlen( _EaxGet ), size_t(32L) ) );

   mEAXGet  = alGetProcAddress( buf );
   if( mEAXGet == NULL)
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: %s is not available", _EaxVer );
      mEAXSet  = NULL;
      return   false;
   }

   return   true;
}



void
AudioManager::LoadSound( SoundObj* snd )
{
   assert( snd );

   const char* file(static_cast<Sound*>(snd)->GetFilename());

   if( file == NULL )
      return;

   LoadWaveFile( file );

   BufferData* bd = mBufferMap[file];

   if( bd == NULL )
      return;

   bd->use++;
   snd->Buffer( bd->buf );
}



void
AudioManager::UnloadSound( SoundObj* snd )
{
   assert( snd );

   const char* file(static_cast<Sound*>(snd)->GetFilename());

   if( file == NULL )
      return;

   snd->Buffer( 0L );

   BufferData* bd = mBufferMap[file];

   if( bd == NULL )
      return;

   bd->use--;

   UnloadWaveFile( file );
}



void
AudioManager::PlaySound( SoundObj* snd )
{
   ALuint   buf(0L);
   ALuint   src(0L);
   ALenum   err(alGetError());

   assert( snd );

   // first check if sound has a buffer
   buf   = snd->Buffer();
   if( alIsBuffer( buf ) == AL_FALSE )
      // no buffer, bail
      return;

   // then check if sound has a source
   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE )
   {
      // no source, gotta get one
      if( ! GetSource( snd ) )
      {
         // no source available
         dtCore::Notify( dtCore::WARN, "AudioManager: play attempt w/o available sources" );
         dtCore::Notify( dtCore::WARN, "AudioManager: try increasing the number of sources at config time" );
         return;
      }

      src   = snd->Source();
   }
   else
   {
      // already has buffer and source
      // could be paused (or playing)
      ALint state(AL_STOPPED);
      alGetSourcei( src, AL_SOURCE_STATE, &state );
      if( ( err = alGetError() ) != AL_NO_ERROR )
      {
         dtCore::Notify( dtCore::WARN, "AudioManager: alGetSourcei(AL_SOURCE_STATE) error %d", err );
         return;
      }

      switch( state )
      {
         case  AL_PAUSED:
            mPauseQueue.push( src );
            // no break, run to next case

         case  AL_PLAYING:
            return;
            break;

         default:
            // either initialized or stopped
            // continue setting the buffer and playing
            break;
      }
   }

   // bind the buffer to the source
   alSourcei( src, AL_BUFFER, buf );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: alSourcei(AL_BUFFER) error %d", err );
      return;
   }

   // set looping flag
   alSourcei( src, AL_LOOPING, (snd->IsLooping())? AL_TRUE: AL_FALSE );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: alSourcei(AL_LOOPING) error %d", err );
   }

   // set source relative flag
   if( snd->IsListenerRelative() )
   {
      // is listener relative
      alSourcei( src, AL_SOURCE_RELATIVE, AL_FALSE );
      if( ( err = alGetError() ) != AL_NO_ERROR )
      {
         dtCore::Notify( dtCore::WARN, "AudioManager: alSourcei(AL_SOURCE_RELATIVE) error %d", err );
      }

      // set initial position and direction
      sgVec3   pos   = { 0.0f, 0.0f, 0.0f };
      sgVec3   dir   = { 0.0f, 1.0f, 0.0f };

      snd->GetPosition( pos );
      snd->GetDirection( dir );

      alSource3f( src,
                  AL_POSITION,
                  static_cast<ALfloat>(pos[0L]),
                  static_cast<ALfloat>(pos[1L]),
                  static_cast<ALfloat>(pos[2L]) );
      if( ( err = alGetError() ) != AL_NO_ERROR )
      {
         dtCore::Notify( dtCore::WARN, "AudioManager: alSource3f(AL_POSITION) error %d", err );
      }

      alSource3f( src,
                  AL_DIRECTION,
                  static_cast<ALfloat>(dir[0L]),
                  static_cast<ALfloat>(dir[1L]),
                  static_cast<ALfloat>(dir[2L]) );
      if( ( err = alGetError() ) != AL_NO_ERROR )
      {
         dtCore::Notify( dtCore::WARN, "AudioManager: alSource3f(AL_DIRECTION) error %d", err );
      }
   }
   else
   {
      // not listener relative
      alSourcei( src, AL_SOURCE_RELATIVE, AL_FALSE );
      if( ( err = alGetError() ) != AL_NO_ERROR )
      {
         dtCore::Notify( dtCore::WARN, "AudioManager: alSourcei(AL_SOURCE_RELATIVE) error %d", err );
      }
   }

   // set gain
   alSourcef( src, AL_GAIN, static_cast<ALfloat>(snd->GetGain()) );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: alSourcef(AL_GAIN) error %d", err );
   }

   // set pitch
   alSourcef( src, AL_PITCH, static_cast<ALfloat>(snd->GetPitch()) );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: alSourcef(AL_PITCH) error %d", err );
   }

   // set reference distance
   if( snd->GetState( Sound::MIN_DIST ) )
   {
      snd->ResetState( Sound::MIN_DIST );
      alSourcef( src, AL_REFERENCE_DISTANCE, static_cast<ALfloat>(snd->GetMinDistance()) );
      if( ( err = alGetError() ) != AL_NO_ERROR )
      {
         dtCore::Notify( dtCore::WARN, "AudioManager: alSourcef(AL_REFERENCE_DISTANCE) error %d", err );
      }
   }

   // set maximum distance
   if( snd->GetState( Sound::MAX_DIST ) )
   {
      snd->ResetState( Sound::MAX_DIST );
      alSourcef( src, AL_MAX_DISTANCE, static_cast<ALfloat>(snd->GetMaxDistance()) );
      if( ( err = alGetError() ) != AL_NO_ERROR )
      {
         dtCore::Notify( dtCore::WARN, "AudioManager: alSourcef(AL_MAX_DISTANCE) error %d", err );
      }
   }

   // set rolloff factor
   if( snd->GetState( Sound::ROL_FACT ) )
   {
      snd->ResetState( Sound::ROL_FACT );
      alSourcef( src, AL_ROLLOFF_FACTOR, static_cast<ALfloat>(snd->GetRolloffFactor()) );
      if( ( err = alGetError() ) != AL_NO_ERROR )
      {
         dtCore::Notify( dtCore::WARN, "AudioManager: alSourcef(AL_ROLLOFF_FACTOR) error %d", err );
      }
   }

   // set minimum gain
   if( snd->GetState( Sound::MIN_GAIN ) )
   {
      snd->ResetState( Sound::MIN_GAIN );
      alSourcef( src, AL_MIN_GAIN, static_cast<ALfloat>(snd->GetMinGain()) );
      if( ( err = alGetError() ) != AL_NO_ERROR )
      {
         dtCore::Notify( dtCore::WARN, "AudioManager: alSourcef(AL_MIN_GAIN) error %d", err );
      }
   }

   // set maximum gain
   if( snd->GetState( Sound::MAX_GAIN ) )
   {
      snd->ResetState( Sound::MAX_GAIN );
      alSourcef( src, AL_MAX_GAIN, static_cast<ALfloat>(snd->GetMaxGain()) );
      if( ( err = alGetError() ) != AL_NO_ERROR )
      {
         dtCore::Notify( dtCore::WARN, "AudioManager: alSourcef(AL_MAX_GAIN) error %d", err );
      }
   }

   snd->AddSender( this );
   snd->AddSender( dtCore::System::GetSystem() );
   mPlayQueue.push( snd->Source() );
}



void
AudioManager::PauseSound( SoundObj* snd )
{
   assert( snd );

   ALuint   src   = snd->Source();
   if( src == 0L )
   {
      // sound is not playing, bail
      return;
   }

   mPauseQueue.push( src );
}



void
AudioManager::StopSound( SoundObj* snd )
{
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE )
   {
      // sound is not playing, bail
      return;
   }

   mStopQueue.push( src );
}



void
AudioManager::RewindSound( SoundObj* snd )
{
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE )
   {
      // sound is not playing, bail
      return;
   }

   mRewindQueue.push( src );
}



void
AudioManager::SetLoop( SoundObj* snd )
{
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE )
   {
      // sound is not playing
      // set flag and bail
      snd->SetState( Sound::LOOP );
      return;
   }

   ALenum   err(alGetError());
   alSourcei( src, AL_LOOPING, AL_TRUE );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: alSourcei(AL_LOOP) error %d", err );
      return;
   }

   SendMessage( Sound::kCommand[Sound::LOOP], snd );
}



void
AudioManager::ResetLoop( SoundObj* snd )
{
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE )
   {
      // sound is not playing
      // set flag and bail
      snd->ResetState( Sound::LOOP );
      return;
   }

   ALenum   err(alGetError());
   alSourcei( src, AL_LOOPING, AL_FALSE );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: alSourcei(AL_LOOP) error %d", err );
      return;
   }

   SendMessage( Sound::kCommand[Sound::UNLOOP], snd );
}



void
AudioManager::SetRelative( SoundObj* snd )
{
   assert( snd );

   if( snd->IsListenerRelative() )
      // already set, bail
      return;

   ALuint   buf   = snd->Buffer();
   if( alIsBuffer( buf ) == AL_FALSE )
   {
      // does not have sound buffer
      // set flag and bail
      snd->ResetState( Sound::POSITION );
      return;
   }
   else
   {
      // check for stereo
      // multiple channels don't get positioned
      ALint numchannels(0L);
      alGetBufferi( buf, AL_CHANNELS, &numchannels );
      if( numchannels == 2L )
      {
         // stereo!
         // set flag and bail
         dtCore::Notify(dtCore::WARN, "AudioManager: A stereo Sound can't be positioned in 3D space");
         snd->ResetState( Sound::POSITION );
         return;
      }
   }

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE )
   {
      // sound is not playing
      // set flag and bail
      snd->SetState( Sound::POSITION );
      return;
   }

   ALenum   err(alGetError());
   alSourcei( src, AL_SOURCE_RELATIVE, AL_FALSE );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: alSourcei(AL_SOURCE_RELATIVE) error %d", err );
      return;
   }

   SendMessage( Sound::kCommand[Sound::REL], snd );
}



void
AudioManager::SetAbsolute( SoundObj* snd )
{
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE )
   {
      // sound is not playing
      // set flag and bail
      snd->ResetState( Sound::POSITION );
      return;
   }

   ALenum   err(alGetError());
   alSourcei( src, AL_SOURCE_RELATIVE, AL_FALSE );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: alSourcei(AL_SOURCE_RELATIVE) error %d", err );
      return;
   }

   SendMessage( Sound::kCommand[Sound::ABS], snd );
}



void
AudioManager::SetGain( SoundObj* snd )
{
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE )
   {
      // sound is not playing, bail
      return;
   }

   ALenum   err(alGetError());
   alSourcef( src, AL_GAIN, static_cast<ALfloat>(snd->GetGain()) );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: alSourcef(AL_GAIN) error %d", err );
      return;
   }

   SendMessage( Sound::kCommand[Sound::GAIN], snd );
}



void
AudioManager::SetPitch( SoundObj* snd )
{
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE )
   {
      // sound is not playing, bail
      return;
   }

   ALenum   err(alGetError());
   alSourcef( src, AL_PITCH, static_cast<ALfloat>(snd->GetPitch()) );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: alSourcef(AL_PITCH) error %d", err );
      return;
   }

   SendMessage( Sound::kCommand[Sound::PITCH], snd );
}



void
AudioManager::SetPosition( SoundObj* snd )
{
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE )
   {
      // sound is not playing, bail
      return;
   }

   sgVec3   pos;
   snd->GetPosition( pos );

   ALenum   err(alGetError());
   alSource3f( src, AL_POSITION, static_cast<ALfloat>(pos[0L]), static_cast<ALfloat>(pos[1L]), static_cast<ALfloat>(pos[2L]) );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: alSource3f(AL_POSITION) error %d", err );
      return;
   }

   SendMessage( Sound::kCommand[Sound::POSITION], snd );
}



void
AudioManager::SetDirection( SoundObj* snd )
{
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE )
   {
      // sound is not playing, bail
      return;
   }

   sgVec3   dir;
   snd->GetDirection( dir );

   ALenum   err(alGetError());
   alSource3f( src, AL_DIRECTION, static_cast<ALfloat>(dir[0L]), static_cast<ALfloat>(dir[1L]), static_cast<ALfloat>(dir[2L]) );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: alSource3f(AL_DIRECTION) error %d", err );
      return;
   }

   SendMessage( Sound::kCommand[Sound::DIRECTION], snd );
}



void
AudioManager::SetVelocity( SoundObj* snd )
{
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE )
   {
      // sound is not playing, bail
      return;
   }

   sgVec3   velo;
   snd->GetVelocity( velo );

   ALenum   err(alGetError());
   alSource3f( src, AL_VELOCITY, static_cast<ALfloat>(velo[0L]), static_cast<ALfloat>(velo[1L]), static_cast<ALfloat>(velo[2L]) );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: alSource3f(AL_VELOCITY) error %d", err );
      return;
   }

   SendMessage( Sound::kCommand[Sound::VELOCITY], snd );
}



void
AudioManager::SetReferenceDistance( SoundObj* snd )
{
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE )
   {
      // sound is not playing, set flag and bail
      snd->SetState( Sound::MIN_DIST );
      return;
   }

   ALfloat  min_dist(static_cast<ALfloat>(snd->GetMinDistance()));
   ALfloat  max_dist(static_cast<ALfloat>(snd->GetMaxDistance()));
   assert( min_dist <= max_dist );

   ALenum   err(alGetError());
   alSourcef( src, AL_REFERENCE_DISTANCE, min_dist );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: alSourcef(AL_REFERENCE_DISTANCE) error %d", err );
      return;
   }
}



void
AudioManager::SetMaximumDistance( SoundObj* snd )
{
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE )
   {
      // sound is not playing, set flag and bail
      snd->SetState( Sound::MIN_DIST );
      return;
   }

   ALfloat  min_dist(static_cast<ALfloat>(snd->GetMinDistance()));
   ALfloat  max_dist(static_cast<ALfloat>(snd->GetMaxDistance()));
   assert( min_dist <= max_dist );

   ALenum   err(alGetError());
   alSourcef( src, AL_MAX_DISTANCE, max_dist );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: alSourcef(AL_MAX_DISTANCE) error %d", err );
      return;
   }
}



void
AudioManager::SetRolloff( SoundObj* snd )
{
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE )
   {
      // sound is not playing, set flag and bail
      snd->SetState( Sound::ROL_FACT );
      return;
   }

   ALenum   err(alGetError());
   alSourcef( src, AL_ROLLOFF_FACTOR, static_cast<ALfloat>(snd->GetRolloffFactor()) );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: alSourcef(AL_ROLLOFF_FACTOR) error %d", err );
      return;
   }
}



void
AudioManager::SetMinimumGain( SoundObj* snd )
{
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE )
   {
      // sound is not playing, set flag and bail
      snd->SetState( Sound::MIN_GAIN );
      return;
   }

   ALfloat  min_gain(static_cast<ALfloat>(snd->GetMinGain()));
   ALfloat  max_gain(static_cast<ALfloat>(snd->GetMaxGain()));
   assert( min_gain <= max_gain );

   ALenum   err(alGetError());
   alSourcef( src, AL_MIN_GAIN, min_gain );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: alSourcef(AL_MIN_GAIN) error %d", err );
      return;
   }
}



void
AudioManager::SetMaximumGain( SoundObj* snd )
{
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE )
   {
      // sound is not playing, set flag and bail
      snd->SetState( Sound::MAX_GAIN );
      return;
   }

   ALfloat  min_gain(static_cast<ALfloat>(snd->GetMinGain()));
   ALfloat  max_gain(static_cast<ALfloat>(snd->GetMaxGain()));
   assert( min_gain <= max_gain );

   ALenum   err(alGetError());
   alSourcef( src, AL_MAX_GAIN, max_gain );
   if( ( err = alGetError() ) != AL_NO_ERROR )
   {
      dtCore::Notify( dtCore::WARN, "AudioManager: alSourcef(AL_MAX_GAIN) error %d", err );
      return;
   }
}



bool
AudioManager::GetSource( SoundObj* snd )
{
   ALuint   src(0L);

   assert( snd );

   if( mAvailable.size() )
   {
      src   = mAvailable.front();
      mAvailable.pop();
   }

   snd->Source( src );

   if( alIsSource( src ) == AL_FALSE )
      return   false;

   mSourceMap[src]   = snd;
   return   true;
}



void
AudioManager::FreeSource( SoundObj* snd )
{
   if( snd == NULL )
      return;

   ALuint   src(snd->Source());
   snd->Source( 0L );

   if( alIsSource( src ) == AL_FALSE )
      return;

   alSourceStop( src );
   alSourceRewind( src );
   alSourcei( src, AL_LOOPING, AL_FALSE );
   alSourcei( src, AL_BUFFER, AL_NONE );
   mSourceMap[src]   = NULL;
   mAvailable.push( src );
}



// public member functions
// default consructor
AudioManager::SoundObj::SoundObj()
:  Sound(),
   mBuffer(0L),
   mSource(0L),
   mState(BIT(STOP))
{
   RegisterInstance( this );
}



// desructor
AudioManager::SoundObj::~SoundObj()
{
   DeregisterInstance( this );

   Clear();
}



void
AudioManager::SoundObj::OnMessage( MessageData* data )
{
   assert( data );

   Sound::OnMessage( data );

   if( data->message == "frame" )
   {
      if( alIsSource( mSource ) == AL_FALSE )
         // no source, don't bother with positions or direction
         return;

      if( ! IsListenerRelative() )
         // not relative, don't care about position or direction
         return;

      dtCore::Transform transform;
      sgMat4            matrix;
      sgVec3            pos   = { 0.0f, 0.0f, 0.0f };
      sgVec3            dir   = { 0.0f, 1.0f, 0.0f };

      GetTransform( &transform );
      transform.GetTranslation( pos );
      transform.Get( matrix );
      sgXformVec3( dir, matrix );

      SetPosition( pos );
      SetDirection( dir );

      return;
   }


   if( data->userData != this )
      return;

   if( data->message == kCommand[PLAY] )
   {
      SetState( PLAY );
      ResetState( PAUSE );
      ResetState( STOP );

      if( mPlayCB )
      {
         mPlayCB( static_cast<Sound*>(this), mPlayCBData );
      }
      return;
   }

   if( data->message == kCommand[PAUSE] )
   {
      ResetState( PLAY );
      SetState( PAUSE );
      ResetState( STOP );
      return;
   }

   if( data->message == kCommand[STOP] )
   {
      ResetState( PLAY );
      ResetState( PAUSE );
      SetState( STOP );

      if( mStopCB )
      {
         mStopCB( static_cast<Sound*>(this), mStopCBData );
      }
      return;
   }

   if( data->message == kCommand[LOOP] )
   {
      SetState( LOOP );
      return;
   }

   if( data->message == kCommand[UNLOOP] )
   {
      ResetState( LOOP );
      return;
   }

   if( data->message == kCommand[REL] )
   {
      SetState( Sound::POSITION );
      return;
   }

   if( data->message == kCommand[ABS] )
   {
      ResetState( Sound::POSITION );
      return;
   }
}



void
AudioManager::SoundObj::SetParent( dtCore::Transformable* parent )
{
   ListenerRelative( bool(parent) );
   dtCore::Transformable::SetParent( parent );

   if( parent )
   {
      dtCore::Transform transform( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
      dtCore::Transformable::SetTransform( &transform, dtCore::Transformable::REL_CS );
   }
}



bool
AudioManager::SoundObj::IsPlaying( void ) const
{
   return   GetState( PLAY );
}



bool
AudioManager::SoundObj::IsPaused( void ) const
{
   return    GetState( PAUSE );
}



bool
AudioManager::SoundObj::IsStopped( void ) const
{
   return    GetState( STOP );
}



bool
AudioManager::SoundObj::IsLooping( void ) const
{
   return    GetState( LOOP );
}



bool
AudioManager::SoundObj::IsListenerRelative( void ) const
{
   return    GetState( Sound::POSITION );
}



void
AudioManager::SoundObj::Command( const char* cmd )
{
   if( cmd == NULL )
      return;

   mCommand.push( cmd );
}



const char*
AudioManager::SoundObj::Command( void )
{
   if( mCommand.empty() )
      return   NULL;

   const char* cmd   = mCommand.front();
   mCommand.pop();

   return   cmd;
}



void
AudioManager::SoundObj::Buffer( ALuint buffer )
{
   mBuffer  = buffer;
}



ALuint
AudioManager::SoundObj::Buffer( void )
{
   return   mBuffer;
}



void
AudioManager::SoundObj::Source( ALuint source )
{
   mSource  = source;
}



ALuint
AudioManager::SoundObj::Source( void )
{
   return   mSource;
}



void
AudioManager::SoundObj::SetState( unsigned int flag )
{
   mState   |= BIT( flag );
}



void
AudioManager::SoundObj::ResetState( unsigned int flag )
{
   mState   &= ~BIT( flag );
}



bool
AudioManager::SoundObj::GetState( unsigned int flag ) const
{
   return   static_cast<bool>(mState & BIT( flag ));
}



void
AudioManager::SoundObj::Clear( void )
{
   mFilename   = "";
   mBuffer     = 0L;
   mSource     = 0L;
   while( mCommand.size() )
   {
      mCommand.pop();
   }

   mState      = BIT(STOP);

   mPlayCB     = NULL;
   mPlayCBData = NULL;
   mStopCB     = NULL;
   mStopCBData = NULL;

   if( mParent.get() )
   {
      mParent->RemoveChild( this );
   }
}



// public member functions
// default consructor
AudioManager::ListenerObj::ListenerObj()
:  Listener()
{
   RegisterInstance( this );

   Clear();
   AddSender( dtCore::System::GetSystem() );
}



AudioManager::ListenerObj::~ListenerObj()
{
   DeregisterInstance( this );

   RemoveSender( dtCore::System::GetSystem() );
   Clear();
}



void
AudioManager::ListenerObj::SetVelocity( const sgVec3& velocity )
{
   mVelo[0L]   = static_cast<ALfloat>(velocity[0L]);
   mVelo[1L]   = static_cast<ALfloat>(velocity[1L]);
   mVelo[2L]   = static_cast<ALfloat>(velocity[2L]);
}



void
AudioManager::ListenerObj::GetVelocity( sgVec3& velocity )  const
{
   velocity[0L]   = static_cast<SGfloat>(mVelo[0L]);
   velocity[1L]   = static_cast<SGfloat>(mVelo[1L]);
   velocity[2L]   = static_cast<SGfloat>(mVelo[2L]);
}



void
AudioManager::ListenerObj::SetGain( float gain )
{
   // force gain to range from zero to one
   mGain = static_cast<ALfloat>(CLAMP( gain, 0.0f, 1.0f ));
}



float
AudioManager::ListenerObj::GetGain( void )   const
{
   return   static_cast<float>(mGain);
}



void
AudioManager::ListenerObj::OnMessage( MessageData* data )
{
   assert( data );

   if( data->message == "frame" )
   {
      dtCore::Transform transform;
      sgMat4            matrix;
      ALfloat           pos[3L]  = { 0.0f, 0.0f, 0.0f };

      union orient
      {
         ALfloat     ort[6L];

         struct
         {  
            ALfloat  at[3L];  
            ALfloat  up[3L];  
         };
      } orient   = { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };

      GetTransform( &transform );
      transform.GetTranslation( pos );

      transform.Get( matrix );
      sgXformVec3( orient.at, matrix );
      sgXformVec3( orient.up, matrix );

      alListenerfv( AL_POSITION, pos );
      alListenerfv( AL_ORIENTATION, orient.ort );

      alListenerfv( AL_VELOCITY, mVelo );
      alListenerf( AL_GAIN, mGain );
   }
}



void
AudioManager::ListenerObj::SetParent( dtCore::Transformable* parent )
{
   dtCore::Transformable::SetParent( parent );
}



void
AudioManager::ListenerObj::Clear( void )
{
   Transformable* parent(GetParent());
   if( parent )
   {
      parent->RemoveChild( this );
   }

   union
   {
      ALfloat     ort[6L];
      struct
      {
         ALfloat  at[3L];
         ALfloat  up[3L];
      };
   }  orient   = { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };

   ALfloat  pos[3L]  = { 0.0f, 0.0f, 0.0f };

   mGain       = 1.0f;
   mVelo[0L]   =
   mVelo[1L]   =
   mVelo[2L]   = 0.0f;

   alListenerf( AL_GAIN, mGain );
   alListenerfv( AL_VELOCITY, mVelo );
   alListenerfv( AL_POSITION, pos );
   alListenerfv( AL_ORIENTATION, orient.ort );
}
