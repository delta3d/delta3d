// ---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 04/07/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
//
// ApplicationNode.h
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef __BLACKLIGHT_ApplicationNode_H__
#define __BLACKLIGHT_ApplicationNode_H__

#include "../export.h"

#include <dtDirector/groupnode.h>


namespace dtExample
{


class TEST_APP_EXPORT ApplicationNode : public dtDirector::GroupNode
{

////////////////////////////////////////////////////////////////////////////////////////////
//Data Structures
////////////////////////////////////////////////////////////////////////////////////////////
public:
   typedef dtDirector::GroupNode BaseClass;

/////////////////////////////////////////////////////////////////////////////////////////////
//Functions
/////////////////////////////////////////////////////////////////////////////////////////////
public:

  ApplicationNode();

  virtual void Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph);

  virtual void BuildPropertyMap();
  virtual bool Update(float simDelta, float delta, int input, bool firstUpdate);

  virtual bool CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value);

protected:

  virtual ~ApplicationNode();


private:
   ApplicationNode(const ApplicationNode&);  //not implemented
   ApplicationNode& operator=(const ApplicationNode&); //not implemented

/////////////////////////////////////////////////////////////////////////////////////////////
//Member Variables
/////////////////////////////////////////////////////////////////////////////////////////////

private:


};


}//namespace dtExample


#endif //__BLACKLIGHT_ApplicationNode_H__


//---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 04/07/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
// ApplicationNode.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------


