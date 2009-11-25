// sound.cpp: Implementation of the Sound class.
//
//////////////////////////////////////////////////////////////////////

#include <cfloat>
#include <dtAudio/dtaudio.h>
#include <dtAudio/sound.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtCore/collisioncategorydefaults.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/serializer.h>

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
   "absolute", "relative",  "mindist",
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

////////////////////////////////////////////////////////////////////////////////
Sound::Sound()
   : Transformable()
   , mFilename("")
   , mPlayCB(NULL)
   , mPlayCBData(NULL)
   , mStopCB(NULL)
   , mStopCBData(NULL)
   , mCommandState(BIT(STOP))
   , mBuffer(0)
   , mIsInitialized(false)
{
   RegisterInstance(this);

   AddSender(&dtCore::System::GetInstance());

   alGenSources(1, &mSource);
   CheckForError("Attempt to create an OpenAL source", __FUNCTION__, __LINE__);

   SetPosition(osg::Vec3(0.0f, 0.0f, 0.0f));
   SetDirection(osg::Vec3(0.0f, 0.0f, 0.0f));
   SetVelocity(osg::Vec3(0.0f, 0.0f, 0.0f));

   SetGain(1.0f);
   SetPitch(1.0f);
   SetRolloffFactor(1.0f);

   SetMaxDistance(FLT_MAX);
   SetMinGain(0.0f);
   SetMaxGain(1.0f);

   SetCollisionCategoryBits(COLLISION_CATEGORY_MASK_SOUND);
}

////////////////////////////////////////////////////////////////////////////////
Sound::~Sound()
{
   if (alIsSource(mSource))
   {
      alDeleteSources(1, &mSource);
   }
   CheckForError("Attempt to delete an OpenAL source", __FUNCTION__, __LINE__);

   //tell the system to stop sending me messages.
   RemoveSender(&dtCore::System::GetInstance());

   DeregisterInstance(this);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::OnMessage(MessageData* data)
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert(data);

   ALint srcState;

   if (data->message == dtCore::System::MESSAGE_POST_FRAME)
   {
      alGetSourcei(GetSource(), AL_SOURCE_STATE, &srcState);
      CheckForError("Getting source state", __FUNCTION__, __LINE__);

      // if the sound has stopped it needs to be marked stopped
      if (srcState == AL_STOPPED && !IsStopped())
      {
         Stop();
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
      ResetState(PAUSE);
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
      ResetState(PLAY);
      ResetState(STOP);
      ResetState(REWIND);
   }
   else if (flag == REWIND)
   {
      ResetState(PLAY);
      ResetState(PAUSE);
      ResetState(STOP);
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


/*****************************
 ** Public Member Functions **
 *****************************/
////////////////////////////////////////////////////////////////////////////////
void Sound::LoadFile(const char* file)
{
   mFilename = file;
   SendMessage(kCommand[LOAD], this);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::UnloadFile()
{
   SendMessage(kCommand[UNLOAD], this);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::Clear()
{
   mFilename = "";

   //DON'T clear the source ID!  We can reuse it!
   //mSource     = 0;

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
/*
   THIS was weird: after playing one time the source stopped being connected to
   the buffer via a call to alGetSourcei(). Started using mBuffer member
   variable to get around this:

   ALint b;
   alGetSourcei(mSource, AL_BUFFER, &b);
   CheckForError("Attempting to get buffer for a source", __FUNCTION__, __LINE__);
   return b;
*/
   return mBuffer;
}

////////////////////////////////////////////////////////////////////////////////
bool Sound::IsLooping() const
{
   ALint looping;

   alGetSourcei(mSource, AL_LOOPING, &looping);
   CheckForError("Getting looping flag for source", __FUNCTION__, __LINE__);

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
   ALint rel;
   alGetSourcei(mSource, AL_SOURCE_RELATIVE, &rel);
   CheckForError("Getting listener relative flag for source",
                   __FUNCTION__, __LINE__);

   return rel != 0;
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

   alSourcei(mSource, AL_BUFFER, b);
   CheckForError("Attempting to set buffer for source", __FUNCTION__, __LINE__);

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
void Sound::Play()
{
   SetState(PLAY);
   mCommand.push(kCommand[PLAY]);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::PlayImmediately()
{
   // first check if sound has a buffer
   ALint buf = GetBuffer();
   if (alIsBuffer(buf) == AL_FALSE)
   {
      dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                  "Invalid buffer when attempting to play sound");
      // no buffer, bail
      return;
   }

   SetState(PLAY);
   alSourcePlay(mSource);
   CheckForError("Attempting to play source", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::Pause()
{
   SetState(PAUSE);
   mCommand.push(kCommand[PAUSE]);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::PauseImmediately()
{
   SetState(PAUSE);
   alSourcePause(mSource);
   CheckForError("Attempting to pause source",__FUNCTION__, __LINE__);
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
   if (alIsSource(mSource))
   {
      alSourceStop(mSource);
      CheckForError("Attempting to stop source", __FUNCTION__, __LINE__);
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
   alSourceRewind(mSource);
   CheckForError("Attempting to rewind source", __FUNCTION__, __LINE__);
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
   alSourcei(mSource, AL_LOOPING, loopInt);
   CheckForError("Attempt to set source looping", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetGain(float gain)
{
   // force gain to range from zero to one
   dtUtil::Clamp<float>(gain, 0.0f, 1.0f);

   alSourcef(mSource, AL_GAIN, gain);
   CheckForError("Attempt to set gain on source", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
float Sound::GetGain() const
{
   float g;
   alGetSourcef(mSource, AL_GAIN, &g);
   CheckForError("Attempt to get gain on source", __FUNCTION__, __LINE__);
   return g;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetPlayTimeOffset(float seconds)
{
   // force second offset to range from zero to the length of the sound
   dtUtil::Clamp<float>(seconds, 0.0f, GetDurationOfPlay());

   alSourcef(mSource, AL_SEC_OFFSET, seconds);
   CheckForError("Attempt to set playback position offset in seconds on source", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
float Sound::GetPlayTimeOffset() const
{
   float seconds;
   alGetSourcef(mSource, AL_SEC_OFFSET, &seconds);
   CheckForError("Attempt to get playback position offset in seconds on source", __FUNCTION__, __LINE__);
   return seconds;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetPitch(float pitch)
{
   dtUtil::Clamp(pitch, 0.000001f, 128.0f);

   alSourcef(mSource, AL_PITCH, pitch);
   ALenum error = alGetError();
   if (error != AL_NO_ERROR && pitch > 2.0)
   {
      // force pitch to range from zero+ to two
      // for some reason openAL chokes on 2+
      // also, openAL states zero to be invalid
      alSourcef(mSource, AL_PITCH, 2.0f);
      if (!CheckForError("Set pitch on source failed.", __FUNCTION__, __LINE__))
      {
         static bool warnedAboutPitchRange = false;
         if (!warnedAboutPitchRange)
         {
            warnedAboutPitchRange = true;
            LOGN_WARNING("audiomanager.cpp", "Setting the pitch to greater than 2.0 is unsupported on many OpenAL implementations.\n"
                     "You have one of those implementations.  All pitches greater than 2.0 will be clamped."
                     "You may try switching to openal-soft to fix it.");
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
float Sound::GetPitch() const
{
   ALfloat p;
   alGetSourcef(mSource, AL_PITCH, &p);
   CheckForError("Attempting to get pitch from source", __FUNCTION__, __LINE__);
   return p;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetListenerRelative(bool relative)
{
   alSourcei(mSource, AL_SOURCE_RELATIVE, relative ? 1 : 0);
   CheckForError("Attempt to set listener relative on source",
                  __FUNCTION__, __LINE__);
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
   alSource3f(mSource, AL_POSITION, pos[0], pos[1], pos[2]);
   CheckForError("Setting source position", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::GetPosition(osg::Vec3& pos) const
{
   alGetSource3f(mSource, AL_POSITION, &pos[0], &pos[1], &pos[2]);
   CheckForError("Getting source position", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetDirection(const osg::Vec3& dir)
{
   //SendMessage( kCommand[DIRECTION], this );
   alSource3f(mSource, AL_DIRECTION, dir[0], dir[1], dir[2]);
   CheckForError("Setting source direction", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::GetDirection(osg::Vec3& dir) const
{
   alGetSource3f(mSource, AL_DIRECTION, &dir[0], &dir[1], &dir[2]);
   CheckForError("Getting source direction", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetVelocity(const osg::Vec3& vel)
{
   alSource3f(mSource, AL_VELOCITY, vel[0], vel[1], vel[2]);
   CheckForError("Setting source velocity", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
void Sound::GetVelocity(osg::Vec3& vel) const
{
   alGetSource3f(mSource, AL_VELOCITY, &vel[0], &vel[1], &vel[2]);
   CheckForError("Getting source velocity", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
DEPRECATE_FUNC float Sound::GetMinDistance() const
{
   DEPRECATE("Sound::GetMiDistance", "This method will soon be removed.");
   return 0.0f;
}

////////////////////////////////////////////////////////////////////////////////
DEPRECATE_FUNC void Sound::SetMinDistance(float dist)
{
   DEPRECATE("Sound::SetMinDistance", "This method will soon be removed.");
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetMaxDistance(float dist)
{
   //make sure max distance is never negative
   float max = dtUtil::Max<float>( 0.0f, dist );

   alSourcef(mSource, AL_MAX_DISTANCE, max);
   CheckForError("Setting source max distance", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
float Sound::GetMaxDistance() const
{
   float md;
   alGetSourcef(mSource, AL_MAX_DISTANCE, &md);
   CheckForError("Getting source max distance", __FUNCTION__, __LINE__);
   return md;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetRolloffFactor(float rolloff)
{
   //make sure rolloff is never negative
   float theRolloff = dtUtil::Max<float>(0.0f, rolloff);

   alSourcef(mSource, AL_ROLLOFF_FACTOR, theRolloff);
   CheckForError("Setting source rolloff", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
float Sound::GetRolloffFactor() const
{
   float r;
   alGetSourcef(mSource, AL_ROLLOFF_FACTOR, &r);
   CheckForError("Getting source rolloff", __FUNCTION__, __LINE__);

   return r;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetMinGain(float minGain)
{
   dtUtil::Clamp<float>(minGain, 0.0f, 1.0f);

   alSourcef(mSource, AL_MIN_GAIN, minGain);
   CheckForError("Setting source min gain", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
float Sound::GetMinGain() const
{
   float mg;
   alGetSourcef(mSource, AL_MIN_GAIN, &mg);
   CheckForError("Getting source min gain", __FUNCTION__, __LINE__);

   return mg;
}

////////////////////////////////////////////////////////////////////////////////
void Sound::SetMaxGain(float maxGain)
{
   dtUtil::Clamp<float>(maxGain, 0.0f, 1.0f);

   alSourcef(mSource, AL_MAX_GAIN, maxGain);
   CheckForError("Setting source max gain", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
float Sound::GetMaxGain() const
{
   float mg;
   alGetSourcef(mSource, AL_MAX_GAIN, &mg);
   CheckForError("Getting source max gain", __FUNCTION__, __LINE__);
   return mg;
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
   alGetBufferi(mBuffer, AL_SIZE,      &dataSize);         // Size in bytes of the audio buffer data.
   alGetBufferi(mBuffer, AL_BITS,      &bitsPerSample);    // The number of bits per sample for the data contained in the buffer.
   alGetBufferi(mBuffer, AL_CHANNELS,  &numChannels);      // The number of channels for the data contained in the buffer.
   alGetBufferi(mBuffer, AL_FREQUENCY, &samplesPerSecond); // The number of samples per second for the data contained in the buffer.

   const float nAvgBytesPerSec = float(samplesPerSecond * numChannels * bitsPerSample) / 8;
   const float flDurationSeconds = (dataSize / nAvgBytesPerSec ) / GetPitch();

   return flDurationSeconds;
}
