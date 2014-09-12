// ---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
//
// StateNode.cpp
// ---------------------------------------------------------------------------------------------------------------------------------

#include "StateNode.h"
#include <dtAI/aiplugininterface.h>
#include <dtAI/aiinterfaceactor.h>
#include <dtAI/aiactorregistry.h>
//#include "../Components/AIComponent.h"
#include <dtDirector/director.h>
#include <dtGame/gamemanager.h>

namespace dtExample
{


   /////////////////////////////////////////////////////////////////////////////
   StateNode::StateNode()
   : BaseClass()
   {
      AddAuthor("Bradley Anderegg");
      AddAuthor("LEgregius");
   }

   /////////////////////////////////////////////////////////////////////////////
   StateNode::~StateNode()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void StateNode::Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph)
   {
      BaseClass::Init(nodeType, graph);
      FindAIInterface();
   }

   /////////////////////////////////////////////////////////////////////////////
   bool StateNode::IsValid() const
   {
      return GetTargetActor() != NULL;
   }


   /////////////////////////////////////////////////////////////////////////////
   void StateNode::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();
      dtCore::ActorIDActorProperty* actorProp = new dtCore::ActorIDActorProperty(
            "Actor", "Actor",
            dtCore::ActorIDActorProperty::SetFuncType(this, &StateNode::SetActor),
            dtCore::ActorIDActorProperty::GetFuncType(this, &StateNode::GetActor),
            "", "The actor that was spawned.");

      mValues.push_back(dtDirector::ValueLink(this, actorProp));

   }

   /////////////////////////////////////////////////////////////////////////////
   bool StateNode::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      return BaseClass::Update(simDelta, delta, input, firstUpdate);
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool StateNode::CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value)
   {
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   void StateNode::FindAIInterface()
   {
      dtAI::AIInterfaceActor* aiInterfaceActorProxy = NULL;
      dtGame::GameManager* gm = GetDirector()->GetGameManager();
      if (gm != NULL)
      {
         gm->FindActorByType(*dtAI::AIActorRegistry::AI_INTERFACE_ACTOR_TYPE, aiInterfaceActorProxy);
      }
      mAIInterface = aiInterfaceActorProxy;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtAI::AIPluginInterface* StateNode::GetAIInterface()
   {
      if (!mAIInterface.valid())
      {
         FindAIInterface();
      }

      if (mAIInterface.valid())
      {
         return mAIInterface->GetAIInterface();
      }
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtAI::AIPluginInterface* StateNode::GetAIInterface() const
   {
      if (mAIInterface.valid())
      {
         return mAIInterface->GetAIInterface();
      }
      return NULL;
   }

   dtGame::GameActorProxy* StateNode::GetTargetActor()
   {
      return mTarget.get();
   }
   const dtGame::GameActorProxy* StateNode::GetTargetActor() const { return mTarget.get(); }


   DT_IMPLEMENT_ACCESSOR_GETTER(StateNode, dtCore::UniqueId, Actor);
   void StateNode::SetActor(const dtCore::UniqueId& id)
   {
      mActor = id;
      dtGame::GameActorProxy* target = NULL;
      if (GetDirector()->GetGameManager() != NULL)
      {
         GetDirector()->GetGameManager()->FindActorById(mActor, target);
      }
      mTarget = target;
   }

   //---------------------
}//namespace dtExample


//---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
// StateNode.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

