// ---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
//
// StateNode.h
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef __BLACKLIGHT_STATENODE_H__
#define __BLACKLIGHT_STATENODE_H__


#include "../export.h"
#include <dtUtil/getsetmacros.h>
#include <dtDirector/actionnode.h>
#include <dtGame/gameactorproxy.h>

namespace dtAI
{
   class AIPluginInterface;
   class AIInterfaceActorProxy;
}

namespace dtExample
{
   class CivilianActor;

   class TEST_APP_EXPORT StateNode : public dtDirector::ActionNode
   {

      ////////////////////////////////////////////////////////////////////////////////////////////
      //Data Structures
      ////////////////////////////////////////////////////////////////////////////////////////////
   public:
      typedef dtDirector::ActionNode BaseClass;

      /////////////////////////////////////////////////////////////////////////////////////////////
      //Functions
      /////////////////////////////////////////////////////////////////////////////////////////////
   public:

      StateNode();

      virtual void Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph);

      virtual void BuildPropertyMap();
      virtual bool Update(float simDelta, float delta, int input, bool firstUpdate);

      virtual bool CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value);

      bool IsValid() const;

      dtAI::AIPluginInterface* GetAIInterface();
      const dtAI::AIPluginInterface* GetAIInterface() const;

      DT_DECLARE_ACCESSOR(dtCore::UniqueId, Actor);

      dtGame::GameActorProxy* GetTargetActor();
      const dtGame::GameActorProxy* GetTargetActor() const;

      void FindAIInterface();

   protected:
      ~StateNode();
    private:
       StateNode(const StateNode&);  //not implemented
       StateNode& operator=(const StateNode&); //not implemented

    /////////////////////////////////////////////////////////////////////////////////////////////
    //Member Variables
    /////////////////////////////////////////////////////////////////////////////////////////////

    private:

       dtCore::ObserverPtr<dtGame::GameActorProxy> mTarget;
       dtCore::ObserverPtr<dtAI::AIInterfaceActorProxy> mAIInterface;

   };


}//namespace dtExample


#endif //__BLACKLIGHT_STATENODE_H__


//---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
// StateNode.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------


