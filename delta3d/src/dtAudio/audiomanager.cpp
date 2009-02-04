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
#include <dtAudio/dtaudio.h>
#include <dtCore/system.h>
#include <dtCore/camera.h>
#include <dtCore/globals.h>
#include <dtUtil/stringutils.h>

// definitions
#if !defined(BIT)
# define BIT(a) (1L<<a)
#endif

// name spaces
using namespace dtAudio;
using namespace dtUtil;

AudioManager::MOB_ptr AudioManager::_Mgr(NULL);
AudioManager::LOB_PTR AudioManager::_Mic(NULL);
const char*           AudioManager::_EaxVer = "EAX2.0";
const char*           AudioManager::_EaxSet = "EAXSet";
const char*           AudioManager::_EaxGet = "EAXGet";
const AudioConfigData AudioManager::_DefCfg;

IMPLEMENT_MANAGEMENT_LAYER(AudioManager)

namespace dtAudio {

////////////////////////////////////////////////////////////////////////////////
//Utitily function used to work with OpenAL's error messaging system.  It's used
//in multiple places throughout dtAudio.  It's not in AudioManager because we
//don't want things like Sound to directly access the AudioManager.
// Returns true on error, false if no error
inline bool CheckForError(const std::string& userMessage,
                          const std::string& msgFunction,
                          int lineNumber)
{
   ALenum error = alGetError();
   if (error != AL_NO_ERROR)
   {
      std::ostringstream finalStream;
      finalStream << "User Message: [" << userMessage << "] OpenAL Message: [" << alGetString(error) << "]";
      dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, msgFunction, lineNumber, finalStream.str().c_str());
      return AL_TRUE;
   }
   else
   {
      // check if we have an ALUT error
      ALenum alutError = alutGetError();
      if (alutError != ALUT_ERROR_NO_ERROR)
      {
         std::ostringstream finalStream;
         finalStream << "User Message: [" << userMessage << "] " << "Alut Message: [" << alutGetErrorString(alutError) << "] Line " << lineNumber;
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, msgFunction, lineNumber, finalStream.str().c_str());
         return AL_TRUE;
      }
   }
   return AL_FALSE;
}

} //namespace dtAudio

////////////////////////////////////////////////////////////////////////////////
// public member functions
// default consructor
AudioManager::AudioManager(const std::string& name /*= "audiomanager"*/)
   : Base(name)
   , mEAXSet(NULL)
   , mEAXGet(NULL)
   , mNumSounds(0)
   , mIsConfigured(false)
{
   RegisterInstance(this);

   AddSender(&dtCore::System::GetInstance());

   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   if (alutInit(NULL, NULL) == AL_FALSE)
   {
      std::cout << "Error initializing alut" << std::cout;
      CheckForError("alutInit(NULL, NULL)", __FUNCTION__, __LINE__);
   }
}

////////////////////////////////////////////////////////////////////////////////
// destructor
AudioManager::~AudioManager()
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   DeregisterInstance(this);

   //stop and clear all Sounds
   SND_LST::iterator it;
   for (it = mSoundList.begin(); it != mSoundList.end(); ++it)
   {
      (*it)->StopImmediately();
      (*it)->SetBuffer(AL_NONE);
      (*it)->SetLooping(false);
      (*it)->RewindImmediately();
   }

   // delete the buffers
   BufferData* bd(NULL);
   for (BUF_MAP::iterator iter(mBufferMap.begin()); iter != mBufferMap.end(); iter++)
   {
      if ((bd = iter->second) == NULL)
      {
         continue;
      }

      iter->second = NULL;
      if(alIsBuffer(bd->buf) != AL_FALSE)
      {
         alDeleteBuffers(1, &bd->buf);
      }
      delete bd;
      CheckForError("alDeleteBuffers(1, &bd->buf )", __FUNCTION__, __LINE__);
   }
   mBufferMap.clear();
   mSoundList.clear();

   alutExit();
   CheckForError("alutExit()", __FUNCTION__, __LINE__);

   RemoveSender(&dtCore::System::GetInstance());
}

////////////////////////////////////////////////////////////////////////////////
// create the singleton manager
void AudioManager::Instantiate()
{
   if (_Mgr.get())
   {
      return;
   }

   _Mgr  = new AudioManager;
   assert(_Mgr.get());

   _Mic  = new Listener;
   assert(_Mic.get());
}

////////////////////////////////////////////////////////////////////////////////
// destroy the singleton manager
void AudioManager::Destroy()
{
   _Mic = NULL;
   _Mgr = NULL;
}

////////////////////////////////////////////////////////////////////////////////
// static instance accessor
AudioManager& AudioManager::GetInstance()
{
   return *_Mgr;
}


////////////////////////////////////////////////////////////////////////////////
// static listener accessor
Listener* AudioManager::GetListener()
{
   return static_cast<Listener*>(_Mic.get());
}

////////////////////////////////////////////////////////////////////////////////
DEPRECATE_FUNC bool AudioManager::GetListenerRelative(Sound* sound)
{
   DEPRECATE("AudioManager::GetListenerRelative", "Sound::IsListenerRelative");

   // No sound?  Why are you asking about the ListenerRelative flag then?
   if (sound == NULL)
   {
      return false;
   }

   return sound->IsListenerRelative();
}

////////////////////////////////////////////////////////////////////////////////
// manager configuration
void AudioManager::Config(const AudioConfigData& data /*= _DefCfg*/)
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   if (mIsConfigured)
   {
      // already configured
      return;
   }

   // set up the distance model
   switch (data.distancemodel)
   {
   case AL_NONE:
      alDistanceModel(AL_NONE);
      break;

   case AL_INVERSE_DISTANCE_CLAMPED:
      alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
      break;

   case AL_INVERSE_DISTANCE:
   default:
      alDistanceModel(AL_INVERSE_DISTANCE);
      break;
   }
   CheckForError("alDistanceModel Changes", __FUNCTION__, __LINE__);

   // set up EAX
   ConfigEAX(data.eax);
   CheckForError("ConfigEAX( data.eax )", __FUNCTION__, __LINE__);
}


////////////////////////////////////////////////////////////////////////////////
// message receiver
void AudioManager::OnMessage(MessageData* data)
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert(data);

   if (data->sender == &dtCore::System::GetInstance())
   {
      // system messages
      if (data->message == dtCore::System::MESSAGE_PRE_FRAME)
      {
         PreFrame(*static_cast<const double*>(data->userData));
         return;
      }

      if (data->message == dtCore::System::MESSAGE_PAUSE)
      {
         // During a system-wide pause, we want the AudioManager to behave
         // as normal. In many games, there are sounds that occur during
         // during a pause, such as background music or GUI clicks. So
         // here we just call the normal functions all at once.
         PreFrame(*static_cast<const double*>(data->userData));
         return;
      }

      if (data->message == dtCore::System::MESSAGE_PAUSE_START)
      {
         mSoundStateMap.clear();

         // Pause all sounds that are currently playing, and
         // save their previous state.
         for (SND_LST::iterator iter = mSoundList.begin(); iter != mSoundList.end(); ++iter)
         {
            Sound* sob = iter->get();

            if (sob == NULL)
            {
               continue;
            }

            if (sob->IsPaused())
            {
               mSoundStateMap.insert(SoundObjectStateMap::value_type(sob, PAUSED));
            }
            else if (sob->IsPlaying())
            {
               mSoundStateMap.insert(SoundObjectStateMap::value_type(sob, PLAYING));
            }
            else if (sob->IsStopped())
            {
               mSoundStateMap.insert(SoundObjectStateMap::value_type(sob, STOPPED));
            }

            sob->PauseImmediately();
         }
      }

      if (data->message == dtCore::System::MESSAGE_PAUSE_END)
      {
         // Restore all paused sounds to their previous state.
         for (SND_LST::iterator iter = mSoundList.begin(); iter != mSoundList.end(); ++iter)
         {
            Sound* sob = iter->get();

            switch (mSoundStateMap[sob])
            {
            case PAUSED:
               //No need to do anything.
               break;
            case PLAYING:
               sob->Play();
               break;
            case STOPPED:
               sob->Stop();
               break;
            default:
               break;
            }
         }
      }
   }
   else
   {
      //A LOAD request typically comes from a sound when it's loading data from
      //a file.  We run it through the AudioManager so the sounds data buffer
      //can be managed (mostly trying to avoid redundant buffers from the same file).
      if (data->message == Sound::kCommand[Sound::LOAD])
      {
         assert(data->userData);
         Sound* snd(static_cast<Sound*>(data->userData));

         ALint buf = LoadFile(snd->GetFilename());

         snd->SetBuffer(buf);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
Sound* AudioManager::NewSound()
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   SOB_PTR snd(NULL);

   // create a new sound object if we don't have one
   if (snd.get() == NULL)
   {
      snd = new Sound();
      assert(snd.get());
   }

   // listen to messages from this guy
   AddSender(snd.get());

   // save the sound
   mSoundList.push_back(snd);

   // hand out the interface to the sound
   return snd.get();
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::FreeSound(Sound* sound)
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   SOB_PTR snd = static_cast<Sound*>(sound);

   if (snd.get() == NULL)
   {
      return;
   }

   // remove sound from list
   SND_LST::iterator iter;
   for (iter = mSoundList.begin(); iter != mSoundList.end(); ++iter)
   {
      if (snd != *iter)
      {
         continue;
      }

      mSoundList.erase(iter);
      break;
   }

   // stop listening to this guys messages
   snd->RemoveSender(this);
   snd->RemoveSender(&dtCore::System::GetInstance());
   RemoveSender(snd.get());

   // free the sound's source and buffer
   UnloadSound(snd.get());
   snd->Clear();
}

////////////////////////////////////////////////////////////////////////////////
DEPRECATE_FUNC ALuint AudioManager::GetSource(Sound* sound)
{
   DEPRECATE("AudioManager::GetSource","Sound::GetSource");

   if (sound == NULL)
   {
      return -1;
   }

   return sound->GetSource();
}

////////////////////////////////////////////////////////////////////////////////
ALint AudioManager::LoadFile(const std::string& file)
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   if (file.empty())
   {
      // no file name, bail...
      return false;
   }

   std::string filename = dtCore::FindFileInPathList(file);
   if (filename.empty())
   {
      // still no file name, bail...
      Log::GetInstance().LogMessage(Log::LOG_WARNING, __FUNCTION__, "AudioManager: can't load file %s", file.c_str());
      return -1;
   }

   BufferData* bd = mBufferMap[file];
   if (bd != 0)
   {
      // file already loaded, bail...
      return bd->buf;
   }

   bd = new BufferData;

   // Clear the errors
   //ALenum err( alGetError() );

   // create buffer for the wave file
   alGenBuffers(1L, &bd->buf);
   if (CheckForError("AudioManager: alGenBuffers error", __FUNCTION__, __LINE__))
   {
      delete bd;
      return -1;
   }

   ALenum format(0);
   ALsizei size(0);
   ALvoid* data = NULL;

   // We are trying to support the new version of ALUT as well as the old intergated
   // version. So we have two cases: DEPRECATED and NON-DEPRECATED.

   // This is not defined in ALUT prior to version 1.
   #ifndef ALUT_API_MAJOR_VERSION

   // DEPRECATED version for ALUT < 1.0.0

   // Man, are we still in the dark ages here???
   // Copy the std::string to a frickin' ALByte array...
   ALbyte fname[256L];
   unsigned int len = std::min(filename.size(), size_t(255L));
   memcpy(fname, filename.c_str(), len);
   fname[len] = 0L;

   ALsizei freq(0);
   #ifdef __APPLE__
   alutLoadWAVFile(fname, &format, &data, &size, &freq);
   #else
   alutLoadWAVFile(fname, &format, &data, &size, &freq, &bd->loop);
   #endif // __APPLE__

   #else

   // NON-DEPRECATED version for ALUT >= 1.0.0
   ALfloat freq(0);
   data = alutLoadMemoryFromFile(filename.c_str(), &format, &size, &freq);
   CheckForError("data = alutLoadMemoryFromFile", __FUNCTION__, __LINE__);

   #endif // ALUT_API_MAJOR_VERSION

   if (data == NULL)
   {
      #ifndef ALUT_API_MAJOR_VERSION
      Log::GetInstance().LogMessage(Log::LOG_WARNING, __FUNCTION__,
         "AudioManager: alutLoadWAVFile error on %s", file.c_str());
      #else
         CheckForError("AudioManager: alutLoadMemoryFromFile error", __FUNCTION__, __LINE__);
      #endif // ALUT_API_MAJOR_VERSION

      alDeleteBuffers(1L, &bd->buf);
      delete bd;
      CheckForError("alDeleteBuffers error", __FUNCTION__, __LINE__);
      return -1;
   }

   bd->format = format;
   bd->freq   = ALsizei(freq);
   bd->size   = size;
   alBufferData(bd->buf, bd->format, data, bd->size, bd->freq);
   if (CheckForError("AudioManager: alBufferData error ", __FUNCTION__, __LINE__))
   {
      alDeleteBuffers(1L, &bd->buf);
      free(data);
      data = NULL;

      delete bd;
      return -1;
   }

   // The ALUT documentation says you are "free" to free the allocated
   // memory after it has been copied to the OpenAL buffer. See:
   // http://www.openal.org/openal_webstf/specs/alut.html#alutLoadMemoryFromFile
   // This works fine on Linux, but crashes with a heap error on Windows.
   // This is probably a Windows implementation bug, so let's just leak a
   // bit in the meantime. Hope you bought your Timberlands...
   // -osb

   #if !defined (WIN32) && !defined (_WIN32) && !defined (__WIN32__)
      //TODO -- figure this out... it doesn't happen in straight OpenAL apps - maday
   if(data != NULL)
   {
      free(data);
   }
   #endif

   mBufferMap[file] = bd;
   bd->file = mBufferMap.find(file)->first.c_str();

   return bd->buf;
}

////////////////////////////////////////////////////////////////////////////////
bool AudioManager::UnloadFile(const std::string& file)
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   if (file.empty())
   {
      // no file name, bail...
      return false;
   }

   BUF_MAP::iterator iter = mBufferMap.find(file);
   if (iter == mBufferMap.end())
   {
      // file is not loaded, bail...
      return false;
   }

   BufferData* bd = iter->second;
   if (bd == 0)
   {
      // bd should never be NULL
      // this code should never run
      mBufferMap.erase(iter);
      return false;
   }

   if (bd->use)
   {
      // buffer still in use, don't remove buffer
      return false;
   }

   alDeleteBuffers(1L, &bd->buf);
   CheckForError("alDeleteBuffers( 1L, &bd->buf );", __FUNCTION__, __LINE__);
   delete bd;

   mBufferMap.erase(iter);
   return true;
}

////////////////////////////////////////////////////////////////////////////////
// private member functions
void AudioManager::PreFrame(const double deltaFrameTime)
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   SOB_PTR     snd(NULL);
   const char* cmd(NULL);

   // flush all the sound commands
   for (unsigned int i = 0; i < mSoundList.size(); ++i)
   {
      snd = mSoundList.at(i);

      if (snd.get() == NULL)
      {
         continue;
      }

      //Position and direct sound before firing commands
      snd->SetPositionFromParent();
      snd->SetDirectionFromParent();
      snd->RunAllCommandsInQueue();
   }
}

////////////////////////////////////////////////////////////////////////////////
bool AudioManager::Configured() const
{
   return mIsConfigured;
}

////////////////////////////////////////////////////////////////////////////////
bool AudioManager::ConfigEAX(bool eax)
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   if (!eax)
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
   if (alIsExtensionPresent(buf) == AL_FALSE)
   {
      Log::GetInstance().LogMessage(Log::LOG_WARNING, __FUNCTION__,
         "AudioManager: %s is not available", _EaxVer);
      return false;
   }

   #ifndef AL_VERSION_1_1
   memset(buf, 0L, 32L);
   memcpy(buf, _EaxSet, std::min( strlen(_EaxSet), size_t(32L)));
   #else
   buf = _EaxSet;
   #endif

   // get the eax-set function
   mEAXSet = alGetProcAddress(buf);
   if (mEAXSet == 0)
   {
      Log::GetInstance().LogMessage(Log::LOG_WARNING, __FUNCTION__,
         "AudioManager: %s is not available", _EaxVer);
      return false;
   }

   #ifndef AL_VERSION_1_1
   memset(buf, 0, 32);
   memcpy(buf, _EaxGet, std::min( strlen(_EaxGet), size_t(32)));
   #else
   buf = _EaxVer;
   #endif

   // get the eax-get function
   mEAXGet = alGetProcAddress(buf);
   if (mEAXGet == 0)
   {
      Log::GetInstance().LogMessage(Log::LOG_WARNING, __FUNCTION__,
         "AudioManager: %s is not available", _EaxVer);
      mEAXSet = 0;
      return false;
   }

   CheckForError("Config eax issue", __FUNCTION__, __LINE__);

   return true;
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::UnloadSound(Sound* snd)
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert(snd);

   const char* file(static_cast<Sound*>(snd)->GetFilename());

   if (file == NULL)
   {
      return;
   }

   snd->SetBuffer(0);

   BufferData* bd = mBufferMap[file];

   if (bd == NULL)
   {
      return;
   }

   --bd->use;

   // If the buffer is not being used by any other sound object...
   if (bd->use == 0)
   {
      ALuint src = snd->GetSource();
      if (alIsSource(src) && snd->IsInitialized())
      {
         // NOTE: Deleting the buffer will fail if the sound source is still
         // playing and thus result in a very sound memory leak and potentially
         // mess up the the use of the sources for sounds; sources that should
         // have been freed will essentially become locked.
         //
         // Therefore:  Ensure the sound source is properly stopped before the
         // sound buffer is deleted.
         snd->StopImmediately();

         //Also ensure the sound's buffer is set to nothing:
         snd->SetBuffer(AL_NONE);
         snd->SetLooping(false);
         snd->RewindImmediately();
      }
   }

   UnloadFile(file);
   CheckForError("Unload Sound Error", __FUNCTION__, __LINE__);
}

