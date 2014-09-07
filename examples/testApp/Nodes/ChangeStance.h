// ---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
//
// ChangeStance.h
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef __BLACKLIGHT_CHANGESTANCE_H__
#define __BLACKLIGHT_CHANGESTANCE_H__


#include "../export.h"

#include "StateNode.h"  


namespace dtExample
{


class TEST_APP_EXPORT ChangeStance : public StateNode
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

  ChangeStance();

  virtual void Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph);

  virtual void BuildPropertyMap();
  virtual bool Update(float simDelta, float delta, int input, bool firstUpdate);

  virtual bool CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value);

protected:

  virtual ~ChangeStance();


private:
   ChangeStance(const ChangeStance&);  //not implemented
   ChangeStance& operator=(const ChangeStance&); //not implemented

/////////////////////////////////////////////////////////////////////////////////////////////
//Member Variables
/////////////////////////////////////////////////////////////////////////////////////////////

private:


};


}//namespace dtExample


#endif //__BLACKLIGHT_CHANGESTANCE_H__


//---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
// ChangeStance.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------


