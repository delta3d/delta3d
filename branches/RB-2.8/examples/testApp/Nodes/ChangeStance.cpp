// ---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
//
// ChangeStance.cpp
// ---------------------------------------------------------------------------------------------------------------------------------

#include "ChangeStance.h"


namespace dtExample
{


/////////////////////////////////////////////////////////////////////////////
ChangeStance::ChangeStance()
  : BaseClass()
{
  AddAuthor("Bradley Anderegg");
}

/////////////////////////////////////////////////////////////////////////////
ChangeStance::~ChangeStance()
{
}

/////////////////////////////////////////////////////////////////////////////
void ChangeStance::Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph)
{
  BaseClass::Init(nodeType, graph);

}

/////////////////////////////////////////////////////////////////////////////
void ChangeStance::BuildPropertyMap()
{
  BaseClass::BuildPropertyMap();
}

/////////////////////////////////////////////////////////////////////////////
bool ChangeStance::Update(float simDelta, float delta, int input, bool firstUpdate)
{
  return BaseClass::Update(simDelta, delta, input, firstUpdate);
}

///////////////////////////////////////////////////////////////////////////////
bool ChangeStance::CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value)
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
// ChangeStance.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

