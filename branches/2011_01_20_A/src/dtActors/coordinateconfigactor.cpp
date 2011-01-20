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
 * William E. Johnson II
 */

#include <dtActors/coordinateconfigactor.h>

#include <dtDAL/booleanactorproperty.h>
#include <dtDAL/enumactorproperty.h>
#include <dtDAL/floatactorproperty.h>
#include <dtDAL/functor.h>
#include <dtDAL/intactorproperty.h>
#include <dtDAL/vectoractorproperties.h>

#include <osg/Math>

namespace dtActors
{
   /////////////////////////////////////////////////////
   // Actor proxy code
   /////////////////////////////////////////////////////
   CoordinateConfigActorProxy::CoordinateConfigActorProxy()
   {

   }

   /////////////////////////////////////////////////////
   CoordinateConfigActorProxy::~CoordinateConfigActorProxy()
   {

   }

   /////////////////////////////////////////////////////
   void CoordinateConfigActorProxy::BuildPropertyMap()
   {
      //dtGame::GameActorProxy::BuildPropertyMap();

      SetInitialOwnership(dtGame::GameActorProxy::Ownership::CLIENT_AND_SERVER_LOCAL);

      CoordinateConfigActor *cca = static_cast<CoordinateConfigActor*>(GetDrawable());

      dtCore::RefPtr<dtDAL::Vec3dActorProperty> currentOriginProp = new dtDAL::Vec3dActorProperty("Current Origin", "Current Origin Translation",
               dtDAL::Vec3dActorProperty::SetFuncType(),
               dtDAL::Vec3dActorProperty::GetFuncType(cca, &CoordinateConfigActor::GetCurrentOriginLocation),
               "The current origin translation that will be used based on the actor property configuration.",
               "Current Coordinate Config");
      currentOriginProp->SetReadOnly(true);
      AddProperty(currentOriginProp.get());

      AddProperty(new dtDAL::Vec3dActorProperty("Origin", "Origin Translation",
         dtDAL::Vec3dActorProperty::SetFuncType(cca, &CoordinateConfigActor::SetOriginLocation),
         dtDAL::Vec3dActorProperty::GetFuncType(cca, &CoordinateConfigActor::GetOriginLocation),
         "Sets a local offset that will be applied to the coordinates after the conversion from incoming to local and vice-versa",
         "Coordinate Config"));

      AddProperty(new dtDAL::Vec3dActorProperty("Geo Origin", "Geo Origin",
         dtDAL::Vec3dActorProperty::SetFuncType(cca, &CoordinateConfigActor::SetGeoOrigin),
         dtDAL::Vec3dActorProperty::GetFuncType(cca, &CoordinateConfigActor::GetGeoOrigin),
         "Sets the same local offset at the Origin Translation property, but using a lat/lon that will be converted via lat lon to UTM translation."
         "This only makes sense in odd cases when a terrain is in a UTM projection, but the origin is provided as a lat lon."
         "If you are using Flat Earth projection, then set the Flat Earth Origin property."
         "You must set Use Geo Origin to true to use this property.",
         "Coordinate Config"));

      AddProperty(new dtDAL::Vec2dActorProperty("Flat Earth Origin", "Flat Earth Origin",
         dtDAL::Vec2dActorProperty::SetFuncType(cca, &CoordinateConfigActor::SetFlatEarthOrigin),
         dtDAL::Vec2dActorProperty::GetFuncType(cca, &CoordinateConfigActor::GetFlatEarthOrigin),
         "Sets the lat lon origin point of reference for flat earth terrain calculations.",
         "Coordinate Config"));

      AddProperty(new dtDAL::BooleanActorProperty("Apply Rotation Conversion Matrix", "Apply Rotation Conversion Matrix",
         dtDAL::BooleanActorProperty::SetFuncType(cca, &CoordinateConfigActor::SetApplyRotationConversionMatrix),
         dtDAL::BooleanActorProperty::GetFuncType(cca, &CoordinateConfigActor::GetApplyRotationConversionMatrix),
         "Set to true if you want a rotation matrix applied to the rotations that will change the frame of reference."
         "This is the case if, for example, the incoming rotation is earth centric and you want ground centric rotations."
         "Set it to false if you want no conversion done.",
         "Coordinate Config"));

      dtCore::RefPtr<dtDAL::Vec3dActorProperty> convertedGeoOriginProp = new dtDAL::Vec3dActorProperty("Converted Geo Origin", "Converted Geo Origin",
               dtDAL::Vec3dActorProperty::SetFuncType(),
               dtDAL::Vec3dActorProperty::GetFuncType(cca, &CoordinateConfigActor::GetConvertedGeoOrigin),
               "The Geo Origin in Lat/Lon/Elevation converted from the current origin translation",
               "Current Coordinate Config");
      convertedGeoOriginProp->SetReadOnly(true);
      AddProperty(convertedGeoOriginProp.get());

      AddProperty(new dtDAL::FloatActorProperty("Globe Radius", "Globe Radius",
         dtDAL::FloatActorProperty::SetFuncType(cca, &CoordinateConfigActor::SetGlobeRadius),
         dtDAL::FloatActorProperty::GetFuncType(cca, &CoordinateConfigActor::GetGlobeRadius),
         "Sets the globe radius of the coordinate set",
         "Coordinate Config"));

      AddProperty(new dtDAL::FloatActorProperty("Magnetic North Offset", "Magnetic North Offset",
         dtDAL::FloatActorProperty::SetFuncType(cca, &CoordinateConfigActor::SetMagneticNorthOffset),
         dtDAL::FloatActorProperty::GetFuncType(cca, &CoordinateConfigActor::GetMagneticNorthOffset),
         "Sets the magnetic north offset to use for the are of the world being simulated.  This is just for reference.",
         "Coordinate Config"));

      AddProperty(new dtDAL::IntActorProperty("UTM Zone", "UTM East-West Zone",
         dtDAL::IntActorProperty::SetFuncType(cca, &CoordinateConfigActor::SetUTMZone),
         dtDAL::IntActorProperty::GetFuncType(cca, &CoordinateConfigActor::GetUTMZone),
         "The Universal Transverse Mercator East-West Zone number to use when converting outgoing coordinates.",
         "Coordinate Config"));

      AddProperty(new dtDAL::BooleanActorProperty("Use Geo Origin", "Use Geo Origin for UTM",
         dtDAL::BooleanActorProperty::SetFuncType(cca, &CoordinateConfigActor::SetUseGeoOrigin),
         dtDAL::BooleanActorProperty::GetFuncType(cca, &CoordinateConfigActor::UseGeoOrigin),
         "Use the Geo origin to calculate the origin for UTM projejctions. False for using the Origin Location."
         "This property is not needed for Flat Earth or Globe projections.",
         "Coordinate Config"));

      AddProperty(new dtDAL::EnumActorProperty<dtUtil::IncomingCoordinateType>("Incoming Coordinate Type", "Incoming Coordinate Type",
         dtDAL::EnumActorProperty<dtUtil::IncomingCoordinateType>::SetFuncType(cca, &CoordinateConfigActor::SetIncomingCoordinateType),
         dtDAL::EnumActorProperty<dtUtil::IncomingCoordinateType>::GetFuncType(cca, &CoordinateConfigActor::GetIncomingCoordinateType),
         "Sets the incoming coordinate type of the coordinate set",
         "Coordinate Config"));

      AddProperty(new dtDAL::EnumActorProperty<dtUtil::LocalCoordinateType>("Local Coordinate Type", "Local Coordinate Type",
         dtDAL::EnumActorProperty<dtUtil::LocalCoordinateType>::SetFuncType(cca, &CoordinateConfigActor::SetLocalCoordinateType),
         dtDAL::EnumActorProperty<dtUtil::LocalCoordinateType>::GetFuncType(cca, &CoordinateConfigActor::GetLocalCoordinateType),
         "Sets the local coordinate type of the coordinate set",
         "Coordinate Config"));
   }

   /////////////////////////////////////////////////////
   dtDAL::ActorProxyIcon* CoordinateConfigActorProxy::GetBillBoardIcon()
   {
      if (!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IMAGE_BILLBOARD_STATICMESH);
      }

      return mBillBoardIcon.get();
   }

   /////////////////////////////////////////////////////
   // Actor code
   /////////////////////////////////////////////////////
   CoordinateConfigActor::CoordinateConfigActor(dtGame::GameActorProxy& proxy)
      : GameActor(proxy)
      , mUseGeoOrigin(false)
   {

   }

   /////////////////////////////////////////////////////
   CoordinateConfigActor::~CoordinateConfigActor()
   {

   }

   /////////////////////////////////////////////////////
   void CoordinateConfigActor::SetOriginLocation(const osg::Vec3d& loc)
   {
      if (!mUseGeoOrigin)
      {
         mCoordinates.SetLocalOffset(loc);
      }
      mOrigin = loc;
   }

   /////////////////////////////////////////////////////
   osg::Vec3d CoordinateConfigActor::GetOriginLocation() const
   {
      return mOrigin;
   }

   /////////////////////////////////////////////////////
   osg::Vec3d CoordinateConfigActor::GetCurrentOriginLocation() const
   {
      osg::Vec3d xyz;
      mCoordinates.GetLocalOffset(xyz);
      return xyz;
   }

   /////////////////////////////////////////////////////
   void CoordinateConfigActor::SetGeoOrigin(const osg::Vec3d &latLonEle)
   {
      if (mUseGeoOrigin)
      {
         mCoordinates.SetUTMLocalOffsetAsLatLon(latLonEle);
      }
      mGeoOrigin = latLonEle;
   }

   /////////////////////////////////////////////////////
   osg::Vec3d CoordinateConfigActor::GetGeoOrigin() const
   {
      return mGeoOrigin;
   }

   /////////////////////////////////////////////////////
   osg::Vec3d CoordinateConfigActor::GetConvertedGeoOrigin() const
   {

      osg::Vec3d xyz;
      double latitude, longitude;
      mCoordinates.GetLocalOffset(xyz);

      mCoordinates.ConvertUTMToGeodetic(mCoordinates.GetUTMZone(), mCoordinates.GetUTMHemisphere(), xyz.x(), xyz.y(), latitude, longitude);

      return osg::Vec3d(osg::RadiansToDegrees(latitude), osg::RadiansToDegrees(longitude), xyz.z());
   }

   /////////////////////////////////////////////////////
   void CoordinateConfigActor::SetGlobeRadius(float radius)
   {
      mCoordinates.SetGlobeRadius(radius);
   }

   /////////////////////////////////////////////////////
   float CoordinateConfigActor::GetGlobeRadius() const
   {
      return mCoordinates.GetGlobeRadius();
   }

   /////////////////////////////////////////////////////
   void CoordinateConfigActor::SetIncomingCoordinateType(dtUtil::IncomingCoordinateType &type)
   {
      mCoordinates.SetIncomingCoordinateType(type);
   }

   /////////////////////////////////////////////////////
   dtUtil::IncomingCoordinateType& CoordinateConfigActor::GetIncomingCoordinateType()
   {
      //enum properties can't handle const enums.
      return const_cast<dtUtil::IncomingCoordinateType&>(mCoordinates.GetIncomingCoordinateType());
   }

   /////////////////////////////////////////////////////
   void CoordinateConfigActor::SetLocalCoordinateType(dtUtil::LocalCoordinateType &type)
   {
      mCoordinates.SetLocalCoordinateType(type);
   }

   /////////////////////////////////////////////////////
   dtUtil::LocalCoordinateType& CoordinateConfigActor::GetLocalCoordinateType()
   {
      //enum properties can't handle const enums.
      return const_cast<dtUtil::LocalCoordinateType&>(mCoordinates.GetLocalCoordinateType());
   }

   /////////////////////////////////////////////////////
   void CoordinateConfigActor::SetMagneticNorthOffset(float magNorth)
   {
      mCoordinates.SetMagneticNorthOffset(magNorth);
   }

   /////////////////////////////////////////////////////
   float CoordinateConfigActor::GetMagneticNorthOffset() const
   {
      return mCoordinates.GetMagneticNorthOffset();
   }

   /////////////////////////////////////////////////////
   int CoordinateConfigActor::GetUTMZone() const
   {
      return int(mCoordinates.GetUTMZone());
   }

   /////////////////////////////////////////////////////
   void CoordinateConfigActor::SetUTMZone(int newZone)
   {
      return mCoordinates.SetUTMZone(unsigned(newZone));
   }

   /////////////////////////////////////////////////////
   void CoordinateConfigActor::SetFlatEarthOrigin(const osg::Vec2d& origin)
   {
      mCoordinates.SetFlatEarthOrigin(origin);
   }

   /////////////////////////////////////////////////////
   osg::Vec2d CoordinateConfigActor::GetFlatEarthOrigin() const
   {
      osg::Vec2d output;
      mCoordinates.GetFlatEarthOrigin(output);
      return output;
   }

   /////////////////////////////////////////////////////
   void CoordinateConfigActor::SetApplyRotationConversionMatrix(bool doApply)
   {
      mCoordinates.SetApplyRotationConversionMatrix(doApply);
   }

   /////////////////////////////////////////////////////
   bool CoordinateConfigActor::GetApplyRotationConversionMatrix() const
   {
      return mCoordinates.GetApplyRotationConversionMatrix();
   }

   /////////////////////////////////////////////////////
   bool CoordinateConfigActor::UseGeoOrigin() const
   {
      return mUseGeoOrigin;
   }

   /////////////////////////////////////////////////////
   void CoordinateConfigActor::SetUseGeoOrigin(bool which)
   {
      mUseGeoOrigin = which;
      SetOriginLocation(mOrigin);
      SetGeoOrigin(mGeoOrigin);
   }
}
