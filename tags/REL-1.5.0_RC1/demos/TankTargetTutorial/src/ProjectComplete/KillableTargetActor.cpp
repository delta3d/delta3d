/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation & MOVES Institute
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
 * @author Curtiss Murphy
 * @author Chris Osborn
 */
#include "KillableTargetActor.h"
#include "TargetChanged.h"
#include <dtDAL/enginepropertytypes.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/basemessages.h>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>
#include <dtCore/shadermanager.h>
#include <dtCore/shaderparameter.h>
#include <dtCore/shaderparamoscillator.h>
#include <dtCore/scene.h>
#include <osg/Switch>

KillableTargetActor::SwitchVisitor::SwitchVisitor( const std::string& state ) :
   osg::NodeVisitor( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ),
   mSwitchState(state)
{
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActor::SwitchVisitor::apply( osg::Switch& switchNode )
{
   for( unsigned i = 0; i < switchNode.getNumChildren(); ++i )
   {
      if( switchNode.getChild(i)->getName() == mSwitchState )
      {
         switchNode.setSingleChildOn(i);
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActor::SwitchVisitor::SetSwitchState( const std::string& state )
{
   mSwitchState = state;
}

///////////////////////////////////////////////////////////////////////////////
const std::string& KillableTargetActor::SwitchVisitor::GetSwitchState() const
{
   return mSwitchState;
}

///////////////////////////////////////////////////////////////////////////////
KillableTargetActor::KillableTargetActor(dtGame::GameActorProxy &proxy) : 
   dtActors::GameMeshActor(proxy),
   mShaderEffect(),
   mMaxHealth(100),
   mCurrentHealth(0),
   mIsTargeted(false)
{
   SetName("KillableTarget");
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActor::TickLocal(const dtGame::Message &tickMessage)
{
   const dtGame::TickMessage &tick = static_cast<const dtGame::TickMessage&>(tickMessage);
   //float deltaSimTime = tick.GetDeltaSimTime();

   // Do something here locally if you want to. Note - if you don't need to do anything 
   // here then you should NOT call RegisterForMessages(dtGame::MessageType::TICK_LOCAL, ...)
   // in OnEnteredWorld().
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActor::TickRemote(const dtGame::Message &tickMessage)
{
   const dtGame::TickMessage &tick = static_cast<const dtGame::TickMessage&>(tickMessage);
   //float deltaSimTime = tick.GetDeltaSimTime();

   // Do something here locally if you want to. Note - if you don't need to do anything 
   // here then you should NOT call RegisterForMessages(dtGame::MessageType::TICK_REMOTE, ...)
   // in OnEnteredWorld().
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActor::ProcessMessage(const dtGame::Message &message)
{
   // HANDLE GAME EVENT MESSAGE - "TankFired"
   if (message.GetMessageType() == dtGame::MessageType::INFO_GAME_EVENT)
   {
      const dtGame::GameEventMessage &eventMsg = 
         static_cast<const dtGame::GameEventMessage&>(message);

      // Note, we are using strings which aren't constants.  In a real application, these 
      // event names should be stored in some sort of shared place and should be constants...
      if(eventMsg.GetGameEvent() != 0 && eventMsg.GetGameEvent()->GetName() == "TankFired")
      {
         if( mIsTargeted && mCurrentHealth > 0)
         {
            SetCurrentHealth(GetCurrentHealth() - 25);
         }
      }
      // test our shaders 
      else if(eventMsg.GetGameEvent() != 0 && eventMsg.GetGameEvent()->GetName() == "TestShaders")
      {
         dtCore::ShaderManager::GetInstance().Clear();
         dtCore::ShaderManager::GetInstance().LoadShaderDefinitions("Shaders/TutorialShaderDefs.xml", false);
         ApplyMyShader();
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

      if( mIsTargeted && mCurrentHealth > 0)
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
   dtCore::ShaderParamOscillator *timerParam;

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
         prevTime = timerParam->GetValue();

      // X DILATION
      timerParam = dynamic_cast<dtCore::ShaderParamOscillator*> (mCurrentShader->FindParameter("MoveXDilation"));
      if (timerParam != NULL)
         prevX = timerParam->GetValue();

      // Y DILATION
      timerParam = dynamic_cast<dtCore::ShaderParamOscillator*> (mCurrentShader->FindParameter("MoveYDilation"));
      if (timerParam != NULL)
         prevY = timerParam->GetValue();

      // Z DILATION
      timerParam = dynamic_cast<dtCore::ShaderParamOscillator*> (mCurrentShader->FindParameter("MoveZDilation"));
      if (timerParam != NULL)
         prevZ = timerParam->GetValue();
   }

   // clean up any previous shaders, if any
   dtCore::ShaderManager::GetInstance().UnassignShaderFromNode(*GetOSGNode());

   dtCore::Shader *templateShader = dtCore::ShaderManager::GetInstance().
      FindShaderTemplate(mCurrentShaderName,"TargetShaders");
   if (templateShader != NULL)
   {
      mCurrentShader = dtCore::ShaderManager::GetInstance().
         AssignShaderFromTemplate( *templateShader, *GetOSGNode() );

      // Put the shader values back (again, to avoid jumping since we are moving XYZ in our shader)
      // TIME DILATION
      timerParam = dynamic_cast<dtCore::ShaderParamOscillator*> (mCurrentShader->FindParameter("TimeDilation"));
      if (timerParam != NULL)
         timerParam->SetValue(prevTime);

      // X DILATION
      timerParam = dynamic_cast<dtCore::ShaderParamOscillator*> (mCurrentShader->FindParameter("MoveXDilation"));
      if (timerParam != NULL)
         timerParam->SetValue(prevX);

      // Y DILATION
      timerParam = dynamic_cast<dtCore::ShaderParamOscillator*> (mCurrentShader->FindParameter("MoveYDilation"));
      if (timerParam != NULL)
         timerParam->SetValue(prevY);

      // Z DILATION
      timerParam = dynamic_cast<dtCore::ShaderParamOscillator*> (mCurrentShader->FindParameter("MoveZDilation"));
      if (timerParam != NULL)
         timerParam->SetValue(prevZ);
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
   if( currentHealth == 0 && mCurrentHealth > currentHealth && mCurrentHealth != 0 )
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
   dtActors::GameMeshActor::OnEnteredWorld();

   // small explosion
	mSmallExplosion = new dtCore::ParticleSystem();
	mSmallExplosion->LoadFile("Particles/explosion_small.osg",true);
   GetGameActorProxy().GetGameManager()->GetScene().AddDrawable(mSmallExplosion.get());
	//AddChild(mSmallExplosion.get());

   // large explosion
	mLargeExplosion = new dtCore::ParticleSystem();
	mLargeExplosion->LoadFile("Particles/explosion_large.osg",true);
   GetGameActorProxy().GetGameManager()->GetScene().AddDrawable(mLargeExplosion.get());
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

   dtActors::GameMeshActorProxy::BuildPropertyMap();
   KillableTargetActor &actor = dynamic_cast<KillableTargetActor&>(GetGameActor());

   // override the default behavior on initial creation. This can be overridden by the user in the map 
   actor.SetUseCache(false);

   // "Max Health" property
   AddProperty( new dtDAL::IntActorProperty( "Max Health", "Max Health",
      dtDAL::MakeFunctor( actor, &KillableTargetActor::SetMaxHealth ),
      dtDAL::MakeFunctorRet( actor, &KillableTargetActor::GetMaxHealth ),
      "Sets/gets the target max health (i.e. the damaged it can sustain before going boom)", GROUP ) );

   // "Current Health" property
   AddProperty( new dtDAL::IntActorProperty( "Current Health", "Current Health",
      dtDAL::MakeFunctor( actor, &KillableTargetActor::SetCurrentHealth ),
      dtDAL::MakeFunctorRet( actor, &KillableTargetActor::GetCurrentHealth ),
      "Sets/gets the target current health", GROUP ) );
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActorProxy::CreateActor()
{
   KillableTargetActor *actor = new KillableTargetActor(*this);
   SetActor(*actor);
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

   dtActors::GameMeshActorProxy::OnEnteredWorld();
}
