// sound.cpp: Implementation of the Sound class.
//
//////////////////////////////////////////////////////////////////////

#include "sound.h"
#include "notify.h"
#include "system.h"

#include <osgDB/FileUtils>

using namespace dtCore;
using namespace std;

IMPLEMENT_MANAGEMENT_LAYER(Sound)


/**
 * Whether or not ALUT has been initialized.
 */
bool Sound::mALUTInitialized = false;

/**
 * The transform of the listener.
 */
Transform Sound::mListenerTransform;

/**
 * The gain of the listener.
 */
float Sound::mListenerGain = 1.0f;


/**
 * Represents a snapshot of the state of a sound.
 */
class SoundStateFrame : public StateFrame
{
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


/**
 * Constructor.
 *
 * @param name the instance name
 */
Sound::Sound(string name)
   : mGain(1.0f),
     mLooping(false),
     mPitch(1.f)
{
   SetName(name);

   RegisterInstance(this);

   if(!mALUTInitialized)
   {
      alutInit(0, NULL);

      mALUTInitialized = true;
   }

   alGenBuffers(1, &mBuffer);
   alGenSources(1, &mSource);

   AddSender(System::GetSystem());
}

/**
 * Destructor.
 */
Sound::~Sound()
{
   DeregisterInstance(this);

   alDeleteSources(1, &mSource);
   alDeleteBuffers(1, &mBuffer);
}

/**
 * Sets the transform of the listener.
 *
 * @param transform the new transform
 */
void Sound::SetListenerTransform(Transform* transform)
{
   sgMat4 matrix;

   transform->Get(matrix);

   mListenerTransform.Set(matrix);

   sgVec3 position;

   mListenerTransform.GetTranslation(position);

   sgVec3 at = {0, 1, 0},
          up = {0, 0, 1};

   sgXformVec3(at, matrix);
   sgXformVec3(up, matrix);

   float orientation[6];

   orientation[0] = at[0];
   orientation[1] = at[1];
   orientation[2] = at[2];

   orientation[3] = up[0];
   orientation[4] = up[1];
   orientation[5] = up[2];

   if(!mALUTInitialized)
   {
      alutInit(0, NULL);

      mALUTInitialized = true;
   }

   alListenerfv(AL_POSITION, position);
   alListenerfv(AL_ORIENTATION, orientation);
}

/**
 * Retrieves the transform of the listener.
 *
 * @param transform the transform to fill
 */
void Sound::GetListenerTransform(Transform* transform)
{
   sgMat4 matrix;

   mListenerTransform.Get(matrix);

   transform->Set(matrix);
}

/**
 * Sets the listener gain.
 *
 * @param gain the new gain
 */
void Sound::SetListenerGain(float gain)
{
   mListenerGain = gain;

   alListenerf(AL_GAIN, mListenerGain);
}

/**
 * Returns the listener gain.
 *
 * @return the current gain
 */
float Sound::GetListenerGain()
{
   return mListenerGain;
}

/**
 * Loads the specified sound file.
 *
 * @param filename the name of the file to load
 */
void Sound::LoadFile(string filename)
{
   int format;
   ALsizei size, freq;
   void *data = NULL;
   ALboolean loop;

   ALbyte buf[256];
   
   mFilename = filename;

    //strcpy(buf, osgDB::findDataFile(mFilename).c_str());
   memcpy(buf, osgDB::findDataFile(mFilename).c_str(), 256);

   

   alutLoadWAVFile(buf, &format, &data, &size, &freq, &loop);

   if(data != NULL)
   {
      alBufferData(mBuffer, format, data, size, freq);

      alSourcei(mSource, AL_BUFFER, mBuffer);

      alutUnloadWAV(format, data, size, freq);
   }
   else
   {
      Notify(WARN, "Sound: Can't load %s", mFilename.c_str());
   }
}

/**
 * Returns the name of the loaded sound file.
 *
 * @return the name of the loaded file
 */
std::string Sound::GetFilename() const
{
   return mFilename;
}

/**
 * Sets the gain of the sound source.
 *
 * @param gain the new gain
 */
void Sound::SetGain(float gain)
{
   if(mGain != gain)
   {
      mGain = gain;

      alSourcef(mSource, AL_GAIN, mGain);

      if(IsBeingRecorded())
      {
         SendMessage(
            "stateFrame",
            new SoundStateFrame(
               this,
               SoundStateFrame::Gain,
               true,
               mGain
            )
         );
      }
   }
}

/**
 * Returns the gain of the sound source.
 *
 * @return the current gain
 */
float Sound::GetGain() const
{
   return mGain;
}

/**
 * Set the pitch of the sound source.
 *
 * @param pitch: the pitch multiplier [0.5, 2.0] (default = 1)
 */
void Sound::SetPitch(float pitch)
{
   if(mPitch != pitch)
   {
      mPitch = pitch;

      if (mPitch<0.5f)     mPitch = 0.5f;
      else if (mPitch>2.f) mPitch = 2.f;

      alSourcef(mSource, AL_PITCH, mPitch);    

      if(IsBeingRecorded())
      {
         SendMessage(
            "stateFrame",
            new SoundStateFrame(
            this,
            SoundStateFrame::Pitch,
            true,
            mPitch
            )
            );
      }
   }
}

/** 
 *  Get the current pitch multiplier of the sound source.
 *
 * @return the pitch multiplier [0.5, 2.0]
 */
float Sound::GetPitch() const
{
   return mPitch;
}


/**
 * Sets whether or not to play the sound in a continuous loop.
 *
 * @param looping true to play the sound in a loop, false
 * otherwise
 */
void Sound::SetLooping(bool looping)
{
   mLooping = looping;

   alSourcei(mSource, AL_LOOPING, mLooping);
}

/**
 * Checks whether or not the sound plays in a continuous loop.
 *
 * @return true if the sound plays in a loop, false otherwise
 */
bool Sound::IsLooping() const
{
   return mLooping;
}

/**
 * Starts playing this sound.
 */
void Sound::Play()
{
   if(!IsPlaying())
   {
      alSourcePlay(mSource);

      if(IsBeingRecorded())
      {
         SendMessage(
            "stateFrame",
            new SoundStateFrame(
               this,
               SoundStateFrame::Playing,
               true,
               1.0f
            )
         );
      }
   }
}

/**
 * Checks whether or not the sound is playing.
 *
 * @return true if the sound is playing, false otherwise
 */
bool Sound::IsPlaying() const
{
   ALint state;

   alGetSourcei(mSource, AL_SOURCE_STATE, &state);

   return state == AL_PLAYING;
}

/**
 * Pauses playing this sound.
 */
void Sound::Pause()
{
   alSourcePause(mSource);
}

/**
 * Stops playing this sound.
 */
void Sound::Stop()
{
   if(IsPlaying())
   {
      alSourceStop(mSource);

      if(IsBeingRecorded())
      {
         SendMessage(
            "stateFrame",
            new SoundStateFrame(
               this,
               SoundStateFrame::Playing,
               false,
               1.0f
            )
         );
      }
   }
}

/**
 * Rewinds to the beginning of this sound.
 */
void Sound::Rewind()
{
   alSourceRewind(mSource);
}

/**
 * Message handler.
 *
 * @param data the received message
 */
void Sound::OnMessage(MessageData *data)
{
   if(data->message == "frame")
   {
      Transform transform;

      GetTransform(&transform);

      sgVec3 position;

      transform.GetTranslation(position);

      sgMat4 matrix;

      transform.Get(matrix);

      sgVec3 direction = {1, 0, 0};

      sgXformVec3(direction, matrix);

      alSourcefv(mSource, AL_POSITION, position);
      alSourcefv(mSource, AL_DIRECTION, direction);
   }
}

/**
 * Generates and returns a key frame that represents the
 * complete recordable state of this object.
 *
 * @return a new key frame
 */
StateFrame* Sound::GenerateKeyFrame()
{
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
StateFrame* Sound::DeserializeFrame(TiXmlElement* element)
{
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

