/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005, MOVES Institute
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
 * @author William E. Johnson II
 */
#ifndef DELTA_COORDINATE_CONFIG_ACTOR
#define DELTA_COORDINATE_CONFIG_ACTOR

#include <osg/Vec2d>
#include <osg/Vec3d>
#include <dtDAL/plugin_export.h>
#include <dtDAL/actorproxyicon.h>
#include <dtUtil/coordinates.h>
#include <dtGame/gameactor.h>

namespace dtActors
{
   class DT_PLUGIN_EXPORT CoordinateConfigActor : public dtGame::GameActor
   {
      public:

         /// Constructor
         CoordinateConfigActor(dtGame::GameActorProxy &proxy);

         /**
          * Gets the dtUtil::Coordinates that this actor is wrapping
          * @return mCoordinates
          */
         const dtUtil::Coordinates& GetCoordinates() const { return mCoordinates; }

         /**
          * Sets the origin location
          * @param loc The location to set
          */
         void SetOriginLocation(const osg::Vec3d &loc);

         /**
          * Gets the origin location
          * @return The location
          */
         osg::Vec3d GetOriginLocation() const;

         /**
          * @return The origin location that will be used based on the configuration
          */
         osg::Vec3d GetCurrentOriginLocation() const;

         /**
          * Sets the origin rotation
          * @param hpr The rotation to set
          */
         void SetOriginRotation(const osg::Vec3 &hpr);

         /**
          * Gets the origin location
          * @return The location
          */
         osg::Vec3 GetOriginRotation() const;

         /**
          * @return The origin rotation that will be used based on the configuration
          */
         osg::Vec3 GetCurrentOriginRotation() const;

         /**
          * Sets the geo origin rotation
          * @param hpr The rotation to set
          */
         void SetGeoOriginRotation(const osg::Vec2d &latlon);

         /**
          * Gets the geo origin rotation as lat/lan
          * @return The location
          */
         osg::Vec2d GetGeoOriginRotation() const;

         /**
          * Sets the geo origin in lat lon elevation
          * @param hpr The rotation to set
          */
         void SetGeoOrigin(const osg::Vec3d &latLonEle);

         /**
          * Gets the geo origin is lat lon elevation
          * @return The location
          */
         osg::Vec3d GetGeoOrigin() const;

         /**
          * @return The geo origin converted back from the currently set origin data on 
          *         the coordinate object.
          */
         osg::Vec3d GetConvertedGeoOrigin() const;

         /**
          * Sets the globe radius
          * @param radius The new radius
          */
         void SetGlobeRadius(float radius);

         /**
          * Gets the globe radius
          * @return The globe radius
          */
         float GetGlobeRadius() const;

         /**
          * Sets the incoming coordinate type
          * @param type The new coordinate type
          */
         void SetIncomingCoordinateType(dtUtil::IncomingCoordinateType &type);

         /**
          * Gets the current local coordinate type
          * @return The type
          */
         dtUtil::IncomingCoordinateType& GetIncomingCoordinateType();

         /**
          * Sets the local coordinate type
          * @param type The new coordinate type
          */
         void SetLocalCoordinateType(dtUtil::LocalCoordinateType &type);

         /**
          * Gets the current local coordinate type
          * @return The type
          */
         dtUtil::LocalCoordinateType& GetLocalCoordinateType();

         /**
          * Sets the magnetic north offset
          * @param magNorth The new offset to set
          */
         void SetMagneticNorthOffset(float magNorth);

         /**
          * Gets the magnetic north offset of the coordinate set
          * @return The offset
          */
         float GetMagneticNorthOffset() const;   

         ///@return the UTM zone that outgoing UTM coordinates should assume.
         int GetUTMZone() const;
         
         ///Set the UTM zone that outgoing UTM coordinates should assume.
         void SetUTMZone(int newZone);


         /**
          * @return true if the Geo Origin property is considered to the be absolute and the regular
          *         origin is considered to be calculated.  False if the opposite is true.
          */
         bool UseGeoOrigin() const;
         
         ///Sets if the GeoOrigin should be the method of setting the origin position.
         void SetUseGeoOrigin(bool which);

         /**
          * @return true if the Geo Origin Rotation property is considered to the be absolute and the regular
          *         origin is considered to be calculated.  False if the opposite is true.
          */
         bool UseGeoOriginRotation() const;

         ///Sets if the GeoOriginRotation should be the method of setting the origin position.
         void SetUseGeoOriginRotation(bool which);

         const dtUtil::Coordinates& GetCoordinateConverter() const { return mCoordinates; }

      protected:

         /// Destructor
         virtual ~CoordinateConfigActor();

      private:

         dtUtil::Coordinates mCoordinates;
         osg::Vec2d mGeoOriginRotation;
         osg::Vec3d mGeoOrigin;
         bool mUseGeoOrigin;
         bool mUseGeoOriginRotation;

         osg::Vec3d mOrigin;
         osg::Vec3 mOriginRotation;

   };

   class DT_PLUGIN_EXPORT CoordinateConfigActorProxy : public dtGame::GameActorProxy
   {
      public:

         // Constructor
         CoordinateConfigActorProxy();

         /// Create the actual actor this proxy encapsulates
         void CreateActor() { mActor = new CoordinateConfigActor(*this); }

         /// Builds the properties associated with this actor proxy
         void BuildPropertyMap();

         /**
          * Gets the billboard used to represent static meshes if this proxy's
          * render mode is RenderMode::DRAW_BILLBOARD_ICON.
          * @return a pointer to the icon
          */
         virtual dtDAL::ActorProxyIcon* GetBillBoardIcon(); 

         /**
          * Gets the current render mode for positional lights.
          * @return ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON.
          */
         virtual const ActorProxy::RenderMode& GetRenderMode() 
         {
            return ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
         }


      protected:

         // Destructor
         virtual ~CoordinateConfigActorProxy();
   };
}
#endif
