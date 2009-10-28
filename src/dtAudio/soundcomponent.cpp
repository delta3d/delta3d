/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008, Alion Science and Technology, BMH Operation
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
 * @author Chris Rodgers
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAudio/soundcomponent.h>
#include <dtAudio/audiomanager.h>
#include <dtAudio/audioactorregistry.h>
#include <dtAudio/soundactorproxy.h>
#include <dtAudio/soundinfo.h>
#include <dtCore/globals.h>


namespace dtAudio
{

   /////////////////////////////////////////////////////////////////////////////
   // SOUND COMPONENT CODE
   /////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString SoundComponent::DEFAULT_NAME("SoundComponent");

   /////////////////////////////////////////////////////////////////////////////
   SoundComponent::SoundComponent(const std::string& name)
      : dtGame::GMComponent(name)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   SoundComponent::~SoundComponent()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundComponent::OnRemovedFromGM()
   {
      dtGame::GMComponent::OnRemovedFromGM();

      RemoveAllSounds();
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SoundComponent::AddSoundType(dtAudio::SoundType& newSoundType)
   {
      bool success = true;

      if (NULL == dtAudio::SoundType::GetValueForName(newSoundType.GetName()))
      {
         //dtAudio::SoundType::AddInstance(&newSoundType);
         dtAudio::SoundType::AddNewType(newSoundType);
         success = true;
      }

      return success;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtAudio::Sound* SoundComponent::AddSound(const std::string& soundFile,
      const dtAudio::SoundType& soundType)
   {
      return AddSound(soundFile, soundFile, soundType);
   }

   /////////////////////////////////////////////////////////////////////////////
   dtAudio::Sound* SoundComponent::AddSound(const std::string& soundFile, const std::string& soundName,
      const dtAudio::SoundType& soundType)
   {
      bool success = false;
      dtCore::RefPtr<dtAudio::Sound> sound;

      if (!dtCore::FindFileInPathList(soundFile).empty())
      {
         // Create the sound object.
         sound = dtAudio::AudioManager::GetInstance().NewSound();
         sound->LoadFile(soundFile.c_str());

         // Attempt adding it to the map.
         success = AddSound(*sound, soundName, soundType);
      }

      return success ? sound.get() : NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SoundComponent::AddSound(dtAudio::Sound& sound, const std::string& soundName,
      const dtAudio::SoundType& soundType)
   {
      // Attempt adding it to the map.
      bool success = mSoundMap.insert(
         std::make_pair(soundName, new dtAudio::SoundInfo(soundType,sound))).second;

      return success;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SoundComponent::RemoveSound(const std::string& soundName)
   {
      bool success = false;

      SoundInfoMap::iterator foundIter = mSoundMap.find(soundName);
      if (foundIter != mSoundMap.end())
      {
         dtAudio::Sound* sound = &foundIter->second->GetSound();
         dtAudio::AudioManager::GetInstance().FreeSound(sound);
         mSoundMap.erase(foundIter);
         success = true;
      }

      return success;
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundComponent::RemoveAllSounds()
   {
      SoundInfoMap::iterator curSoundInfo = mSoundMap.begin();
      SoundInfoMap::iterator endSoundInfoMap = mSoundMap.end();
      for (; curSoundInfo != endSoundInfoMap; ++curSoundInfo)
      {
         dtAudio::Sound *sound = &curSoundInfo->second->GetSound();
         dtAudio::AudioManager::GetInstance().FreeSound(sound);
      }
      mSoundMap.clear();
   }

   /////////////////////////////////////////////////////////////////////////////
   dtAudio::SoundInfo* SoundComponent::GetSoundInfo(const std::string& soundName)
   {
      SoundInfoMap::iterator foundIter = mSoundMap.find(soundName);
      return foundIter != mSoundMap.end() ? foundIter->second.get() : NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtAudio::SoundInfo* SoundComponent::GetSoundInfo(const std::string& soundName) const
   {
      SoundInfoMap::const_iterator foundIter = mSoundMap.find(soundName);
      return foundIter != mSoundMap.end() ? foundIter->second.get() : NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtAudio::Sound* SoundComponent::GetSound(const std::string& soundName)
   {
      dtAudio::SoundInfo* soundInfo = GetSoundInfo(soundName);
      return soundInfo != NULL ? &soundInfo->GetSound() : NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtAudio::Sound* SoundComponent::GetSound(const std::string& soundName) const
   {
      const dtAudio::SoundInfo* soundInfo = GetSoundInfo(soundName);
      return soundInfo != NULL ? &soundInfo->GetSound() : NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundComponent::GetSoundsByType(const dtAudio::SoundType& soundType, SoundArray& outArray)
   {
      SoundInfoMap::iterator curSoundInfo = mSoundMap.begin();
      SoundInfoMap::iterator endSoundInfoMap = mSoundMap.end();
      for (; curSoundInfo != endSoundInfoMap; ++curSoundInfo)
      {
         if (curSoundInfo->second->GetType() == soundType)
         {
            outArray.push_back(&curSoundInfo->second->GetSound());
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundComponent::GetAllSounds(SoundArray& outArray)
   {
      SoundInfoMap::iterator curSoundInfo = mSoundMap.begin();
      SoundInfoMap::iterator endSoundInfoMap = mSoundMap.end();
      for (; curSoundInfo != endSoundInfoMap; ++curSoundInfo)
      {
         outArray.push_back(&curSoundInfo->second->GetSound());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SoundComponent::DoSoundCommand(const dtAudio::SoundCommand& command, const std::string& soundName)
   {
      SoundInfo* soundInfo = GetSoundInfo(soundName);
      return soundInfo != NULL ? DoSoundCommand(command, soundInfo->GetSound()) : false;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SoundComponent::DoSoundCommand(const dtAudio::SoundCommand& command, dtAudio::Sound& sound)
   {
      if (command == dtAudio::SoundCommand::SOUND_COMMAND_PLAY)
      {
         if (!sound.IsPlaying())
         {
            sound.Play();
         }
      }
      else if (command == dtAudio::SoundCommand::SOUND_COMMAND_PAUSE)
      {
         if (!sound.IsPaused() && ! sound.IsStopped())
         {
            sound.Pause();
         }
      }
      else if (command == dtAudio::SoundCommand::SOUND_COMMAND_STOP)
      {
         if (!sound.IsStopped())
         {
            sound.Stop();
         }
      }
      else if (command == dtAudio::SoundCommand::SOUND_COMMAND_REWIND)
      {
         sound.Rewind();
      }

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   int SoundComponent::DoSoundCommand(const dtAudio::SoundCommand& command, SoundArray& soundArray)
   {
      int successes = 0;

      dtAudio::Sound* curSound = NULL;
      SoundArray::iterator curSoundIter = soundArray.begin();
      SoundArray::iterator endSoundArray = soundArray.end();
      for (; curSoundIter != endSoundArray; ++curSoundIter)
      {
         curSound = *curSoundIter;
         if (curSound != NULL)
         {
            if (DoSoundCommand(command, *curSound))
            {
               ++successes;
            }
         }
      }

      return successes;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SoundComponent::Play(const std::string& soundName)
   {
      return DoSoundCommand(dtAudio::SoundCommand::SOUND_COMMAND_PLAY, soundName);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SoundComponent::Stop(const std::string& soundName)
   {
      return DoSoundCommand(dtAudio::SoundCommand::SOUND_COMMAND_STOP, soundName);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SoundComponent::Pause(const std::string& soundName)
   {
      return DoSoundCommand(dtAudio::SoundCommand::SOUND_COMMAND_PAUSE, soundName);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SoundComponent::Rewind(const std::string& soundName)
   {
      return DoSoundCommand(dtAudio::SoundCommand::SOUND_COMMAND_REWIND, soundName);
   }

   /////////////////////////////////////////////////////////////////////////////
   int SoundComponent::PauseAllSoundsByType(const dtAudio::SoundType& soundType)
   {
      SoundArray soundArray;
      GetSoundsByType(soundType, soundArray);

      return DoSoundCommand(dtAudio::SoundCommand::SOUND_COMMAND_PAUSE, soundArray);
   }

   /////////////////////////////////////////////////////////////////////////////
   int SoundComponent::PauseAllSounds()
   {
      SoundArray soundArray;
      GetAllSounds(soundArray);

      return DoSoundCommand(dtAudio::SoundCommand::SOUND_COMMAND_PAUSE, soundArray);
   }

   /////////////////////////////////////////////////////////////////////////////
   int SoundComponent::StopAllSoundsByType(const dtAudio::SoundType& soundType)
   {
      SoundArray soundArray;
      GetSoundsByType(soundType, soundArray);

      return DoSoundCommand(dtAudio::SoundCommand::SOUND_COMMAND_STOP, soundArray);
   }

   /////////////////////////////////////////////////////////////////////////////
   int SoundComponent::StopAllSounds()
   {
      SoundArray soundArray;
      GetAllSounds(soundArray);

      return DoSoundCommand(dtAudio::SoundCommand::SOUND_COMMAND_STOP, soundArray);
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundComponent::RemoveSoundActorsFromWorld()
   {
      typedef std::vector<dtDAL::ActorProxy*> ProxyArray;
      ProxyArray proxyArray;

      // Evacuate all the sound actors.
      dtGame::GameManager* gm = GetGameManager();
      gm->FindActorsByType(*dtAudio::AudioActorRegistry::SOUND_ACTOR_TYPE, proxyArray);

      ProxyArray::iterator curProxy = proxyArray.begin();
      ProxyArray::iterator endProxyList = proxyArray.end();
      for (; curProxy != endProxyList; ++curProxy)
      {
         mSoundProxyArray.push_back(static_cast<dtAudio::SoundActorProxy*>(*curProxy));
         gm->DeleteActor(*(*curProxy));
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundComponent::AddSoundActorsToWorld()
   {
      // Evacuate all the sound actors.
      dtGame::GameManager* gm = GetGameManager();

      SoundProxyRefArray::iterator curProxy = mSoundProxyArray.begin();
      SoundProxyRefArray::iterator endProxyArray = mSoundProxyArray.end();
      for (; curProxy != endProxyArray; ++curProxy)
      {
         gm->AddActor(*(*curProxy), false, false);
      }

      ClearSoundActorArray();
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundComponent::ClearSoundActorArray()
   {
      mSoundProxyArray.clear();
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundComponent::GetSoundActorSounds(SoundArray& outArray)
   {
      typedef std::vector<dtDAL::ActorProxy*> ProxyArray;
      ProxyArray proxyArray;
      GetGameManager()->FindActorsByType(*dtAudio::AudioActorRegistry::SOUND_ACTOR_TYPE, proxyArray);

      dtAudio::SoundActor* soundActor = NULL;
      ProxyArray::iterator curProxy = proxyArray.begin();
      ProxyArray::iterator endProxyArray = proxyArray.end();
      for (; curProxy != endProxyArray; ++curProxy)
      {
         (*curProxy)->GetActor(soundActor);
         outArray.push_back(soundActor->GetSound());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   int SoundComponent::GetSoundActorContainedCount() const
   {
      return int(mSoundProxyArray.size());
   }

}
