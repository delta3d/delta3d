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

#ifndef DELTA_WAYPOINTRENDERINFO
#define DELTA_WAYPOINTRENDERINFO

#include <dtAI/export.h>
#include <dtAI/primitives.h>
#include <osg/Vec3>
#include <osg/Vec4>
#include <dtUtil/refstring.h>
#include <dtDAL/propertycontainer.h>

namespace dtAI
{

   class DT_AI_EXPORT WaypointRenderInfo : public dtDAL::PropertyContainer
   {
      public:
         typedef osg::Vec4 Color;

      public:
         WaypointRenderInfo();
         virtual ~WaypointRenderInfo();

         ///you must call init to create the properties
         void Init();

         ///sets reasonable defaults to all parameters except position, does not reset the position
         void SetDefaults();

      protected:

         //override this to create properties
         virtual void BuildPropertyMap();

      public:
         
         osg::Vec3 mWorldSpacePos;

         bool mRenderWaypoints, mRenderWaypointID, mRenderWaypointText, mRenderNavMesh, mRenderNavMeshText;

         //Waypoints
         float mWaypointSize;
         Color mWaypointColor;

         //Waypoint Font
         dtUtil::RefString mWaypointFontFile;
         float mWaypointFontSizeScalar;
         osg::Vec3 mWaypointTextOffset;
         Color mWaypointFontColor;

         //Nav Mesh
         float mNavMeshWidth;
         Color mNavMeshColor;

         //Nav Mesh Font
         dtUtil::RefString mNavMeshFontFile;
         float mNavMeshFontSizeScalar;
         ///the text is scaled across the navmesh path
         float mNavMeshTextOffsetScalar;
         Color mNavMeshFontColor;
   };

} // namespace dtAI

#endif // DELTA_WAYPOINTRENDERINFO
