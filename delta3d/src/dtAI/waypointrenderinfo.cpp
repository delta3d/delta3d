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

   //////////////////////////////////////////////////////////////////////////////
   void WaypointRenderInfo::Init()
   {
      BuildPropertyMap();
   }
   
   //////////////////////////////////////////////////////////////////////////////
   void WaypointRenderInfo::SetDefaults()
   {
      //we actually don't want to reset this, simiarly to the Waypoint ID
      //this comes from supporting the pure virtual functions in the base
      //mWorldSpacePos = osg::Vec3(0.0f, 0.0f, 0.0f);

      //set the rest to reasonable defaults
      mWaypointSize = 10.0f;
      mWaypointColor = Color(1.0f, 1.0f, 1.0f, 1.0f);

      mWaypointFontFile = dtUtil::RefString("Arial.ttf");
      mWaypointFontSizeScalar = 0.5f;
      mWaypointTextOffset = osg::Vec3(0.0f, 0.0f, 0.3f);
      mWaypointFontColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
      mNavMeshColor = Color(1.0f, 1.0f, 1.0f, 1.0f);

      mNavMeshFontFile = dtUtil::RefString("Arial.ttf");
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
   }


} // namespace dtAI
