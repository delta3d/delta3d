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
#include <dtDAL/map.h>
#include <dtDAL/project.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/enginepropertytypes.h>

#include <dtAI/aiactorregistry.h>
#include <dtAI/aiinterfaceactor.h>
#include <dtAI/aiplugininterface.h>
#include <dtAI/aidebugdrawable.h>

#include <osgDB/FileNameUtils>

#include "aiutilityapp.h"

const std::string AIComponent::DEFAULT_NAME("AIComponent");

/////////////////////////////////////////////////////////////
AIComponent::AIComponent(const std::string& name)
   : dtAI::BaseAIComponent(name)
{

}

/////////////////////////////////////////////////////////////
void AIComponent::CleanUp()
{
   if (GetAIPluginInterface())
   {
      GetGameManager()->GetScene().RemoveDrawable(GetAIPluginInterface()->GetDebugDrawable());
   }

   dtAI::BaseAIComponent::CleanUp();

   AIUtilityApp& aiApp = dynamic_cast<AIUtilityApp&>(GetGameManager()->GetApplication());
   aiApp.SetAIPluginInterface(NULL);
}

/////////////////////////////////////////////////////////////
void AIComponent::ProcessMessage(const dtGame::Message& message)
{
   dtAI::BaseAIComponent::ProcessMessage(message);

   if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_LOADED)
   {
      if (GetAIPluginInterface())
      {
         GetGameManager()->GetScene().AddDrawable(GetAIPluginInterface()->GetDebugDrawable());
      }

      AIUtilityApp& aiApp = dynamic_cast<AIUtilityApp&>(GetGameManager()->GetApplication());

      aiApp.SetAIPluginInterface(GetAIPluginInterface());
   }
}

/////////////////////////////////////////////////////////////
void AIComponent::AddAIInterfaceToMap(const std::string& map)
{
   dtDAL::Map& m = dtDAL::Project::GetInstance().GetMap(map);

   std::string mapFileName = osgDB::getNameLessExtension(m.GetFileName());

   int numToTackOn = 0;
   std::string stringNumToTackOn;

   std::string path;

   path  = dtDAL::Project::GetInstance().GetContext()
            + "/" + dtDAL::DataType::STATIC_MESH.GetName() + "/" + mapFileName + ".ai";

   while (dtUtil::FileUtils::GetInstance().FileExists(path))
   {
      ++numToTackOn;
      stringNumToTackOn = dtUtil::ToString(numToTackOn);
      path  = dtDAL::Project::GetInstance().GetContext()
               + "/" + dtDAL::DataType::STATIC_MESH.GetName() + "/" + mapFileName + stringNumToTackOn + ".ai";
   }

   dtDAL::LibraryManager::GetInstance().LoadActorRegistry("dtAI");
   m.AddLibrary("dtAI", "1.0");

   dtCore::RefPtr<dtAI::AIInterfaceActorProxy> aiActor;
   GetGameManager()->CreateActor(*dtAI::AIActorRegistry::AI_INTERFACE_ACTOR_TYPE, aiActor);
   m.AddProxy(*aiActor);

   if (!aiActor->GetAIInterface()->SaveWaypointFile(path))
   {
      throw dtUtil::Exception("Unable to save waypoint file to path \"" + path + "\".  Unknown error.", __FILE__, __LINE__);
   }

   dtDAL::ResourceDescriptor rd(dtDAL::DataType::STATIC_MESH.GetName() + ":" + mapFileName + stringNumToTackOn + ".ai");
   dtDAL::ResourceActorProperty* rap = NULL;
   aiActor->GetProperty(dtAI::AIInterfaceActorProxy::PROPERTY_WAYPOINT_FILE_NAME, rap);
   if (rap == NULL)
   {
      throw dtUtil::Exception("AI interface actor doesn't have a waypoint resource property, so it can't be added to the map.", __FILE__, __LINE__);
   }
   rap->SetValue(&rd);
   dtDAL::Project::GetInstance().SaveMap(m);

   SetAIPluginInterfaceProxy(aiActor);
   AIUtilityApp& aiApp = dynamic_cast<AIUtilityApp&>(GetGameManager()->GetApplication());
   GetGameManager()->GetScene().AddDrawable(GetAIPluginInterface()->GetDebugDrawable());
   aiApp.SetAIPluginInterface(GetAIPluginInterface());
}


