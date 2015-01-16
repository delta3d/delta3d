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

#ifndef __BLACKLIGHT_STATENODE_H__
#define __BLACKLIGHT_STATENODE_H__


#include "../export.h"
#include <dtUtil/getsetmacros.h>
#include <dtDirector/actionnode.h>
#include <dtGame/gameactorproxy.h>

namespace dtAI
{
   class AIPluginInterface;
   class AIInterfaceActor;
}

namespace dtExample
{
   class CivilianActor;

   class TEST_APP_EXPORT StateNode : public dtDirector::ActionNode
   {

      ////////////////////////////////////////////////////////////////////////////////////////////
      //Data Structures
      ////////////////////////////////////////////////////////////////////////////////////////////
   public:
      typedef dtDirector::ActionNode BaseClass;

      /////////////////////////////////////////////////////////////////////////////////////////////
      //Functions
      /////////////////////////////////////////////////////////////////////////////////////////////
   public:

      StateNode();

      virtual void Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph);

      virtual void BuildPropertyMap();
      virtual bool Update(float simDelta, float delta, int input, bool firstUpdate);

      virtual bool CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value);

      bool IsValid() const;

      dtAI::AIPluginInterface* GetAIInterface();
      const dtAI::AIPluginInterface* GetAIInterface() const;

      DT_DECLARE_ACCESSOR(dtCore::UniqueId, Actor);

      dtGame::GameActorProxy* GetTargetActor();
      const dtGame::GameActorProxy* GetTargetActor() const;

      void FindAIInterface();

   protected:
      ~StateNode();
    private:
       StateNode(const StateNode&);  //not implemented
       StateNode& operator=(const StateNode&); //not implemented

    /////////////////////////////////////////////////////////////////////////////////////////////
    //Member Variables
    /////////////////////////////////////////////////////////////////////////////////////////////

    private:

       dtCore::ObserverPtr<dtGame::GameActorProxy> mTarget;
       dtCore::ObserverPtr<dtAI::AIInterfaceActor> mAIInterface;

   };


}//namespace dtExample


#endif //__BLACKLIGHT_STATENODE_H__


//---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
// StateNode.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------


