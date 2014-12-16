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
#include <dtCore/propertycontainer.h>

#include <dtCore/propertymacros.h>

namespace dtAI
{

   class DT_AI_EXPORT WaypointRenderInfo : public dtCore::PropertyContainer
   {
   public:
      typedef osg::Vec4 Color;

   public:
      WaypointRenderInfo();
      virtual ~WaypointRenderInfo();

      /// you must call init to create the properties
      void Init();

      /// sets reasonable defaults to all parameters except position, does not reset the position
      void SetDefaults();

      static dtCore::RefPtr<dtCore::ObjectType> TYPE;
      /*override*/ const dtCore::ObjectType& GetObjectType() const;
   protected:
      // override this to create properties
      virtual void BuildPropertyMap();

   public:
      osg::Vec3 mWorldSpacePos;

      DT_DECLARE_ACCESSOR(bool, EnableDepthTest);
      DT_DECLARE_ACCESSOR(bool, RenderWaypoints);
      DT_DECLARE_ACCESSOR(bool, RenderWaypointID);
      DT_DECLARE_ACCESSOR(bool, RenderWaypointText);
      DT_DECLARE_ACCESSOR(bool, RenderNavMesh);
      DT_DECLARE_ACCESSOR(bool, RenderNavMeshText);

      /// @return true if any of the rendering options are enabled
      bool IsAnyRenderingEnabled() const;
      /// Enable or Disable all the rendering options
      void SetAllRenderingOptions(bool value);

      // Waypoints
      DT_DECLARE_ACCESSOR(float, WaypointSize);
      DT_DECLARE_ACCESSOR(Color, WaypointColor);

      // Waypoint Font
      DT_DECLARE_ACCESSOR(std::string, WaypointFontFile);
      DT_DECLARE_ACCESSOR(float,       WaypointFontSizeScalar);
      DT_DECLARE_ACCESSOR(osg::Vec3,   WaypointTextOffset);
      DT_DECLARE_ACCESSOR(Color,       WaypointFontColor);

      // Nav Mesh
      DT_DECLARE_ACCESSOR(float, NavMeshWidth);
      DT_DECLARE_ACCESSOR(Color, NavMeshColor);

      // Nav Mesh Font
      DT_DECLARE_ACCESSOR(std::string, NavMeshFontFile);
      DT_DECLARE_ACCESSOR(float,       NavMeshFontSizeScalar);
      /// the text is scaled across the navmesh path
      DT_DECLARE_ACCESSOR(float, NavMeshTextOffsetScalar);
      DT_DECLARE_ACCESSOR(Color, NavMeshFontColor);
   };

} // namespace dtAI

#endif // DELTA_WAYPOINTRENDERINFO
