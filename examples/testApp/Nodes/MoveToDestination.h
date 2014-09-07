// ---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
//
// MoveToDestination.h
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef __BLACKLIGHT_MOVETODESTINATION_H__
#define __BLACKLIGHT_MOVETODESTINATION_H__


#include "../export.h"

#include <string>
#include <osg/Vec3>

#include "StateNode.h"  


namespace dtExample
{


class TEST_APP_EXPORT MoveToDestination : public StateNode
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

  MoveToDestination();

  virtual void Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph);

  virtual void BuildPropertyMap();
  virtual bool Update(float simDelta, float delta, int input, bool firstUpdate);

  virtual bool CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value);

protected:

  virtual ~MoveToDestination();

  bool GetDestinationByName();
  std::string GetDestinationName();

  bool GetDestinationByPos();
  osg::Vec3 GetDestinationPos();

private:
   MoveToDestination(const MoveToDestination&);  //not implemented
   MoveToDestination& operator=(const MoveToDestination&); //not implemented

/////////////////////////////////////////////////////////////////////////////////////////////
//Member Variables
/////////////////////////////////////////////////////////////////////////////////////////////

private:

   osg::Vec3 mDestinationPos;
   std::string mDestinationName;

};


}//namespace dtExample


#endif //__BLACKLIGHT_MOVETODESTINATION_H__


//---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
// MoveToDestination.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------


