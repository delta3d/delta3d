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

#include "dtActors/coordinateconfigactor.h"
#include <dtDAL/enginepropertytypes.h>

#include <osg/Math>

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
      dtGame::GameActorProxy::BuildPropertyMap();
      
      CoordinateConfigActor *cca = dynamic_cast<CoordinateConfigActor*>(mActor.get());
      if(cca == NULL)
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type CoordinateConfigActor");

      dtCore::RefPtr<dtDAL::Vec3dActorProperty> currentOriginProp = new dtDAL::Vec3dActorProperty("Current Origin", "Current Origin Translation", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetOriginLocation), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::GetCurrentOriginLocation), 
         "The current origin translation that will be used based on the actor property configuration.",
         "Current Coordinate Config");
      currentOriginProp->SetReadOnly(true);
      AddProperty(currentOriginProp.get());

      AddProperty(new dtDAL::Vec3dActorProperty("Origin", "Origin Translation", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetOriginLocation), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::GetOriginLocation), 
         "Sets the origin location of the coordinate set",
         "Coordinate Config"));

      AddProperty(new dtDAL::Vec3ActorProperty("Origin Rotation", "Origin Rotation", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetOriginRotation), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::GetOriginRotation), 
         "Sets the origin rotation of the coordinate set",
         "Coordinate Config"));

      dtCore::RefPtr<dtDAL::Vec3ActorProperty> currentOriginRotProp = new dtDAL::Vec3ActorProperty("Current Origin Rotation", "Current Origin Rotation", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetOriginRotation), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::GetCurrentOriginRotation), 
         "The current origin translation that will be used based on the actor property configuration.", "Current Coordinate Config");
      currentOriginRotProp->SetReadOnly(true);
      AddProperty(currentOriginRotProp.get());

      AddProperty(new dtDAL::Vec3dActorProperty("Geo Origin", "Geo Origin", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetGeoOrigin), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::GetGeoOrigin), 
         "Sets the geo origin in lat, lon, elevation",
         "Coordinate Config"));

      dtCore::RefPtr<dtDAL::Vec3dActorProperty> convertedGeoOriginProp = new dtDAL::Vec3dActorProperty("Converted Geo Origin", "Converted Geo Origin", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetGeoOrigin), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::GetConvertedGeoOrigin), 
         "The Geo Origin in Lat/Lon/Elevation converted from the current origin translation",
         "Current Coordinate Config");
      convertedGeoOriginProp->SetReadOnly(true);
      AddProperty(convertedGeoOriginProp.get());

      AddProperty(new dtDAL::Vec2dActorProperty("Geo Origin Rotation", "Geo Origin Rotation", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetGeoOriginRotation), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::GetGeoOriginRotation), 
         "Sets the geo origin rotation of the coordinate set",
         "Coordinate Config"));

      AddProperty(new dtDAL::FloatActorProperty("Globe Radius", "Globe Radius", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetGlobeRadius), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::GetGlobeRadius), 
         "Sets the globe radius of the coordinate set",
         "Coordinate Config"));

      AddProperty(new dtDAL::FloatActorProperty("Magnetic North Offset", "Magnetic North Offset", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetMagneticNorthOffset), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::GetMagneticNorthOffset), 
         "Sets the magnetic north offset of the coordinate set",
         "Coordinate Config"));

      AddProperty(new dtDAL::IntActorProperty("UTM Zone", "UTM East-West Zone", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetUTMZone), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::GetUTMZone), 
         "The Universal Transverse Mercator East-West Zone number to use when converting outgoing coordinates.",
         "Coordinate Config"));

      AddProperty(new dtDAL::BooleanActorProperty("Use Geo Origin", "Use Geo Origin", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetUseGeoOrigin), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::UseGeoOrigin), 
         "Use the Geo origin to calculate the origin. False for using the regular origin.",
         "Coordinate Config"));

      AddProperty(new dtDAL::BooleanActorProperty("Use Geo Origin Rotation", "Use Geo Origin Rotation", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetUseGeoOriginRotation), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::UseGeoOriginRotation), 
         "Use the Geo origin rotation to calculate the rotation offset. False for using the regular rotation property.",
         "Coordinate Config"));

      AddProperty(new dtDAL::EnumActorProperty<dtUtil::IncomingCoordinateType>("Incoming Coordinate Type", "Incoming Coordinate Type", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetIncomingCoordinateType), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::GetIncomingCoordinateType), 
         "Sets the incoming coordinate type of the coordinate set",
         "Coordinate Config"));

      AddProperty(new dtDAL::EnumActorProperty<dtUtil::LocalCoordinateType>("Local Coordinate Type", "Local Coordinate Type", 
         dtDAL::MakeFunctor(*cca, &CoordinateConfigActor::SetLocalCoordinateType), 
         dtDAL::MakeFunctorRet(*cca, &CoordinateConfigActor::GetLocalCoordinateType), 
         "Sets the local coordinate type of the coordinate set",
         "Coordinate Config"));

   }

   dtDAL::ActorProxyIcon* CoordinateConfigActorProxy::GetBillBoardIcon()
   {
      if(!mBillBoardIcon.valid())
         mBillBoardIcon = new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::STATICMESH);
      
      return mBillBoardIcon.get();
   }

   /////////////////////////////////////////////////////
   // Actor code
   /////////////////////////////////////////////////////
   CoordinateConfigActor::CoordinateConfigActor(dtGame::GameActorProxy &proxy) : 
      GameActor(proxy), mUseGeoOrigin(false), mUseGeoOriginRotation(false)
   {
      
   }

   CoordinateConfigActor::~CoordinateConfigActor()
   {

   }

   void CoordinateConfigActor::SetOriginLocation(const osg::Vec3d &loc)
   {
      if (!mUseGeoOrigin)
      {
         mCoordinates.SetOriginLocation(loc[0], loc[1], loc[2]);
      }
      mOrigin = loc;
   }

   osg::Vec3d CoordinateConfigActor::GetOriginLocation() const
   {
      return mOrigin;
   }

   osg::Vec3d CoordinateConfigActor::GetCurrentOriginLocation() const 
   {
      double x, y, z;
      mCoordinates.GetOriginLocation(x, y, z);
      return osg::Vec3d(x, y, z);
   }

   void CoordinateConfigActor::SetOriginRotation(const osg::Vec3 &hpr)
   {
      if (!mUseGeoOriginRotation)
      {
         mCoordinates.SetOriginRotation(hpr[0], hpr[1], hpr[2]);
      }
      mOriginRotation = hpr;
   }

   osg::Vec3 CoordinateConfigActor::GetOriginRotation() const 
   {
      return mOriginRotation;
   }

   osg::Vec3 CoordinateConfigActor::GetCurrentOriginRotation() const 
   {
      float h, p, r;
      mCoordinates.GetOriginRotation(h, p, r);
      return osg::Vec3(h, p, r);
   }

   void CoordinateConfigActor::SetGeoOriginRotation(const osg::Vec2d &latlon)
   {
      if (mUseGeoOriginRotation)
      {
         mCoordinates.SetGeoOriginRotation(latlon.x(), latlon.y());
      }
      mGeoOriginRotation = latlon;
   }

   osg::Vec2d CoordinateConfigActor::GetGeoOriginRotation() const
   {
      return mGeoOriginRotation;
   }

   void CoordinateConfigActor::SetGeoOrigin(const osg::Vec3d &latLonEle)
   {
      if (mUseGeoOrigin)
      {
         mCoordinates.SetGeoOrigin(latLonEle.x(), latLonEle.y(), latLonEle.z());
      }
      mGeoOrigin = latLonEle;
   }

   osg::Vec3d CoordinateConfigActor::GetGeoOrigin() const
   {
         return mGeoOrigin;
   }

   osg::Vec3d CoordinateConfigActor::GetConvertedGeoOrigin() const
   {
      double x, y, z;
      double latitude, longitude;
      mCoordinates.GetOriginLocation(x, y, z);      

      mCoordinates.ConvertUTMToGeodetic(mCoordinates.GetUTMZone(), x, y, latitude, longitude);

      return osg::Vec3d(osg::RadiansToDegrees(latitude), osg::RadiansToDegrees(longitude), z);
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
      //enum properties can't handle const enums. 
      return const_cast<dtUtil::IncomingCoordinateType&>(mCoordinates.GetIncomingCoordinateType());
   }

   void CoordinateConfigActor::SetLocalCoordinateType(dtUtil::LocalCoordinateType &type)
   {
      mCoordinates.SetLocalCoordinateType(type);
   }

   dtUtil::LocalCoordinateType& CoordinateConfigActor::GetLocalCoordinateType()
   {
      //enum properties can't handle const enums. 
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

   int CoordinateConfigActor::GetUTMZone() const
   {
      return int(mCoordinates.GetUTMZone());
   }
   
   void CoordinateConfigActor::SetUTMZone(int newZone)
   {
      return mCoordinates.SetUTMZone(unsigned(newZone));
   }

   bool CoordinateConfigActor::UseGeoOrigin() const
   {
      return mUseGeoOrigin;
   }
   
   void CoordinateConfigActor::SetUseGeoOrigin(bool which)
   {
      mUseGeoOrigin = which;
      SetOriginLocation(mOrigin);
      SetGeoOrigin(mGeoOrigin);
   }

   bool CoordinateConfigActor::UseGeoOriginRotation() const
   {
      return mUseGeoOriginRotation;
   }

   void CoordinateConfigActor::SetUseGeoOriginRotation(bool which)
   {
      mUseGeoOriginRotation = which;
      SetOriginRotation(mOriginRotation);
      SetGeoOriginRotation(mGeoOriginRotation);
   }
}
