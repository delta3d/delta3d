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

#include <dtAudio/sound.h>
#include <dtAudio/soundactorproxy.h>

#include <dtCore/transform.h>
#include <dtCore/transformable.h>

#include <dtDAL/actoridactorproperty.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   SoundAction::SoundAction()
      : ActionNode()
   {
      mSoundActor = "";

      AddAuthor("Michael Guerrero");
   }

   /////////////////////////////////////////////////////////////////////////////
   SoundAction::~SoundAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "Play"));
      mInputs.push_back(InputLink(this, "Stop"));
      mInputs.push_back(InputLink(this, "Pause"));

      // We don't clear the outputs in order to keep the "Out" link
      mOutputs.push_back(OutputLink(this, "Finished"));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SoundAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::ActorIDActorProperty* actorProp = new dtDAL::ActorIDActorProperty(
         "Actor", "Actor",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &SoundAction::SetSoundActor),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &SoundAction::GetSoundActor),
         "dtAudio::Sound", "The sound actor.");
      AddProperty(actorProp);

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
               PlaySoundsOnActors();

               // Fire the "Out" link
               ActionNode::Update(simDelta, delta, input, firstUpdate);
            }
            else
            {
               if (!AreAnySoundsOnActorsStillPlaying())
               {
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
            StopSoundsOnActors();
            shouldContinueUpdating = false;
         }
         break;

      case INPUT_PAUSE:
         {           
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

   ////////////////////////////////////////////////////////////////////////////////
   bool SoundAction::AreAnySoundsOnActorsStillPlaying()
   {
      bool stillPlaying = false;

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
