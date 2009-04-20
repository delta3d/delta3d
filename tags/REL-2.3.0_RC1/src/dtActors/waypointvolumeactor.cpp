/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2008 MOVES Institute
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
* Michael Guerrero
*/
#include <dtActors/waypointvolumeactor.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtCore/transform.h>

#include <assert.h>

namespace dtActors
{
   IMPLEMENT_MANAGEMENT_LAYER(WaypointVolumeActor)

   //////////////////////////////////////////////////////////////////////////////
   WaypointVolumeActor::WaypointVolumeActor(const std::string &name)
   {
      RegisterInstance(this);
      SetName("VolumeActor");
   }

   //////////////////////////////////////////////////////////////////////////////
   WaypointVolumeActor::~WaypointVolumeActor()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   bool WaypointVolumeActor::IsPointInVolume(float x, float y, float z)
   {
      Transformable::CollisionGeomType *type = GetCollisionGeomType();

      std::vector<float> dimensions;
      GetCollisionGeomDimensions(dimensions);

      osg::Vec3 point(x, y, z);

      if (type == &Transformable::CollisionGeomType::CUBE)
      {
         float halfWidth  = dimensions[0] * 0.5f;
         float halfLength = dimensions[1] * 0.5f;
         float halfHeight = dimensions[2] * 0.5f;

         dtCore::Transform transform;
         GetTransform(transform);

         osg::Vec3 translation;
         transform.GetTranslation(translation);

         osg::Matrix rotation;
         transform.GetRotation(rotation);

         rotation.invert(rotation);

         point -= translation;
         point = point * rotation;

         osg::Vec3 botLeft(-halfWidth, -halfLength, -halfHeight);
         osg::Vec3 topRight(halfWidth, halfLength, halfHeight);

         if (point.x() > botLeft.x() && point.x() < topRight.x() &&
             point.y() > botLeft.y() && point.y() < topRight.y() &&
             point.z() > botLeft.z() && point.z() < topRight.z())
         {
            return true;
         }
      }
      else if (type == &Transformable::CollisionGeomType::SPHERE)
      {
         dtCore::Transform transform;
         GetTransform(transform);

         osg::Vec3 translation;
         transform.GetTranslation(translation);

         float lengthSquared = (point - translation).length2();
         float radiusSquared = osg::square(dimensions[0]);

         if (lengthSquared < radiusSquared)
         {
            return true;
         }
      }
      else
      {
         assert(!"Geometry type not supported");
      }

      return false;
   }

   ////////////////////////////END WaypointVolumeActor///////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   ///////////////////////////BEGIN WaypointVolumeActorProxy/////////////////////
   //////////////////////////////////////////////////////////////////////////////
   WaypointVolumeActorProxy::WaypointVolumeActorProxy()
   {
      SetClassName("dtActors::WaypointVolumeActor");
   }

   //////////////////////////////////////////////////////////////////////////////
   WaypointVolumeActorProxy::~WaypointVolumeActorProxy()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void WaypointVolumeActorProxy::BuildPropertyMap()
   {
      const std::string GROUPNAME = "Spacing";

      dtDAL::TransformableActorProxy::BuildPropertyMap();
      WaypointVolumeActor &volume = static_cast<WaypointVolumeActor &>(*GetActor());

      //Description...
      AddProperty(new dtDAL::FloatActorProperty("name","label",
         dtDAL::MakeFunctor(volume, &WaypointVolumeActor::SetWaypointSpacing),
         dtDAL::MakeFunctorRet(volume, &WaypointVolumeActor::GetWaypointSpacing),
         "Sets/gets the spacing between waypoints in explorer tool.",GROUPNAME));
   }

   //////////////////////////////////////////////////////////////////////////////
   void WaypointVolumeActorProxy::OnEnteredWorld()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void WaypointVolumeActorProxy::CreateActor()
   {
      SetActor(*new WaypointVolumeActor);
   }
}
