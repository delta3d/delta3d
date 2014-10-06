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

#include "DirectorNodePluginRegistry.h"

#include "ChangeStance.h"
#include "MoveToDestination.h"
#include "SelectDestination.h"
#include "SelectTarget.h"
#include "StateNode.h"
#include "TurnToFaceDirection.h"
#include "ApplicationNode.h"
#include "CivilianAvatar.h"
#include "LookAt.h"


#include <dtDirector/colors.h>


namespace dtExample
{

   dtCore::RefPtr<dtDirector::NodeType> DirectorNodePluginRegistry::SELECT_DESTINATION_NODE_TYPE(new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE,             "SelectDestination",        "General",     "AI", "", NULL, dtDirector::Colors::GREEN2));
   dtCore::RefPtr<dtDirector::NodeType> DirectorNodePluginRegistry::MOVE_TO_DESTINATION_NODE_TYPE(new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE,             "MoveToDestination",        "General",     "AI", "", NULL, dtDirector::Colors::GREEN2));
   dtCore::RefPtr<dtDirector::NodeType> DirectorNodePluginRegistry::SELECT_TARGET_NODE_TYPE(new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE,             "SelectTarget",        "General",     "AI", "", NULL, dtDirector::Colors::GREEN2));
   dtCore::RefPtr<dtDirector::NodeType> DirectorNodePluginRegistry::TURN_TO_FACE_DIRECTION_NODE_TYPE(new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE,             "TurnToFaceDirection",        "General",     "AI", "", NULL, dtDirector::Colors::GREEN2));
   dtCore::RefPtr<dtDirector::NodeType> DirectorNodePluginRegistry::LOOK_AT_NODE_TYPE(new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE,             "LookAt",        "General",     "AI", "", NULL, dtDirector::Colors::GREEN2));
   dtCore::RefPtr<dtDirector::NodeType> DirectorNodePluginRegistry::CHANGE_STANCE_NODE_TYPE(new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE,             "ChangeStance",        "General",     "AI", "", NULL, dtDirector::Colors::GREEN2));

   dtCore::RefPtr<dtDirector::NodeType> DirectorNodePluginRegistry::CIVILIAN_AVATAR_NODE_TYPE(new dtDirector::NodeType(dtDirector::NodeType::MACRO_NODE,             "CivilianAvatar",        "General",     "Actors", "", NULL, dtDirector::Colors::GREEN2));
   dtCore::RefPtr<dtDirector::NodeType> DirectorNodePluginRegistry::MARINE_AVATAR_NODE_TYPE(new dtDirector::NodeType(dtDirector::NodeType::MACRO_NODE,             "MarineAvatar",        "General",     "Actors", "", NULL, dtDirector::Colors::GREEN2));
   dtCore::RefPtr<dtDirector::NodeType> DirectorNodePluginRegistry::APPLICATION_NODE_TYPE(new dtDirector::NodeType(dtDirector::NodeType::MISC_NODE,             "ApplicationNode",        "General",     "Applications", "", NULL, dtDirector::Colors::CYAN));


   //////////////////////////////////////////////////////////////////////////
   DirectorNodePluginRegistry::DirectorNodePluginRegistry()
   : dtDirector::NodePluginRegistry("testApp", "Nodes for editing game logic.")
   {
   }

   //////////////////////////////////////////////////////////////////////////
   DirectorNodePluginRegistry::~DirectorNodePluginRegistry()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   std::string DirectorNodePluginRegistry::GetNodeLibraryType() const
   {
      return "EditGame";
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorNodePluginRegistry::RegisterNodeTypes()
   {
      mNodeFactory->RegisterType<SelectDestination>(SELECT_DESTINATION_NODE_TYPE.get());
      mNodeFactory->RegisterType<MoveToDestination>(MOVE_TO_DESTINATION_NODE_TYPE.get());
      mNodeFactory->RegisterType<SelectTarget>(SELECT_TARGET_NODE_TYPE.get());
      mNodeFactory->RegisterType<TurnToFaceDirection>(TURN_TO_FACE_DIRECTION_NODE_TYPE.get());
      mNodeFactory->RegisterType<LookAt>(LOOK_AT_NODE_TYPE.get());
      mNodeFactory->RegisterType<ChangeStance>(CHANGE_STANCE_NODE_TYPE.get());

      mNodeFactory->RegisterType<ApplicationNode>(APPLICATION_NODE_TYPE.get());
      mNodeFactory->RegisterType<CivilianAvatar>(CIVILIAN_AVATAR_NODE_TYPE.get());
   }

}

//////////////////////////////////////////////////////////////////////////
extern "C" TEST_APP_EXPORT dtDirector::NodePluginRegistry* CreateNodePluginRegistry()
{
   return new dtExample::DirectorNodePluginRegistry;
}

//////////////////////////////////////////////////////////////////////////
extern "C" TEST_APP_EXPORT void DestroyNodePluginRegistry(dtDirector::NodePluginRegistry* registry)
{
   delete registry;
}
