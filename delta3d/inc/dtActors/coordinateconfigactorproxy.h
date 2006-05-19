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

#include <dtDAL/plugin_export.h>
#include <dtUtil/coordinates.h>
#include <dtGame/gameactor.h>

namespace dtActors
{
   class CoordinateConfigActor : public dtGame::GameActor
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
          * Sets the origin rotation
          * @param hpr The rotation to set
          */
         void SetOriginRotation(const osg::Vec3f &hpr);

         /**
          * Gets the origin location
          * @return The location
          */
         osg::Vec3f GetOriginRotation() const;

         /**
          * Sets the geo origin rotation
          * @param hpr The rotation to set
          */
         void SetGeoOriginRotation(const osg::Vec2d &latlon);

         /**
          * Gets the geo origin location
          * @return The location
          */
         osg::Vec2d GetGeoOriginRotation() const;

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

      protected:

         /// Destructor
         virtual ~CoordinateConfigActor();

      private:

         dtUtil::Coordinates mCoordinates;
   };

   class CoordinateConfigActorProxy : public dtGame::GameActorProxy
   {
      public:

         // Constructor
         CoordinateConfigActorProxy();

         /// Create the actual actor this proxy encapsulates
         void CreateActor() { mActor = new CoordinateConfigActor(*this); }

         /// Builds the properties associated with this actor proxy
         void BuildPropertyMap();

      protected:

         // Destructor
         virtual ~CoordinateConfigActorProxy();
   };
}
#endif
