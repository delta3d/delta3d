/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 MOVES Institute
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
 * Author: MG
 */

#include <dtDirectorAudioNodes/soundaction.h>

#include <dtAudio/audiomanager.h>
#include <dtAudio/sound.h>
#include <dtAudio/soundactorproxy.h>

#include <dtCore/transform.h>
#include <dtCore/transformable.h>

#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/booleanactorproperty.h>
#include <dtDAL/floatactorproperty.h>
#include <dtDAL/project.h>
#include <dtDAL/resourceactorproperty.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   SoundAction::SoundAction()
      : LatentActionNode()
      , mpSound(NULL)
      , mSoundResourceResource(dtDAL::ResourceDescriptor::NULL_RESOURCE)
      , mSoundActor("")
      , mPitch(1.0f)
      , mGain(1.0f)
      , mListenerRelative(false)
      , mLooping(false)
   {
      AddAuthor("Michael Guerrero");
      AddAuthor("Eric R. Heine");
   }

   /////////////////////////////////////////////////////////////////////////////
   SoundAction::~SoundAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      LatentActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "Play", "Plays or resumes the sound effect."));
      mInputs.push_back(InputLink(this, "Stop", "Stops and resets the sound effect."));
      mInputs.push_back(InputLink(this, "Pause", "Pauses the sound effect so it can be resumed later."));

      // We don't clear the outputs in order to keep the "Out" link
      mOutputs.push_back(OutputLink(this, "Finished"));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundAction::BuildPropertyMap()
   {
      LatentActionNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::ActorIDActorProperty* actorProp = new dtDAL::ActorIDActorProperty(
         "Actor", "Actor",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &SoundAction::SetSoundActor),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &SoundAction::GetSoundActor),
         "dtAudio::Sound", "The sound actor.");
      AddProperty(actorProp);

      AddProperty(new dtDAL::ResourceActorProperty(
         dtDAL::DataType::SOUND, "Sound", "Sound",
         dtDAL::ResourceActorProperty::SetDescFuncType(this, &SoundAction::SetSoundResource),
         dtDAL::ResourceActorProperty::GetDescFuncType(this, &SoundAction::GetSoundResource),
         "A sound resource to play from this action."));

      AddProperty(new dtDAL::FloatActorProperty(
         "Gain", "Gain",
         dtDAL::FloatActorProperty::SetFuncType(this, &SoundAction::SetGain),
         dtDAL::FloatActorProperty::GetFuncType(this, &SoundAction::GetGain),
         "The resource sound's pitch."));

      AddProperty(new dtDAL::FloatActorProperty(
         "Pitch", "Pitch",
         dtDAL::FloatActorProperty::SetFuncType(this, &SoundAction::SetPitch),
         dtDAL::FloatActorProperty::GetFuncType(this, &SoundAction::GetPitch),
         "The resource sound's pitch."));

      AddProperty(new dtDAL::BooleanActorProperty(
         "ListenerRelative", "Listener Relative",
         dtDAL::BooleanActorProperty::SetFuncType(this, &SoundAction::SetListenerRelative),
         dtDAL::BooleanActorProperty::GetFuncType(this, &SoundAction::GetListenerRelative),
         "Whether the resource sound is listener relative or not."));

      AddProperty(new dtDAL::BooleanActorProperty(
         "Looping", "Looping",
         dtDAL::BooleanActorProperty::SetFuncType(this, &SoundAction::SetLooping),
         dtDAL::BooleanActorProperty::GetFuncType(this, &SoundAction::GetLooping),
         "Whether the resource sound is looping or not."));

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, actorProp, true, true, true));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SoundAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      bool shouldContinueUpdating = true;

      switch (input)
      {
      case INPUT_PLAY:
         {
            if (firstUpdate)
            {
               PlaySoundResource();
               PlaySoundsOnActors();

               // Fire the "Out" link
               LatentActionNode::Update(simDelta, delta, input, firstUpdate);
            }
            else
            {
               if (!AreAnySoundsOnActorsStillPlaying())
               {
                  dtAudio::AudioManager::GetInstance().FreeSound(mpSound);
                  mpSound = NULL;

                  OutputLink* link = GetOutputLink("Finished");
                  if (link)
                  {
                     link->Activate();
                  }

                  shouldContinueUpdating = false;
               }
            }

         }
         break;
      case INPUT_STOP:
         {
            StopSoundResource();
            StopSoundsOnActors();
            shouldContinueUpdating = false;
         }
         break;

      case INPUT_PAUSE:
         {
            PauseSoundResource();
            PauseSoundsOnActors();
            shouldContinueUpdating = false;
         }
         break;
      }

      return shouldContinueUpdating;
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundAction::SetSoundActor(const dtCore::UniqueId& value)
   {
      mSoundActor = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId SoundAction::GetSoundActor()
   {
      return mSoundActor;
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundAction::SetSoundResource(const dtDAL::ResourceDescriptor& value)
   {
      mSoundResourceResource = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtDAL::ResourceDescriptor SoundAction::GetSoundResource()
   {
      return mSoundResourceResource;
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundAction::SetGain(float value)
   {
      mGain = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   float SoundAction::GetGain()
   {
      return mGain;
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundAction::SetPitch(float value)
   {
      mPitch = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   float SoundAction::GetPitch()
   {
      return mPitch;
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundAction::SetListenerRelative(bool value)
   {
      mListenerRelative = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SoundAction::GetListenerRelative()
   {
      return mListenerRelative;
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundAction::SetLooping(bool value)
   {
      mLooping = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SoundAction::GetLooping()
   {
      return mLooping;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundAction::PlaySoundsOnActors()
   {
      int count = GetPropertyCount("Actor");

      for (int index = 0; index < count; index++)
      {
         dtAudio::SoundActorProxy* proxy =
            dynamic_cast<dtAudio::SoundActorProxy*>(GetActor("Actor", index));

         if (proxy)
         {
            dtAudio::Sound* actorSound = proxy->GetSound();

            if (!actorSound->IsPlaying())
            {
               actorSound->Play();
            }
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundAction::PlaySoundResource()
   {
      if (mpSound == NULL && mSoundResourceResource != dtDAL::ResourceDescriptor::NULL_RESOURCE)
      {
         mpSound = dtAudio::AudioManager::GetInstance().NewSound();
         mpSound->LoadFile(dtDAL::Project::GetInstance().GetResourcePath(mSoundResourceResource).c_str());
         mpSound->SetGain(GetGain());
         mpSound->SetPitch(GetPitch());
         mpSound->SetListenerRelative(GetListenerRelative());
         mpSound->SetLooping(GetLooping());
      }

      if (mpSound != NULL)
      {
         mpSound->Play();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundAction::StopSoundsOnActors()
   {
      int count = GetPropertyCount("Actor");

      for (int index = 0; index < count; index++)
      {
         dtAudio::SoundActorProxy* proxy =
            dynamic_cast<dtAudio::SoundActorProxy*>(GetActor("Actor", index));

         if (proxy)
         {
            dtAudio::Sound* actorSound = proxy->GetSound();

            if (actorSound->IsPlaying())
            {
               actorSound->Stop();
            }
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundAction::StopSoundResource()
   {
      if (mpSound != NULL)
      {
         mpSound->Stop();
         dtAudio::AudioManager::GetInstance().FreeSound(mpSound);
         mpSound = NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SoundAction::PauseSoundsOnActors()
   {
      int count = GetPropertyCount("Actor");

      for (int index = 0; index < count; index++)
      {
         dtAudio::SoundActorProxy* proxy =
            dynamic_cast<dtAudio::SoundActorProxy*>(GetActor("Actor", index));

         if (proxy)
         {
            dtAudio::Sound* actorSound = proxy->GetSound();

            if (actorSound->IsPlaying())
            {
               actorSound->Pause();
            }
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SoundAction::PauseSoundResource()
   {
      if (mpSound != NULL)
      {
         mpSound->Pause();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool SoundAction::AreAnySoundsOnActorsStillPlaying()
   {
      bool stillPlaying = false;

      if (mpSound != NULL)
      {
         if (mpSound->IsPlaying())
         {
            stillPlaying = true;
         }
      }

      int count = GetPropertyCount("Actor");

      for (int index = 0; index < count; index++)
      {
         dtAudio::SoundActorProxy* proxy =
            dynamic_cast<dtAudio::SoundActorProxy*>(GetActor("Actor", index));

         if (proxy)
         {
            dtAudio::Sound* actorSound = proxy->GetSound();

            if (actorSound->IsPlaying())
            {
               stillPlaying = true;
               break;
            }
         }
      }

      return stillPlaying;
   }

}

////////////////////////////////////////////////////////////////////////////////
