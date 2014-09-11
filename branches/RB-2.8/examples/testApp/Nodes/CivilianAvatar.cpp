// ---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 04/07/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
//
// CivilianAvatar.cpp
// ---------------------------------------------------------------------------------------------------------------------------------

#include "CivilianAvatar.h"
#include "../civilianaiactorcomponent.h"
#include "StateNode.h"
#include "SelectDestination.h"
#include <dtGame/gamemanager.h>
#include <dtCore/propertymacros.h>


namespace dtExample
{


   /////////////////////////////////////////////////////////////////////////////
   CivilianAvatar::CivilianAvatar()
   : BaseClass()
   {
      AddAuthor("Bradley Anderegg");
      AddAuthor("LEgregius");
   }

   /////////////////////////////////////////////////////////////////////////////
   CivilianAvatar::~CivilianAvatar()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR(CivilianAvatar, std::string, NewActorName)
   DT_IMPLEMENT_ACCESSOR(CivilianAvatar, osg::Vec3, NewActorPos)
   DT_IMPLEMENT_ACCESSOR(CivilianAvatar, dtCore::ResourceDescriptor, NewActorPrefab)
   DT_IMPLEMENT_ACCESSOR(CivilianAvatar, dtCore::UniqueId, ExistingActor)

   DT_IMPLEMENT_ACCESSOR(CivilianAvatar, std::string, DestinationName)


   /////////////////////////////////////////////////////////////////////////////
   void CivilianAvatar::Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph)
   {
      BaseClass::Init(nodeType, graph);

   }

   /////////////////////////////////////////////////////////////////////////////
   dtGame::GameActorProxy* CivilianAvatar::CreatePrefab(dtGame::GameManager& gm)
   {
      //   AIComponent* comp = NULL;
      dtGame::GameActorProxy* act = NULL;
      std::vector<dtCore::RefPtr<dtCore::BaseActorObject> > actors;
      gm.CreateActorsFromPrefab(GetNewActorPrefab(), actors);

      std::vector<dtCore::RefPtr<dtCore::BaseActorObject> >::iterator i, iend;
      i = actors.begin();
      iend = actors.end();
      for (; i != iend; ++i)
      {
         dtCore::RefPtr<dtCore::BaseActorObject> actor = *i;
         if (actor->IsGameActor())
         {
            act = static_cast<dtGame::GameActorProxy*>(actor.get());
            act->SetTranslation(GetNewActorPos());
            if (act->HasComponent(CivilianAIActorComponent::TYPE))
            {
               gm.AddActor(*act, false, false);
               break;
            }
         }
      }
      return act;
   }


   /////////////////////////////////////////////////////////////////////////////
   void CivilianAvatar::InitActor(dtGame::GameActorProxy* actor)
   {
      CivilianAIActorComponent* civAI = actor->GetComponent<CivilianAIActorComponent>();
      if (civAI == NULL)
      {
         LOG_ERROR("Actor")
      }
      civAI->Initialize();
   }

   /////////////////////////////////////////////////////////////////////////////
   void CivilianAvatar::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      dtUtil::RefString GROUP("Avatar");

      typedef dtCore::PropertyRegHelper<CivilianAvatar&, CivilianAvatar> RegHelperType;
      RegHelperType propReg(*this, this, GROUP);

      DT_REGISTER_PROPERTY(NewActorName, "The name of the new Actor", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(NewActorPos, "The pos of the new Actor", RegHelperType, propReg);
      DT_REGISTER_ACTOR_ID_PROPERTY("", ExistingActor, "Existing Actor", "The id of the Actor", RegHelperType, propReg);
      DT_REGISTER_RESOURCE_PROPERTY(dtCore::DataType::PREFAB, NewActorPrefab, "New Actor Prefab", "A prefab for the new Actor", RegHelperType, propReg);

      DT_REGISTER_PROPERTY(DestinationName, "The name of the person's destination.", RegHelperType, propReg);
   }

   /////////////////////////////////////////////////////////////////////////////
   void CivilianAvatar::OnStart()
   {
      dtGame::GameActorProxy* actor = NULL;
      dtGame::GameManager* gm = GetDirector()->GetGameManager();
      if (gm == NULL) return;

      if (!GetExistingActor().ToString().empty())
      {
         gm->FindGameActorById(GetExistingActor(), actor);
      }

      if (actor == NULL && !GetNewActorPrefab().IsEmpty())
      {
         actor = CreatePrefab(*gm);
      }

      if (actor == NULL)
      {
         LOG_ERROR("Unable to create or link to a Civilian Actor.");
      }
      else
      {
         InitActor(actor);

         // TODO verify it has the civ ai actor component.  In theory, one could be added.
         SetActorOnChildNodes(actor, *gm);

         BaseClass::OnStart();
      }
   }


   /////////////////////////////////////////////////////////////////////////////
   bool CivilianAvatar::Update(float simDelta, float delta, int input, bool firstUpdate)
   {

      return BaseClass::Update(simDelta, delta, input, firstUpdate);
   }

   ///////////////////////////////////////////////////////////////////////////////

   void CivilianAvatar::SetActorOnChildNodes(dtGame::GameActorProxy* actor, dtGame::GameManager& gm)
   {
      typedef std::vector<dtDirector::Node*> NodeArray;
      NodeArray nodes;

      GetDirectorScript()->GetAllNodes(nodes);
      NodeArray::iterator iter = nodes.begin();
      NodeArray::iterator iterEnd = nodes.end();

      for(;iter != iterEnd; ++iter)
      {
         StateNode* sn = dynamic_cast<StateNode*>(*iter);
         if(sn != NULL)
         {
            sn->SetActor(actor->GetId());
            SelectDestination* sd = dynamic_cast<SelectDestination*>(sn);
            if (sd != NULL)
            {
               sd->SetDestinationName(GetDestinationName());
            }
         }
         else if ((*iter)->AsValueNode() != NULL && (*iter)->AsValueNode()->GetPropertyType() == dtCore::DataType::ACTOR)
         {
            (*iter)->AsValueNode()->SetActorID(actor->GetId(), "Value");
         }
      }

   }

   ///////////////////////////////////////////////////////////////////////////////
   bool CivilianAvatar::CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value)
   {
      return true;
   }


   //---------------------
}//namespace dtExample


//---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 04/07/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
// CivilianAvatar.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

