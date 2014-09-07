// ---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
//
// SelectDestination.h
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef __BLACKLIGHT_SELECTDESTINATION_H__
#define __BLACKLIGHT_SELECTDESTINATION_H__


#include "../export.h"

#include "StateNode.h"  


namespace dtExample
{


   class TEST_APP_EXPORT SelectDestination : public StateNode
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

      SelectDestination();

      virtual void Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph);

      virtual void BuildPropertyMap();
      virtual bool Update(float simDelta, float delta, int input, bool firstUpdate);

      virtual bool CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value);

      DT_DECLARE_ACCESSOR(osg::Vec3, DestinationPosition);
      DT_DECLARE_ACCESSOR(float, Radius);
      DT_DECLARE_ACCESSOR(std::string, DestinationName);
   protected:

      virtual ~SelectDestination();


   private:
      SelectDestination(const SelectDestination&);  //not implemented
      SelectDestination& operator=(const SelectDestination&); //not implemented

      /////////////////////////////////////////////////////////////////////////////////////////////
      //Member Variables
      /////////////////////////////////////////////////////////////////////////////////////////////

   private:


   };


}//namespace dtExample


#endif //__BLACKLIGHT_SELECTDESTINATION_H__


//---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
// SelectDestination.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------


