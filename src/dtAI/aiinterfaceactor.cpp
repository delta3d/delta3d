/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 Alion Science and Technology
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

#include <dtAI/aiinterfaceactor.h>
#include <dtAI/deltaaiinterface.h>
#include <dtAI/waypointtypes.h>
#include <dtCore/resourceactorproperty.h>
#include <dtUtil/datapathutils.h>

namespace dtAI
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString AIInterfaceActorProxy::CLASS_NAME("dtAI::AIInterface");
   const dtUtil::RefString AIInterfaceActorProxy::PROPERTY_WAYPOINT_FILE_NAME("dtAI::WaypointFilename");


   ////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   ////////////////////////////////////////////////////////////////////////////
   AIInterfaceActor::AIInterfaceActor()
   {

   }

   /////////////////////////////////////////////////////////////////////////////
   AIInterfaceActor::~AIInterfaceActor()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Node* AIInterfaceActor::GetOSGNode()
   {
      return dtAI::WaypointManager::GetInstance().GetOSGNode();
   }

   /////////////////////////////////////////////////////////////////////////////
   const osg::Node* AIInterfaceActor::GetOSGNode() const
   {
      return dtAI::WaypointManager::GetInstance().GetOSGNode();
   }


   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////
   AIInterfaceActorProxy::AIInterfaceActorProxy()
   {
      SetClassName(AIInterfaceActorProxy::CLASS_NAME.Get());
   }

   /////////////////////////////////////////////////////////////////////////////
    AIInterfaceActorProxy::~AIInterfaceActorProxy()
    {
    }

    ////////////////////////////////////////////////////////////////////////////
    void AIInterfaceActorProxy::CreateDrawable()
    {
       AIInterfaceActor* actor = new AIInterfaceActor();
       SetDrawable(*actor);

       mAIInterface = CreateAIInterface();

       mAIInterface->RegisterWaypointType<Waypoint>(WaypointTypes::DEFAULT_WAYPOINT.get());
       mAIInterface->RegisterWaypointType<NamedWaypoint>(WaypointTypes::NAMED_WAYPOINT.get());
       mAIInterface->RegisterWaypointType<TacticalWaypoint>(WaypointTypes::TACTICAL_WAYPOINT.get());
       mAIInterface->RegisterWaypointType<WaypointCollection>(WaypointTypes::WAYPOINT_COLLECTION.get());
    }

    ////////////////////////////////////////////////////////////////////////////
    AIPluginInterface* AIInterfaceActorProxy::CreateAIInterface()
    {
       return new DeltaAIInterface();
    }

    ////////////////////////////////////////////////////////////////////////////
    void AIInterfaceActorProxy::BuildPropertyMap()
    {
       dtCore::BaseActorObject::BuildPropertyMap();

       const dtUtil::RefString GROUPNAME = "AIInterface";

       AddProperty(new dtCore::ResourceActorProperty(*this, dtCore::DataType::STATIC_MESH,
                   PROPERTY_WAYPOINT_FILE_NAME,
                   PROPERTY_WAYPOINT_FILE_NAME,
                   dtCore::ResourceActorProperty::SetFuncType(this, &AIInterfaceActorProxy::LoadFile),
                   "Loads the waypoint and connectivity graph.", GROUPNAME));
    }

    ////////////////////////////////////////////////////////////////////////////
    void AIInterfaceActorProxy::LoadFile(const std::string& fileName)
    {
         mAIInterface->ClearMemory();
         if (!fileName.empty())
         {
            std::string fullpath = dtUtil::FindFileInPathList(fileName);

            bool success = mAIInterface->LoadWaypointFile(fullpath);

            if (!success)
            {
               LOG_ERROR("Unable to load Waypoint File '" + fileName + "'")
            }
         }
    }

    ////////////////////////////////////////////////////////////////////////////
    bool AIInterfaceActorProxy::IsPlaceable() const
    {
       return false;
    }

    ////////////////////////////////////////////////////////////////////////////
    AIPluginInterface* AIInterfaceActorProxy::GetAIInterface()
    {
      return mAIInterface.get();
    }

    ////////////////////////////////////////////////////////////////////////////
    const AIPluginInterface* AIInterfaceActorProxy::GetAIInterface() const
    {
       return mAIInterface.get();
    }
}
