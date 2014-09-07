// ---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
//
// SelectTarget.cpp
// ---------------------------------------------------------------------------------------------------------------------------------

#include "SelectTarget.h"


namespace dtExample
{


/////////////////////////////////////////////////////////////////////////////
SelectTarget::SelectTarget()
  : BaseClass()
{
  AddAuthor("Bradley Anderegg");
}

/////////////////////////////////////////////////////////////////////////////
SelectTarget::~SelectTarget()
{
}

/////////////////////////////////////////////////////////////////////////////
void SelectTarget::Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph)
{
  BaseClass::Init(nodeType, graph);

}

/////////////////////////////////////////////////////////////////////////////
void SelectTarget::BuildPropertyMap()
{
  BaseClass::BuildPropertyMap();
}

/////////////////////////////////////////////////////////////////////////////
bool SelectTarget::Update(float simDelta, float delta, int input, bool firstUpdate)
{
  return BaseClass::Update(simDelta, delta, input, firstUpdate);
}

///////////////////////////////////////////////////////////////////////////////
bool SelectTarget::CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value)
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
// SelectTarget.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

