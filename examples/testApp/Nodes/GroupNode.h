// ---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 04/07/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
//
// GroupNode.h
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef __BLACKLIGHT_GroupNode_H__
#define __BLACKLIGHT_GroupNode_H__

#include "../export.h"

#include <dtDirectorNodes/referencescriptaction.h>


namespace dtExample
{


class TEST_APP_EXPORT GroupNode : public dtDirector::ReferenceScriptAction
{

////////////////////////////////////////////////////////////////////////////////////////////
//Data Structures
////////////////////////////////////////////////////////////////////////////////////////////
public:
   typedef dtDirector::ReferenceScriptAction BaseClass;

/////////////////////////////////////////////////////////////////////////////////////////////
//Functions
/////////////////////////////////////////////////////////////////////////////////////////////
public:

  GroupNode();

  virtual void Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph);

  virtual void BuildPropertyMap();
  virtual bool Update(float simDelta, float delta, int input, bool firstUpdate);

  virtual bool CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value);

protected:

  virtual ~GroupNode();


private:
   GroupNode(const GroupNode&);  //not implemented
   GroupNode& operator=(const GroupNode&); //not implemented

/////////////////////////////////////////////////////////////////////////////////////////////
//Member Variables
/////////////////////////////////////////////////////////////////////////////////////////////

private:


};


}//namespace dtExample


#endif //__BLACKLIGHT_GroupNode_H__


//---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 04/07/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
// GroupNode.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------


