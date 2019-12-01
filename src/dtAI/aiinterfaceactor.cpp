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
#include <dtAI/aidebugdrawable.h>
#include <dtAI/waypointrenderinfo.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/propertycontaineractorproperty.h>
#include <dtUtil/datapathutils.h>

namespace dtAI
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString AIInterfaceActor::CLASS_NAME("dtAI::AIInterface");
   const dtUtil::RefString AIInterfaceActor::PROPERTY_WAYPOINT_FILE_NAME("dtAI::WaypointFilename");
   const dtUtil::RefString AIInterfaceActor::PROPERTY_WAYPOINT_RENDER_INFO("dtAI::WaypointRenderInfo");

   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////
   AIInterfaceActor::AIInterfaceActor()
   {
      SetClassName(AIInterfaceActor::CLASS_NAME.Get());
   }

   /////////////////////////////////////////////////////////////////////////////
   AIInterfaceActor::~AIInterfaceActor()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void AIInterfaceActor::CreateDrawable()
   {

      mAIInterface = CreateAIInterface();

      mAIInterface->RegisterWaypointType<Waypoint>(WaypointTypes::DEFAULT_WAYPOINT.get());
      mAIInterface->RegisterWaypointType<NamedWaypoint>(WaypointTypes::NAMED_WAYPOINT.get());
      mAIInterface->RegisterWaypointType<TacticalWaypoint>(WaypointTypes::TACTICAL_WAYPOINT.get());
      mAIInterface->RegisterWaypointType<WaypointCollection>(WaypointTypes::WAYPOINT_COLLECTION.get());

      SetDrawable(*mAIInterface->GetDebugDrawable());
      // Turn off all rendering
      mAIInterface->GetDebugDrawable()->GetRenderInfo()->SetAllRenderingOptions(false);

   }

   ////////////////////////////////////////////////////////////////////////////
   AIPluginInterface* AIInterfaceActor::CreateAIInterface()
   {
      return new DeltaAIInterface();
   }

   ////////////////////////////////////////////////////////////////////////////
   void AIInterfaceActor::BuildPropertyMap()
   {
      dtCore::BaseActorObject::BuildPropertyMap();

      const dtUtil::RefString GROUPNAME = "AIInterface";

      AIDebugDrawable* debugDrawable = GetDrawable<AIDebugDrawable>();
      AddProperty(new dtCore::SimplePropertyContainerActorProperty<WaypointRenderInfo>(
            PROPERTY_WAYPOINT_RENDER_INFO,
            PROPERTY_WAYPOINT_RENDER_INFO,
            dtCore::PropertyContainerActorProperty<WaypointRenderInfo>::SetFuncType(debugDrawable, &AIDebugDrawable::SetRenderInfo),
            dtCore::PropertyContainerActorProperty<WaypointRenderInfo>::GetFuncType(debugDrawable, &AIDebugDrawable::GetRenderInfo),
            "Draw the waypoint and connectiving graph.", GROUPNAME));

      AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::STATIC_MESH,
            PROPERTY_WAYPOINT_FILE_NAME,
            PROPERTY_WAYPOINT_FILE_NAME,
            dtCore::ResourceActorProperty::SetDescFuncType(this, &AIInterfaceActor::SetAIFile),
            dtCore::ResourceActorProperty::GetDescFuncType(this, &AIInterfaceActor::GetAIFile),
            "Loads the waypoint and connectivity graph.", GROUPNAME));

   }

   DT_IMPLEMENT_ACCESSOR_GETTER(AIInterfaceActor, dtCore::ResourceDescriptor, AIFile);
   void AIInterfaceActor::SetAIFile(const dtCore::ResourceDescriptor& rd)
   {
      mAIFile = rd;
      LoadFile(dtCore::ResourceActorProperty::GetResourcePath(rd));
   }


   ////////////////////////////////////////////////////////////////////////////
   void AIInterfaceActor::LoadFile(const std::string& fileName)
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
   bool AIInterfaceActor::IsPlaceable() const
   {
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   AIPluginInterface* AIInterfaceActor::GetAIInterface()
   {
      return mAIInterface.get();
   }

   ////////////////////////////////////////////////////////////////////////////
   const AIPluginInterface* AIInterfaceActor::GetAIInterface() const
   {
      return mAIInterface.get();
   }
}
