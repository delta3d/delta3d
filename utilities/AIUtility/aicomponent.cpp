/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009, Alion Science and Technology, BMH Operation
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Bradley Anderegg
 */


#include "aicomponent.h"

#include <dtCore/scene.h>

#include <dtABC/application.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtCore/map.h>
#include <dtCore/project.h>
#include <dtCore/actorfactory.h>
#include <dtCore/resourceactorproperty.h>
#include <dtAI/aiactorregistry.h>
#include <dtAI/aiinterfaceactor.h>
#include <dtAI/aiplugininterface.h>
#include <dtAI/aidebugdrawable.h>

#include <osgDB/FileNameUtils>

#include "aiutilityapp.h"
#include <dtAI/waypointgraph.h>
#include <dtAI/navmesh.h>

/////////////////////////////////////////////////////////////
AIComponent::AIComponent()
   : dtAI::BaseAIComponent()
{

}

////////////////////////////////////////////////////////////////////////////////
void AIComponent::CleanUp()
{
   if (GetAIPluginInterface())
   {
      GetGameManager()->GetScene().RemoveChild(GetAIPluginInterface()->GetDebugDrawable());
   }

   dtAI::BaseAIComponent::CleanUp();

   //AIUtilityApp& aiApp = dynamic_cast<AIUtilityApp&>(GetGameManager()->GetApplication());
   //aiApp.SetAIPluginInterface(NULL, false);
}

////////////////////////////////////////////////////////////////////////////////
void AIComponent::ProcessMessage(const dtGame::Message& message)
{
   dtAI::BaseAIComponent::ProcessMessage(message);

   if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_LOADED)
   {
      dtAI::AIPluginInterface* aiInterface = GetAIPluginInterface();
      bool useRenderFallback = false;

      if (aiInterface)
      {
         // Don't allow the sheer volume of waypoint render data to bring the app down
         useRenderFallback = ShouldUseRenderFallback(aiInterface);

         std::vector<dtAI::WaypointInterface*> waypointArray;
         aiInterface->GetWaypoints(waypointArray);

         // Creating another bool for clarity
         bool createText = !useRenderFallback;

         dtAI::AIDebugDrawable* debugDrawable = new dtAI::AIDebugDrawable;
         debugDrawable->SetWaypoints(waypointArray, createText);

         // Set a custom debug drawable so that we control exactly what is rendered
         aiInterface->SetDebugDrawable(debugDrawable);

         GetGameManager()->GetScene().AddChild(debugDrawable);
      }

      AIUtilityApp& aiApp = dynamic_cast<AIUtilityApp&>(GetGameManager()->GetApplication());

      aiApp.SetAIPluginInterface(GetAIPluginInterface(), useRenderFallback);
   }
}

////////////////////////////////////////////////////////////////////////////////
void AIComponent::AddAIInterfaceToMap(const std::string& map)
{
   dtCore::Map& currentMap = dtCore::Project::GetInstance().GetMap(map);

   std::string mapFileName = osgDB::getNameLessExtension(currentMap.GetFileName());

   int numToTackOn = 0;
   std::string stringNumToTackOn;

   std::string path;

   path = dtCore::Project::GetInstance().GetContext() + "/" +
      dtCore::DataType::STATIC_MESH.GetName() + "/" + mapFileName + ".ai";

   while (dtUtil::FileUtils::GetInstance().FileExists(path))
   {
      ++numToTackOn;
      stringNumToTackOn = dtUtil::ToString(numToTackOn);
      path  = dtCore::Project::GetInstance().GetContext() + "/" +
         dtCore::DataType::STATIC_MESH.GetName() + "/" + mapFileName + stringNumToTackOn + ".ai";
   }

   // Make sure the dtAI actor library is loaded
   if (!currentMap.HasLibrary("dtAI"))
   {
      dtCore::ActorFactory::GetInstance().LoadActorRegistry("dtAI");
      currentMap.AddLibrary("dtAI", "1.0");
   }

   dtCore::RefPtr<dtAI::AIInterfaceActor> aiActor;
   GetGameManager()->CreateActor(*dtAI::AIActorRegistry::AI_INTERFACE_ACTOR_TYPE, aiActor);
   currentMap.AddProxy(*aiActor);

   // Automatically save it in ContextDirectory/staticmeshes
   if (!aiActor->GetAIInterface()->SaveWaypointFile(path))
   {
      throw dtUtil::Exception("Unable to save waypoint file to path \"" + path + "\".  Unknown error.", __FILE__, __LINE__);
   }

   dtCore::ResourceDescriptor rd(dtCore::DataType::STATIC_MESH.GetName() + ":" + mapFileName + stringNumToTackOn + ".ai");
   dtCore::ResourceActorProperty* rap = NULL;
   aiActor->GetProperty(dtAI::AIInterfaceActor::PROPERTY_WAYPOINT_FILE_NAME, rap);
   if (rap == NULL)
   {
      throw dtUtil::Exception("AI interface actor doesn't have a waypoint resource property, so it can't be added to the map.", __FILE__, __LINE__);
   }
   rap->SetValue(rd);

   // Auto save until this becomes part of the undo stack
   dtCore::Project::GetInstance().SaveMap(currentMap);

   SetAIPluginInterface(aiActor);
   AIUtilityApp& aiApp = dynamic_cast<AIUtilityApp&>(GetGameManager()->GetApplication());
   GetGameManager()->GetScene().AddChild(GetAIPluginInterface()->GetDebugDrawable());
   aiApp.SetAIPluginInterface(GetAIPluginInterface(), false);
}

////////////////////////////////////////////////////////////////////////////////
bool AIComponent::ShouldUseRenderFallback(dtAI::AIPluginInterface* aiInterface)
{
   bool useRenderFallback = false;

   if (aiInterface->GetNumWaypoints() >= SUGGESTED_MAX_RENDERABLE_WAYPOINTS_WITH_TEXT)
   {
      useRenderFallback = true;
   }
   else
   {
      dtAI::NavMesh* baseNavMesh = aiInterface->GetWaypointGraph().GetNavMeshAtSearchLevel(0);

      if (baseNavMesh && baseNavMesh->GetNavMesh().size() >= SUGGESTED_MAX_RENDERABLE_EDGES)
      {
         useRenderFallback = true;
      }
   }

   return useRenderFallback;
}


////////////////////////////////////////////////////////////////////////////////
