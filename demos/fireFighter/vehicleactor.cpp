/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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

#include <fireFighter/vehicleactor.h>

#include <dtCore/object.h>

#include <dtDAL/actorproxyicon.h>
#include <dtDAL/enumactorproperty.h>
#include <dtDAL/functor.h>

///////////////////////////////////////////////////////
VehicleActorProxy::VehicleActorProxy()
{

}

VehicleActorProxy::~VehicleActorProxy()
{

}

void VehicleActorProxy::BuildPropertyMap()
{
   dtGame::GameActorProxy::BuildPropertyMap();

   VehicleActor& va = static_cast<VehicleActor&>(GetGameActor());
   AddProperty(new dtDAL::EnumActorProperty<VehicleActor::CoordSys>("CoordinateSystem", "CoordinateSystem",
      dtUtil::MakeFunctor(&VehicleActor::SetCoordSys, va),
      dtUtil::MakeFunctor(&VehicleActor::GetCoordSys, va),
      "Sets the coordinate system"));
}

void VehicleActorProxy::BuildInvokables()
{
   dtGame::GameActorProxy::BuildInvokables();
}

dtDAL::ActorProxyIcon* VehicleActorProxy::GetBillBoardIcon()
{
   if (!mBillBoardIcon.valid())
   {
      mBillBoardIcon = new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IMAGE_BILLBOARD_STATICMESH);
   }
   return mBillBoardIcon.get();
}

///////////////////////////////////////////////////////
IMPLEMENT_ENUM(VehicleActor::CoordSys);
VehicleActor::CoordSys VehicleActor::CoordSys::SYS_ABS("ABS");
VehicleActor::CoordSys VehicleActor::CoordSys::SYS_REL("REL");

VehicleActor::VehicleActor(dtGame::GameActorProxy &proxy)
   : dtGame::GameActor(proxy)
   , mEngineRunning(true)
   , mCoordSys(&VehicleActor::CoordSys::SYS_REL)
{

}

VehicleActor::~VehicleActor()
{

}

void VehicleActor::StartEngines()
{
   mEngineRunning = true;
}

void VehicleActor::ShutDownEngines()
{
   mEngineRunning = false;
}

void VehicleActor::SetCoordSys(VehicleActor::CoordSys& sys)
{
   mCoordSys = &sys;
}

VehicleActor::CoordSys& VehicleActor::GetCoordSys()
{
   return *mCoordSys;
}
