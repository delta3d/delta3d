// ---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
//
// TurnToFaceDirection.h
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef __BLACKLIGHT_TURNTOFACEDIRECTION_H__
#define __BLACKLIGHT_TURNTOFACEDIRECTION_H__


#include "../export.h"

#include "StateNode.h"  


namespace dtExample
{


class TEST_APP_EXPORT TurnToFaceDirection : public StateNode
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

  TurnToFaceDirection();

  virtual void Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph);

  virtual void BuildPropertyMap();
  virtual bool Update(float simDelta, float delta, int input, bool firstUpdate);

  virtual bool CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value);

protected:

  virtual ~TurnToFaceDirection();


private:
   TurnToFaceDirection(const TurnToFaceDirection&);  //not implemented
   TurnToFaceDirection& operator=(const TurnToFaceDirection&); //not implemented

/////////////////////////////////////////////////////////////////////////////////////////////
//Member Variables
/////////////////////////////////////////////////////////////////////////////////////////////

private:


};


}//namespace dtExample


#endif //__BLACKLIGHT_TURNTOFACEDIRECTION_H__


//---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
// TurnToFaceDirection.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------


