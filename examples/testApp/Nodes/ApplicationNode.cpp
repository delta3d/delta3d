// ---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 04/07/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
//
// ApplicationNode.cpp
// ---------------------------------------------------------------------------------------------------------------------------------

#include "ApplicationNode.h"


namespace dtExample
{


/////////////////////////////////////////////////////////////////////////////
ApplicationNode::ApplicationNode()
  : BaseClass()
{
  AddAuthor("Bradley Anderegg");
}

/////////////////////////////////////////////////////////////////////////////
ApplicationNode::~ApplicationNode()
{
}

/////////////////////////////////////////////////////////////////////////////
void ApplicationNode::Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph)
{
  BaseClass::Init(nodeType, graph);

}

/////////////////////////////////////////////////////////////////////////////
void ApplicationNode::BuildPropertyMap()
{
  BaseClass::BuildPropertyMap();
}

/////////////////////////////////////////////////////////////////////////////
bool ApplicationNode::Update(float simDelta, float delta, int input, bool firstUpdate)
{
  return BaseClass::Update(simDelta, delta, input, firstUpdate);
}

///////////////////////////////////////////////////////////////////////////////
bool ApplicationNode::CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value)
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
// ApplicationNode.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

