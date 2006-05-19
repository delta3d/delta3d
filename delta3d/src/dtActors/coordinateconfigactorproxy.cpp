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
#include "dtActors/coordinateconfigactorproxy.h"
#include <dtDAL/enginepropertytypes.h>

namespace dtActors
{
   /////////////////////////////////////////////////////
   // Actor proxy code
   /////////////////////////////////////////////////////
   CoordinateConfigActorProxy::CoordinateConfigActorProxy()
   {

   }

   CoordinateConfigActorProxy::~CoordinateConfigActorProxy()
   {

   }

   void CoordinateConfigActorProxy::BuildPropertyMap()
   {
      CoordinateConfigActor *cca = dynamic_cast<CoordinateConfigActor*>(mActor.get());
      if(cca == NULL)
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type CoordinateConfigActor");

      AddProperty(new dtDAL::Vec3dActorProperty("Origin Location", "Origin Location", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetOriginLocation), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::GetOriginLocation), 
         "Sets the origin location of the coordinate set"));

      AddProperty(new dtDAL::Vec3fActorProperty("Origin Rotation", "Origin Rotation", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetOriginRotation), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::GetOriginRotation), 
         "Sets the origin rotation of the coordinate set"));

      AddProperty(new dtDAL::Vec2dActorProperty("Geo Origin Rotation", "Geo Origin Rotation", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetGeoOriginRotation), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::GetGeoOriginRotation), 
         "Sets the geo origin rotation of the coordinate set"));

      AddProperty(new dtDAL::FloatActorProperty("Globe Radius", "Globe Radius", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetGlobeRadius), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::GetGlobeRadius), 
         "Sets the globe radius of the coordinate set"));

      AddProperty(new dtDAL::FloatActorProperty("Magnetic North Offset", "Magnetic North Offset", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetMagneticNorthOffset), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::GetMagneticNorthOffset), 
         "Sets the magnetic north offset of the coordinate set"));

      AddProperty(new dtDAL::EnumActorProperty<dtUtil::IncomingCoordinateType>("Incoming Coordinate Type", "Incoming Coordinate Type", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetIncomingCoordinateType), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::GetIncomingCoordinateType), 
         "Sets the incoming coordinate type of the coordinate set"));

      AddProperty(new dtDAL::EnumActorProperty<dtUtil::LocalCoordinateType>("Local Coordinate Type", "Local Coordinate Type", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetLocalCoordinateType), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::GetLocalCoordinateType), 
         "Sets the local coordinate type of the coordinate set"));
   }

   /////////////////////////////////////////////////////
   // Actor code
   /////////////////////////////////////////////////////
   CoordinateConfigActor::CoordinateConfigActor(dtGame::GameActorProxy &proxy) : GameActor(proxy)
   {
      
   }

   CoordinateConfigActor::~CoordinateConfigActor()
   {

   }

   void CoordinateConfigActor::SetOriginLocation(const osg::Vec3d &loc)
   {
      mCoordinates.SetOriginLocation(loc[0], loc[1], loc[2]);
   }

   osg::Vec3d CoordinateConfigActor::GetOriginLocation() const
   {
      double x, y, z;
      mCoordinates.GetOriginLocation(x, y, z);
      return osg::Vec3d(x, y, z);
   }

   void CoordinateConfigActor::SetOriginRotation(const osg::Vec3f &hpr)
   {
      mCoordinates.SetOriginRotation(hpr[0], hpr[1], hpr[2]);
   }

   osg::Vec3f CoordinateConfigActor::GetOriginRotation() const 
   {
      float h, p, r;
      mCoordinates.GetOriginRotation(h, p, r);
      return osg::Vec3f(h, p, r);
   }

   void CoordinateConfigActor::SetGeoOriginRotation(const osg::Vec2d &latlon)
   {
      mCoordinates.SetGeoOriginRotation(latlon);
   }

   osg::Vec2d CoordinateConfigActor::GetGeoOriginRotation() const
   {
      return mCoordinates.GetGeoOriginRotation();
   }

   void CoordinateConfigActor::SetGlobeRadius(float radius)
   {
      mCoordinates.SetGlobeRadius(radius);
   }

   float CoordinateConfigActor::GetGlobeRadius() const
   {
      return mCoordinates.GetGlobeRadius();
   }

   void CoordinateConfigActor::SetIncomingCoordinateType(dtUtil::IncomingCoordinateType &type)
   {
      mCoordinates.SetIncomingCoordinateType(type);
   }

   dtUtil::IncomingCoordinateType& CoordinateConfigActor::GetIncomingCoordinateType()
   {
      return const_cast<dtUtil::IncomingCoordinateType&>(mCoordinates.GetIncomingCoordinateType());
   }

   void CoordinateConfigActor::SetLocalCoordinateType(dtUtil::LocalCoordinateType &type)
   {
      mCoordinates.SetLocalCoordinateType(type);
   }

   dtUtil::LocalCoordinateType& CoordinateConfigActor::GetLocalCoordinateType()
   {
      return const_cast<dtUtil::LocalCoordinateType&>(mCoordinates.GetLocalCoordinateType());
   }

   void CoordinateConfigActor::SetMagneticNorthOffset(float magNorth)
   {
      mCoordinates.SetMagneticNorthOffset(magNorth);
   }

   float CoordinateConfigActor::GetMagneticNorthOffset() const
   {
      return mCoordinates.GetMagneticNorthOffset();
   }
}
