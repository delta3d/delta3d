#include <cassert>
#include <stack>

#include <osg/Vec3>
#include <osg/io_utils>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>
#include <osgDB/Registry>

#ifdef __APPLE__
  #include <OpenAL/alut.h>
#else
  #include <AL/alut.h>
#endif

#include <dtAudio/audiomanager.h>
#include <dtAudio/dtaudio.h>
#include <dtCore/system.h>
#include <dtCore/camera.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>

#include <iostream>

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

IMPLEMENT_MANAGEMENT_LAYER(AudioManager)

namespace dtAudio
{
   /////////////////////////////////////////////////////////////////////////////
   // OSG Object for holding sound buffer data loaded from the following
   // OSG plugin. This will allow the Audio Manager to access buffer information
   // supplied from alut, after the file has been loaded from memory but before
   // the buffer is registered with OpenAL; this is to allow the Audio Manager
   // to have veto power over the loaded file, before its buffer is
   // officially registered.
   /////////////////////////////////////////////////////////////////////////////
   class WrapperOSGSoundObject : public osg::Object
   {
   public:
      typedef osg::Object BaseClass;

      WrapperOSGSoundObject()
         : BaseClass()
         , mRawData(NULL)
      {}

      explicit WrapperOSGSoundObject(bool threadSafeRefUnref)
         : BaseClass(threadSafeRefUnref)
         , mRawData(NULL)
      {}

      WrapperOSGSoundObject(const osg::Object& obj,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
         : BaseClass(obj, copyop)
         , mRawData(NULL)
      {}

      ALvoid* mRawData;
      AudioManager::BufferData mBufferData;

      META_Object("dtAudio", WrapperOSGSoundObject);
   };

   /////////////////////////////////////////////////////////////////////////////
   // OSG Plugin for loading encrypted/non-encrypted sound files through OSG.
   /////////////////////////////////////////////////////////////////////////////
   class ReaderWriterWAV : public osgDB::ReaderWriter
   {
   public:

      //////////////////////////////////////////////////////////////////////////
      ReaderWriterWAV()
      {
         supportsExtension("wav","Wav sound format");
      }

      //////////////////////////////////////////////////////////////////////////
      virtual const char* className() const { return "WAV Sound Reader"; }

      //////////////////////////////////////////////////////////////////////////
      virtual osgDB::ReaderWriter::ReadResult readObject(const std::string& file,
         const osgDB::ReaderWriter::Options* options =NULL) const
      {
         std::string ext = osgDB::getLowerCaseFileExtension(file);
         if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

         if (!dtUtil::FileUtils::GetInstance().FileExists(file))
         {
            return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND);
         }

         std::ifstream confStream(file.c_str(), std::ios_base::binary);

         if (!confStream.is_open())
         {
            return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE);
         }

         return readObject(confStream, options);
      }

      //////////////////////////////////////////////////////////////////////////
      virtual ReadResult readObject(std::istream& fin,const Options* = NULL) const
      {
         osgDB::ReaderWriter::ReadResult result = osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::FILE_NOT_HANDLED);

         if (fin.fail())
         {
            return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE);
         }

         fin.seekg(0,std::ios_base::end);
         unsigned int ulzipFileLength = fin.tellg();
         fin.seekg(0,std::ios_base::beg);

         char* memBuffer = new (std::nothrow) char [ulzipFileLength];
         if (memBuffer == NULL)
         {
            return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE);
         }

         fin.read(memBuffer, ulzipFileLength);

         dtCore::RefPtr<WrapperOSGSoundObject> userData = new WrapperOSGSoundObject;
         AudioManager::BufferData& bf = userData->mBufferData;

         // NON-DEPRECATED version for ALUT >= 1.0.0
         ALfloat freq(0);
         userData->mRawData = alutLoadMemoryFromFileImage(
            memBuffer, ALsizei(ulzipFileLength), &bf.format, &bf.size, &freq);
         bf.freq = ALsizei(freq);
         CheckForError("data = alutLoadMemoryFromFileImage", __FUNCTION__, __LINE__);
         delete [] memBuffer;

         return ReaderWriter::ReadResult(userData.get(), ReaderWriter::ReadResult::FILE_LOADED);
      }
   };
   REGISTER_OSGPLUGIN(wav, ReaderWriterWAV)


////////////////////////////////////////////////////////////////////////////////
// Utility function used to work with OpenAL's error messaging system. It's used
// in multiple places throughout dtAudio. It's not in AudioManager because we
// don't want things like Sound to directly access the AudioManager.
// Returns true on error, false if no error
bool CheckForError(const std::string& userMessage,
                          const std::string& msgFunction,
                          int lineNumber)
{
   ALenum error = alGetError();
   if (error != AL_NO_ERROR)
   {
      std::ostringstream finalStream;

      const ALchar* alErrorString = alGetString(error);
      std::string errorStringToPrint((alErrorString ? alErrorString: ""));

      // If no OpenAL context is present, we will be unable to get a descriptive
      // error string so the best we can do is to return the error code.
      if (errorStringToPrint.empty())
      {
         errorStringToPrint = std::string("description unavailable - OpenAL code: ") + dtUtil::ToString(error);
      }

      finalStream << "User Message: \"" << userMessage << "\" - OpenAL Message: \"" << errorStringToPrint << "\"";
      dtUtil::Log::GetInstance("audiomanager.cpp").LogMessage(dtUtil::Log::LOG_WARNING, msgFunction, lineNumber, finalStream.str().c_str());

      return true;
   }
   else
   {
      // check if we have an ALUT error
      ALenum alutError = alutGetError();
      if (alutError != ALUT_ERROR_NO_ERROR)
      {
         std::ostringstream finalStream;
         finalStream << "User Message: \"" << userMessage << "\" " << " - Alut Message: \"" << alutGetErrorString(alutError) << "\" Line " << lineNumber;
         dtUtil::Log::GetInstance("audiomanager.cpp").LogMessage(dtUtil::Log::LOG_WARNING, msgFunction, lineNumber, finalStream.str().c_str());
         return true;
      }
   }
   return false;
}

} //namespace dtAudio

////////////////////////////////////////////////////////////////////////////////
AudioManager::AudioManager(const std::string& name /*= "audiomanager"*/,
                           ALCdevice* dev /* = NULL */, ALCcontext* cntxt /* = NULL */, bool shutdownPassedInContexts)
   : Base(name)
   , mEAXSet(NULL)
   , mEAXGet(NULL)
   , mNumSounds(0)
   , mIsConfigured(false)
   , mDevice(NULL)
   , mContext(NULL)
   , mShutdownContexts(false)
{
   RegisterInstance(this);

   dtCore::System::GetInstance().TickSignal.connect_slot(this, &AudioManager::OnSystem);

   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);

   if (dev == NULL && cntxt == NULL)
   {
      if (alutInit(NULL, NULL) == AL_FALSE)
      {
         std::cout << "Error initializing alut" << std::endl;
         CheckForError("alutInit(NULL, NULL)", __FUNCTION__, __LINE__);
      }
      CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);

      mContext = alcGetCurrentContext();
      CheckForError("Trying to get context. ", __FUNCTION__, __LINE__);

      mDevice = alcGetContextsDevice(mContext);
      CheckForError("Trying to get context's device. ", __FUNCTION__, __LINE__);
   }
   else
   {
      if(alutInitWithoutContext(NULL, NULL) == AL_FALSE)
      {
         std::cout << "Error initializing alut" << std::endl;
         CheckForError("alutInit(NULL, NULL)", __FUNCTION__, __LINE__);
      }
      CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);

      mDevice = dev;
      mContext = cntxt;
      mShutdownContexts = shutdownPassedInContexts;
   }
}

////////////////////////////////////////////////////////////////////////////////
AudioManager::~AudioManager()
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   DeregisterInstance(this);

   //stop and clear all Sounds
   SND_LST::iterator it;
   for (it = mSoundList.begin(); it != mSoundList.end(); ++it)
   {
      ReleaseSoundSource(**it, "Error freeing source in Audio Manager destructor.",
         __FUNCTION__, __LINE__);
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

      ReleaseSoundBuffer(bd->buf, "alDeleteBuffers(1, &bd->buf )", __FUNCTION__, __LINE__);
      delete bd;
   }
   mBufferMap.clear();
   mSoundList.clear();

   alutExit();
   CheckForError("alutExit()", __FUNCTION__, __LINE__);

   if (mShutdownContexts)
   {
      LOGN_INFO("audiomanager.cpp", "Shutting down custom contexts.");
      // alut won't close the context if it's not the one that created it, so it needs to be done after alutExit()
      // or alut exit won't work.
      CloseContext();
      CloseDevice();
   }

   dtCore::System::GetInstance().TickSignal.disconnect(this);

}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::Instantiate(const std::string& name, ALCdevice* dev, ALCcontext* cntxt, bool shutdownPassedInContexts)
{
   if (_Mgr.get())
   {
      return;
   }

   _Mgr  = new AudioManager(name, dev, cntxt, shutdownPassedInContexts);
   assert(_Mgr.get());

   _Mic  = new Listener;
   assert(_Mic.get());
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::Destroy()
{
   _Mic = NULL;
   _Mgr = NULL;
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::SetOpenALDevice(const ALCchar* deviceName)
{
   //first make sure we cleanup the device and context we were using before
   CloseContext();
   CloseDevice();

   OpenDevice(deviceName);
   CreateContext();
}

////////////////////////////////////////////////////////////////////////////////
AudioManager& AudioManager::GetInstance()
{
   return *_Mgr;
}


////////////////////////////////////////////////////////////////////////////////
Listener* AudioManager::GetListener()
{
   return static_cast<Listener*>(_Mic.get());
}


////////////////////////////////////////////////////////////////////////////////
void AudioManager::SetDistanceModel(ALenum dm)
{
   CheckForError("Cleanup al error.", __FUNCTION__, __LINE__);
   alDistanceModel(dm);
   CheckForError("alDistanceModel()", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
void SetDopplerFactor(float f)
{
   CheckForError("Cleanup OpenAL error.", __FUNCTION__, __LINE__);
   alDopplerFactor(f);
   CheckForError("alDopplerFactor", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
void SetSpeedOfSound(float s)
{
   CheckForError("Cleanup OpenAL error.", __FUNCTION__, __LINE__);
   alSpeedOfSound(s);
   CheckForError("alSpeedOfSound", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   // system messages
   if (str == dtCore::System::MESSAGE_PRE_FRAME)
   {
      PreFrame(deltaSim);
   }
   else if (str == dtCore::System::MESSAGE_PAUSE)
   {
      // During a system-wide pause, we want the AudioManager to behave
      // as normal. In many games, there are sounds that occur during
      // during a pause, such as background music or GUI clicks. So
      // here we just call the normal functions all at once.
      PreFrame(deltaReal);
   }
   else if (str == dtCore::System::MESSAGE_PAUSE_START)
   {
      PauseSounds();
   }
   else if (str == dtCore::System::MESSAGE_PAUSE_END)
   {
      UnPauseSounds();
   }
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::OnSoundCommand(const dtUtil::RefString& command, Sound* snd)
{
   //A LOAD request typically comes from a sound when it's loading data from
   //a file.  We run it through the AudioManager so the sounds data buffer
   //can be managed (mostly trying to avoid redundant buffers from the same file).
   if (command == Sound::kCommand[Sound::LOAD] && snd != nullptr)
   {
      LoadSoundBuffer(*snd);
   }
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::PauseSounds()
{
   //mSoundStateMap.clear();

   // Pause all sounds that are currently playing, and
   // save their previous state.
   for (SND_LST::iterator iter = mSoundList.begin(); iter != mSoundList.end(); ++iter)
   {
      Sound* sob = iter->get();
      if (sob == NULL)
      {
         continue;
      }

      //if (sob->IsPaused())
      //{
      //   mSoundStateMap.insert(SoundObjectStateMap::value_type(sob, PAUSED));
      //}
      //else if (sob->IsPlaying())
      //{
      //   mSoundStateMap.insert(SoundObjectStateMap::value_type(sob, PLAYING));
      //}
      //else if (sob->IsStopped())
      //{
      //   mSoundStateMap.insert(SoundObjectStateMap::value_type(sob, STOPPED));
      //}

      if (sob->IsPlaying())
      {
         sob->PauseImmediately();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::UnPauseSounds()
{
   // Restore all paused sounds to their previous state.
   for (SND_LST::iterator iter = mSoundList.begin(); iter != mSoundList.end(); ++iter)
   {
      Sound* sob = iter->get();
      if (sob == NULL)
      {
         continue;
      }

      //switch (mSoundStateMap[sob])
      //{
      //case PAUSED:
      //   //No need to do anything.
      //   break;
      //case PLAYING:
      //   break;
      //case STOPPED:
      //   sob->Stop();
      //   break;
      //default:
      //   break;
      //}

      if (sob->IsPaused())
      {
         sob->PauseImmediately();
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
   snd->SoundCommand.connect_slot(this, &AudioManager::OnSoundCommand);

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
   //snd->RemoveSender(this);
   dtCore::System::GetInstance().TickSignal.disconnect(snd);
   snd->SoundCommand.disconnect(this);

   // free the sound's source and buffer
   UnloadSound(snd.get());
   snd->Clear();
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

   std::string filename;
   if (dtUtil::FileUtils::GetInstance().FileExists(file))
   {
      filename = file;
   }
   else
   {
      filename = dtUtil::FindFileInPathList(file);
   }

   if (filename.empty())
   {
      // still no file name, bail...
      Log::GetInstance("audiomanager.cpp").LogMessage(Log::LOG_WARNING, __FUNCTION__, "AudioManager: can't load file %s", file.c_str());
      return AL_NONE;
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
      return AL_NONE;
   }

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

   // Load the sound through OSG so that the sound file
   // may or may not be loaded from an encrypted file.
   dtCore::RefPtr<osg::Object> osgObj = osgDB::readRefObjectFile(filename);
   WrapperOSGSoundObject* userData = dynamic_cast<WrapperOSGSoundObject*>(osgObj.get());
   if(userData != NULL)
   {
      data = userData->mRawData;
   }

   #endif // ALUT_API_MAJOR_VERSION

   if (data == NULL)
   {
      #ifndef ALUT_API_MAJOR_VERSION
      Log::GetInstance("audiomanager.cpp").LogMessage(Log::LOG_WARNING, __FUNCTION__,
         "AudioManager: alutLoadWAVFile error on %s", file.c_str());
      #else
         CheckForError("AudioManager: alutLoadMemoryFromFile error", __FUNCTION__, __LINE__);
      #endif // ALUT_API_MAJOR_VERSION

      ReleaseSoundBuffer(bd->buf, "alDeleteBuffers error", __FUNCTION__, __LINE__);
      delete bd;

      return AL_NONE;
   }

#ifndef ALUT_API_MAJOR_VERSION
   bd->format = format;
   bd->freq   = ALsizei(freq);
   bd->size   = size;
#else
   BufferData& userBD = userData->mBufferData;
   bd->format = userBD.format;
   bd->freq   = userBD.freq;
   bd->size   = userBD.size;
#endif // ALUT_API_MAJOR_VERSION

   alBufferData(bd->buf, bd->format, data, bd->size, bd->freq);

#if !defined (_MSC_VER) || !defined (DONT_ALUT_FREE)
   free(data);
   data = NULL;
#endif

   if (CheckForError("AudioManager: alBufferData error ", __FUNCTION__, __LINE__))
   {
      ReleaseSoundBuffer(bd->buf, "alDeleteBuffers error prior to deleting data.",
         __FUNCTION__, __LINE__);

      delete bd;
      return AL_NONE;
   }

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

   ReleaseSoundBuffer(bd->buf, "alDeleteBuffers( 1L, &bd->buf );", __FUNCTION__, __LINE__);
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
      Log::GetInstance("audiomanager.cpp").LogMessage(Log::LOG_WARNING, __FUNCTION__,
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
      Log::GetInstance("audiomanager.cpp").LogMessage(Log::LOG_WARNING, __FUNCTION__,
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
      Log::GetInstance("audiomanager.cpp").LogMessage(Log::LOG_WARNING, __FUNCTION__,
         "AudioManager: %s is not available", _EaxVer);
      mEAXSet = 0;
      return false;
   }

   CheckForError("Config eax issue", __FUNCTION__, __LINE__);

   return true;
}

////////////////////////////////////////////////////////////////////////////////
int AudioManager::LoadSoundBuffer(Sound& snd)
{
   const char* file = snd.GetFilename();
   int useCount = 0;

   if (file != NULL)
   {
      // Load a new or an existing sound buffer.
      if (LoadFile(file) != AL_NONE)
      {
         BufferData* bd = mBufferMap[file];
         snd.SetBuffer(bd->buf);
         useCount = (++bd->use);
      }
      else
      {
         std::ostringstream errorMessage;
         errorMessage << "Unable to generate a sound buffer for file \""
            << file << "\"";
         LOG_ERROR(errorMessage.str());
      }
   }
   else
   {
      LOG_ERROR("Sound object does not specify a file name.");
   }

   return useCount;
}

////////////////////////////////////////////////////////////////////////////////
int AudioManager::UnloadSound(Sound* snd)
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert(snd);

   int useCount = -1;

   const char* file = snd->GetFilename();

   if (file == NULL)
   {
      return useCount;
   }

   snd->SetBuffer(AL_NONE);

   BufferData* bd = mBufferMap[file];

   if (bd == NULL)
   {
      return useCount;
   }

   // Decrease the buffer reference count.
   if (bd->use > 0)
   {
      useCount = (--bd->use);
   }
   else // Something is wrong!
   {
      std::ostringstream errorMessage;
      errorMessage << "Sound buffer reference count for file \""
         << file << "\" was already at: " << bd->use;
      LOG_ERROR(errorMessage.str());

      // Ensure that the buffer will be stopped,
      // and deleted in the call to UnloadFile.
      useCount = bd->use = 0;
   }

   // If the buffer is not being used by any other sound object...
   if (useCount == 0)
   {
      ReleaseSoundSource(*snd, "Sound source delete error", __FUNCTION__, __LINE__);
   }

   UnloadFile(file);
   CheckForError("Unload Sound Error", __FUNCTION__, __LINE__);

   return useCount;
}

////////////////////////////////////////////////////////////////////////////////
bool AudioManager::ReleaseSoundSource(Sound& snd, const std::string& errorMessage,
   const std::string& callerFunctionName, int callerFunctionLineNum )
{
   return snd.ReleaseSource();
}

////////////////////////////////////////////////////////////////////////////////
bool AudioManager::ReleaseSoundBuffer(ALuint bufferHandle, const std::string& errorMessage,
   const std::string& callerFunctionName, int callerFunctionLineNum )
{
   bool success = false;
   if (alIsBuffer(bufferHandle) == AL_TRUE)
   {
      alDeleteBuffers(1L, &bufferHandle);

      // Determine if OpenAL has encountered an error.
      success = CheckForError(errorMessage, callerFunctionName, callerFunctionLineNum);
   }
   else
   {
      dtUtil::Log::GetInstance("audiomanager.cpp").LogMessage(dtUtil::Log::LOG_WARNING, callerFunctionName,
         callerFunctionLineNum, "Sound buffer was invalid.");
   }

   return success;
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::OpenDevice(const ALCchar* deviceName)
{
   mDevice = alcOpenDevice(deviceName);
   if (mDevice == NULL)
   {
      Log::GetInstance("audiomanager.cpp").LogMessage(Log::LOG_ERROR, __FUNCTION__,
                      "AudioManager can't open audio device.\n");
   }
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::CreateContext()
{
   mContext = alcCreateContext(mDevice, NULL);
   if (!mContext)
   {
      Log::GetInstance("audiomanager.cpp").LogMessage(Log::LOG_ERROR, __FUNCTION__,
                      "AudioManager can't create audio context.\n");
   }

   if (!alcMakeContextCurrent(mContext))
   {
      Log::GetInstance("audiomanager.cpp").LogMessage(Log::LOG_ERROR, __FUNCTION__,
                      "AudioManager can't make audio context current.\n");
   }
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::CloseDevice()
{
   if (mDevice)
   {
      alcCloseDevice(mDevice);
      CheckForError("Attempted to close OpenAL device.", __FUNCTION__, __LINE__);
      mDevice = NULL;
   }
}

////////////////////////////////////////////////////////////////////////////////
void AudioManager::CloseContext()
{
   alcMakeContextCurrent(NULL);
   CheckForError("Attempted to change current OpenAL context to nothing.",
                   __FUNCTION__, __LINE__);
   if (mContext)
   {
      alcDestroyContext(mContext);
      CheckForError("Attempted to destroy current OpenAL context.",
                     __FUNCTION__, __LINE__);
      mContext = NULL;
   }
}
