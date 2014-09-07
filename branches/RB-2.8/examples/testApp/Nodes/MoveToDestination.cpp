// ---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
//
// MoveToDestination.cpp
// ---------------------------------------------------------------------------------------------------------------------------------

#include "MoveToDestination.h"

#include "../civilianaiactorcomponent.h"
#include <dtAI/aiplugininterface.h>


namespace dtExample
{


/////////////////////////////////////////////////////////////////////////////
MoveToDestination::MoveToDestination()
  : BaseClass()
{
  AddAuthor("Bradley Anderegg");
}

/////////////////////////////////////////////////////////////////////////////
MoveToDestination::~MoveToDestination()
{
}

/////////////////////////////////////////////////////////////////////////////
void MoveToDestination::Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph)
{
  BaseClass::Init(nodeType, graph);
}

bool MoveToDestination::GetDestinationByName()
{
   return false;
}
/////////////////////////////////////////////////////////////////////////////
std::string MoveToDestination::GetDestinationName()
{
   return mDestinationName;
}

bool MoveToDestination::GetDestinationByPos()
{
   return false;
}
/////////////////////////////////////////////////////////////////////////////
osg::Vec3 MoveToDestination::GetDestinationPos()
{
   return mDestinationPos;
}

/////////////////////////////////////////////////////////////////////////////
void MoveToDestination::BuildPropertyMap()
{
  BaseClass::BuildPropertyMap();
}

/////////////////////////////////////////////////////////////////////////////
bool MoveToDestination::Update(float simDelta, float delta, int input, bool firstUpdate)
{
   bool cont = IsValid();

   dtGame::GameActorProxy* actor = GetTargetActor();
   CivilianAIActorComponent* civAI = actor->GetComponent<CivilianAIActorComponent>();
   if(cont && firstUpdate)
   {
       if(civAI != NULL && civAI->HasDestination())
       {
          cont = true;
       }
       else if(GetDestinationByName())
       {
          std::string stringDest = GetDestinationName();
          cont = civAI->SetDestByClosestNamedWaypoint(stringDest, civAI->GetPosition());
       }
       else if(GetDestinationByPos())
       {
          osg::Vec3 dest = GetDestinationPos();
          cont = civAI->SetDestByPosition(dest);
       }
   }

   cont = civAI != NULL && civAI->HasDestination() && !civAI->IsAtDestination();
   if (!cont)
   {
      // trigger default out.
      BaseClass::Update(simDelta, delta, input, firstUpdate);
   }

   return cont;
}

///////////////////////////////////////////////////////////////////////////////
bool MoveToDestination::CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value)
{
  return true;
}


//---------------------
}//namespace dtExample


//---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
// MoveToDestination.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

