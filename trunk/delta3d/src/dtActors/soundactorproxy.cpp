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

#include <dtActors/soundactorproxy.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproxyicon.h>
#include <dtAudio/audiomanager.h>
#include <dtGame/gamemanager.h>
#include <dtGame/invokable.h>
#include <dtUtil/mathdefines.h>

using namespace dtAudio;
using namespace dtDAL;

namespace dtActors
{
   ///////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString SoundActorProxy::PROPERTY_DIRECTION("Direction"); // "Direction"
   const dtUtil::RefString SoundActorProxy::PROPERTY_GAIN("Gain"); // "Gain"
   const dtUtil::RefString SoundActorProxy::PROPERTY_INITIAL_OFFSET_TIME("Initial offset Time"); // "Initial offset Time"
   const dtUtil::RefString SoundActorProxy::PROPERTY_LISTENER_RELATIVE("Listener Relative"); // "Listener Relative"
   const dtUtil::RefString SoundActorProxy::PROPERTY_LOOPING("Looping"); // "Looping"
   const dtUtil::RefString SoundActorProxy::PROPERTY_MAX_DISTANCE("Max Distance"); // "Max Distance"
   const dtUtil::RefString SoundActorProxy::PROPERTY_MIN_DISTANCE("Min Distance"); // "Min Distance"
   const dtUtil::RefString SoundActorProxy::PROPERTY_MAX_GAIN("Max Gain"); // "Max Gain"
   const dtUtil::RefString SoundActorProxy::PROPERTY_MIN_GAIN("Min Gain"); // "Min Gain"
   const dtUtil::RefString SoundActorProxy::PROPERTY_MAX_RANDOM_TIME("Max Random Time"); // "Max Random Time"
   const dtUtil::RefString SoundActorProxy::PROPERTY_MIN_RANDOM_TIME("Min Random Time"); // "Min Random Time"
   const dtUtil::RefString SoundActorProxy::PROPERTY_PITCH("Pitch"); // "Pitch"
   const dtUtil::RefString SoundActorProxy::PROPERTY_PLAY_AS_RANDOM("Play As Random SFX"); // "Play As Random SFX"
   const dtUtil::RefString SoundActorProxy::PROPERTY_ROLLOFF_FACTOR("Rolloff Factor"); // "Rolloff Factor"
   const dtUtil::RefString SoundActorProxy::PROPERTY_SOUND_EFFECT("The Sound Effect"); // "The Sound Effect"
   const dtUtil::RefString SoundActorProxy::PROPERTY_VELOCITY("Velocity"); // "Velocity"



   //////////////////////////////////////////////////////////////////////////////
   SoundActor::SoundActor(dtGame::GameActorProxy &proxy)  
      : dtGame::GameActor(proxy)
      , mSound(dtAudio::AudioManager::GetInstance().NewSound())
   {
      AddChild(mSound.get());
   }

   ///////////////////////////////////////////////////////////////////////////////
   SoundActor::~SoundActor()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtAudio::Sound* SoundActor::GetSound()
   {
      return mSound.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtAudio::Sound* SoundActor::GetSound() const
   {
      return mSound.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   SoundActorProxy::SoundActorProxy()
      : mRandomSoundEffect(false)
      , mMinRandomTime(5)
      , mMaxRandomTime(30)
      , mOffsetTime(0)
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
      SetClassName("dtAudio::Sound");
   }
   
   ///////////////////////////////////////////////////////////////////////////////
    SoundActorProxy::~SoundActorProxy()
    {
         dtAudio::Sound *snd = static_cast<SoundActor&>(GetGameActor()).GetSound();

         if(snd != NULL)
         {
            if(snd->GetFilename())
               snd->UnloadFile();
         }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void SoundActorProxy::CreateActor()
    {
       SoundActor* actor = new SoundActor(*this);
       SetActor(*actor);
    }

    ///////////////////////////////////////////////////////////////////////
    void SoundActorProxy::OnEnteredWorld()
    {
      if(mRandomSoundEffect)
      {
         int time = dtUtil::RandRange(mMinRandomTime, mMaxRandomTime) + mOffsetTime;
         GetGameManager()->SetTimer("PlaySoundTimer", this, time);
      }
    }

    ///////////////////////////////////////////////////////////////////////
    void SoundActorProxy::OnRemovedFromWorld()
    {
       dtAudio::Sound* sound = static_cast<SoundActor&>(GetGameActor()).GetSound();
       if( sound != NULL )
       {
          sound->Stop();
       }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void SoundActorProxy::BuildInvokables()
    {
       dtGame::GameActorProxy::BuildInvokables();

       AddInvokable(*new dtGame::Invokable("HandleActorTimers",
          dtDAL::MakeFunctor(*this, &SoundActorProxy::HandleActorTimers)));

       RegisterForMessagesAboutSelf(dtGame::MessageType::INFO_TIMER_ELAPSED, "HandleActorTimers");
    }

    ///////////////////////////////////////////////////////////////////////////////
    void SoundActorProxy::HandleActorTimers(const dtGame::Message& msg)
    {
       if(msg.GetMessageType() == dtGame::MessageType::INFO_TIMER_ELAPSED)
       {
          if(mRandomSoundEffect)
          {
             Play();
             int time = dtUtil::RandRange(mMinRandomTime, mMaxRandomTime);
             GetGameManager()->SetTimer("PlaySoundTimer", this, time);
          }
       }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void SoundActorProxy::BuildPropertyMap()
    {
        const dtUtil::RefString &GROUPNAME = "Sound";
        TransformableActorProxy::BuildPropertyMap();

        Sound *sound = static_cast<SoundActor&>(GetGameActor()).GetSound();
        
        // This property toggles whether or not a sound loops. A
        // value of true will loop the sound, while a value of false
        // will not loop/stop looping a sound.
        // Default is false
        AddProperty(new BooleanActorProperty(
           PROPERTY_LOOPING,
           PROPERTY_LOOPING,
            MakeFunctor(*sound, &Sound::SetLooping),
            MakeFunctorRet(*sound, &Sound::IsLooping),
            "Toggles if a sound loops continuously.", GROUPNAME));

        // This property manipulates the gain of a sound. It uses
        // a float type to represent the gain value.
        // Clamped between 0 - 1 by default.
        // Default is 1.0f
        AddProperty(new FloatActorProperty(
           PROPERTY_GAIN,
           PROPERTY_GAIN,
            MakeFunctor(*sound, &Sound::SetGain),
            MakeFunctorRet(*sound, &Sound::GetGain),
            "Sets the gain of a sound.", GROUPNAME));

        // This property manipulates the pitch of a sound. It uses
        // a float type to represent the pitch value.
        // Default is 1.0f
        AddProperty(new FloatActorProperty(
           PROPERTY_PITCH,
           PROPERTY_PITCH,
            MakeFunctor(*sound, &Sound::SetPitch),
            MakeFunctorRet(*sound, &Sound::GetPitch),
            "Sets the pitch of a sound.", GROUPNAME));

        // This property toggles whether or not a sound is listerner
        // relative.
        // Default is false
        AddProperty(new BooleanActorProperty(
           PROPERTY_LISTENER_RELATIVE,
           PROPERTY_LISTENER_RELATIVE,
            MakeFunctor(*sound, &Sound::ListenerRelative),
            MakeFunctorRet(*sound, &Sound::IsListenerRelative),
            "Toggles if a sound is relative to the listener.", GROUPNAME));

        // This property manipulates the minimum distance of a sound. It uses
        // a float type to represent the minimum distance.
        // Default is 1.0f
        AddProperty(new FloatActorProperty(
           PROPERTY_MIN_DISTANCE,
           PROPERTY_MIN_DISTANCE,
            MakeFunctor(*sound, &Sound::SetMinDistance),
            MakeFunctorRet(*sound, &Sound::GetMinDistance),
            "Sets the minimum distance of a sound.", GROUPNAME));

        // This property manipulates the maximum distance of a sound. It uses
        // a float type to represent the maximum distance.
        // Default is 3.402823466e+38F (OxFFFFFFFF).
        AddProperty(new FloatActorProperty(
           PROPERTY_MAX_DISTANCE,
           PROPERTY_MAX_DISTANCE,
            MakeFunctor(*sound, &Sound::SetMaxDistance),
            MakeFunctorRet(*sound, &Sound::GetMaxDistance),
            "Sets the maximum distance of a sound.", GROUPNAME));

        // This property manipulates the roll off factor of a sound. It uses
        // a float type to represent the roll off factor.
        // Default is 1.0f
        AddProperty(new FloatActorProperty(
           PROPERTY_ROLLOFF_FACTOR,
           PROPERTY_ROLLOFF_FACTOR,
            MakeFunctor(*sound, &Sound::SetRolloffFactor),
            MakeFunctorRet(*sound, &Sound::GetRolloffFactor),
            "Sets the rolloff factor of a sound.", GROUPNAME));

        // This property manipulates the minimum gain of a sound. It uses
        // a float type to represent the minimum gain.
        // Default is 0.0f
        AddProperty(new FloatActorProperty(
           PROPERTY_MIN_GAIN,
           PROPERTY_MIN_GAIN,
            MakeFunctor(*sound, &Sound::SetMinGain),
            MakeFunctorRet(*sound, &Sound::GetMinGain),
            "Sets the minimum gain of a sound.", GROUPNAME));

        // This property manipulates the maximum gain of a sound. It uses
        // a float type to represent the maximum gain.
        // Default is 1.0f
        AddProperty(new FloatActorProperty(
           PROPERTY_MAX_GAIN,
           PROPERTY_MAX_GAIN,
            MakeFunctor(*sound, &Sound::SetMaxGain),
            MakeFunctorRet(*sound, &Sound::GetMaxGain),
            "Sets the maximum gain of a sound.", GROUPNAME));

        // This property manipulates the direction of a sound. It uses
        // 3 values to represent the sound's direction.
        // Default is 0, 1, 0
        AddProperty(new Vec3ActorProperty(
           PROPERTY_DIRECTION,
           PROPERTY_DIRECTION,
            MakeFunctor(*this, &SoundActorProxy::SetDirection),
            MakeFunctorRet(*this, &SoundActorProxy::GetDirection),
            "Sets the direction of a sound.", GROUPNAME));

        // This property manipulates the velocity of a sound. It uses
        // 3 values to represent the velocity.
        // Default is 0, 0, 0
        AddProperty(new Vec3ActorProperty(
           PROPERTY_VELOCITY,
           PROPERTY_VELOCITY,
            MakeFunctor(*this, &SoundActorProxy::SetVelocity),
            MakeFunctorRet(*this, &SoundActorProxy::GetVelocity),
            "Sets the velocity of a sound.", GROUPNAME));

        // new properties
        AddProperty(new IntActorProperty(
           PROPERTY_MAX_RANDOM_TIME,
           PROPERTY_MAX_RANDOM_TIME,
           MakeFunctor(*this, &SoundActorProxy::SetMaxRandomTime),
           MakeFunctorRet(*this, &SoundActorProxy::GetMaxRandomTime),
           "Coincides with Play As Random SFX flag", GROUPNAME));

        AddProperty(new IntActorProperty(
           PROPERTY_MIN_RANDOM_TIME,
           PROPERTY_MIN_RANDOM_TIME,
           MakeFunctor(*this, &SoundActorProxy::SetMinRandomTime),
           MakeFunctorRet(*this, &SoundActorProxy::GetMinRandomTime),
           "Coincides with Play As Random SFX flag", GROUPNAME));

        AddProperty(new IntActorProperty(
           PROPERTY_INITIAL_OFFSET_TIME,
           PROPERTY_INITIAL_OFFSET_TIME,
           MakeFunctor(*this, &SoundActorProxy::SetOffsetTime),
           MakeFunctorRet(*this, &SoundActorProxy::GetOffsetTime),
           "Used initially so all sounds that are random are not playing at the start.", GROUPNAME));

        AddProperty(new BooleanActorProperty(
           PROPERTY_PLAY_AS_RANDOM,
           PROPERTY_PLAY_AS_RANDOM,
           MakeFunctor(*this, &SoundActorProxy::SetToHaveRandomSoundEffect),
           MakeFunctorRet(*this, &SoundActorProxy::IsARandomSoundEffect),
           "Will have a timer go off and play sound so often", GROUPNAME));

        AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::SOUND, 
           PROPERTY_SOUND_EFFECT,
           PROPERTY_SOUND_EFFECT,
           dtDAL::MakeFunctor(*this, &SoundActorProxy::LoadFile), 
           "Loads the sound for this to use"));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void SoundActorProxy::LoadFile(const std::string &fileName)
    {
        Sound* snd = static_cast<SoundActor&>(GetGameActor()).GetSound();

        if(!fileName.empty())
            snd->LoadFile(fileName.c_str());
    }

    ///////////////////////////////////////////////////////////////////////////////
    void SoundActorProxy::SetDirection(const osg::Vec3 &dir)
    {
        Sound* snd = static_cast<SoundActor&>(GetGameActor()).GetSound();

        snd->SetDirection(dir);
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 SoundActorProxy::GetDirection()
    {
        Sound* snd = static_cast<SoundActor&>(GetGameActor()).GetSound();

        osg::Vec3 pos;
        snd->GetDirection(pos);
        return pos;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void SoundActorProxy::SetVelocity(const osg::Vec3 &vel)
    {
        Sound* snd = static_cast<SoundActor&>(GetGameActor()).GetSound();

        snd->SetVelocity(vel);
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 SoundActorProxy::GetVelocity()
    {
        Sound* snd = static_cast<SoundActor&>(GetGameActor()).GetSound();

        osg::Vec3 pos;
        snd->GetVelocity(pos);
        return pos;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void SoundActorProxy::Play()
    {
        Sound* snd = static_cast<SoundActor&>(GetGameActor()).GetSound();

        snd->Play();
    }

    ///////////////////////////////////////////////////////////////////////////////
    dtDAL::ActorProxyIcon* SoundActorProxy::GetBillBoardIcon()
    {
        if(!mBillBoardIcon.valid()) 
        {
            mBillBoardIcon =
                 new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::SOUND);
        }

        return mBillBoardIcon.get();
    }
}
