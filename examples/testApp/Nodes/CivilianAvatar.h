/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __BLACKLIGHT_CivilianAvatar_H__
#define __BLACKLIGHT_CivilianAvatar_H__

#include <dtUtil/getsetmacros.h>


#include "../export.h"

#include "GroupNode.h"

namespace dtGame
{
   class GameManager;
}

namespace dtExample
{

   class TEST_APP_EXPORT CivilianAvatar : public GroupNode
   {

      ////////////////////////////////////////////////////////////////////////////////////////////
      //Data Structures
      ////////////////////////////////////////////////////////////////////////////////////////////
   public:
      typedef GroupNode BaseClass;

      /////////////////////////////////////////////////////////////////////////////////////////////
      //Functions
      /////////////////////////////////////////////////////////////////////////////////////////////
   public:

      CivilianAvatar();

      virtual void Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph);

      virtual void BuildPropertyMap();
      virtual void OnStart();
      virtual bool Update(float simDelta, float delta, int input, bool firstUpdate);

      virtual bool CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value);

      DT_DECLARE_ACCESSOR(std::string, NewActorName)
      DT_DECLARE_ACCESSOR(osg::Vec3, NewActorPos)
      DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, NewActorPrefab)
      DT_DECLARE_ACCESSOR(dtCore::UniqueId, ExistingActor)

      DT_DECLARE_ACCESSOR(std::string, DestinationName)

   protected:

      virtual ~CivilianAvatar();

      void SetActorOnChildNodes(dtGame::GameActorProxy* human, dtGame::GameManager& gm);
      virtual dtGame::GameActorProxy* CreatePrefab(dtGame::GameManager& gm);
      virtual void InitActor(dtGame::GameActorProxy* actor);

   private:
      CivilianAvatar(const CivilianAvatar&);  //not implemented
      CivilianAvatar& operator=(const CivilianAvatar&); //not implemented

      /////////////////////////////////////////////////////////////////////////////////////////////
      //Member Variables
      /////////////////////////////////////////////////////////////////////////////////////////////

   private:

   };


}//namespace dtExample


#endif //__BLACKLIGHT_CivilianAvatar_H__


//---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 04/07/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
// CivilianAvatar.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------


