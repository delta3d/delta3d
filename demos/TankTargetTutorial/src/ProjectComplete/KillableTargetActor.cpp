/* -*-c++-*-
 * TutorialLibrary - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2006-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @author Curtiss Murphy
 * @author Chris Osborn
 */

#include "KillableTargetActor.h"
#include "TargetChanged.h"

#include <dtCore/scene.h>
#include <dtCore/shadermanager.h>
#include <dtCore/shaderparameter.h>
#include <dtCore/shaderparamoscillator.h>

#include <dtCore/functor.h>
#include <dtCore/intactorproperty.h>

#include <dtGame/actorupdatemessage.h>
#include <dtGame/basemessages.h>
#include <dtGame/gamemanager.h>

#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>

#include <osg/Switch>

KillableTargetActor::SwitchVisitor::SwitchVisitor(const std::string& state)
   : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
   , mSwitchState(state)
{
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActor::SwitchVisitor::apply(osg::Switch& switchNode)
{
   for (unsigned i = 0; i < switchNode.getNumChildren(); ++i)
   {
      if (switchNode.getChild(i)->getName() == mSwitchState)
      {
         switchNode.setSingleChildOn(i);
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActor::SwitchVisitor::SetSwitchState(const std::string& state)
{
   mSwitchState = state;
}

///////////////////////////////////////////////////////////////////////////////
const std::string& KillableTargetActor::SwitchVisitor::GetSwitchState() const
{
   return mSwitchState;
}

///////////////////////////////////////////////////////////////////////////////
KillableTargetActor::KillableTargetActor(dtGame::GameActorProxy& parent)
   : dtActors::GameMeshDrawable(parent)
   , mShaderEffect()
   , mMaxHealth(100)
   , mCurrentHealth(0)
   , mIsTargeted(false)
{
   SetName("KillableTarget");
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActor::OnTickLocal(const dtGame::TickMessage& tickMessage)
{
   //const dtGame::TickMessage& tick = static_cast<const dtGame::TickMessage&>(tickMessage);
   //float deltaSimTime = tick.GetDeltaSimTime();

   // Do something here locally if you want to. Note - if you don't need to do anything
   // here then you should NOT call RegisterForMessages(dtGame::MessageType::TICK_LOCAL, ...)
   // in OnEnteredWorld().
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActor::OnTickRemote(const dtGame::TickMessage& tickMessage)
{
   //const dtGame::TickMessage& tick = static_cast<const dtGame::TickMessage&>(tickMessage);
   //float deltaSimTime = tick.GetDeltaSimTime();

   // Do something here locally if you want to. Note - if you don't need to do anything
   // here then you should NOT call RegisterForMessages(dtGame::MessageType::TICK_REMOTE, ...)
   // in OnEnteredWorld().
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActor::ProcessMessage(const dtGame::Message& message)
{
   // HANDLE GAME EVENT MESSAGE - "TankFired"
   if (message.GetMessageType() == dtGame::MessageType::INFO_GAME_EVENT)
   {
      const dtGame::GameEventMessage& eventMsg =
         static_cast<const dtGame::GameEventMessage&>(message);

      // Note, we are using strings which aren't constants.  In a real application, these
      // event names should be stored in some sort of shared place and should be constants...
      if (eventMsg.GetGameEvent() != 0 && eventMsg.GetGameEvent()->GetName() == "TankFired")
      {
         if (mIsTargeted && mCurrentHealth > 0)
         {
            SetCurrentHealth(GetCurrentHealth() - 25);
         }
      }
      // test our shaders
      else if (eventMsg.GetGameEvent() != 0 && eventMsg.GetGameEvent()->GetName() == "TestShaders")
      {
         // Note, this behavior is now done in the InputComponent.cpp using the new Shader method.
         //dtCore::ShaderManager::GetInstance().Clear();
         //dtCore::ShaderManager::GetInstance().LoadShaderDefinitions("Shaders/TutorialShaderDefs.xml", false);
         //ApplyMyShader();
      }
      // reset me!
      else if (eventMsg.GetGameEvent() != 0 && eventMsg.GetGameEvent()->GetName() == "ResetStuff")
      {
         SetTransform(mOriginalPosition);
         ResetState();
      }
   }
   // HANDLE TARGET CHANGED MESSAGE
   else if (message.GetMessageType() == TutorialMessageType::TANK_TARGET_CHANGED)
   {
      const TargetChangedMessage& targetChanged = static_cast<const TargetChangedMessage&>(message);
      mIsTargeted = (targetChanged.GetNewTargetUniqueId() ==  GetUniqueId());

      if (mIsTargeted && mCurrentHealth > 0)
      {
         // Ahhhh! Get that gun outta my face!
         mCurrentShaderName = "Green";
      }
      else
      {
         //mCurrentShaderName = "Green";
         mCurrentShaderName = "Normal";
      }

      ApplyMyShader();
   }

}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActor::ApplyMyShader()
{
   float prevTime = 0.0, prevX = 0.0, prevY = 0.0, prevZ = 0.0;
   dtCore::ShaderParamOscillator* timerParam = NULL;

   if (mCurrentShader != NULL && mCurrentShader->GetName() == mCurrentShaderName)
   {
      // don't reload stuff or we kill our processing.
      return;
   }

   // if we had a previous shader, then get the current values of our float timers.  Then,
   // after we reassign them, we can put them back where they were.  Avoids 'jumping'
   if (mCurrentShader != NULL)
   {
      // TIME DILATION
      timerParam = dynamic_cast<dtCore::ShaderParamOscillator*> (mCurrentShader->FindParameter("TimeDilation"));
      if (timerParam != NULL)
      {
         prevTime = timerParam->GetValue();
      }

      // X DILATION
      timerParam = dynamic_cast<dtCore::ShaderParamOscillator*> (mCurrentShader->FindParameter("MoveXDilation"));
      if (timerParam != NULL)
      {
         prevX = timerParam->GetValue();
      }

      // Y DILATION
      timerParam = dynamic_cast<dtCore::ShaderParamOscillator*> (mCurrentShader->FindParameter("MoveYDilation"));
      if (timerParam != NULL)
      {
         prevY = timerParam->GetValue();
      }

      // Z DILATION
      timerParam = dynamic_cast<dtCore::ShaderParamOscillator*> (mCurrentShader->FindParameter("MoveZDilation"));
      if (timerParam != NULL)
      {
         prevZ = timerParam->GetValue();
      }
   }

   // clean up any previous shaders, if any
   dtCore::ShaderManager::GetInstance().UnassignShaderFromNode(*GetOSGNode());

   dtCore::ShaderProgram* templateShader = dtCore::ShaderManager::GetInstance().
      FindShaderPrototype(mCurrentShaderName,"TargetShaders");
   if (templateShader != NULL)
   {
      mCurrentShader = dtCore::ShaderManager::GetInstance().
         AssignShaderFromPrototype(*templateShader, *GetOSGNode());

      // Put the shader values back (again, to avoid jumping since we are moving XYZ in our shader)
      // TIME DILATION
      timerParam = dynamic_cast<dtCore::ShaderParamOscillator*> (mCurrentShader->FindParameter("TimeDilation"));
      if (timerParam != NULL)
      {
         timerParam->SetValue(prevTime);
      }

      // X DILATION
      timerParam = dynamic_cast<dtCore::ShaderParamOscillator*> (mCurrentShader->FindParameter("MoveXDilation"));
      if (timerParam != NULL)
      {
         timerParam->SetValue(prevX);
      }

      // Y DILATION
      timerParam = dynamic_cast<dtCore::ShaderParamOscillator*> (mCurrentShader->FindParameter("MoveYDilation"));
      if (timerParam != NULL)
      {
         timerParam->SetValue(prevY);
      }

      // Z DILATION
      timerParam = dynamic_cast<dtCore::ShaderParamOscillator*> (mCurrentShader->FindParameter("MoveZDilation"));
      if (timerParam != NULL)
      {
         timerParam->SetValue(prevZ);
      }
   }
   else
   {
      LOG_ERROR("KillableTargetActor could not load shader for group[TargetShaders] with name [" + mCurrentShaderName + "]");
      mCurrentShader = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActor::SetShaderEffect(const std::string& shaderEffect)
{
   mShaderEffect = shaderEffect;
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActor::SetMaxHealth(int maxHealth)
{
   mMaxHealth = maxHealth;
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActor::SetCurrentHealth(int currentHealth)
{
   currentHealth = dtUtil::Max(currentHealth, 0);
   if (currentHealth == 0 && mCurrentHealth > currentHealth && mCurrentHealth != 0)
   {
      mLargeExplosion->SetEnabled(true);
      SwitchVisitor switchVisitor("Destroyed");
      GetOSGNode()->accept(switchVisitor);

      mCurrentShaderName = "Normal";
      ApplyMyShader();
   }
   else if (currentHealth != 0 && mCurrentHealth > currentHealth)
   {
      mSmallExplosion->SetEnabled(true);
   }
   else if (currentHealth > 0 && mCurrentHealth == 0)
   {
      SwitchVisitor switchVisitor("Good");
      GetOSGNode()->accept(switchVisitor);
   }
   mCurrentHealth = currentHealth;

   GetGameActorProxy().NotifyFullActorUpdate();
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActor::OnEnteredWorld()
{
   // Initialize private state
   dtActors::GameMeshDrawable::OnEnteredWorld();

   // small explosion
   mSmallExplosion = new dtCore::ParticleSystem();
   mSmallExplosion->LoadFile("Particles/explosion_small.osg",true);
   GetGameActorProxy().GetGameManager()->GetScene().AddChild(mSmallExplosion.get());
   //AddChild(mSmallExplosion.get());

   // large explosion
   mLargeExplosion = new dtCore::ParticleSystem();
   mLargeExplosion->LoadFile("Particles/explosion_large.osg",true);
   GetGameActorProxy().GetGameManager()->GetScene().AddChild(mLargeExplosion.get());
   //AddChild(mLargeExplosion.get());

   GetTransform(mOriginalPosition);
   ResetState();
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActor::ResetState()
{
   mIsTargeted = false;

   // Force the shader to reload
   mCurrentShaderName = "Normal";
   mCurrentShader = NULL;
   //mTextureDilationParam = NULL;
   ApplyMyShader();

   mSmallExplosion->SetEnabled(false);
   mSmallExplosion->SetTransform(mOriginalPosition);
   mLargeExplosion->SetEnabled(false);
   mLargeExplosion->SetTransform(mOriginalPosition);

   SetCurrentHealth(mMaxHealth);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

KillableTargetActorProxy::KillableTargetActorProxy()
{
   SetClassName("KillableTarget");
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActorProxy::BuildPropertyMap()
{
   const std::string GROUP = "KillableTarget";

   dtActors::GameMeshActor::BuildPropertyMap();
   KillableTargetActor* actor = GetDrawable<KillableTargetActor>();

   // override the default behavior on initial creation. This can be overridden by the user in the map
   actor->SetUseCache(false);

   // "Max Health" property
   AddProperty(new dtCore::IntActorProperty("Max Health", "Max Health",
      dtCore::IntActorProperty::SetFuncType(actor, &KillableTargetActor::SetMaxHealth),
      dtCore::IntActorProperty::GetFuncType(actor, &KillableTargetActor::GetMaxHealth),
      "Sets/gets the target max health (i.e. the damaged it can sustain before going boom)", GROUP));

   // "Current Health" property
   AddProperty(new dtCore::IntActorProperty("Current Health", "Current Health",
      dtCore::IntActorProperty::SetFuncType(actor, &KillableTargetActor::SetCurrentHealth),
      dtCore::IntActorProperty::GetFuncType(actor, &KillableTargetActor::GetCurrentHealth),
      "Sets/gets the target current health", GROUP));
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActorProxy::CreateDrawable()
{
   KillableTargetActor* actor = new KillableTargetActor(*this);
   SetDrawable(*actor);
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActorProxy::OnEnteredWorld()
{
   // Register for invokables - NEVER register for both Local & Remote ticks!
   // ONLY register for messages you need. With lots of actors, the overhead
   // can hit your performance.
   //if (IsRemote())
      // We don't need a remote tick, so don't register for it! But here is how...
      //RegisterForMessages(dtGame::MessageType::TICK_REMOTE, dtGame::GameActorProxy::TICK_REMOTE_INVOKABLE);
   //else
      // We don't need a local tick, so don't register for it! But here is how...
      //RegisterForMessages(dtGame::MessageType::TICK_LOCAL, dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);

   // Register for the messages using the default Invokable - ProcessMessage()
   RegisterForMessages(dtGame::MessageType::INFO_GAME_EVENT);
   RegisterForMessages(TutorialMessageType::TANK_TARGET_CHANGED);

   dtActors::GameMeshActor::OnEnteredWorld();
}
