// ---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
//
// TurnToFaceDirection.cpp
// ---------------------------------------------------------------------------------------------------------------------------------

#include "TurnToFaceDirection.h"


namespace dtExample
{


/////////////////////////////////////////////////////////////////////////////
TurnToFaceDirection::TurnToFaceDirection()
  : BaseClass()
{
  AddAuthor("Bradley Anderegg");
}

/////////////////////////////////////////////////////////////////////////////
TurnToFaceDirection::~TurnToFaceDirection()
{
}

/////////////////////////////////////////////////////////////////////////////
void TurnToFaceDirection::Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph)
{
  BaseClass::Init(nodeType, graph);

  //mActor->SetIgnoreRotation(true);

}


/////////////////////////////////////////////////////////////////////////////
void TurnToFaceDirection::BuildPropertyMap()
{
   BaseClass::BuildPropertyMap();
}

/////////////////////////////////////////////////////////////////////////////
bool TurnToFaceDirection::Update(float simDelta, float delta, int input, bool firstUpdate)
{
   return BaseClass::Update(simDelta, delta, input, firstUpdate);

  //bool success = FindAndSetHumanActor(entity.getKnowledge());
  //if (success)
  //{
  //   //THIS ONLY LOOKS AT THE TERRORIST RIGHT NOW, FIX WITH PARAMETER TO NODE
  //   MarineActor* target = NULL;
  //   mGM->FindActorByName("Terrorist_02", target);
  //   if(target != NULL)
  //   {          
  //      mActor->SetIgnoreRotation(true);

  //      if(mActor->RotateToPoint(target->GetPosition(), mGM->GetSimulationTime()))
  //      {           
  //         //mActor->SetIgnoreRotation(false);
  //         return 1;
  //      }
  //      else
  //      {
  //         return 1;
  //      }
  //   }   
}

///////////////////////////////////////////////////////////////////////////////
bool TurnToFaceDirection::CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value)
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
// TurnToFaceDirection.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

