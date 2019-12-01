/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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

#ifndef DELTA_FIRE_FIGHTER_DDG_ACTOR
#define DELTA_FIRE_FIGHTER_DDG_ACTOR

#include <fireFighter/shipactor.h>
#include <fireFighter/export.h>
#include <dtCore/transform.h>
#include <dtCore/loadable.h>

namespace dtCore
{
   class ParticleSystem;
}

namespace dtAudio
{
   class Sound;
}

class FIRE_FIGHTER_EXPORT DDGActor : public ShipActor, public dtCore::Loadable
{
   public:

      /// Constructor
      DDGActor(dtGame::GameActorProxy& parent);

      /**
       * The following functions were taken entirely from the
       * original version of the fire fighter game and changed only
       * for compatibility with the new API as to preserve functionality
       */
      virtual void SetModelPosition();

      void SetForwardStack(dtCore::ParticleSystem* tForwardStack, dtCore::Transform tForwardStackPosition);

      void SetAfterStack(dtCore::ParticleSystem* tAfterStack, dtCore::Transform tAfterStackPosition);

      void EngageForwardStack();

      void DisengageForwardStack();

      void EngageAfterStack();

      void DisengageAfterStack();

      void SetStackSound(dtAudio::Sound* tStackSound, const dtCore::Transform& tStackSoundPosition);

      void PlayStackSound();

      void StopStackSound();

      void LoadFile(const std::string& fileName);

   protected:

      bool CheckStack(dtCore::ParticleSystem* stack, bool stackEngaged);

      dtCore::RefPtr<dtCore::ParticleSystem> forwardStack;
      dtCore::RefPtr<dtCore::ParticleSystem> afterStack;
      dtCore::RefPtr<dtCore::ParticleSystem> portWake;
      dtCore::RefPtr<dtCore::ParticleSystem> starboardWake;
      dtCore::RefPtr<dtCore::ParticleSystem> portBowWake;
      dtCore::RefPtr<dtCore::ParticleSystem> stbdBowWake;
      dtCore::RefPtr<dtCore::ParticleSystem> portRooster;
      dtCore::RefPtr<dtCore::ParticleSystem> stbdRooster;
      dtCore::RefPtr<dtCore::ParticleSystem> fwdStack;
      dtCore::RefPtr<dtCore::ParticleSystem> aftStack;
      dtCore::Transform forwardStackPosition;
      dtCore::Transform afterStackPosition;
      bool forwardStackEngaged;
      bool afterStackEngaged;
      dtCore::RefPtr<dtAudio::Sound> stackSound;
      dtCore::Transform stackSoundPosition;

      /// Destructor
      virtual ~DDGActor();

   private:
};

class FIRE_FIGHTER_EXPORT DDGActorProxy : public ShipActorProxy
{
   public:

      /// Constructor
      DDGActorProxy();

      /// Builds the properties of this actor
      virtual void BuildPropertyMap();

      /// Builds the invokables of this actor
      virtual void BuildInvokables();

      /// Instantiates the actor itself
      virtual void CreateDrawable() { SetDrawable(*new DDGActor(*this)); }

   protected:

      /// Destructor
      virtual ~DDGActorProxy();

   private:
};

#endif
