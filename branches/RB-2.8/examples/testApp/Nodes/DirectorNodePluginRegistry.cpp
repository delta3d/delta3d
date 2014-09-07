
#include "DirectorNodePluginRegistry.h"

#include "ChangeStance.h"
#include "MoveToDestination.h"
#include "SelectDestination.h"
#include "SelectTarget.h"
#include "StateNode.h"
#include "TurnToFaceDirection.h"
#include "ApplicationNode.h"
#include "CivilianAvatar.h"


#include <dtDirector/colors.h>


namespace dtExample
{

   dtCore::RefPtr<dtDirector::NodeType> DirectorNodePluginRegistry::SELECT_DESTINATION_NODE_TYPE(new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE,             "SelectDestination",        "General",     "AI", "", NULL, dtDirector::Colors::GREEN2));
   dtCore::RefPtr<dtDirector::NodeType> DirectorNodePluginRegistry::MOVE_TO_DESTINATION_NODE_TYPE(new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE,             "MoveToDestination",        "General",     "AI", "", NULL, dtDirector::Colors::GREEN2));
   dtCore::RefPtr<dtDirector::NodeType> DirectorNodePluginRegistry::SELECT_TARGET_NODE_TYPE(new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE,             "SelectTarget",        "General",     "AI", "", NULL, dtDirector::Colors::GREEN2));
   dtCore::RefPtr<dtDirector::NodeType> DirectorNodePluginRegistry::TURN_TO_FACE_DIRECTION_NODE_TYPE(new dtDirector::NodeType(dtDirector::NodeType::ACTION_NODE,             "TurnToFaceDirection",        "General",     "AI", "", NULL, dtDirector::Colors::GREEN2));
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
