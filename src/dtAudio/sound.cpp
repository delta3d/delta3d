// sound.cpp: Implementation of the Sound class.
// 
//////////////////////////////////////////////////////////////////////

#include <cfloat>
#include <dtAudio/dtaudio.h>
#include <dtAudio/sound.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtCore/project.h>
#include <dtUtil/serializer.h>
#include <dtUtil/mathdefines.h>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/util/XMLString.hpp>

// namespaces
using namespace dtAudio;
XERCES_CPP_NAMESPACE_USE

// static member variables
const char* Sound::kCommand[kNumCommands] =
{
   "",         "load",      "unload",
   "play",     "pause",     "stop",
   "rewind",   "loop",      "unloop",
   "queue",    "gain",      "pitch",
   "position", "direction", "velocity",
   "absolute", "relative",  "refdist",
   "maxdist",  "rolloff",   "mingain",
   "maxgain"
};

IMPLEMENT_MANAGEMENT_LAYER(Sound)

Sound::FrameData::FrameData()
   : mGain(0.0f)
   , mPitch(0.0f)
   , mPlaying(0)
{
}

Sound::FrameData::FrameData(float gain, float pitch, unsigned int playing)
   : mGain(gain)
   , mPitch(pitch)
   , mPlaying(playing)
{
}

Sound::FrameData::~FrameData()
{
}

/********************************
 ** Protected Member Functions **
 ********************************/
ALint IsSource(ALuint source)
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   if (source == AL_NONE)
   {
      return 0;
   }
   ALint result = alIsSource(source);
   // clear any errors from the alIsSource not liking the value passed in.
   // The call to alIsSource just needs to return true if it's valid.
   alGetError();
   return result;
}

////////////////////////////////////////////////////////////////////////////////
Sound::Sound()
   : Transformable("Sound")
   , mFileName("")
   , mPlayCB(NULL)
   , mPlayCBData(NULL)
   , mStopCB(NULL)
   , mStopCBData(NULL)
   , mCommandState(BIT(STOP))
   , mSource(AL_NONE)
   , mBuffer(AL_NONE)
   , mIsInitialized(false)
   , mGain(1.0f)
   , mPitch(1.0f)
   , mSecondOffset(0.0f)
   , mMaxDistance(FLT_MAX)
   , mReferenceDistance(1.0f) //GK added
   , mRolloffFactor(1.0f)
   , mMinGain(0.0f)
   , mMaxGain(1.0f)
   , mListenerRelative(false)
   , mPosition()
   , mDirection()
   , mVelocity()
   , mUserDefinedSource(false)
{
   RegisterInstance(this);

   dtCore::System::GetInstance().TickSignal.connect_slot(this, &Sound::OnSystem);

   SetPosition(osg::Vec3(0.0f, 0.0f, 0.0f));
   SetDirection(osg::Vec3(0.0f, 0.0f, 0.0f));
   SetVelocity(osg::Vec3(0.0f, 0.0f, 0.0f));

   SetGain(1.0f);
   SetPitch(1.0f);
   SetRolloffFactor(1.0f);

   SetMaxDistance(FLT_MAX);
   SetMinGain(0.0f);
   SetMaxGain(1.0f);
   SetReferenceDistance(1.0f); //GK added
}

////////////////////////////////////////////////////////////////////////////////
Sound::~Sound()
{
   if (IsSource(mSource))
   {
      alDeleteSources(1, &mSource);
      CheckForError("Attempt to delete an OpenAL source", __FUNCTION__, __LINE__);
   } 

   //tell the system to stop sending me messages.
   dtCore::System::GetInstance().TickSignal.disconnect(this);


   DeregisterInstance(this);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::OnSystem(const dtUtil::RefString& str, double /*deltaSim*/, double /*deltaReal*/)

{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   ALint srcState;

   if (str == dtCore::System::MESSAGE_POST_FRAME)
   {
      if (IsSource(mSource))
      {
         alGetSourcei(mSource, AL_SOURCE_STATE, &srcState);
         CheckForError("Getting source state", __FUNCTION__, __LINE__);

         //If the sound has stopped it needs to be marked stopped and the
         //source needs to be deallocated. Saves memory -- some sound hardware
         //was only allowing for 32 sources.  Don't worry, we'll reallocate when
         //it's time to play again.
         if (srcState == AL_STOPPED && !IsStopped())
         {
            Stop();
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetPositionFromParent()
{
   // the transform has already been set by parent classes. We just need to
   // copy the SoundObj position and direction to the AL object

   // extract current transform from actor
   dtCore::Transform transform;
   GetTransform(transform, dtCore::Transformable::ABS_CS);

   osg::Vec3 pos(0.0f, 0.0f, 0.0f);
   transform.GetTranslation(pos);

   // set the value on the sound object
   SetPosition(pos);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetDirectionFromParent()
{
   dtCore::Transform transform;
   GetTransform(transform, dtCore::Transformable::ABS_CS);

   // extract separate values for position and direction now
   osg::Vec3 dir (0.0f, 0.0f, 0.0f);

   transform.GetRotation(dir);

   // set the value on the sound object
   SetDirection(dir);
}

void Sound::SetState(unsigned int flag)
{
   mCommandState |= BIT(flag);

   //a few state flags are mutually exclusive
   if (flag == PLAY)
   {
      ResetState(STOP);
      //ResetState(PAUSE);
      ResetState(REWIND);
   }
   else if (flag == STOP)
   {
      ResetState(PLAY);
      ResetState(PAUSE);
      ResetState(REWIND);
   }
   else if (flag == PAUSE)
   {
      //ResetState(PLAY);
      ResetState(STOP);
      ResetState(REWIND);
   }
   else if (flag == REWIND)
   {
      ResetState(PLAY);
      ResetState(PAUSE);
      SetState(STOP);
   }
   else if (flag == LOOP)
   {
      ResetState(UNLOOP);
   }
   else if (flag == UNLOOP)
   {
      ResetState(LOOP);
   }
   else if (flag == REL)
   {
      ResetState(ABS);
   }
   else if (flag == ABS)
   {
      ResetState(REL);
   }
}

////////////////////////////////////////////////////////////////////////////////
bool Sound::RestoreSource()
{
   int isSource = IsSource(mSource);
   if (CheckForError("Checking for valid source ( does a context not exist? )",
         __FUNCTION__, __LINE__))
   {
      return false;
   }

   if (isSource == AL_TRUE)
   {
      return true; // nothing to do -- source doesn't need to be restored
   }
   
   alGenSources(1, &mSource);
   if (CheckForError("Attempting to generate a source.", __FUNCTION__,
                                                             __LINE__))
   {
      return false;
   }

   SetBuffer(mBuffer);
   SetGain(mGain);
   SetPitch(mPitch);
   SetPlayTimeOffset(mSecondOffset);
   SetMaxDistance(mMaxDistance);
   SetReferenceDistance(mReferenceDistance); //GK added
   SetRolloffFactor(mRolloffFactor);
   SetMinGain(mMinGain);
   SetMaxGain(mMaxGain);
   SetListenerRelative(mListenerRelative);
   SetPosition(mPosition);
   SetDirection(mDirection);
   SetVelocity(mVelocity); 
   
   if (GetState(LOOP))
   {
      SetLooping(true);
   }
   else
   {
      SetLooping(false);
   }

   return true;
}

/*****************************
 ** Public Member Functions **
 *****************************/
////////////////////////////////////////////////////////////////////////////////
void Sound::LoadFile(const char* file)
{
   mFileName = file;
   SoundCommand.emit_signal(kCommand[LOAD], this);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::LoadResource(const dtCore::ResourceDescriptor& rd)
{
   mFileName = dtCore::Project::GetInstance().GetResourcePath(rd);
   SoundCommand.emit_signal(kCommand[LOAD], this);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::UnloadFile()
{
   SoundCommand.emit_signal(kCommand[UNLOAD], this);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::Clear()
{
   mFileName = "";
   mUserDefinedSource = false;
 
   //clear out command queue
   while (mCommand.size())
   {
      mCommand.pop();
   }

   mCommandState = BIT(STOP);

   mPlayCB     = NULL;
   mPlayCBData = NULL;
   mStopCB     = NULL;
   mStopCBData = NULL;

   if (GetParent())
   {
      GetParent()->RemoveChild(this);
   }

   ReleaseSource();
}

////////////////////////////////////////////////////////////////////////////////
bool Sound::ReleaseSource()
{
   bool retVal = true;   

   int sourceAllocated = IsSource(mSource);
   retVal &= !CheckForError("Checking for valid source ( does a context not exist? )",
      __FUNCTION__, __LINE__);   
   if (sourceAllocated == AL_TRUE)
   {
      // NOTE: Deleting the buffer will fail if the sound source is still
      // playing and thus result in a very bad memory leak and potentially
      // mess up the the use of the sources for sounds; sources that should
      // have been freed will essentially become locked.
      //
      // Therefore:  Ensure the sound source is properly stopped before the
      // sound buffer is deleted.
      alSourceStop(mSource);      
      retVal &= !CheckForError("Attempting to stop source", __FUNCTION__, __LINE__);
      RewindImmediately();

      alDeleteSources(1, &mSource);
      retVal &= !CheckForError("Attempted to delete source.", __FUNCTION__, __LINE__);

      mSource = 0;

      SetSource(AL_NONE);
   }

   return retVal;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::RunAllCommandsInQueue()
{
   //fire off queued up commands
   while (mCommand.size() > 0)
   {
      const char* nextCmd = mCommand.front();
      mCommand.pop();

      if (nextCmd == kCommand[PLAY])
      {
         if (mPlayCB)
         {
            mPlayCB(static_cast<Sound*>(this), mPlayCBData);
         }
         PlayImmediately();
      }
      else if (nextCmd == kCommand[PAUSE])
      {
         PauseImmediately();
      }
      else if (nextCmd == kCommand[STOP])
      {
         if (mStopCB)
         {
            mStopCB(static_cast<Sound*>(this), mStopCBData);
         }

         StopImmediately();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
ALint Sound::GetBuffer()
{
   return mBuffer;
}

////////////////////////////////////////////////////////////////////////////////
bool Sound::IsLooping() const
{
   unsigned int looping = GetState(LOOP);   

   if (looping)
   {
      return true;
   }
   else
   {
      return false;
   }
}

////////////////////////////////////////////////////////////////////////////////
bool Sound::IsListenerRelative() const
{
   return mListenerRelative;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetBuffer(ALint b)
{
   if (b == 0)
   {
      // assume user is resetting buffer
      return;
   }

   // This check was added to prevent a crash-on-exit for OSX -osb
   //ALint bufValue;
   //alGetSourcei(mSource, AL_BUFFER, &bufValue);
   //CheckForError("Checking buffer before attaching it to a source", __FUNCTION__, __LINE__);
   //if (bufValue == 0)
   //{
   //   std::cout << "alGetSource failed" << std::endl;
   //   return;
   //}

   if (alIsBuffer(b) == AL_FALSE)
   {
      dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                  "Invalid buffer when attempting to set source's buffer");
      // no buffer, bail
      return;
   }

   if (IsSource(mSource))
   {
      alSourcei(mSource, AL_BUFFER, b);
      CheckForError("Attempting to set buffer for source", __FUNCTION__, __LINE__);
   } 

   mBuffer = b;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetPlayCallback(CallBack cb, void* param)
{
   mPlayCB = cb;

   mPlayCBData = mPlayCB ? param : NULL;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetStopCallback(CallBack cb, void* param)
{
   mStopCB  = cb;

   mStopCBData = mStopCB ? param : NULL;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetSource(ALuint s)
{
   mSource = s;

   if (mSource != AL_NONE)
   {
      mUserDefinedSource = true; 
   }
}

////////////////////////////////////////////////////////////////////////////////
void Sound::Play()
{
   SetState(PLAY);
   mCommand.push(kCommand[PLAY]);
}

////////////////////////////////////////////////////////////////////////////////
bool Sound::PlayImmediately()
{
   // first check if sound has a buffer
   ALint buf = GetBuffer();
   if (alIsBuffer(buf) == AL_FALSE)
   {
      dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                  "Invalid buffer when attempting to play sound");
      // no buffer, bail
      return false;
   }

   SetState(PLAY);

   //Sources get deallocated when stopped: Restore source (if necessary)
   if (! RestoreSource())
   {
      return false; // unable to restore source
   }
   
   alSourcePlay(mSource);
   return !CheckForError("Attempting to play source", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::Pause()
{   
   mCommand.push(kCommand[PAUSE]);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::PauseImmediately()
{
   unsigned int paused = GetState(PAUSE);

   if (! paused)
   {
      SetState(PAUSE);

      if (IsSource(mSource))
      {
         alSourcePause(mSource);
         CheckForError("Attempting to pause source",__FUNCTION__, __LINE__);
      }
   }
   else
   {
      ResetState(PAUSE);
      if (GetState(PLAY))
      {
         PlayImmediately();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void Sound::Stop()
{
   SetState(STOP);
   mCommand.push(kCommand[STOP]);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::StopImmediately()
{
   SetState(STOP);
   if (IsSource(mSource) && !mUserDefinedSource)
   {
      //alSourceStop(mSource);
      //CheckForError("Attempting to stop source", __FUNCTION__, __LINE__);
      //
      //In order to prevent circular function calls the preceeding code was
      //moved into the ReleaseSource method.

      ReleaseSource();  //Don't worry, we'll reallocate the source when we need
                        //to play again... saves # of sources on sound hardware
                        //(believe it or not, there are implementations that 
                        //still exist that limit the # of sources to 32).
   }
}

////////////////////////////////////////////////////////////////////////////////
void Sound::Rewind()
{
   SetState(REWIND);
   mCommand.push(kCommand[REWIND]);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::RewindImmediately()
{
   SetState(REWIND);
   if (IsSource(mSource))
   {
      alSourceRewind(mSource);
      CheckForError("Attempting to rewind source", __FUNCTION__, __LINE__);
   }
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetLooping(bool loop)
{
   int loopInt;
   if (loop)
   {
      SetState(LOOP);
      loopInt = 1;
   }
   else
   {
      SetState(UNLOOP);
      loopInt = 0;
   }

   if (IsSource(mSource))
   {
      alSourcei(mSource, AL_LOOPING, loopInt);
      CheckForError("Attempt to set source looping", __FUNCTION__, __LINE__);
   }   
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetGain(float gain)
{
   // force gain to range from zero to one
   dtUtil::Clamp<float>(gain, 0.0f, 1.0f);

   if (IsSource(mSource))
   {
      alSourcef(mSource, AL_GAIN, gain);
      CheckForError("Attempt to set gain on source", __FUNCTION__, __LINE__);

      //It is not necessarily the case the OpenAL will set gain to the value
      //I request (Min / max gain comes into play)
      alGetSourcef(mSource, AL_GAIN, &gain);
      CheckForError("Attempt to get gain on source", __FUNCTION__, __LINE__);
   }

   mGain = gain;
}

////////////////////////////////////////////////////////////////////////////////
float Sound::GetGain() const
{
   return mGain;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetPlayTimeOffset(float seconds)
{
   // force second offset to range from zero to the length of the sound
   dtUtil::Clamp<float>(seconds, 0.0f, GetDurationOfPlay());   

   int isSource = IsSource(mSource);
   CheckForError("Attempt determine if source is valid (is there a context?)",
                   __FUNCTION__, __LINE__);   

   if (isSource == AL_TRUE)
   {      
      alSourcef(mSource, AL_SEC_OFFSET, seconds);
      CheckForError("Attempt to set playback position offset in seconds on source",
                    __FUNCTION__, __LINE__);      
      
   }

   mSecondOffset = seconds;
}

////////////////////////////////////////////////////////////////////////////////
float Sound::GetPlayTimeOffset() const
{
   return mSecondOffset;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetPitch(float pitch)
{
   dtUtil::Clamp(pitch, 0.000001f, 128.0f);

   mPitch = pitch;

   if (IsSource(mSource) == AL_FALSE)
   {
      //nothing else to do
      return;
   }

   alSourcef(mSource, AL_PITCH, pitch);
   ALenum error = alGetError();
   if (error != AL_NO_ERROR && pitch > 2.0)
   {
      // force pitch to range from zero+ to two
      // for some reason openAL chokes on 2+
      // also, openAL states zero to be invalid
      pitch = 2.0f;
      alSourcef(mSource, AL_PITCH, pitch);
      if (!CheckForError("Set pitch on source failed.", __FUNCTION__, __LINE__))
      {
         static bool warnedAboutPitchRange = false;
         if (!warnedAboutPitchRange)
         {
            warnedAboutPitchRange = true;
            LOGN_WARNING("audiomanager.cpp", "Setting the pitch to greater than 2.0 is unsupported on many OpenAL implementations.\n"
                     "You have one of those implementations.  All pitches greater than 2.0 will be clamped.");
         }
      }

      //just in case OpenAL doesn't set to the same value I ask it to.
      alGetSourcef(mSource, AL_PITCH, &pitch);
      CheckForError("Get pitch on source failed.", __FUNCTION__, __LINE__);
   }
   else if (error != AL_NO_ERROR)
   {
      LOG_ERROR("OpenAL Error: unable to set pitch.");
   }

   mPitch = pitch;
}

////////////////////////////////////////////////////////////////////////////////
float Sound::GetPitch() const
{
   return mPitch;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetListenerRelative(bool relative)
{
   if (IsSource(mSource))
   {
      alSourcei(mSource, AL_SOURCE_RELATIVE, relative ? 1 : 0);
      CheckForError("Attempt to set listener relative on source",
                     __FUNCTION__, __LINE__);
      
   }
   mListenerRelative = relative;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetTransform(const dtCore::Transform& xform, dtCore::Transformable::CoordSysEnum cs)
{
   // properly set transform to transformable object
   dtCore::Transformable::SetTransform(xform, cs);

   // get new transform, and break up into
   // position and direction for sound object
   dtCore::Transform transform;
   osg::Vec3         pos(0.0f, 0.0f, 0.0f);
   osg::Vec3         dir(0.0f, 0.0f, 0.0f);

   GetTransform(transform, ABS_CS);

   transform.GetTranslation(pos);
   transform.GetRotation(dir);

   SetPosition(pos);
   SetDirection(dir);
}


////////////////////////////////////////////////////////////////////////////////
void Sound::SetPosition(const osg::Vec3& pos)
{
   if (IsSource(mSource) == AL_TRUE)
   {
      alSource3f(mSource, AL_POSITION, pos[0], pos[1], pos[2]);
      CheckForError("Setting source position", __FUNCTION__, __LINE__);      
   }

   mPosition = pos;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::GetPosition(osg::Vec3& pos) const
{
   pos = mPosition;
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec3& Sound::GetPosition()
{
   return mPosition;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetDirection(const osg::Vec3& dir)
{
   if (IsSource(mSource) == AL_TRUE)
   {   
      alSource3f(mSource, AL_DIRECTION, dir[0], dir[1], dir[2]);
      CheckForError("Setting source direction", __FUNCTION__, __LINE__);      
   }

   mDirection = dir;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::GetDirection(osg::Vec3& dir) const
{
   dir = mDirection;   
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec3 Sound::GetDirection()
{
   return mDirection;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetVelocity(const osg::Vec3& vel)
{
   if (IsSource(mSource) == AL_TRUE)
   {
      alSource3f(mSource, AL_VELOCITY, vel[0], vel[1], vel[2]);
      CheckForError("Setting source velocity", __FUNCTION__, __LINE__);      
   }

   mVelocity = vel;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::GetVelocity(osg::Vec3& vel) const
{
   vel = mVelocity;
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec3 Sound::GetVelocity() const
{
   return mVelocity;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetMaxDistance(float dist)
{
   //make sure max distance is never negative
   float max = dtUtil::Max<float>( 0.0f, dist );

   if (IsSource(mSource) == AL_TRUE)
   {
      alSourcef(mSource, AL_MAX_DISTANCE, max);
      CheckForError("Setting source max distance", __FUNCTION__, __LINE__);
   }

   mMaxDistance = max;
}

////////////////////////////////////////////////////////////////////////////////
float Sound::GetMaxDistance() const
{
   return mMaxDistance;
}

////////////////////////////////////////////////////////////////////////////////
// GK added
void Sound::SetReferenceDistance(float dist)
{
   //make sure max distance is never negative
   float max = dtUtil::Max<float>( 0.0f, dist );

   if (IsSource(mSource) == AL_TRUE)
   {
      alSourcef(mSource, AL_REFERENCE_DISTANCE, max);
      CheckForError("Setting source reference distance", __FUNCTION__, __LINE__);
   }

   mReferenceDistance = max;
}

////////////////////////////////////////////////////////////////////////////////
// GK added
float Sound::GetReferenceDistance() const
{
   return mReferenceDistance;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetRolloffFactor(float rolloff)
{
   //make sure rolloff is never negative
   float theRolloff = dtUtil::Max<float>(0.0f, rolloff);

   if (IsSource(mSource) == AL_TRUE)
   {
      alSourcef(mSource, AL_ROLLOFF_FACTOR, theRolloff);
      CheckForError("Setting source rolloff", __FUNCTION__, __LINE__);
   }

   mRolloffFactor = theRolloff;
}

////////////////////////////////////////////////////////////////////////////////
float Sound::GetRolloffFactor() const
{
   return mRolloffFactor;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetMinGain(float minGain)
{
   dtUtil::Clamp<float>(minGain, 0.0f, 1.0f);

   if (IsSource(mSource))
   {
      alSourcef(mSource, AL_MIN_GAIN, minGain);
      CheckForError("Setting source min gain", __FUNCTION__, __LINE__);
   }

   mMinGain = minGain;
}

////////////////////////////////////////////////////////////////////////////////
float Sound::GetMinGain() const
{
   return mMinGain;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetMaxGain(float maxGain)
{
   dtUtil::Clamp<float>(maxGain, 0.0f, 1.0f);

   if (IsSource(mSource))
   {
      alSourcef(mSource, AL_MAX_GAIN, maxGain);
      CheckForError("Setting source max gain", __FUNCTION__, __LINE__);
   }

   mMaxGain = maxGain;
}

////////////////////////////////////////////////////////////////////////////////
float Sound::GetMaxGain() const
{
   return mMaxGain;
}

Sound::FrameData* Sound::CreateFrameData() const
{
   FrameData* fd = new FrameData(GetGain(),GetPitch(),IsPlaying());
   return fd;
}

void Sound::UseFrameData(const FrameData* fd)
{
   SetGain(fd->mGain);
   SetPitch(fd->mPitch);
   if (!IsPlaying())
   {
      if (fd->mPlaying)
      {
         Play();
      }
   }
   else  // sound is playing
   {
      if (!fd->mPlaying)
      {
         Stop();
      }
   }
}

DOMElement* Sound::Serialize(const FrameData* d, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc) const
{
   XMLCh* SOUND = XMLString::transcode("Sound");
   DOMElement* element = doc->createElement(SOUND);
   XMLString::release(&SOUND);

   XMLCh* MYNAME = XMLString::transcode(GetName().c_str());
   XMLCh* NAME = XMLString::transcode("Name");
   element->setAttribute(NAME, MYNAME);
   XMLString::release(&NAME);
   XMLString::release(&MYNAME);

   DOMElement* gelement = dtUtil::Serializer::ToFloat(d->mGain, "Gain", doc);
   element->appendChild(gelement);

   DOMElement* pelement = dtUtil::Serializer::ToFloat(d->mPitch, "Pitch", doc);
   element->appendChild(pelement);

   DOMElement* playelement = dtUtil::Serializer::ToInt(d->mPlaying, "Playing", doc);
   element->appendChild(playelement);

   return element;
}

float Sound::GetDurationOfPlay() const
{
   int dataSize = 0, bitsPerSample = 0, numChannels = 0;
   int samplesPerSecond = 0;
   if (mBuffer != AL_NONE && alIsBuffer(mBuffer)) 
   {
      alGetBufferi(mBuffer, AL_SIZE,      &dataSize);         // Size in bytes of the audio buffer data.
      CheckForError("Attempt to get size of data buffer.", __FUNCTION__, __LINE__);
      alGetBufferi(mBuffer, AL_BITS,      &bitsPerSample);    // The number of bits per sample for the data contained in the buffer.
      CheckForError("Attempt to get bits per sample in buffer.", __FUNCTION__, __LINE__);
      alGetBufferi(mBuffer, AL_CHANNELS,  &numChannels);      // The number of channels for the data contained in the buffer.
      CheckForError("Attempt to get # of channels in buffer.", __FUNCTION__, __LINE__);
      alGetBufferi(mBuffer, AL_FREQUENCY, &samplesPerSecond); // The number of samples per second for the data contained in the buffer.
      CheckForError("Attempt to get frequency of buffer.", __FUNCTION__, __LINE__);
   }

   const float nAvgBytesPerSec = float(samplesPerSecond * numChannels * bitsPerSample) / 8;
   const float flDurationSeconds = (dataSize / nAvgBytesPerSec ) / GetPitch();

   return flDurationSeconds;
}
