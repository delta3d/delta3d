/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
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
 */

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

