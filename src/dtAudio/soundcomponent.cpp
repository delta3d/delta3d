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
#include <dtAudio/soundactor.h>
#include <dtAudio/soundinfo.h>
#include <dtCore/project.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <sstream>


namespace dtAudio
{

   /////////////////////////////////////////////////////////////////////////////
   // SOUND COMPONENT CODE
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   SoundComponent::SoundComponent(dtCore::SystemComponentType& type)
      : dtGame::GMComponent(type)
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

      std::string fileName;
      if (dtUtil::FileUtils::GetInstance().IsAbsolutePath(soundFile))
      {
         fileName = soundFile;
      }
      else
      {
         fileName = dtUtil::FindFileInPathList(soundFile);

         if(fileName.empty())
         {
            try
            {
               fileName = dtCore::Project::GetInstance().GetResourcePath(dtCore::ResourceDescriptor(soundFile));
            }
            catch (...)
            {
               // Print out happens at the end of this method if file is bad.
            }
         }
      }

      if (!fileName.empty())
      {
         // Create the sound object.
         sound = dtAudio::AudioManager::GetInstance().NewSound();
         sound->LoadFile(fileName.c_str());
         sound->SetName(soundName);

         // Attempt adding it to the map.
         success = AddSound(*sound, soundName, soundType);
      }
      else
      {
         std::ostringstream oss;
         oss << "could not load sound file \"" << soundFile << "\"." << std::endl;
         LOG_WARNING(oss.str());
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
   void SoundComponent::RemoveAllSoundsByType(const dtAudio::SoundType& soundType)
   {
      SoundArray soundArray;
      GetSoundsByType(soundType, soundArray);

      dtAudio::Sound* curSound = NULL;
      SoundArray::iterator curSoundIter = soundArray.begin();
      SoundArray::iterator endSoundArray = soundArray.end();
      for (; curSoundIter != endSoundArray; ++curSoundIter)
      {
         curSound = *curSoundIter;
         RemoveSound(curSound->GetName());
      }
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
      typedef dtCore::ActorPtrVector ProxyArray;
      ProxyArray proxyArray;

      // Evacuate all the sound actors.
      dtGame::GameManager* gm = GetGameManager();
      gm->FindActorsByType(*dtAudio::AudioActorRegistry::SOUND_ACTOR_TYPE, proxyArray);

      ProxyArray::iterator curActor = proxyArray.begin();
      ProxyArray::iterator endActorList = proxyArray.end();
      for (; curActor != endActorList; ++curActor)
      {
         mSoundActorArray.push_back(static_cast<dtAudio::SoundActor*>(*curActor));
         gm->DeleteActor(*(*curActor));
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundComponent::AddSoundActorsToWorld()
   {
      // Evacuate all the sound actors.
      dtGame::GameManager* gm = GetGameManager();

      SoundActorRefArray::iterator curActor = mSoundActorArray.begin();
      SoundActorRefArray::iterator endActorArray = mSoundActorArray.end();
      for (; curActor != endActorArray; ++curActor)
      {
         gm->AddActor(*(*curActor), false, false);
      }

      ClearSoundActorArray();
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundComponent::ClearSoundActorArray()
   {
      mSoundActorArray.clear();
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundComponent::GetSoundActorSounds(SoundArray& outArray)
   {
      typedef dtCore::ActorPtrVector ActorArray;
      ActorArray actorArray;
      GetGameManager()->FindActorsByType(*dtAudio::AudioActorRegistry::SOUND_ACTOR_TYPE, actorArray);

      ActorArray::iterator curActor = actorArray.begin();
      ActorArray::iterator endActorArray = actorArray.end();
      for (; curActor != endActorArray; ++curActor)
      {
         dtAudio::SoundActor* sndActor = dynamic_cast<dtAudio::SoundActor*>(*curActor);
         if (sndActor != NULL)
         {
            outArray.push_back(sndActor->GetSound());
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   int SoundComponent::GetSoundActorContainedCount() const
   {
      return int(mSoundActorArray.size());
   }

}
