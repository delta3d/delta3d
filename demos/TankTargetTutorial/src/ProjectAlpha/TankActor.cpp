/*
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
* @author Curtiss Murphy
*/
#include "TankActor.h"
#include "ActorsRegistry.h"
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/actorproxyicon.h>
#include <dtCore/loadable.h>
#include <dtCore/isector.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/basemessages.h>
#include <dtABC/application.h>
#include <dtCore/camera.h>
#include <dtCore/keyboard.h>
#include <dtCore/particlesystem.h>
#include <dtUtil/matrixutil.h>
#include <dtUtil/mathdefines.h>

///////////////////////////////////////////////////////////////////////////////
TankActor::TankActor(dtGame::GameActorProxy &proxy) : 
   dtActors::GameMeshActor(proxy), 
   mVelocity(0.0f), 
   mTurnRate(0.0f)
{
   SetName("HoverTank");
}


///////////////////////////////////////////////////////////////////////////////
void TankActor::SetVelocity(float velocity)
{
	mVelocity = velocity;
}

///////////////////////////////////////////////////////////////////////////////
void TankActor::SetTurnRate(float rate)
{
   mTurnRate = rate;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TankActorProxy::TankActorProxy()
{
   SetClassName("HoverTank");
}

///////////////////////////////////////////////////////////////////////////////
void TankActorProxy::BuildPropertyMap()
{
   const std::string GROUP = "HoverTank";

   dtActors::GameMeshActorProxy::BuildPropertyMap();
   TankActor &actor = dynamic_cast<TankActor&>(GetGameActor());

   // TUTORIAL - ADD YOUR PROPERTIES HERE

   // "Velocity" property
   //AddProperty(new dtDAL::FloatActorProperty("Velocity","Velocity",
   //   dtDAL::MakeFunctor(actor, &TankActor::SetVelocity),
   //   dtDAL::MakeFunctorRet(actor, &TankActor::GetVelocity),
   //   "Sets/gets the hover tank's velocity.", GROUP));

   // "Turnrate" property
   //AddProperty(new dtDAL::FloatActorProperty("Turnrate","Turn Rate",
   //   dtDAL::MakeFunctor(actor, &TankActor::SetTurnRate),
   //   dtDAL::MakeFunctorRet(actor, &TankActor::GetTurnRate),
   //   "Sets/gets the hover tank's turn rate in degrees per second.", GROUP));
}

///////////////////////////////////////////////////////////////////////////////
void TankActorProxy::CreateActor()
{
   // TUTORIAL - CREATE YOUR ACTOR HERE
   // Note - We give the actor to the proxy and it holds a ref ptr to it - it gets
   // cleaned up nicely when the proxy gets removed.
   //TankActor *actor = new TankActor(*this);
   //SetActor(*actor);
}

