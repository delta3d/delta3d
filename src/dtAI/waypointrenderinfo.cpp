/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 MOVES Institute
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

#include <dtAI/waypointrenderinfo.h>

namespace dtAI
{
   //////////////////////////////////////////////////////////////////////////////
   //WaypointRenderInfo
   //////////////////////////////////////////////////////////////////////////////
   WaypointRenderInfo::WaypointRenderInfo()
   {
      SetDefaults();
   }

   //////////////////////////////////////////////////////////////////////////////
   WaypointRenderInfo::~WaypointRenderInfo()
   {

   }

   dtCore::RefPtr<dtCore::ObjectType> WaypointRenderInfo::TYPE(new dtCore::ObjectType("WaypointRenderInfo","dtAI"));
   const dtCore::ObjectType& WaypointRenderInfo::GetObjectType() const { return *TYPE; }

   //////////////////////////////////////////////////////////////////////////////
   void WaypointRenderInfo::Init()
   {
      BuildPropertyMap();
      InitDefaults();
   }

   //////////////////////////////////////////////////////////////////////////////
   bool WaypointRenderInfo::IsAnyRenderingEnabled() const
   {
      return mRenderWaypoints || mRenderWaypointID || mRenderWaypointText || mRenderNavMesh || mRenderNavMeshText;
   }

   //////////////////////////////////////////////////////////////////////////////
   void WaypointRenderInfo::SetAllRenderingOptions(bool value)
   {
      SetRenderWaypoints(value);
      SetRenderWaypointID(value);
      SetRenderWaypointText(value);
      SetRenderNavMesh(value);
      SetRenderNavMeshText(value);
   }

   //////////////////////////////////////////////////////////////////////////////
   void WaypointRenderInfo::SetDefaults()
   {
      //we actually don't want to reset this, similarly to the Waypoint ID
      //this comes from supporting the pure virtual functions in the base
      //mWorldSpacePos = osg::Vec3(0.0f, 0.0f, 0.0f);
      SetAllRenderingOptions(true);

      //set the rest to reasonable defaults
      mWaypointSize = 10.0f;
      mWaypointColor = Color(1.0f, 1.0f, 1.0f, 1.0f);

      mWaypointFontFile = "arial.ttf";
      mWaypointFontSizeScalar = 0.5f;
      mWaypointTextOffset = osg::Vec3(0.0f, 0.0f, 0.3f);
      mWaypointFontColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
      mNavMeshColor = Color(1.0f, 1.0f, 1.0f, 1.0f);

      mNavMeshFontFile = "arial.ttf";
      mNavMeshFontSizeScalar = 0.5f;
      mNavMeshTextOffsetScalar = 1.0f;
      mNavMeshFontColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
      mNavMeshColor = Color(0.5f, 1.0f, 0.5f, 1.0f);
      mNavMeshWidth = 2.5f;
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointRenderInfo::BuildPropertyMap()
   {
      //TODO Create Properties
      typedef dtCore::PropertyRegHelper<dtCore::PropertyContainer, WaypointRenderInfo> RegHelperType;

      RegHelperType regHelper(*this, this, "WaypointRenderInfo");

      DT_REGISTER_PROPERTY(EnableDepthTest, "Used to toggle depth testing on the waypoints and navmesh.", RegHelperType, regHelper);
      DT_REGISTER_PROPERTY(RenderWaypoints, "Used to toggle waypoint rendering.", RegHelperType, regHelper);
      DT_REGISTER_PROPERTY(RenderWaypointID, "Used to toggle rendering the waypoint id above the waypoint.", RegHelperType, regHelper);
      DT_REGISTER_PROPERTY(RenderWaypointText, "Used to toggle waypoint text rendering.", RegHelperType, regHelper);
      DT_REGISTER_PROPERTY(RenderNavMesh, "Used to toggle navmesh rendering.", RegHelperType, regHelper);
      DT_REGISTER_PROPERTY(RenderNavMeshText, "Used to toggle navmesh text rendering.", RegHelperType, regHelper);
      DT_REGISTER_PROPERTY(WaypointSize, "Used to change the size of the waypoints.", RegHelperType, regHelper);
      DT_REGISTER_PROPERTY(WaypointColor, "Used to set the rendered waypoint color.", RegHelperType, regHelper);
      DT_REGISTER_PROPERTY(WaypointFontFile, "Used to set the font file use for the waypoint text.", RegHelperType, regHelper);
      DT_REGISTER_PROPERTY(WaypointFontSizeScalar, "Used to change the size of the waypoint text.", RegHelperType, regHelper);
      DT_REGISTER_PROPERTY(WaypointTextOffset, "Used to change the text offset from the waypoint.", RegHelperType, regHelper);
      DT_REGISTER_PROPERTY(WaypointFontColor, "Used to change the font color of the waypoint text.", RegHelperType, regHelper);
      DT_REGISTER_PROPERTY(NavMeshWidth, "Used to change the size of the rendered navmesh.", RegHelperType, regHelper);
      DT_REGISTER_PROPERTY(NavMeshColor, "Used to change the color of the navmesh.", RegHelperType, regHelper);
      DT_REGISTER_PROPERTY(NavMeshFontFile, "Used to change the font file of the navmesh text.", RegHelperType, regHelper);
      DT_REGISTER_PROPERTY(NavMeshFontSizeScalar, "Used to change the size of the navmesh font.", RegHelperType, regHelper);
      DT_REGISTER_PROPERTY(NavMeshTextOffsetScalar, "Used to offset the navmesh text.", RegHelperType, regHelper);
      DT_REGISTER_PROPERTY(NavMeshFontColor, "Used to change the navmesh text color.", RegHelperType, regHelper);

   }

   DT_IMPLEMENT_ACCESSOR(WaypointRenderInfo, bool, EnableDepthTest)
   DT_IMPLEMENT_ACCESSOR(WaypointRenderInfo, bool, RenderWaypoints)
   DT_IMPLEMENT_ACCESSOR(WaypointRenderInfo, bool, RenderWaypointID)
   DT_IMPLEMENT_ACCESSOR(WaypointRenderInfo, bool, RenderWaypointText)
   DT_IMPLEMENT_ACCESSOR(WaypointRenderInfo, bool, RenderNavMesh)
   DT_IMPLEMENT_ACCESSOR(WaypointRenderInfo, bool, RenderNavMeshText)

   //Waypoints
   DT_IMPLEMENT_ACCESSOR(WaypointRenderInfo, float, WaypointSize)
   DT_IMPLEMENT_ACCESSOR(WaypointRenderInfo, WaypointRenderInfo::Color, WaypointColor)

   //Waypoint Font
   DT_IMPLEMENT_ACCESSOR(WaypointRenderInfo, std::string, WaypointFontFile)
   DT_IMPLEMENT_ACCESSOR(WaypointRenderInfo, float, WaypointFontSizeScalar)
   DT_IMPLEMENT_ACCESSOR(WaypointRenderInfo, osg::Vec3, WaypointTextOffset)
   DT_IMPLEMENT_ACCESSOR(WaypointRenderInfo, WaypointRenderInfo::Color, WaypointFontColor)

   //Nav Mesh
   DT_IMPLEMENT_ACCESSOR(WaypointRenderInfo, float, NavMeshWidth)
   DT_IMPLEMENT_ACCESSOR(WaypointRenderInfo, WaypointRenderInfo::Color, NavMeshColor)

   //Nav Mesh Font
   DT_IMPLEMENT_ACCESSOR(WaypointRenderInfo, std::string, NavMeshFontFile)
   DT_IMPLEMENT_ACCESSOR(WaypointRenderInfo, float, NavMeshFontSizeScalar)
   ///the text is scaled across the navmesh path
   DT_IMPLEMENT_ACCESSOR(WaypointRenderInfo, float, NavMeshTextOffsetScalar)
   DT_IMPLEMENT_ACCESSOR(WaypointRenderInfo, WaypointRenderInfo::Color, NavMeshFontColor)


} // namespace dtAI
