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

#ifndef DELTA_SOUND_COMPONENT
#define DELTA_SOUND_COMPONENT

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAudio/export.h>
#include <dtAudio/soundtype.h>
#include <dtAudio/soundcommand.h>
#include <dtGame/gmcomponent.h>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace dtAudio
{
   class Sound;
   class SoundActorProxy;
   class SoundInfo;


   /////////////////////////////////////////////////////////////////////////////
   // SOUND COMPONENT CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_AUDIO_EXPORT SoundComponent : public dtGame::GMComponent
   {
   public:
      ///////////////////////////////////////////////////////////////////////
      // CLASS CONSTANTS
      ///////////////////////////////////////////////////////////////////////
      static const dtUtil::RefString DEFAULT_NAME;

      ///////////////////////////////////////////////////////////////////////
      // TYPE DECLARATIONS
      ///////////////////////////////////////////////////////////////////////
      typedef std::map<std::string, dtCore::RefPtr<dtAudio::SoundInfo> > SoundInfoMap;
      typedef std::vector<dtAudio::SoundInfo*> SoundInfoArray;
      typedef std::vector<dtAudio::Sound*> SoundArray;
      typedef std::vector<dtCore::RefPtr<dtAudio::SoundActorProxy> > SoundProxyRefArray;



      SoundComponent(const std::string& name = DEFAULT_NAME.Get());

      /**
       * Called when this component is removed from the game manager
       */
      virtual void OnRemovedFromGM();

      /**
       * Add a new enumerator for a sound type.
       * This will add the new type to the existing Sound Type enumeration
       * only if it does not already exist by the same name as another.
       * @param newSounType Enumerator sound type to add to the existing Sound Type enumeration.
       * @return TRUE if the enumerator was suceessfully added.
       */
      bool AddSoundType(dtAudio::SoundType& newSoundType);

      /**
       * Register a sound resource by a type and use the resource name (soundFile) as the name/handle.
       * @param soundFile Relative path and file name of the sound resource file.
       *        that is also used as a handle to the sound.
       * @param soundType Type of sound being added.
       * @return Pointer to the new sound that was created and added; NULL if not found or one of the same soundName exists.
       */
      dtAudio::Sound* AddSound(const std::string& soundFile,
         const dtAudio::SoundType& soundType = dtAudio::SoundType::SOUND_TYPE_DEFAULT);

      /**
       * Register a sound resource by a name and a type.
       * @param soundFile Relative path and file name of the sound resource file.
       * @param soundName Name used as a handle to the sound.
       * @param soundType Type of sound being added.
       * @return Pointer to the new sound that was created and added; NULL if not found or one of the same soundName exists.
       */
      dtAudio::Sound* AddSound(const std::string& soundFile, const std::string& soundName,
         const dtAudio::SoundType& soundType = dtAudio::SoundType::SOUND_TYPE_DEFAULT);

      /**
       * Register a sound resource by a name and a type.
       * @param sound Sound object that is to be added to this component.
       * @param soundName Name used as a handle to the sound.
       * @param soundType Type of sound being added.
       * @return TRUE if the sound was added successfully; FALSE if one of the same name exists.
       */
      bool AddSound(dtAudio::Sound& sound, const std::string& soundName,
         const dtAudio::SoundType& soundType = dtAudio::SoundType::SOUND_TYPE_DEFAULT);

      /**
       * Remove the sound specified by name.
       * @param soundName Name used as a handle to the sound.
       * @return TRUE if the sound was found AND removed.
       */
      bool RemoveSound(const std::string& soundName);

      /**
       * Remove all sounds registered with this component.
       * Note, this does not remove Sound Actors from the Game Manager.
       */
      void RemoveAllSounds();

      /**
       * Get the sound object that holds the sound object and its type.
       * @param soundName Name used as a handle to the sound.
       */
      SoundInfo* GetSoundInfo(const std::string& soundName);
      const SoundInfo* GetSoundInfo(const std::string& soundName) const;

      /**
       * Get the sound object mapped to the specified sound name.
       * @param soundName Name used as a handle to the sound.
       */
      dtAudio::Sound* GetSound(const std::string& soundName);
      const dtAudio::Sound* GetSound(const std::string& soundName) const;

      /**
       * Get all sounds registered with this component by type.
       * @param soundType Type of sounds to be returned.
       * @param outArray Container to captur all sound objects of soundType.
       */
      void GetSoundsByType(const dtAudio::SoundType& soundType, SoundArray& outArray);

      /**
       * Get all sounds registered with this component.
       * @param outArray Container to captur all sound objects of soundType.
       */
      void GetAllSounds(SoundArray& outArray);

      /**
       * Perform an operation on a sound.
       * @param command Enumerator used to signify a specific operation to be performed on the sound.
       * @param soundName Name used as a handle to the sound.
       * @return TRUE if the sound was found.
       */
      bool DoSoundCommand(const dtAudio::SoundCommand& command, const std::string& soundName);

      /**
       * Perform an operation on the sound contained in the specified sound.
       * @param command Enumerator used to signify a specific operation to be performed on the sound.
       * @param sound Sound object to operated on.
       * @return TRUE if the sound was found.
       */
      bool DoSoundCommand(const dtAudio::SoundCommand& command, dtAudio::Sound& sound);

      /**
       * Perform a sound command on a collection of sounds.
       * @param command Enumerator used to signify a specific operation to be performed on all the sounds.
       * @param soundArray Container with all sounds to be operated on.
       * @return Total command successes.
       */
      int DoSoundCommand(const dtAudio::SoundCommand& command, SoundArray& soundArray);

      /**
       * Convenience method for playing a sound by name/handle.
       * @param soundName Name used as a handle to the sound.
       * @return TRUE if the sound was found.
       */
      bool Play(const std::string& soundName);

      /**
       * Convenience method for stopping a sound by name/handle.
       * @param soundName Name used as a handle to the sound.
       * @return TRUE if the sound was found.
       */
      bool Stop(const std::string& soundName);

      /**
       * Convenience method for pausing a sound by name/handle.
       * @param soundName Name used as a handle to the sound.
       * @return TRUE if the sound was found.
       */
      bool Pause(const std::string& soundName);

      /**
       * Convenience method for rewinding a sound by name/handle.
       * @param soundName Name used as a handle to the sound.
       * @return TRUE if the sound was found.
       */
      bool Rewind(const std::string& soundName);

      /**
       * Convenience method.
       * Perform a pause operation on all sounds of the specified type.
       * @param soundType Type of sounds to be returned.
       * @return Total command successes.
       */
      int PauseAllSoundsByType(const dtAudio::SoundType& soundType);

      /**
       * Convenience method.
       * Perform a pause operation on all registered sounds.
       * @return Total command successes.
       */
      int PauseAllSounds();

      /**
       * Convenience method.
       * Perform a stop operation on all sounds of the specified type.
       * @param soundType Type of sounds to be returned.
       * @return Total command successes.
       */
      int StopAllSoundsByType(const dtAudio::SoundType& soundType);

      /**
       * Convenience method.
       * Perform a pause operation on all registered sounds.
       * @return Total command successes.
       */
      int StopAllSounds();

      /**
       * Convenience method for removing all Sound Actors from the Game Manager.
       * This method is an indirect way of pausing all Sound Actors in the game.
       * All the actors are maintained by this component so that their resources
       * stay loaded in memory out side of the Game Manager.
       */
      void RemoveSoundActorsFromWorld();

      /**
       * Inserts all stored Sound Actors back into the Game Manager so they
       * can play their sounds again, triggered by their elapsed timers.
       * Note that this will remove all the actors from this component's internal list.
       */
      void AddSoundActorsToWorld();

      /**
       * Clear the container that is temporarily holding on to any sound actors that
       * have been evacuated from the world by a call to RemoveSoundActorsFromWorld.
       * This will cause the sounds actors to be deleted if no other references point
       * to the sound acors' proxies.
       */
      void ClearSoundActorArray();

      /**
       * Access all sounds held by Sound Actors in the Game Manager.
       * @param outArray Container to capture all sound objects in Sound Actors.
       */
      void GetSoundActorSounds(SoundArray& outArray);

      /**
       * @return Count of Sound Actors current held by this component outside of the Game Manager.
       */
      int GetSoundActorContainedCount() const;

   protected:

      virtual ~SoundComponent();

   private:

      /// Map of Sound Info structures keyed on sound name/handle.
      SoundInfoMap mSoundMap;

      /// Array to temporarily evacuate Sound Actors from the Game Manager.
      SoundProxyRefArray mSoundProxyArray;
   };
} // namespace dtAudio

#endif
