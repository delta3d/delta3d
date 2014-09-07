// ---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
//
// SelectTarget.h
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef __BLACKLIGHT_SELECTTARGET_H__
#define __BLACKLIGHT_SELECTTARGET_H__


#include "../export.h"

#include "StateNode.h"  


namespace dtExample
{


class TEST_APP_EXPORT SelectTarget : public StateNode
{

////////////////////////////////////////////////////////////////////////////////////////////
//Data Structures
////////////////////////////////////////////////////////////////////////////////////////////
public:
   typedef StateNode BaseClass;

/////////////////////////////////////////////////////////////////////////////////////////////
//Functions
/////////////////////////////////////////////////////////////////////////////////////////////
public:

  SelectTarget();

  virtual void Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph);

  virtual void BuildPropertyMap();
  virtual bool Update(float simDelta, float delta, int input, bool firstUpdate);

  virtual bool CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value);

protected:

  virtual ~SelectTarget();


private:
   SelectTarget(const SelectTarget&);  //not implemented
   SelectTarget& operator=(const SelectTarget&); //not implemented

/////////////////////////////////////////////////////////////////////////////////////////////
//Member Variables
/////////////////////////////////////////////////////////////////////////////////////////////

private:


};


}//namespace dtExample


#endif //__BLACKLIGHT_SELECTTARGET_H__


//---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
// SelectTarget.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------


