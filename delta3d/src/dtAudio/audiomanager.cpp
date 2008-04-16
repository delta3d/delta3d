 #include <cassert>
#include <stack>

#include <osg/Vec3>
#include <osg/io_utils>

#ifdef __APPLE__
  #include <OpenAL/alut.h>
#else
  #include <AL/alut.h>
#endif

#include <dtAudio/audiomanager.h>
#include <dtCore/system.h>
#include <dtCore/camera.h>
#include <dtCore/globals.h>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/stringutils.h>

#if   defined(WIN32) | defined(_WIN32)
#pragma warning( disable : 4800 )
#endif

// definitions
#if   !  defined(BIT)
#define  BIT(a)      (1L<<a)
#endif

// name spaces
using namespace   dtAudio;
using namespace   dtUtil;

AudioManager::MOB_ptr   AudioManager::_Mgr(NULL);
AudioManager::LOB_PTR   AudioManager::_Mic(NULL);
const char*             AudioManager::_EaxVer   = "EAX2.0";
const char*             AudioManager::_EaxSet   = "EAXSet";
const char*             AudioManager::_EaxGet   = "EAXGet";
const AudioConfigData   AudioManager::_DefCfg;

const std::string AudioManager::ERROR_CLEARING_STRING = "Clearing Error code "
"system at start of method, if this appears then an error occurred before this "
"method was called.";

IMPLEMENT_MANAGEMENT_LAYER(AudioManager::SoundObj)
IMPLEMENT_MANAGEMENT_LAYER(AudioManager::ListenerObj)
IMPLEMENT_MANAGEMENT_LAYER(AudioManager)

////////////////////////////////////////////////////////////////////////////////
// public member functions
// default consructor
AudioManager::AudioManager( const std::string& name /*= "audiomanager"*/ )
:  Base(name),
   mEAXSet(NULL),
   mEAXGet(NULL),
   mNumSources(0L),
   mSource(NULL)
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

   AddSender( &dtCore::System::GetInstance() );

   //CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   alutInit( 0L, NULL );
   CheckForError("alutInit( 0L, NULL )", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
// destructor
AudioManager::~AudioManager()
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   DeregisterInstance( this );

   try
   {
      // stop all sources
      if(mSource)
      {
         for( ALsizei ii(0); ii < mNumSources; ii++ )
         { 
            unsigned int sourceNum = mSource[ii];
            if(alIsSource( sourceNum ))
            {
               alSourceStop( mSource[ii] );
               // This check was added to prevent a crash-on-exit for OSX -osb
               ALint bufValue;
               alGetSourcei( mSource[ii], AL_BUFFER, &bufValue);
               if (bufValue != 0)
               {
                  alSourcei( mSource[ii], AL_BUFFER, AL_NONE );
               }
            }
         }
         CheckForError("Source stopping and changing buffer to 0", __FUNCTION__, __LINE__);

         // delete the sources
         alDeleteSources( mNumSources, mSource );
         delete   mSource;
         CheckForError("alDeleteSources( mNumSources, mSource );", __FUNCTION__, __LINE__);
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
         CheckForError("alDeleteBuffers( 1L, &bd->buf )", __FUNCTION__, __LINE__);
      }
      mBufferMap.clear();
      mSoundList.clear();

      while( mSoundCommand.size() )
         mSoundCommand.pop();

      while( mSoundRecycle.size() )
         mSoundRecycle.pop();

      alutExit();
      //CheckForError("alutExit()", __FUNCTION__, __LINE__);

      RemoveSender( &dtCore::System::GetInstance() );
   }
   catch(...)
   {
      LOG_ERROR("Caught an exception of unknown type in the destructor of the AudioManager");
   }
}

////////////////////////////////////////////////////////////////////////////////
// create the singleton manager
void AudioManager::Instantiate( void )
{
   if( _Mgr.get() )
      return;

   _Mgr  = new AudioManager;
   assert( _Mgr.get() );

   _Mic  = new ListenerObj;
   assert( _Mic.get() );
}

////////////////////////////////////////////////////////////////////////////////
// destroy the singleton manager
void AudioManager::Destroy( void )
{
   _Mic  = NULL;
   _Mgr  = NULL;
}

////////////////////////////////////////////////////////////////////////////////
// static instance accessor
AudioManager& AudioManager::GetInstance( void )
{
   return   *_Mgr;
}

////////////////////////////////////////////////////////////////////////////////
bool  AudioManager::CheckForError( const std::string& userMessage, 
                                  const std::string& msgFunction,
                                  int lineNumber)
{
   ALint error = 0; 
   if((error = alGetError()) != AL_NO_ERROR)
   {
      std::ostringstream finalStream;
      finalStream << "User Message [" << userMessage << "] " << "OpenALMessage [" << alGetString(error) << "] Line " << lineNumber;
      dtUtil::Log::GetInstance().LogMessage( Log::LOG_WARNING, __FUNCTION__, finalStream.str().c_str());
      return AL_TRUE;
   }
   return AL_FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// static listener accessor
Listener* AudioManager::GetListener( void )
{
   return   static_cast<Listener*>(_Mic.get());
}

////////////////////////////////////////////////////////////////////////////////
// manager configuration
void AudioManager::Config( const AudioConfigData& data /*= _DefCfg*/ )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   if( mSource )
   {
      // already configured
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

   CheckForError("alDistanceModel Changes", __FUNCTION__, __LINE__);

   // set up the sources
   if( ! ConfigSources( data.numSources ) )
      return;

   CheckForError("ConfigSources( data.numSources )", __FUNCTION__, __LINE__);

   // set up EAX
   ConfigEAX( data.eax );
   CheckForError("ConfigEAX( data.eax )", __FUNCTION__, __LINE__);
}


////////////////////////////////////////////////////////////////////////////////
// message receiver
void AudioManager::OnMessage( MessageData* data )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert( data );

   if( data->sender == &dtCore::System::GetInstance() )
   {
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

      if( data->message == "pause" )
      {
         // During a system-wide pause, we want the AudioManager to behave
         // as normal. In many games, there are sounds that occur during
         // during a pause, such as background music or GUI clicks. So
         // here we just call the normal functions all at once.
         PreFrame( *static_cast<const double*>(data->userData) );
         Frame( *static_cast<const double*>(data->userData) );
         PostFrame( *static_cast<const double*>(data->userData) );      
         return;
      }

      if( data->message == "pause_start" )
      {
         mSoundStateMap.clear();
         
         // Pause all sounds that are currently playing, and
         // save their previous state.
         for( SND_LST::iterator iter = mSoundList.begin(); iter != mSoundList.end(); iter++ )
         {
            SoundObj* sob = iter->get();

            if( sob->IsPaused() )
            {
               mSoundStateMap.insert( SoundObjectStateMap::value_type( sob, PAUSED ) );
            }
            else if( sob->IsPlaying() )
            {
               mSoundStateMap.insert( SoundObjectStateMap::value_type( sob, PLAYING ) );
            }
            else if( sob->IsStopped() )
            {
               mSoundStateMap.insert( SoundObjectStateMap::value_type( sob, STOPPED ) );
            }

            PauseSound( sob );
         }
      }

      if( data->message == "pause_end" )
      {
         // Restore all paused sounds to their previous state.
         for( SND_LST::iterator iter = mSoundList.begin(); iter != mSoundList.end(); iter++ )
         {
            SoundObj* sob = iter->get();
            
            switch( mSoundStateMap[ sob ] )
            {
               case PAUSED:
               {
                  PauseSound( sob );
                  break;
               }
               case PLAYING:
               {
                  PlaySound( sob );
                  break;
               }
               case STOPPED:
               {
                  StopSound( sob );
                  break;
               }
               default:
               {
                  break;
               }
            }
         }
      }
   }
   else
   {
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
   CheckForError("Something went wrong in the OnMessage Method", __FUNCTION__, __LINE__);
}


////////////////////////////////////////////////////////////////////////////////
Sound* AudioManager::NewSound( void )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   SOB_PTR  snd(NULL);

   // first look if we can recycle a sound
   if( !mSoundRecycle.empty() )
   {
      snd   = mSoundRecycle.front();
      assert( snd.get() );

      snd->Clear();
      mSoundRecycle.pop();
   }

   // create a new sound object if we don't have one
   if( snd.get() == NULL )
   {
      snd   = new SoundObj();
      assert( snd.get() );
   }

   // listen to messages from this guy
   AddSender( snd.get() );

   // save the sound
   mSoundList.push_back( snd );

   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);

   // hand out the interface to the sound
   return   static_cast<Sound*>(snd.get());
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::FreeSound( Sound*& sound )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   SOB_PTR  snd   = static_cast<SoundObj*>(sound);

   // remove user's copy of pointer
   sound = NULL;

   if( snd.get() == NULL )
      return;

   // remove sound from list
   SND_LST::iterator iter;
   for( iter = mSoundList.begin(); iter != mSoundList.end(); iter++ )
   {
      if( snd != *iter )
         continue;

      mSoundList.erase( iter );
      break;
   }

   // stop listening to this guys messages
   snd->RemoveSender( this );
   snd->RemoveSender( &dtCore::System::GetInstance() );
   RemoveSender( snd.get() );

   // free the sound's source and buffer
   FreeSource( snd.get() );
   UnloadSound( snd.get() );
   snd->Clear();

   // recycle this sound
   mSoundRecycle.push( snd );
}

////////////////////////////////////////////////////////////////////////////////
bool AudioManager::LoadFile( const std::string& file )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   if( file.empty() )
   {
      // no file name, bail...
      return false;
   }

   std::string filename = dtCore::FindFileInPathList(file);
   if( filename.empty() )
   {
      // still no file name, bail...
      Log::GetInstance().LogMessage(Log::LOG_WARNING, __FUNCTION__, "AudioManager: can't load file %s", file.c_str() );
      return false;
   }

   BufferData* bd = mBufferMap[file];
   if( bd != 0 )
   {
      // file already loaded, bail...
      return false;
   }

   bd = new BufferData;

   // Clear the errors
   //ALenum err( alGetError() );

   // create buffer for the wave file
   alGenBuffers( 1L, &bd->buf );
   if(CheckForError("AudioManager: alGenBuffers error", __FUNCTION__, __LINE__))
   {
      delete   bd;
      return   false;
   }

   ALenum format(0);
   ALsizei size(0);
   ALvoid *data = NULL;

   // We are trying to support the new version of ALUT as well as the old intergated
   // version. So we have two cases: DEPRECATED and NON-DEPRECATED.

   // This is not defined in ALUT prior to version 1.
   #ifndef ALUT_API_MAJOR_VERSION 

   // DEPRECATED version for ALUT < 1.0.0

   // Man, are we still in the dark ages here???
   // Copy the std::string to a frickin' ALByte array...
   ALbyte fname[256L];
   unsigned int len = std::min( filename.size(), size_t(255L) );
   memcpy( fname, filename.c_str(), len );
   fname[len] = 0L;
   
   ALsizei freq(0);
   #ifdef __APPLE__
   alutLoadWAVFile( fname, &format, &data, &size, &freq );
   #else
   alutLoadWAVFile( fname, &format, &data, &size, &freq, &bd->loop );
   #endif // __APPLE__
   
   #else

   // NON-DEPRECATED version for ALUT >= 1.0.0
   ALfloat freq(0);
   data = alutLoadMemoryFromFile( filename.c_str(), &format, &size, &freq );
   CheckForError("data = alutLoadMemoryFromFile", __FUNCTION__, __LINE__);

   #endif // ALUT_API_MAJOR_VERSION 

   if( data == 0 )
   {
      #ifndef ALUT_API_MAJOR_VERSION
      Log::GetInstance().LogMessage( Log::LOG_WARNING, __FUNCTION__,
         "AudioManager: alutLoadWAVFile error on %s", file.c_str() );
      #else
         CheckForError("AudioManager: alutLoadMemoryFromFile error", __FUNCTION__, __LINE__);
      #endif // ALUT_API_MAJOR_VERSION 

      alDeleteBuffers( 1L, &bd->buf );
      delete bd;
      CheckForError("alDeleteBuffers error", __FUNCTION__, __LINE__);
      return false;
   }

   alBufferData( bd->buf, format, data, size, ALsizei(freq) );
   if(CheckForError("AudioManager: alBufferData error ", __FUNCTION__, __LINE__))
   {
      alDeleteBuffers( 1L, &bd->buf );
      free(data);

      delete bd;
      return false;
   }

   // The ALUT documentation says you are "free" to free the allocated
   // memory after it has been copied to the OpenAL buffer. See: 
   // http://www.openal.org/openal_webstf/specs/alut.html#alutLoadMemoryFromFile
   // This works fine on Linux, but crashes with a heap error on Windows. 
   // This is probably a Windows implementation bug, so let's just leak a
   // bit in the meantime. Hope you bought your Timberlands...
   // -osb

   #if !defined (WIN32) && !defined (_WIN32) && !defined (__WIN32__)
      free(data);
   #endif

   mBufferMap[file] = bd;
   bd->file = mBufferMap.find(file)->first.c_str();

   return true;
}

////////////////////////////////////////////////////////////////////////////////
bool AudioManager::UnloadFile( const std::string& file )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   if( file.empty() )
   {
      // no file name, bail...
      return false;
   }

   BUF_MAP::iterator iter = mBufferMap.find(file);
   if( iter == mBufferMap.end() )
   {
      // file is not loaded, bail...
      return false;
   }

   BufferData* bd = iter->second;
   if( bd == 0 )
   {
      // bd should never be NULL
      // this code should never run
      mBufferMap.erase(iter);
      return false;
   }

   if( bd->use )
   {
      // buffer still in use, don't remove buffer
      return   false;
   }

   alDeleteBuffers( 1L, &bd->buf );
   delete bd;
   CheckForError("alDeleteBuffers( 1L, &bd->buf );", __FUNCTION__, __LINE__);

   mBufferMap.erase(iter);
   return true;
}



////////////////////////////////////////////////////////////////////////////////
// private member functions
void AudioManager::PreFrame( const double deltaFrameTime )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
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
   }
   CheckForError("PreFrame Error checking", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::Frame( const double deltaFrameTime )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   SRC_LST::iterator             iter;
   std::stack<SRC_LST::iterator> stk;
   ALuint                        src(0L);
   ALint                         state(AL_STOPPED);
   SOB_PTR                       snd(NULL);

   // signal any sources commanded to stop
   while( mStopQueue.size() )
   {
      src   = mStopQueue.front();
      mStopQueue.pop();

      assert( alIsSource( src ) );

      alSourceStop( src );
   }

   CheckForError("Processing stop queue error", __FUNCTION__, __LINE__);

   // push the new sources onto the active list
   while( mPlayQueue.size() )
   {
      src   = mPlayQueue.front();
      mPlayQueue.pop();

      assert( alIsSource( src ) );

      mActiveList.push_back( src );
   }
   CheckForError("alIsSource error", __FUNCTION__, __LINE__);

   // start any new sounds and
   // remove any sounds that have stopped
   for( iter = mActiveList.begin(); iter != mActiveList.end(); iter++ )
   {
      src   = *iter;
      assert( alIsSource( src ) == AL_TRUE );

      alGetSourcei( src, AL_SOURCE_STATE, &state );
      if(CheckForError("AudioManager: alGetSourcei(AL_SOURCE_STATE) error", __FUNCTION__, __LINE__))
         continue;
      
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
      if(CheckForError("AudioManager: alGetSourcei(AL_SOURCE_STATE) error", __FUNCTION__, __LINE__))
         continue;

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

   CheckForError("alSourceRewind", __FUNCTION__, __LINE__);

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

////////////////////////////////////////////////////////////////////////////////
void AudioManager::PostFrame( const double deltaFrameTime )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   SOB_PTR     snd(NULL);
   ALuint      src(0L);

   // for all sounds that have stopped
   while( mStopQueue.size() )
   {
      // free the source for later use
      src   = mStopQueue.front();
      mStopQueue.pop();

      snd   = mSourceMap[src];
      if (snd.valid())
      {
         snd->RemoveSender( this );
         snd->RemoveSender( &dtCore::System::GetInstance() );

         FreeSource( snd.get() );
      }
   }
   CheckForError("Post Frame Error", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::Pause( const double deltaFrameTime )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   SRC_LST::iterator             iter;
   std::stack<SRC_LST::iterator> stk;
   ALuint                        src(0L);
   ALint                         state(AL_STOPPED);
   SOB_PTR                       snd(NULL);

   // signal any sources commanded to stop
   while( mStopQueue.size() )
   {
      src   = mStopQueue.front();
      mStopQueue.pop();

      assert( alIsSource( src ) );

      alSourceStop( src );
   }

   CheckForError("Source stop error", __FUNCTION__, __LINE__);

   // start any new sounds and
   // remove any sounds that have stopped
   for( iter = mActiveList.begin(); iter != mActiveList.end(); iter++ )
   {
      src   = *iter;
      assert( alIsSource( src ) == AL_TRUE );

      alGetSourcei( src, AL_SOURCE_STATE, &state );
      
      if(CheckForError("AudioManager: alGetSourcei(AL_SOURCE_STATE) error", __FUNCTION__, __LINE__))
         continue;

      switch( state )
      {
         case  AL_PLAYING:
         case  AL_PAUSED:
            // don't need to do anything
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
      if(CheckForError("AudioManager: alGetSourcei(AL_SOURCE_STATE) error", __FUNCTION__, __LINE__))
         continue;

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

   CheckForError("alSourceRewind error", __FUNCTION__, __LINE__);
      
   LOGN_ALWAYS("audiomanager.cpp", "paused5");
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

////////////////////////////////////////////////////////////////////////////////
bool AudioManager::Configured( void ) const
{
   return   mSource != NULL;
}

////////////////////////////////////////////////////////////////////////////////
bool AudioManager::ConfigSources( unsigned int num )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   if( num == 0L )
      return   false;

   mNumSources = ALsizei(num);

   mSource  = new ALuint[mNumSources];
   if (mSource == NULL)
   {
      LOG_ERROR("Failed allocating " + dtUtil::ToString(mNumSources) + " sources.");
      return false;
   }

   ALenum   error(alGetError());
   alGenSources( mNumSources, mSource );
   if(CheckForError("AudioManager: alGenSources Error", __FUNCTION__, __LINE__))
   {
      if( mSource )
      {
         delete   mSource;
         mSource  = NULL;
      }
      return   false;
   }

   for( ALsizei ii(0); ii < mNumSources; ii++ )
   {
      if (alIsSource( mSource[ii] ) == AL_FALSE)
      {
         LOG_ERROR("Source " + dtUtil::ToString(ii) + " is not valid.");
         return false;
      }
      mAvailable.push( mSource[ii] );
   }

   return   true;
}



////////////////////////////////////////////////////////////////////////////////
bool AudioManager::ConfigEAX( bool eax )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   if( !eax )
   {
      return false;
   }

   #ifndef AL_VERSION_1_1
   ALubyte buf[32L];
   memset( buf, 0L, 32L );
   memcpy( buf, _EaxVer, std::min( strlen(_EaxVer), size_t(32L) ) );
   #else
   const ALchar* buf = _EaxVer;
   #endif

   // check for EAX support
   if( alIsExtensionPresent(buf) == AL_FALSE )
   {
      Log::GetInstance().LogMessage( Log::LOG_WARNING, __FUNCTION__,
         "AudioManager: %s is not available", _EaxVer );
      return   false;
   }

   #ifndef AL_VERSION_1_1
   memset( buf, 0L, 32L );
   memcpy( buf, _EaxSet, std::min( strlen(_EaxSet), size_t(32L) ) );
   #else
   buf = _EaxSet;
   #endif
   
   // get the eax-set function
   mEAXSet = alGetProcAddress(buf);
   if( mEAXSet == 0 )
   {
      Log::GetInstance().LogMessage( Log::LOG_WARNING, __FUNCTION__,
         "AudioManager: %s is not available", _EaxVer );
      return   false;
   }
   
   #ifndef AL_VERSION_1_1
   memset( buf, 0, 32 );
   memcpy( buf, _EaxGet, std::min( strlen(_EaxGet), size_t(32) ) );
   #else
   buf = _EaxVer;
   #endif
   
   // get the eax-get function
   mEAXGet = alGetProcAddress(buf);
   if( mEAXGet == 0 )
   {
      Log::GetInstance().LogMessage( Log::LOG_WARNING, __FUNCTION__,
         "AudioManager: %s is not available", _EaxVer );
      mEAXSet = 0;
      return false;
   }

   CheckForError("Config eax issue", __FUNCTION__, __LINE__);

   return true;
}



////////////////////////////////////////////////////////////////////////////////
void AudioManager::LoadSound( SoundObj* snd )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert( snd );

   const char* file(static_cast<Sound*>(snd)->GetFilename());

   if( file == NULL )
      return;

   LoadFile( file );

   BufferData* bd = mBufferMap[file];

   if( bd == NULL )
      return;

   bd->use++;
   snd->Buffer( bd->buf );
   CheckForError("LoadSound Error", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::UnloadSound( SoundObj* snd )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert( snd );

   const char* file(static_cast<Sound*>(snd)->GetFilename());

   if( file == NULL )
      return;

   snd->Buffer(0);

   BufferData* bd = mBufferMap[file];

   if( bd == NULL )
      return;

   bd->use--;

   UnloadFile( file );
   CheckForError("Unload Sound Error", __FUNCTION__, __LINE__);
}



////////////////////////////////////////////////////////////////////////////////
void AudioManager::PlaySound( SoundObj* snd )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   ALuint   buf(0);
   ALuint   src(0);
   bool source_is_new = false;

   assert( snd );

   // first check if sound has a buffer
   buf   = snd->Buffer();
   if( alIsBuffer( buf ) == AL_FALSE )
   {
      CheckForError("alIsBuffer error", __FUNCTION__, __LINE__);
      // no buffer, bail
      return;
   }

   // then check if sound has a source
   src = snd->Source();
   if( ! snd->IsInitialized() )
   {
      // no source, gotta get one
      if( ! GetSource( snd ) )
      {
         // no source available
         Log::GetInstance().LogMessage( Log::LOG_ERROR, __FUNCTION__, 
            "AudioManager: play attempt w/o available sources" );
         Log::GetInstance().LogMessage( Log::LOG_ERROR, __FUNCTION__,
            "AudioManager: try increasing the number of sources at config time" );
         return;
      }

      src   = snd->Source();
	   source_is_new = true; 
   }
   else
   {
      // already has buffer and source
      // could be paused (or playing)
      ALint state(AL_STOPPED);
      alGetSourcei( src, AL_SOURCE_STATE, &state );
      
      if(CheckForError("AudioManager: alGetSourcei(AL_SOURCE_STATE) error", __FUNCTION__, __LINE__))
         return;

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
   if(CheckForError("AudioManager: alSourcei(AL_BUFFER) error", __FUNCTION__, __LINE__))
      return;

   // set looping flag
   alSourcei( src, AL_LOOPING, (snd->IsLooping())? AL_TRUE: AL_FALSE );
   CheckForError("AudioManager: alSourcei(AL_LOOPING) error", __FUNCTION__, __LINE__);

   // set source relative flag
   if( snd->IsListenerRelative() )
   {
      // is listener relative
      alSourcei( src, AL_SOURCE_RELATIVE, AL_FALSE );
      CheckForError("AudioManager: alSourcei(AL_SOURCE_RELATIVE) error", __FUNCTION__, __LINE__);

      // set initial position and direction
      osg::Vec3   pos;
      osg::Vec3   dir;

      snd->GetPosition( pos );
      snd->GetDirection( dir );

      alSource3f( src,
                  AL_POSITION,
                  static_cast<ALfloat>(pos[0]),
                  static_cast<ALfloat>(pos[1]),
                  static_cast<ALfloat>(pos[2]) );

      CheckForError("AudioManager: alSource3f(AL_POSITION) error", __FUNCTION__, __LINE__);

      alSource3f( src,
         AL_DIRECTION,
         static_cast<ALfloat>(dir[0]),
         static_cast<ALfloat>(dir[1]),
         static_cast<ALfloat>(dir[2]) );
      CheckForError("AudioManager: alSource3f(AL_DIRECTION) error", __FUNCTION__, __LINE__);
   }
   else
   {
      // not listener relative
      alSourcei( src, AL_SOURCE_RELATIVE, AL_FALSE );
      CheckForError("AudioManager: alSourcei(AL_SOURCE_RELATIVE) error", __FUNCTION__, __LINE__);
   }

   // set gain
   alSourcef( src, AL_GAIN, static_cast<ALfloat>(snd->GetGain()) );
   CheckForError("AudioManager: alSourcef(AL_GAIN) error", __FUNCTION__, __LINE__);

   // set pitch
   alSourcef( src, AL_PITCH, static_cast<ALfloat>(snd->GetPitch()) );
   CheckForError("AudioManager: alSourcef(AL_PITCH) error", __FUNCTION__, __LINE__);

   // set reference distance
   if( snd->GetState( Sound::MIN_DIST ) || source_is_new)
   {
      snd->ResetState( Sound::MIN_DIST );
      alSourcef( src, AL_REFERENCE_DISTANCE, static_cast<ALfloat>(snd->GetMinDistance()) );
      CheckForError("AudioManager: alSourcef(AL_REFERENCE_DISTANCE) error", __FUNCTION__, __LINE__);
   }

   // set maximum distance
   if( snd->GetState( Sound::MAX_DIST ) || source_is_new)
   {
      snd->ResetState( Sound::MAX_DIST );
      alSourcef( src, AL_MAX_DISTANCE, static_cast<ALfloat>(snd->GetMaxDistance()) );
      CheckForError("AudioManager: alSourcef(AL_MAX_DISTANCE) error", __FUNCTION__, __LINE__);
   }

   // set rolloff factor
   if( snd->GetState( Sound::ROL_FACT ) || source_is_new)
   {
      snd->ResetState( Sound::ROL_FACT );
      alSourcef( src, AL_ROLLOFF_FACTOR, static_cast<ALfloat>(snd->GetRolloffFactor()) );
      CheckForError("AudioManager: alSourcef(AL_ROLLOFF_FACTOR) error", __FUNCTION__, __LINE__);
   }

   // set minimum gain
   if( snd->GetState( Sound::MIN_GAIN ) || source_is_new)
   {
      snd->ResetState( Sound::MIN_GAIN );
      alSourcef( src, AL_MIN_GAIN, static_cast<ALfloat>(snd->GetMinGain()) );
      CheckForError("AudioManager: alSourcef(AL_MIN_GAIN) error", __FUNCTION__, __LINE__);
   }

   // set maximum gain
   if( snd->GetState( Sound::MAX_GAIN ) || source_is_new)
   {
      snd->ResetState( Sound::MAX_GAIN );
      alSourcef( src, AL_MAX_GAIN, static_cast<ALfloat>(snd->GetMaxGain()) );
      CheckForError("AudioManager: alSourcef(AL_MAX_GAIN) error", __FUNCTION__, __LINE__);
   }

   if (source_is_new)
   {
      snd->AddSender( this );
      snd->AddSender( &dtCore::System::GetInstance() );
      mPlayQueue.push( snd->Source() );
   }
}



////////////////////////////////////////////////////////////////////////////////
void AudioManager::PauseSound( SoundObj* snd )
{
   assert( snd );

   if( !snd->IsInitialized() )
   {
      // sound is not playing, bail
      return;
   }

   mPauseQueue.push( snd->Source() );
}



////////////////////////////////////////////////////////////////////////////////
void AudioManager::StopSound( SoundObj* snd )
{
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE || !snd->IsInitialized() )
   {
      // sound is not playing, bail
      return;
   }

   mStopQueue.push( src );
}



////////////////////////////////////////////////////////////////////////////////
void AudioManager::RewindSound( SoundObj* snd )
{
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE || !snd->IsInitialized() )
   {
      // sound is not playing, bail
      return;
   }

   mRewindQueue.push( src );
}



////////////////////////////////////////////////////////////////////////////////
void AudioManager::SetLoop( SoundObj* snd )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE  || !snd->IsInitialized() )
   {
      // sound is not playing
      // set flag and bail
      snd->SetState( Sound::LOOP );
      return;
   }

   
   alSourcei( src, AL_LOOPING, AL_TRUE );
   if(CheckForError("AudioManager: alSourcei(AL_LOOP) error", __FUNCTION__, __LINE__))
      return;

   SendMessage( Sound::kCommand[Sound::LOOP], snd );
}



////////////////////////////////////////////////////////////////////////////////
void AudioManager::ResetLoop( SoundObj* snd )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE || !snd->IsInitialized() )
   {
      // sound is not playing
      // set flag and bail
      snd->ResetState( Sound::LOOP );
      return;
   }

   
   alSourcei( src, AL_LOOPING, AL_FALSE );
   if(CheckForError("AudioManager: alSourcei(AL_LOOP) error %X", __FUNCTION__, __LINE__))
      return;

   SendMessage( Sound::kCommand[Sound::UNLOOP], snd );
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::SetRelative( SoundObj* snd )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
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
         Log::GetInstance().LogMessage(Log::LOG_INFO, __FUNCTION__, 
            "AudioManager: A stereo Sound can't be positioned in 3D space");
         snd->ResetState( Sound::POSITION );
         return;
      }
   }

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE || !snd->IsInitialized() )
   {
      // sound is not playing
      // set flag and bail
      snd->SetState( Sound::POSITION );
      return;
   }

   CheckForError("alGetBufferi && alIsSource calls check", __FUNCTION__, __LINE__);
   alSourcei( src, AL_SOURCE_RELATIVE, AL_FALSE );
   if(CheckForError("AudioManager: alSourcei(AL_SOURCE_RELATIVE) error", __FUNCTION__, __LINE__))
      return;

   SendMessage( Sound::kCommand[Sound::REL], snd );
}



////////////////////////////////////////////////////////////////////////////////
void AudioManager::SetAbsolute( SoundObj* snd )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE || !snd->IsInitialized() )
   {
      // sound is not playing
      // set flag and bail
      snd->ResetState( Sound::POSITION );
      return;
   }

   
   alSourcei( src, AL_SOURCE_RELATIVE, AL_FALSE );
   if(CheckForError("AudioManager: alSourcei(AL_SOURCE_RELATIVE) error", __FUNCTION__, __LINE__))
      return;

   SendMessage( Sound::kCommand[Sound::ABS], snd );
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::SetGain( SoundObj* snd )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE || !snd->IsInitialized() )
   {
      // sound is not playing, bail
      return;
   }

   
   alSourcef( src, AL_GAIN, static_cast<ALfloat>(snd->GetGain()) );
   if(CheckForError("AudioManager: alSourcef(AL_GAIN) error", __FUNCTION__, __LINE__))
      return;

   SendMessage( Sound::kCommand[Sound::GAIN], snd );
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::SetPitch( SoundObj* snd )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE || !snd->IsInitialized() )
   {
      // sound is not playing, bail
      return;
   }

   
   alSourcef( src, AL_PITCH, static_cast<ALfloat>(snd->GetPitch()) );
   if(CheckForError("AudioManager: alSourcef(AL_PITCH) error", __FUNCTION__, __LINE__))
      return;

   SendMessage( Sound::kCommand[Sound::PITCH], snd );
}



////////////////////////////////////////////////////////////////////////////////
void AudioManager::SetPosition( SoundObj* snd )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE || !snd->IsInitialized() )
   {
      // sound is not playing, bail
      return;
   }

   osg::Vec3   pos;
   snd->GetPosition( pos );

   
   alSource3f( src, AL_POSITION, static_cast<ALfloat>(pos[0]), static_cast<ALfloat>(pos[1]), static_cast<ALfloat>(pos[2]) );
   if(CheckForError("AudioManager: alSource3f(AL_POSITION) error", __FUNCTION__, __LINE__))
      return;

   SendMessage( Sound::kCommand[Sound::POSITION], snd );
}



////////////////////////////////////////////////////////////////////////////////
void AudioManager::SetDirection( SoundObj* snd )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE || !snd->IsInitialized() )
   {
      // sound is not playing, bail
      return;
   }

   osg::Vec3   dir;
   snd->GetDirection( dir );

   alSource3f( src, AL_DIRECTION, static_cast<ALfloat>(dir[0]), static_cast<ALfloat>(dir[1]), static_cast<ALfloat>(dir[2]) );
   if(CheckForError("AudioManager: alSource3f(AL_DIRECTION) error", __FUNCTION__, __LINE__))
      return;

   SendMessage( Sound::kCommand[Sound::DIRECTION], snd );
}



////////////////////////////////////////////////////////////////////////////////
void AudioManager::SetVelocity( SoundObj* snd )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE || !snd->IsInitialized() )
   {
      // sound is not playing, bail
      return;
   }

   osg::Vec3   velo;
   snd->GetVelocity( velo );


   alSource3f( src, AL_VELOCITY, static_cast<ALfloat>(velo[0]), static_cast<ALfloat>(velo[1]), static_cast<ALfloat>(velo[2]) );
   if(CheckForError("AudioManager: alSource3f(AL_VELOCITY) error", __FUNCTION__, __LINE__))
      return;

   SendMessage( Sound::kCommand[Sound::VELOCITY], snd );
}



////////////////////////////////////////////////////////////////////////////////
void AudioManager::SetReferenceDistance( SoundObj* snd )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE || !snd->IsInitialized() )
   {
      // sound is not playing, set flag and bail
      snd->SetState( Sound::MIN_DIST );
      return;
   }

   ALfloat  min_dist(static_cast<ALfloat>(snd->GetMinDistance()));
   ALfloat  max_dist(static_cast<ALfloat>(snd->GetMaxDistance()));
   min_dist = min_dist; //no-op to prevent warnings
   max_dist = max_dist; //no-op to prevent warnings
   assert( min_dist <= max_dist );

   alSourcef( src, AL_REFERENCE_DISTANCE, min_dist );
   if(CheckForError("AudioManager: alSourcef(AL_REFERENCE_DISTANCE) error", __FUNCTION__, __LINE__))
      return;
}



////////////////////////////////////////////////////////////////////////////////
void AudioManager::SetMaximumDistance( SoundObj* snd )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE || !snd->IsInitialized() )
   {
      // sound is not playing, set flag and bail
      snd->SetState( Sound::MIN_DIST );
      return;
   }

   ALfloat  min_dist(static_cast<ALfloat>(snd->GetMinDistance()));
   ALfloat  max_dist(static_cast<ALfloat>(snd->GetMaxDistance()));
   min_dist = min_dist; //no-op to prevent warnings
   max_dist = max_dist; //no-op to prevent warnings
   assert( min_dist <= max_dist );

   
   alSourcef( src, AL_MAX_DISTANCE, max_dist );
   if(CheckForError("AudioManager: alSourcef(AL_MAX_DISTANCE) error", __FUNCTION__, __LINE__))
      return;
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::SetRolloff( SoundObj* snd )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE || !snd->IsInitialized() )
   {
      // sound is not playing, set flag and bail
      snd->SetState( Sound::ROL_FACT );
      return;
   }

   
   alSourcef( src, AL_ROLLOFF_FACTOR, static_cast<ALfloat>(snd->GetRolloffFactor()) );
   if(CheckForError("AudioManager: alSourcef(AL_ROLLOFF_FACTOR) error", __FUNCTION__, __LINE__))
      return;
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::SetMinimumGain( SoundObj* snd )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE || !snd->IsInitialized() )
   {
      // sound is not playing, set flag and bail
      snd->SetState( Sound::MIN_GAIN );
      return;
   }

   ALfloat  min_gain(static_cast<ALfloat>(snd->GetMinGain()));

   if( min_gain <= snd->GetMaxGain() )
   {
      
      alSourcef( src, AL_MIN_GAIN, min_gain );
      if(CheckForError("AudioManager: alSourcef(AL_MIN_GAIN) error", __FUNCTION__, __LINE__))
         return;
   }
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::SetMaximumGain( SoundObj* snd )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert( snd );

   ALuint   src   = snd->Source();
   if( alIsSource( src ) == AL_FALSE || !snd->IsInitialized() )
   {
      // sound is not playing, set flag and bail
      snd->SetState( Sound::MAX_GAIN );
      return;
   }

   ALfloat  max_gain(static_cast<ALfloat>(snd->GetMaxGain()));
   
   if( snd->GetMinGain() <= max_gain )
   {
      
      alSourcef( src, AL_MAX_GAIN, max_gain );
      if(CheckForError("AudioManager: alSourcef(AL_MAX_GAIN) error ", __FUNCTION__, __LINE__))
         return;
   }
}



////////////////////////////////////////////////////////////////////////////////
bool AudioManager::GetSource( SoundObj* snd )
{
   ALuint   src(0L);

   assert( snd );

   if( mAvailable.size() )
   {
      src   = mAvailable.front();
      mAvailable.pop();
   }

   snd->Source( src );
   snd->SetInitialized( true );

   if( alIsSource( src ) == AL_FALSE || !snd->IsInitialized() )
      return false;

   mSourceMap[src]   = snd;
   return true;
}



////////////////////////////////////////////////////////////////////////////////
void AudioManager::FreeSource( SoundObj* snd )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   if( snd == NULL )
      return;

   ALuint   src(snd->Source());
   
   // This code was previously: snd->Source( 0L );
   // However, since 0L is a valid source that OpenAL can assign,
   // we cannot enforce that a source of 0 means we "have no source".
   // Instead, we reset it's initialized state.
   snd->SetInitialized( false );
   
   if( alIsSource( src ) == AL_FALSE )
      return;

   alSourceStop( src );
   alSourceRewind( src );
   alSourcei( src, AL_LOOPING, AL_FALSE );
   alSourcei( src, AL_BUFFER, AL_NONE );
   mSourceMap[src]   = NULL;
   mAvailable.push( src );
   CheckForError("FreeSource", __FUNCTION__, __LINE__);
}



////////////////////////////////////////////////////////////////////////////////
// public member functions
// default consructor
AudioManager::SoundObj::SoundObj()
:  Sound(),
   mBuffer(0L),
   mSource( SourceObj() ),
   mState(BIT(STOP))
{
   RegisterInstance( this );
}

////////////////////////////////////////////////////////////////////////////////
// desructor
AudioManager::SoundObj::~SoundObj()
{
   DeregisterInstance( this );

   Clear();
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::SoundObj::OnMessage( MessageData* data )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert( data );

   Sound::OnMessage( data );

   if( data->message == "frame" )
   {
      if( alIsSource( Source() ) == AL_FALSE || !IsInitialized() )
         // no source, don't bother with positions or direction
         return;

      if( ! IsListenerRelative() )
         // not relative, don't care about position or direction
         return;

      dtCore::Transform transform;
      osg::Matrix       matrix;
      osg::Vec3            pos   ( 0.0f, 0.0f, 0.0f );
      osg::Vec3            dir   ( 0.0f, 1.0f, 0.0f );

      GetTransform( transform );
      transform.GetTranslation( pos );
      transform.Get( matrix );
      dir = osg::Matrix::transform3x3(dir, matrix);

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
   CheckForError("End of OnMessage", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::SoundObj::SetParent( dtCore::DeltaDrawable* parent )
{
   ListenerRelative( parent != NULL );
   dtCore::Transformable::SetParent( parent );

   if( parent )
   {
      dtCore::Transform transform;
      SetTransform( transform, dtCore::Transformable::REL_CS );
   }
}



////////////////////////////////////////////////////////////////////////////////
bool AudioManager::SoundObj::IsPlaying( void ) const
{
   return   GetState( PLAY );
}

////////////////////////////////////////////////////////////////////////////////
bool AudioManager::SoundObj::IsPaused( void ) const
{
   return    GetState( PAUSE );
}

////////////////////////////////////////////////////////////////////////////////
bool AudioManager::SoundObj::IsStopped( void ) const
{
   return    GetState( STOP );
}

////////////////////////////////////////////////////////////////////////////////
bool AudioManager::SoundObj::IsLooping( void ) const
{
   return    GetState( LOOP );
}

////////////////////////////////////////////////////////////////////////////////
bool AudioManager::SoundObj::IsListenerRelative( void ) const
{
   return    GetState( Sound::POSITION );
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::SoundObj::Command( const char* cmd )
{
   if( cmd == NULL )
      return;

   mCommand.push( cmd );
}

////////////////////////////////////////////////////////////////////////////////
const char* AudioManager::SoundObj::Command( void )
{
   if( mCommand.empty() )
      return   NULL;

   const char* cmd   = mCommand.front();
   mCommand.pop();

   return   cmd;
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::SoundObj::Buffer( ALuint buffer )
{
   mBuffer  = buffer;
}

////////////////////////////////////////////////////////////////////////////////
ALuint AudioManager::SoundObj::Buffer( void )
{
   return   mBuffer;
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::SoundObj::Source( ALuint source )
{
   mSource.mSource = source;
}

////////////////////////////////////////////////////////////////////////////////
ALuint AudioManager::SoundObj::Source( void )
{
   return mSource.mSource;
}

////////////////////////////////////////////////////////////////////////////////
bool AudioManager::SoundObj::IsInitialized() const
{
   return mSource.mInitialized;
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::SoundObj::SetInitialized( bool isInitialized )
{
   mSource.mInitialized = isInitialized;
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::SoundObj::SetState( unsigned int flag )
{
   mState   |= BIT( flag );
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::SoundObj::ResetState( unsigned int flag )
{
   mState   &= ~BIT( flag );
}

////////////////////////////////////////////////////////////////////////////////
bool AudioManager::SoundObj::GetState( unsigned int flag ) const
{
   return   static_cast<bool>(mState & BIT( flag ));
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::SoundObj::Clear( void )
{
   mFilename   = "";
   mBuffer     = 0L;
   mSource     = SourceObj();
   while( mCommand.size() )
   {
      mCommand.pop();
   }

   mState      = BIT(STOP);

   mPlayCB     = NULL;
   mPlayCBData = NULL;
   mStopCB     = NULL;
   mStopCBData = NULL;

   if( GetParent() )
   {
      GetParent()->RemoveChild( this );
   }
}

////////////////////////////////////////////////////////////////////////////////
// public member functions
// default consructor
AudioManager::ListenerObj::ListenerObj()
:  Listener()
{
   RegisterInstance( this );

   Clear();
   AddSender( &dtCore::System::GetInstance() );
}



////////////////////////////////////////////////////////////////////////////////
AudioManager::ListenerObj::~ListenerObj()
{
   DeregisterInstance( this );

   RemoveSender( &dtCore::System::GetInstance() );
   Clear();
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::ListenerObj::SetVelocity( const osg::Vec3& velocity )
{
   mVelo[0L]   = static_cast<ALfloat>(velocity[0L]);
   mVelo[1L]   = static_cast<ALfloat>(velocity[1L]);
   mVelo[2L]   = static_cast<ALfloat>(velocity[2L]);
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::ListenerObj::GetVelocity( osg::Vec3& velocity )  const
{
   velocity[0]   = static_cast<double>(mVelo[0]);
   velocity[1]   = static_cast<double>(mVelo[1]);
   velocity[2]   = static_cast<double>(mVelo[2]);
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::ListenerObj::SetGain( float gain )
{
   // force gain to range from zero to one
   dtUtil::Clamp<float>( gain, 0.0f, 1.0f );
   mGain = static_cast<ALfloat>(gain);
}

////////////////////////////////////////////////////////////////////////////////
float AudioManager::ListenerObj::GetGain( void )   const
{
   return   static_cast<float>(mGain);
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::ListenerObj::OnMessage( MessageData* data )
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert( data );

   if( data->message == "frame" )
   {
      dtCore::Transform transform;
      osg::Matrix       matrix;
      ALfloat           pos[3];

      union orient
      {
         ALfloat     ort[6];

         struct
         {  
            ALfloat  at[3];  
            ALfloat  up[3];  
         };
      } orient;

      GetTransform( transform );
      osg::Vec3 tmp;
      transform.GetTranslation( tmp );
      pos[0] = tmp[0]; 
      pos[1] = tmp[1]; 
      pos[2] = tmp[2];
      transform.Get( matrix );
      
      //assign at and up vectors directly from the matrix
      orient.at[0] = matrix(1,0);
      orient.at[1] = matrix(1,1);
      orient.at[2] = matrix(1,2);
      orient.up[0] = matrix(2,0);
      orient.up[1] = matrix(2,1);
      orient.up[2] = matrix(2,2);

      alListenerfv( AL_POSITION, pos );
      alListenerfv( AL_ORIENTATION, orient.ort );

      alListenerfv( AL_VELOCITY, mVelo );
      alListenerf( AL_GAIN, mGain );
      CheckForError("AL Listener value changing", __FUNCTION__, __LINE__);
   }
}


////////////////////////////////////////////////////////////////////////////////
void AudioManager::ListenerObj::Clear( void )
{
   //dtCore::Transformable* parent(GetParent());
   //if( parent )
   //{
   //   parent->RemoveChild( this );
   //}
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);

   union
   {
      ALfloat     ort[6];
      struct
      {
         ALfloat  at[3];
         ALfloat  up[3];
      };
   }  orient = { 0.0f };
   orient.ort[0] = 0.0f;
   orient.ort[1] = 1.0f;
   orient.ort[2] = 0.0f;
   orient.ort[3] = 0.0f;
   orient.ort[4] = 0.0f;
   orient.ort[5] = 1.0f;

   ALfloat  pos[3L]  = { 0.0f, 0.0f, 0.0f };

   mGain       = 1.0f;
   mVelo[0L]   =
   mVelo[1L]   =
   mVelo[2L]   = 0.0f;

   alListenerf( AL_GAIN, mGain );
   alListenerfv( AL_VELOCITY, mVelo );
   alListenerfv( AL_POSITION, pos );
   alListenerfv( AL_ORIENTATION, orient.ort );
   CheckForError("AL Listener value changing", __FUNCTION__, __LINE__);
}
