/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
 * 
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * William E. Johnson II
 */

#include <dtAudio/soundactor.h>

#include <dtAudio/audiomanager.h>

#include <dtCore/actorproxyicon.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/vectoractorproperties.h>
#include <dtCore/exceptionenum.h>

#include <dtGame/gamemanager.h>
#include <dtGame/invokable.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>

#include <dtUtil/mathdefines.h>

namespace dtAudio
{
   ///////////////////////////////////////////////////////////////////////////////
   // CLASS CONSTANTS
   ///////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString SoundActor::CLASS_NAME("dtAudio::Sound");
   const dtUtil::RefString SoundActor::INVOKABLE_TIMER_HANDLER("HandleActorTimers");
   const dtUtil::RefString SoundActor::PROPERTY_DIRECTION("Direction"); // "Direction"
   const dtUtil::RefString SoundActor::PROPERTY_GAIN("Gain"); // "Gain"
   const dtUtil::RefString SoundActor::PROPERTY_INITIAL_OFFSET_TIME("Initial offset Time"); // "Initial offset Time"
   const dtUtil::RefString SoundActor::PROPERTY_LISTENER_RELATIVE("Listener Relative"); // "Listener Relative"
   const dtUtil::RefString SoundActor::PROPERTY_LOOPING("Looping"); // "Looping"
   const dtUtil::RefString SoundActor::PROPERTY_MAX_DISTANCE("Max Distance"); // "Max Distance"
   const dtUtil::RefString SoundActor::PROPERTY_REFERENCE_DISTANCE("Reference Distance"); // "Reference Distance"
   const dtUtil::RefString SoundActor::PROPERTY_MAX_GAIN("Max Gain"); // "Max Gain"
   const dtUtil::RefString SoundActor::PROPERTY_MAX_RANDOM_TIME("Max Random Time"); // "Max Random Time"
   const dtUtil::RefString SoundActor::PROPERTY_MIN_RANDOM_TIME("Min Random Time"); // "Min Random Time"
   const dtUtil::RefString SoundActor::PROPERTY_PITCH("Pitch"); // "Pitch"
   const dtUtil::RefString SoundActor::PROPERTY_PLAY_AS_RANDOM("Play As Random SFX"); // "Play As Random SFX"
   const dtUtil::RefString SoundActor::PROPERTY_PLAY_AT_STARTUP("Play Sound at Startup"); // "Play Sound at Startup"
   const dtUtil::RefString SoundActor::PROPERTY_ROLLOFF_FACTOR("Rolloff Factor"); // "Rolloff Factor"
   const dtUtil::RefString SoundActor::PROPERTY_SOUND_EFFECT("The Sound Effect"); // "The Sound Effect"
   const dtUtil::RefString SoundActor::PROPERTY_VELOCITY("Velocity"); // "Velocity"
   const dtUtil::RefString SoundActor::TIMER_NAME("PlaySoundTimer");
   const dtUtil::RefString SoundActor::PLAY_END_TIMER_NAME("PlaySoundTimerEnd");

   const float SoundActor::DEFAULT_RANDOM_TIME_MAX = 30.0f;
   const float SoundActor::DEFAULT_RANDOM_TIME_MIN = 5.0f;


   //////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   //////////////////////////////////////////////////////////////////////////////
   SoundDrawable::SoundDrawable(dtGame::GameActorProxy& owner)
   : dtGame::GameActor(owner)
   {
      //make sure AudioManager has been initialized
      if(!dtAudio::AudioManager::GetInstance().IsInitialized())
      {
         dtAudio::AudioManager::Instantiate();
      }
      mSound = dtAudio::AudioManager::GetInstance().NewSound();

      AddChild(mSound.get());

   }

   ///////////////////////////////////////////////////////////////////////////////
   SoundDrawable::~SoundDrawable()
   {
      if (mSound.valid())
      {
         dtAudio::AudioManager::GetInstance().FreeSound(mSound.get());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtAudio::Sound* SoundDrawable::GetSound()
   {
      return mSound.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtAudio::Sound* SoundDrawable::GetSound() const
   {
      return mSound.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   ///////////////////////////////////////////////////////////////////////////////
   SoundActor::SoundActor()
   : mRandomSoundEffect(false)
   , mMinRandomTime(SoundActor::DEFAULT_RANDOM_TIME_MIN)
   , mMaxRandomTime(SoundActor::DEFAULT_RANDOM_TIME_MAX)
   , mOffsetTime(0.0f)
   , mPlaySoundAtStartup(true)
   {
      /**
       * @note You must instantiate, configure, and shutdown the
       * audiomanager in your application
       * ex.
       * \code
       dtAudio::AudioManager::Instantiate();
       dtAudio::AudioManager::GetManager()->Config(AudioConfigData&)
       * \endcode
       */
      SetClassName(SoundActor::CLASS_NAME.Get());
   }

   ///////////////////////////////////////////////////////////////////////////////
   SoundActor::~SoundActor()
   {
      dtAudio::Sound* snd = GetDrawable<SoundDrawable>()->GetSound();

      if (snd != NULL)
      {
         if (snd->GetFilename())
         {
            snd->UnloadFile();
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundActor::CreateDrawable()
   {
      SoundDrawable* actor = new SoundDrawable(*this);
      SetDrawable(*actor);
   }

   ///////////////////////////////////////////////////////////////////////
   void SoundActor::OnEnteredWorld()
   {
      if(mPlaySoundAtStartup)
      {
         PlayQueued(mOffsetTime);
      }
   }

   ///////////////////////////////////////////////////////////////////////
   void SoundActor::OnRemovedFromWorld()
   {
      Stop();

      // I don't think this has to be done.
      GetGameManager()->ClearTimer(SoundActor::TIMER_NAME.Get(), this);
      GetGameManager()->ClearTimer(SoundActor::PLAY_END_TIMER_NAME, this);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundActor::BuildInvokables()
   {
      dtGame::GameActorProxy::BuildInvokables();

      AddInvokable(*new dtGame::Invokable(SoundActor::INVOKABLE_TIMER_HANDLER.Get(),
            dtUtil::MakeFunctor(&SoundActor::HandleActorTimers, this)));

      RegisterForMessagesAboutSelf(dtGame::MessageType::INFO_TIMER_ELAPSED,
            SoundActor::INVOKABLE_TIMER_HANDLER.Get());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundActor::HandleActorTimers(const dtGame::TimerElapsedMessage& msg)
   {
      if (msg.GetTimerName() == SoundActor::TIMER_NAME)
      {
         Play();

         if (mRandomSoundEffect)
         {
            PlayQueued(0.0f);
         }
      }
      else if (msg.GetTimerName() == SoundActor::PLAY_END_TIMER_NAME)
      {
         Stop();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundActor::BuildPropertyMap()
   {
      static const dtUtil::RefString GROUPNAME = "Sound";
      GameActorProxy::BuildPropertyMap();

      // This property toggles whether or not a sound loops. A
      // value of true will loop the sound, while a value of false
      // will not loop/stop looping a sound.
      // Default is false
      AddProperty(new dtCore::BooleanActorProperty(
            PROPERTY_LOOPING,
            PROPERTY_LOOPING,
            dtCore::BooleanActorProperty::SetFuncType(this, &SoundActor::SetLooping),
            dtCore::BooleanActorProperty::GetFuncType(this, &SoundActor::IsLooping),
            "Toggles if a sound loops continuously.", GROUPNAME));

      // This property manipulates the gain of a sound. It uses
      // a float type to represent the gain value.
      // Clamped between 0 - 1 by default.
      // Default is 1.0f
      AddProperty(new dtCore::FloatActorProperty(
            PROPERTY_GAIN,
            PROPERTY_GAIN,
            dtCore::FloatActorProperty::SetFuncType(this, &SoundActor::SetGain),
            dtCore::FloatActorProperty::GetFuncType(this, &SoundActor::GetGain),
            "Sets the gain of a sound.", GROUPNAME));

      // This property manipulates the pitch of a sound. It uses
      // a float type to represent the pitch value.
      // Default is 1.0f
      AddProperty(new dtCore::FloatActorProperty(
            PROPERTY_PITCH,
            PROPERTY_PITCH,
            dtCore::FloatActorProperty::SetFuncType(this, &SoundActor::SetPitch),
            dtCore::FloatActorProperty::GetFuncType(this, &SoundActor::GetPitch),
            "Sets the pitch of a sound.", GROUPNAME));

      // This property toggles whether or not a sound is listerner
      // relative.
      // Default is false
      AddProperty(new dtCore::BooleanActorProperty(
            PROPERTY_LISTENER_RELATIVE,
            PROPERTY_LISTENER_RELATIVE,
            dtCore::BooleanActorProperty::SetFuncType(this, &SoundActor::SetListenerRelative),
            dtCore::BooleanActorProperty::GetFuncType(this, &SoundActor::IsListenerRelative),
            "Toggles if a sound is relative to the listener.", GROUPNAME));

      // This property manipulates the maximum distance of a sound. It uses
      // a float type to represent the maximum distance.
      // Default is 3.402823466e+38F (OxFFFFFFFF).
      AddProperty(new dtCore::FloatActorProperty(
            PROPERTY_MAX_DISTANCE,
            PROPERTY_MAX_DISTANCE,
            dtCore::FloatActorProperty::SetFuncType(this, &SoundActor::SetMaxDistance),
            dtCore::FloatActorProperty::GetFuncType(this, &SoundActor::GetMaxDistance),
            "Sets the maximum distance of a sound.", GROUPNAME));

      // This property manipulates the reference distance of a sound. It uses
      // a float type to represent the reference distance.
      // Default is 1.0.
      AddProperty(new dtCore::FloatActorProperty(
            PROPERTY_REFERENCE_DISTANCE,
            PROPERTY_REFERENCE_DISTANCE,
            dtCore::FloatActorProperty::SetFuncType(this, &SoundActor::SetReferenceDistance),
            dtCore::FloatActorProperty::GetFuncType(this, &SoundActor::GetReferenceDistance),
            "Sets the reference distance of a sound.", GROUPNAME));


      // This property manipulates the roll off factor of a sound. It uses
      // a float type to represent the roll off factor.
      // Default is 1.0f
      AddProperty(new dtCore::FloatActorProperty(
            PROPERTY_ROLLOFF_FACTOR,
            PROPERTY_ROLLOFF_FACTOR,
            dtCore::FloatActorProperty::SetFuncType(this, &SoundActor::SetRolloffFactor),
            dtCore::FloatActorProperty::GetFuncType(this, &SoundActor::GetRolloffFactor),
            "Sets the rolloff factor of a sound.", GROUPNAME));

      // This property manipulates the maximum gain of a sound. It uses
      // a float type to represent the maximum gain.
      // Default is 1.0f
      AddProperty(new dtCore::FloatActorProperty(
            PROPERTY_MAX_GAIN,
            PROPERTY_MAX_GAIN,
            dtCore::FloatActorProperty::SetFuncType(this, &SoundActor::SetMaxGain),
            dtCore::FloatActorProperty::GetFuncType(this, &SoundActor::GetMaxGain),
            "Sets the maximum gain of a sound.", GROUPNAME));

      // This property manipulates the direction of a sound. It uses
      // 3 values to represent the sound's direction.
      // Default is 0, 1, 0
      AddProperty(new dtCore::Vec3ActorProperty(
            PROPERTY_DIRECTION,
            PROPERTY_DIRECTION,
            dtCore::Vec3ActorProperty::SetFuncType(this, &SoundActor::SetDirection),
            dtCore::Vec3ActorProperty::GetFuncType(this, &SoundActor::GetDirection),
            "Sets the direction of a sound.", GROUPNAME));

      // This property manipulates the velocity of a sound. It uses
      // 3 values to represent the velocity.
      // Default is 0, 0, 0
      AddProperty(new dtCore::Vec3ActorProperty(
            PROPERTY_VELOCITY,
            PROPERTY_VELOCITY,
            dtCore::Vec3ActorProperty::SetFuncType(this, &SoundActor::SetVelocity),
            dtCore::Vec3ActorProperty::GetFuncType(this, &SoundActor::GetVelocity),
            "Sets the velocity of a sound.", GROUPNAME));

      // new properties
      AddProperty(new dtCore::FloatActorProperty(
            PROPERTY_MAX_RANDOM_TIME,
            PROPERTY_MAX_RANDOM_TIME,
            dtCore::FloatActorProperty::SetFuncType(this, &SoundActor::SetMaxRandomTime),
            dtCore::FloatActorProperty::GetFuncType(this, &SoundActor::GetMaxRandomTime),
            "Maximum seconds to wait between random executions of the sound", GROUPNAME));

      AddProperty(new dtCore::FloatActorProperty(
            PROPERTY_MIN_RANDOM_TIME,
            PROPERTY_MIN_RANDOM_TIME,
            dtCore::FloatActorProperty::SetFuncType(this, &SoundActor::SetMinRandomTime),
            dtCore::FloatActorProperty::GetFuncType(this, &SoundActor::GetMinRandomTime),
            "Minimum seconds to wait between random executions of the sound", GROUPNAME));

      AddProperty(new dtCore::FloatActorProperty(
            PROPERTY_INITIAL_OFFSET_TIME,
            PROPERTY_INITIAL_OFFSET_TIME,
            dtCore::FloatActorProperty::SetFuncType(this, &SoundActor::SetOffsetTime),
            dtCore::FloatActorProperty::GetFuncType(this, &SoundActor::GetOffsetTime),
            "Time in seconds to wait before the sound is played when it enters the Game Manager", GROUPNAME));

      AddProperty(new dtCore::BooleanActorProperty(
            PROPERTY_PLAY_AS_RANDOM,
            PROPERTY_PLAY_AS_RANDOM,
            dtCore::BooleanActorProperty::SetFuncType(this, &SoundActor::SetToHaveRandomSoundEffect),
            dtCore::BooleanActorProperty::GetFuncType(this, &SoundActor::IsARandomSoundEffect),
            "Will have a timer go off and play sound so often", GROUPNAME));

      AddProperty(new dtCore::BooleanActorProperty(
            PROPERTY_PLAY_AT_STARTUP,
            PROPERTY_PLAY_AT_STARTUP,
            dtCore::BooleanActorProperty::SetFuncType(this, &SoundActor::SetPlayAtStartup),
            dtCore::BooleanActorProperty::GetFuncType(this, &SoundActor::IsPlayedAtStartup),
            "Will play sound at startup", GROUPNAME));

      AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::SOUND,
            PROPERTY_SOUND_EFFECT,
            PROPERTY_SOUND_EFFECT,
            dtCore::ResourceActorProperty::SetDescFuncType(this, &SoundActor::SetSoundResource),
            dtCore::ResourceActorProperty::GetDescFuncType(this, &SoundActor::GetSoundResource),
            "Loads the sound for this to use"));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundActor::SetSoundResource(const dtCore::ResourceDescriptor& soundResource)
   {
      mSoundResource = soundResource;
      Sound* snd = GetSound();

      snd->LoadResource(soundResource);
   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtCore::ResourceDescriptor& SoundActor::GetSoundResource() const
   {
      return mSoundResource;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundActor::SetDirection(const osg::Vec3& dir)
   {
      Sound* snd = GetSound();

      snd->SetDirection(dir);
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 SoundActor::GetDirection()
   {
      Sound* snd = GetSound();

      osg::Vec3 pos;
      snd->GetDirection(pos);
      return pos;
   }
   
    ///////////////////////////////////////////////////////////////////////////////
    void SoundActor::SetVelocity(const osg::Vec3& vel)
    {
        Sound* snd = GetSound();

        snd->SetVelocity(vel);
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 SoundActor::GetVelocity()
    {
        Sound* snd = GetSound();

        osg::Vec3 pos;
        snd->GetVelocity(pos);
        return pos;
    }
   

   ///////////////////////////////////////////////////////////////////////////////
   void SoundActor::Play()
   {
      if (mSoundResource.IsEmpty())
      {
         throw dtCore::InvalidActorException("Resource is not set on the sound actor.", __FILE__, __LINE__);
      }

      ALint buf = GetSound()->GetBuffer();
      ALint freq = 0, size = 0, bits = 0, channels = 0;
      alGetBufferi(buf, AL_FREQUENCY, &freq);
      alGetBufferi(buf, AL_SIZE, &size);
      alGetBufferi(buf, AL_BITS, &bits);
      alGetBufferi(buf, AL_CHANNELS, &channels);

      int bytesPerSec = freq * (bits/8) * channels;

      float seconds = float(size) / float(bytesPerSec);
      // Add a half a second just to be safe.
      seconds += 0.5f;

      GetSound()->Play();

      if (!GetSound()->IsLooping())
      {
         GetGameManager()->SetTimer(SoundActor::PLAY_END_TIMER_NAME, this, seconds);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundActor::Stop()
   {
      dtAudio::Sound* sound = GetSound();
      if (sound != NULL)
      {
         sound->Stop();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundActor::PlayQueued(float offsetSeconds)
   {
      if (mRandomSoundEffect)
      {
         offsetSeconds += float(dtUtil::RandFloat(mMinRandomTime, mMaxRandomTime));
      }

      GetGameManager()->SetTimer(SoundActor::TIMER_NAME, this, offsetSeconds);
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::ActorProxyIcon* SoundActor::GetBillBoardIcon()
   {
      if (!mBillBoardIcon.valid())
      {
         mBillBoardIcon =
               new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_SOUND);
      }

      return mBillBoardIcon.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtAudio::Sound* SoundActor::GetSound()
   {
      SoundDrawable* actor = NULL;
      GetDrawable(actor);
      return actor->GetSound();
   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtAudio::Sound* SoundActor::GetSound() const
   {
      const SoundDrawable* actor = NULL;
      GetDrawable(actor);
      return actor->GetSound();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundActor::SetLooping(bool looping)
   {
      Sound* snd = GetSound();
      snd->SetLooping(looping);
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool SoundActor::IsLooping() const
   {
      return GetSound()->IsLooping();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundActor::SetGain(float gain)
   {
      GetSound()->SetGain(gain);
   }

   ///////////////////////////////////////////////////////////////////////////////
   float SoundActor::GetGain() const
   {
      return GetSound()->GetGain();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundActor::SetPitch(float pitch)
   {
      GetSound()->SetPitch(pitch);
   }

   ///////////////////////////////////////////////////////////////////////////////
   float SoundActor::GetPitch() const
   {
      return GetSound()->GetPitch();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundActor::SetListenerRelative(bool lisrel)
   {
      GetSound()->SetListenerRelative(lisrel);
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool SoundActor::IsListenerRelative() const
   {
      return GetSound()->IsListenerRelative();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundActor::SetMaxDistance(float max)
   {
      GetSound()->SetMaxDistance(max);
   }

   ///////////////////////////////////////////////////////////////////////////////
   float SoundActor::GetMaxDistance() const
   {
      return GetSound()->GetMaxDistance();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundActor::SetReferenceDistance(float dist)
   {
      Sound* snd = GetSound();
      snd->SetReferenceDistance(dist);

   }

   ///////////////////////////////////////////////////////////////////////////////
   float SoundActor::GetReferenceDistance() const
   {
      return GetSound()->GetReferenceDistance();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundActor::SetRolloffFactor(float rolloff)
   {
      Sound* snd = GetSound();
      snd->SetRolloffFactor(rolloff);
   }

   ///////////////////////////////////////////////////////////////////////////////
   float SoundActor::GetRolloffFactor() const
   {
      return GetSound()->GetRolloffFactor();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundActor::SetMaxGain(float max)
   {
      Sound* snd = GetSound();
      snd->SetMaxGain(max);
   }

   ///////////////////////////////////////////////////////////////////////////////
   float SoundActor::GetMaxGain() const
   {
      return GetSound()->GetMaxGain();
   }
}
