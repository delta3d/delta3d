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
#include <dtCore/floatshaderparameter.h>
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
   static const int CYCLETIME = 3;
   const dtGame::TickMessage &tick = static_cast<const dtGame::TickMessage&>(tickMessage);
   //float deltaSimTime = tick.GetDeltaSimTime();

   if (mCurrentShader.valid() && mIsTargeted)
   {
      dtCore::FloatShaderParameter *param = 
         dynamic_cast<dtCore::FloatShaderParameter*> (mCurrentShader->FindParameter("TimeDilation"));
      if (param != NULL)
      {
         // Calc a number from 0 to 1 over X seconds. 
         double simTime = GetGameActorProxy().GetGameManager()->GetSimulationTime();
         float variance = simTime - ((int) simTime) + ((int)simTime)%CYCLETIME; // a num from 0 to 4.999
         float timeDistort = variance/(float)CYCLETIME;
         param->SetValue(timeDistort);// a num from 1 to 0 to 1.
         // old code to make the num go from 1 to 0 to 1 over X seconds
         //float timeDistort = fabs(variance/(float)CYCLETIME * 2.0f - 1.0f);
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActor::TickRemote(const dtGame::Message &tickMessage)
{
   const dtGame::TickMessage &tick = static_cast<const dtGame::TickMessage&>(tickMessage);
   //float deltaSimTime = tick.GetDeltaSimTime();
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
            mCurrentHealth -= 25;
            mCurrentHealth = dtUtil::Max(mCurrentHealth, 0);
            if( mCurrentHealth <= 0 )
            {
               mLargeExplosion->SetEnabled(true);
               SwitchVisitor switchVisitor("Destroyed");
               GetOSGNode()->accept(switchVisitor);

               mCurrentShaderName = "Normal";
               ApplyMyShader();
            }
            else 
            {
               mSmallExplosion->SetEnabled(true);
            }
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
         mCurrentShaderName = "Normal";
      }

      ApplyMyShader();
   } 


}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActor::ApplyMyShader()
{
   mCurrentShader = dtCore::ShaderManager::GetInstance().FindShader(mCurrentShaderName,"TargetShaders");
   if (mCurrentShader.valid())
      dtCore::ShaderManager::GetInstance().AssignShader( *mCurrentShader.get(), *GetOSGNode() );
   else 
      LOG_ERROR("KillableTargetActor could not load shader for group[TargetShaders] with name [" + mCurrentShaderName + "]"); 
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
   mCurrentHealth = mMaxHealth;
   mIsTargeted = false;

   mCurrentShaderName = "Normal";
   ApplyMyShader();

	mSmallExplosion->SetEnabled(false);
   mSmallExplosion->SetTransform(mOriginalPosition);
	mLargeExplosion->SetEnabled(false);
   mLargeExplosion->SetTransform(mOriginalPosition);

   SwitchVisitor switchVisitor("Good");
   GetOSGNode()->accept(switchVisitor);
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

   // "Max Health" property
   AddProperty( new dtDAL::IntActorProperty( "Max Health", "Max Health",
      dtDAL::MakeFunctor( actor, &KillableTargetActor::SetMaxHealth ),
      dtDAL::MakeFunctorRet( actor, &KillableTargetActor::GetMaxHealth ),
      "Sets/gets the target max health (i.e. the damaged it can sustain before going boom.", GROUP ) );
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActorProxy::CreateActor()
{
   mActor = new KillableTargetActor(*this);
}

///////////////////////////////////////////////////////////////////////////////
void KillableTargetActorProxy::OnEnteredWorld()
{

   // Register an invokable for tick messages. Local or Remote only, not both!
   if (IsRemote())
      RegisterForMessages(dtGame::MessageType::TICK_REMOTE,
         dtGame::GameActorProxy::TICK_REMOTE_INVOKABLE);
   else
   {
      RegisterForMessages(dtGame::MessageType::TICK_LOCAL,
         dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);

      // Register for the messages using the default Invokable - ProcessMessage()
      RegisterForMessages(dtGame::MessageType::INFO_GAME_EVENT);
      RegisterForMessages(TutorialMessageType::TANK_TARGET_CHANGED);
   }

   dtActors::GameMeshActorProxy::OnEnteredWorld();
}
