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
#include <dtUtil/stringutils.h>
#include <dtUtil/datapathutils.h>

#include <osg/Endian>

#include <fstream>
#include <iostream>

// definitions
#if !defined(BIT)
# define BIT(a) (1L<<a)
#endif

// name spaces
using namespace dtUtil;

namespace dtAudio {

   AudioManager::MOB_ptr AudioManager::_Mgr(NULL);
   AudioManager::LOB_PTR AudioManager::_Mic(NULL);
   const char*           AudioManager::_EaxVer = "EAX2.0";
   const char*           AudioManager::_EaxSet = "EAXSet";
   const char*           AudioManager::_EaxGet = "EAXGet";


#pragma pack(push, 1)
   typedef unsigned int   uint32_t;
   typedef unsigned short uint16_t;

   struct WAV_Header
   {
      struct
      {
         uint32_t Chunk_ID;
         uint32_t ChunkSize;
         uint32_t Format;
      } riffChunkDescriptor;
      struct
      {
         uint32_t Subchunk1ID;
         uint32_t Subchunk1Size;
         uint16_t AudioFormat;
         uint16_t NumChannels;
         uint32_t SampleRate;
         uint32_t ByteRate;
         uint16_t BlockAlign;
         uint16_t BitsPerSample;
      } fmtSubChunk;
      struct
      {
         uint32_t Subchunk2ID;
         uint32_t Subchunk2Size;
      } dataSubChunk;
   };
#pragma pack(pop)

   IMPLEMENT_MANAGEMENT_LAYER(AudioManager)

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
      finalStream << "User Message: [" << userMessage << "] OpenAL Message: [" << alGetString(error) << "]";
      dtUtil::Log::GetInstance("audiomanager.cpp").LogMessage(dtUtil::Log::LOG_WARNING, msgFunction, lineNumber, finalStream.str().c_str());
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
         dtUtil::Log::GetInstance("audiomanager.cpp").LogMessage(dtUtil::Log::LOG_WARNING, msgFunction, lineNumber, finalStream.str().c_str());
         return AL_TRUE;
      }
   }
   return AL_FALSE;
}


////////////////////////////////////////////////////////////////////////////////
// public member functions
// default consructor
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

   AddSender(&dtCore::System::GetInstance());

   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);

   if (dev == NULL && cntxt == NULL)
   {
      if (alutInit(NULL, NULL) == AL_FALSE)
      {
         std::cout << "Error initializing alut" << std::cout;
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
         std::cout << "Error initializing alut" << std::cout;
         CheckForError("alutInit(NULL, NULL)", __FUNCTION__, __LINE__);
      }
      CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);

      mDevice = dev;
      mContext = cntxt;
      mShutdownContexts = shutdownPassedInContexts;
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

   RemoveSender(&dtCore::System::GetInstance());
}

////////////////////////////////////////////////////////////////////////////////
// create the singleton manager
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
// destroy the singleton manager
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

         LoadSoundBuffer(*snd);
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
ALint AudioManager::LoadFile(const std::string& file)
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   if (file.empty())
   {
      // no file name, bail...
      return false;
   }

   std::string filename = dtUtil::FindFileInPathList(file);
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

   // create buffer for the wave file
   alGenBuffers(1L, &bd->buf);

   //if there are no errors creating the buffer then load the file 
   if(CheckForError("AudioManager: alGenBuffers error", __FUNCTION__, __LINE__) || 
      !LoadWavImpl(filename, *bd))
   {
      delete bd;
      return AL_NONE;
   }

   mBufferMap[file] = bd;
   bd->file = mBufferMap.find(file)->first.c_str();

   return bd->buf;
}

bool AudioManager::LoadWavImpl(const std::string& filename, BufferData& bd)
{
#ifdef DELTA_USE_ALUT
   ALenum format(0);
   ALsizei size(0);
   ALvoid* data = NULL;
   ALfloat freq(0);

   data = alutLoadMemoryFromFile(filename.c_str(), &format, &size, &freq);
   CheckForError("data = alutLoadMemoryFromFile", __FUNCTION__, __LINE__);

   if (data == NULL)
   {
      CheckForError("AudioManager: alutLoadMemoryFromFile error", __FUNCTION__, __LINE__);
      ReleaseSoundBuffer(bd.buf, "alDeleteBuffers error", __FUNCTION__, __LINE__);
      return false;
   }

   bd.format = format;
   bd.freq   = ALsizei(freq);
   bd.size   = size;
   alBufferData(bd.buf, bd.format, data, bd.size, bd.freq);
   
   if (CheckForError("AudioManager: alBufferData error ", __FUNCTION__, __LINE__))
   {
      ReleaseSoundBuffer(bd.buf, "alDeleteBuffers error prior to deleting data.",
         __FUNCTION__, __LINE__);

      free(data);
      data = NULL;

      return false;
   }
   
   return true;
#else

   bool success = false;

   // load the wav file...
   {
      std::ifstream filestream(filename.c_str(), std::ios::in | std::ios::binary);
      if (!filestream.fail())
      {
         // get file size
         filestream.seekg(0, std::ios_base::end);
         const long filesize = filestream.tellg();
         filestream.seekg(0, std::ios_base::beg);
         printf("\tfile size: %d bytes\n", filesize);

         WAV_Header header;
         filestream.read((char *)&header, sizeof(header));

         // print details about wav header
         //TODO- make these printouts dtUtil Log Debugs
      /*   printf("\twave header:\n");
         printf("\t\tChunk_ID:\t%c%c%c%c\n", ((char *)(&header.riffChunkDescriptor.Chunk_ID))[0],
            ((char *)(&header.riffChunkDescriptor.Chunk_ID))[1], ((char *)(&header.riffChunkDescriptor.Chunk_ID))[2],
            ((char *)(&header.riffChunkDescriptor.Chunk_ID))[3]);
         printf("\t\tChunkSize:\t%d\n",      header.riffChunkDescriptor.ChunkSize);
         printf("\t\tFormat:\t%c%c%c%c\n",   ((char *)(&header.riffChunkDescriptor.Format))[0],
            ((char *)(&header.riffChunkDescriptor.Format))[1], ((char *)(&header.riffChunkDescriptor.Format))[2],
            ((char *)(&header.riffChunkDescriptor.Format))[3]);
         printf("\t\tSubchunk1ID:\t%c%c%c%c\n", ((char *)(&header.fmtSubChunk.Subchunk1ID))[0],
            ((char *)(&header.fmtSubChunk.Subchunk1ID))[1], ((char *)(&header.fmtSubChunk.Subchunk1ID))[2],
            ((char *)(&header.fmtSubChunk.Subchunk1ID))[3]);
         printf("\t\tSubchunk1Size:\t%d\n",  header.fmtSubChunk.Subchunk1Size);
         printf("\t\tAudioFormat:\t%d\n",    header.fmtSubChunk.AudioFormat);
         printf("\t\tNumChannels:\t%d\n",    header.fmtSubChunk.NumChannels);
         printf("\t\tSampleRate:\t%d\n",     header.fmtSubChunk.SampleRate);
         printf("\t\tByteRate:\t%d\n",       header.fmtSubChunk.ByteRate);
         printf("\t\tBlockAlign:\t%d\n",     header.fmtSubChunk.BlockAlign);
         printf("\t\tBitsPerSample:\t%d\n",  header.fmtSubChunk.BitsPerSample);
         printf("\t\tSubchunk2ID:\t%c%c%c%c\n", ((char *)(&header.dataSubChunk.Subchunk2ID))[0],
            ((char *)(&header.dataSubChunk.Subchunk2ID))[1], ((char *)(&header.dataSubChunk.Subchunk2ID))[2],
            ((char *)(&header.dataSubChunk.Subchunk2ID))[3]);
         printf("\t\tSubchunk2Size:\t%d\n",  header.dataSubChunk.Subchunk2Size);*/

         // note: Subchunk2Size field is often wrong
         // correct result is filesize - headersize
         const size_t dataSize = filesize - sizeof(header);

         {
            std::vector<char> data;
            data.resize(dataSize);

            filestream.read(&data[0], data.size());
            if (osg::getCpuByteOrder() == osg::BigEndian)
            {
               osg::swapBytes(&data[0], data.size());
            }
            {
               if (header.fmtSubChunk.NumChannels == 1)
               {
                  bd.format = AL_FORMAT_MONO16;
               }
               else if (header.fmtSubChunk.NumChannels == 2)
               {
                  bd.format = AL_FORMAT_STEREO16;
               }

               bd.freq   = ALsizei(header.fmtSubChunk.SampleRate);
               bd.size   = dataSize;
               
               alBufferData(bd.buf, bd.format, &data[0], bd.size, bd.freq);
            }
            const ALenum error = alGetError();
            switch (error)
            {
            case AL_NO_ERROR:
               success = true;
               break;
            default:
               printf("warning: OpenAL buffer data attempt failed\n");
               printf("\terror: %s\n", alGetString(error));
               break;
            }
         }
      }
   }

   return success;
#endif
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
      dtUtil::Log::GetInstance("audiomanager.cpp").LogMessage(callerFunctionName,
         callerFunctionLineNum, "Sound buffer was invalid.", dtUtil::Log::LOG_WARNING);
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

} //namespace dtAudio

