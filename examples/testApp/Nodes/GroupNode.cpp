// ---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 04/07/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
//
// GroupNode.cpp
// ---------------------------------------------------------------------------------------------------------------------------------

#include "GroupNode.h"


namespace dtExample
{


/////////////////////////////////////////////////////////////////////////////
GroupNode::GroupNode()
  : BaseClass()
{
  AddAuthor("Bradley Anderegg");
}

/////////////////////////////////////////////////////////////////////////////
GroupNode::~GroupNode()
{
}

/////////////////////////////////////////////////////////////////////////////
void GroupNode::Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph)
{
  BaseClass::Init(nodeType, graph);

}

/////////////////////////////////////////////////////////////////////////////
void GroupNode::BuildPropertyMap()
{
  BaseClass::BuildPropertyMap();
}

/////////////////////////////////////////////////////////////////////////////
bool GroupNode::Update(float simDelta, float delta, int input, bool firstUpdate)
{
  return BaseClass::Update(simDelta, delta, input, firstUpdate);
}

///////////////////////////////////////////////////////////////////////////////
bool GroupNode::CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value)
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
// GroupNode.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

