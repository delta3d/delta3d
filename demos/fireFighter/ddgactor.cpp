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

#include <fireFighter/ddgactor.h>
#include <fireFighter/utilityfunctions.h>
#include <dtAudio/sound.h>
#include <dtCore/particlesystem.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/datatype.h>
#include <dtCore/functor.h>
#include <dtUtil/log.h>
#include <osg/MatrixTransform>

/////////////////////////////////////////////////
DDGActorProxy::DDGActorProxy()
{

}

DDGActorProxy::~DDGActorProxy()
{

}

void DDGActorProxy::BuildPropertyMap()
{
   ShipActorProxy::BuildPropertyMap();

   DDGActor& ddg = *GetDrawable<DDGActor>();

   AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::STATIC_MESH,
      "Model", "Model",
      dtCore::ResourceActorProperty::SetFuncType(&ddg, &DDGActor::LoadFile),
      "Loads the model file for the ship"));
}

void DDGActorProxy::BuildInvokables()
{
   ShipActorProxy::BuildInvokables();
}

/////////////////////////////////////////////////
DDGActor::DDGActor(dtGame::GameActorProxy& parent)
   : ShipActor(parent)
   , forwardStackEngaged(true)
   , afterStackEngaged(true)
{
   mCoordSys = &VehicleActor::CoordSys::SYS_ABS;
   SetPosition(position);

   float deltaZ        = 4.0f;
   float deltaY        = -3.0f;
   float deltaWakeY    = 25.0f;
   float deltaRoosterY = -2.0f;

   portWake = new dtCore::ParticleSystem;
   portWake->LoadFile("Particles/wake.osg");
   position.Set(3.75f, 60.0f + deltaWakeY, -2.0f + deltaZ, 0.0f, 0.0f, 0.0f);
   SetPortWake(portWake.get(), position);
   AddChild(portWake.get());

   //stbd wake
   starboardWake = new dtCore::ParticleSystem;
   starboardWake->LoadFile("Particles/wake.osg");
   position.Set(-3.75f, 60.0f + deltaWakeY, -3.0f + deltaZ, 0.0f, 0.0f, 0.0f);
   SetStbdWake(starboardWake.get(), position);
   AddChild(starboardWake.get());

   //port bow wake
   portBowWake = new dtCore::ParticleSystem;
   portBowWake->LoadFile("Particles/breakingWake.osg");
   position.Set(1.0f, -63.0f + deltaY, 0/*-5.5f + deltaZ*/, -40.0f, 0.0f, 0.0f);
   SetPortBowWake(portBowWake.get(), position);
   AddChild(portBowWake.get());

   //stbd bow wake
   stbdBowWake = new dtCore::ParticleSystem;
   stbdBowWake->LoadFile("Particles/breakingWake.osg");
   position.Set(-1.0f, -63.0f + deltaY, 0/*-5.5f + deltaZ*/, 40.0f, 0.0f, 0.0f);
   SetStbdBowWake(stbdBowWake.get(), position);
   AddChild(stbdBowWake.get());

   //port rooster tail
   portRooster = new dtCore::ParticleSystem;
   portRooster->LoadFile("Particles/breakingWake.osg");
   position.Set(0.25f, 81.0f + deltaRoosterY, 0/*-4.5f + deltaZ*/, 45.0f, 0.0f, 0.0f);
   SetPortRooster(portRooster.get(), position);
   AddChild(portRooster.get());

   //stbd rooster tail
   stbdRooster = new dtCore::ParticleSystem;
   stbdRooster->LoadFile("Particles/breakingWake.osg");
   position.Set(-0.25f, 81.0f + deltaRoosterY, 0/*-4.5f + deltaZ*/, -45.0f, 0.0f, 0.0f);
   SetStbdRooster(stbdRooster.get(), position);
   AddChild(stbdRooster.get());

   //forward stack
   fwdStack = new dtCore::ParticleSystem;
   fwdStack->LoadFile("Particles/stackSmoke.osg");
   position.Set(0.0f, -6.0f, 22.0f + deltaZ, 0.0f, 0.0f, 0.0f);
   SetForwardStack(fwdStack.get(), position);
   AddChild(fwdStack.get());

   //after stack
   aftStack = new dtCore::ParticleSystem;
   aftStack->LoadFile("Particles/stackSmoke.osg");
   position.Set(0.0f, 15.0f, 18.0f + deltaZ, 0.0f, 0.0f, 0.0f);
   SetAfterStack(aftStack.get(), position);
   AddChild(aftStack.get());
}

DDGActor::~DDGActor()
{

}

void DDGActor::SetModelPosition()
{
   dtCore::Transform tempPos;
   dtCore::Transform newPos = GetPosition();

   float speedOffset = 3.5f * GetSpeed() / GetMaxAheadSpeed();
   newPos = GetPosition();
   SetTransform(newPos, *mCoordSys == VehicleActor::CoordSys::SYS_ABS ? ABS_CS : REL_CS);

   if (CheckWake(portWake.get()))
   {
      tempPos = Offset2DPosition(&newPos, &portWakePosition);
      portWake->SetTransform(tempPos);
   }

   if (CheckWake(stbdWake.get()))
   {
      tempPos = Offset2DPosition(&newPos, &stbdWakePosition);
      stbdWake->SetTransform(tempPos);
   }

   if (CheckWake(portBowWake.get()))
   {
      tempPos = Offset2DPosition(&newPos, &portBowWakePosition);
      AdjustZ(&tempPos, speedOffset, true);
      portBowWake->SetTransform(tempPos);
   }

   if (CheckWake(stbdBowWake.get()))
   {
      tempPos = Offset2DPosition(&newPos, &stbdBowWakePosition);
      AdjustZ(&tempPos, speedOffset, true);
      stbdBowWake->SetTransform(tempPos);
   }

   if (CheckWake(portRooster.get()))
   {
      tempPos = Offset2DPosition(&newPos, &portRoosterPosition);
      AdjustZ(&tempPos, 1.2f * speedOffset, true);
      AdjustX(&tempPos, GetHeel() / -9.0f, true);
      portRooster->SetTransform(tempPos);
   }

   if (CheckWake(stbdRooster.get()))
   {
      tempPos = Offset2DPosition(&newPos, &stbdRoosterPosition);
      AdjustZ(&tempPos, 1.2f * speedOffset, true);
      AdjustX(&tempPos, GetHeel() / -9.0f, true);
      stbdRooster->SetTransform(tempPos);
   }

   if (CheckStack(forwardStack.get(), forwardStackEngaged))
   {
   }

   if (CheckStack(afterStack.get(), forwardStackEngaged))
   {
   }

   if (stackSound.valid())
   {
      float gain = GetThrottlePosition() / GetMaxAheadSpeed();
      float pitch = GetThrottlePosition() / GetMaxAheadSpeed();

      if (gain > 1.0f)
      {
         gain = 1.0f;
      }

      if (pitch > 1.0f)
      {
         pitch = 1.0f;
      }

      stackSound->SetGain(gain);
      stackSound->SetPitch(pitch);
   }
}

bool DDGActor::CheckStack(dtCore::ParticleSystem* stack, bool stackEngaged)
{
   if (mEngineRunning)
   {
      EngageForwardStack();
      EngageAfterStack();
   }
   else
   {
      DisengageForwardStack();
      DisengageAfterStack();
   }

   if (stack != NULL)
   {
      if (stack->IsEnabled())
      {
         if (!stackEngaged)
         {
            stack->SetEnabled(false);
         }
      }
      else
      {
         if (stackEngaged)
         {
            stack->SetEnabled(true);
         }
      }

      return stack->IsEnabled();
   }
   else
   {
      return false;
   }
}

void DDGActor::SetForwardStack(dtCore::ParticleSystem* tForwardStack, dtCore::Transform tForwardStackPosition)
{
   if (tForwardStack != NULL)
   {
      forwardStack = tForwardStack;
      forwardStackPosition = tForwardStackPosition;
      forwardStack->SetTransform(forwardStackPosition, REL_CS);
   }
}

void DDGActor::SetAfterStack(dtCore::ParticleSystem* tAfterStack, dtCore::Transform tAfterStackPosition)
{
   if (tAfterStack != NULL)
   {
      afterStack = tAfterStack;
      afterStackPosition = tAfterStackPosition;
      afterStack->SetTransform(afterStackPosition, REL_CS);
   }
}

void DDGActor::EngageForwardStack()
{
   forwardStackEngaged = true;
}

void DDGActor::DisengageForwardStack()
{
   forwardStackEngaged = false;
}

void DDGActor::EngageAfterStack()
{
   afterStackEngaged = true;
}

void DDGActor::DisengageAfterStack()
{
   afterStackEngaged = false;
}

void DDGActor::SetStackSound(dtAudio::Sound* tStackSound, const dtCore::Transform& tStackSoundPosition)
{
   stackSound = tStackSound;
   stackSoundPosition = tStackSoundPosition;
   stackSound->SetTransform(tStackSoundPosition, dtCore::Transformable::REL_CS);
   stackSound->SetLooping(true);
}

void DDGActor::PlayStackSound()
{
   if (stackSound != NULL)
   {
      stackSound->Play();
   }
}

void DDGActor::StopStackSound()
{
   if (stackSound != NULL)
   {
      stackSound->Stop();
   }
}

void DDGActor::LoadFile(const std::string& fileName)
{
   osg::Node* node = dtCore::Loadable::LoadFile(fileName);
   if (node == NULL)
   {
      LOG_ERROR("Failed to load the ddg model file: " + fileName);
      return;
   }

   GetMatrixNode()->addChild(node);
}
