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
const float MAXTANKVELOCITY = 15.0f;

///////////////////////////////////////////////////////////////////////////////
TankActor::TankActor(dtGame::GameActorProxy &proxy) : 
   dtActors::GameMeshActor(proxy), 
   mDust(NULL), 
   mVelocity(0.0f), 
   mTurnRate(0.0f),
   mIsEngineRunning(false),
   mLastReportedVelocity(0.0f),
   mIsector( new dtCore::Isector() )
{
   SetName("HoverTank");
}

///////////////////////////////////////////////////////////////////////////////
void TankActor::TickLocal(const dtGame::Message &tickMessage)
{
   const dtGame::TickMessage &tick = static_cast<const dtGame::TickMessage&>(tickMessage);
   float deltaSimTime = tick.GetDeltaSimTime();

   // TUTORIAL - ADD YOUR LOCAL BEHAVIOR HERE
}

///////////////////////////////////////////////////////////////////////////////
void TankActor::TickRemote(const dtGame::Message &tickMessage)
{
   const dtGame::TickMessage &tick = static_cast<const dtGame::TickMessage&>(tickMessage);
   float deltaSimTime = tick.GetDeltaSimTime();

   // TUTORIAL - ADD YOUR REMOTE BEHAVIOR HERE
}

///////////////////////////////////////////////////////////////////////////////
void TankActor::ComputeVelocityAndTurn(float deltaSimTime)
{
   // calculate current velocity
   float decelDirection = (mVelocity >= 0.0) ? -1.0f : 1.0f;
   float accelDirection = 0.0f;
   float acceleration = 0.0;

   dtCore::Keyboard *keyboard = GetGameActorProxy().GetGameManager()->GetApplication().GetKeyboard();

   // which way is hte user trying to go? 
   if (keyboard->GetKeyState(Producer::Key_I))
      accelDirection = -1.0f;
   else if (keyboard->GetKeyState(Producer::Key_K))
      accelDirection = 1.0f;

   // speed up based on user and current speed (ie, too fast)
   if (mIsEngineRunning && accelDirection != 0.0f)
   {
       //  boosted too fast, slow down
      if ((accelDirection > 0 && mVelocity > MAXTANKVELOCITY) ||
            (accelDirection < 0 && mVelocity < -MAXTANKVELOCITY))
         acceleration = deltaSimTime*(MAXTANKVELOCITY/3.0f)*decelDirection;
      // hold speed
      else if (mVelocity == accelDirection * MAXTANKVELOCITY)
         acceleration = 0;
      // speed up normally - woot!
      else 
         acceleration = accelDirection*deltaSimTime*(MAXTANKVELOCITY/2.0f);
   }
   else if (mVelocity > -0.1 && mVelocity < 0.1)
      acceleration = -mVelocity; // close enough to 0, so just stop
   else // coast to stop
      acceleration = deltaSimTime*(MAXTANKVELOCITY/6.0f)*decelDirection;

   //std::cerr << "Ticking - deltaTime[" << deltaSimTime << "], acceleration [" << acceleration << "]" << std::endl;

   SetVelocity(mVelocity + acceleration);

   if (mIsEngineRunning && keyboard->GetKeyState(Producer::Key_L))
      SetTurnRate(-0.1f);
   else if (mIsEngineRunning && keyboard->GetKeyState(Producer::Key_J))
      SetTurnRate(0.1f);
   else 
      SetTurnRate(0.0f);
}

///////////////////////////////////////////////////////////////////////////////
void TankActor::MoveTheTank(float deltaSimTime)
{
   dtCore::Transform tx;
   osg::Matrix mat;
   osg::Quat q;
   osg::Vec3 viewDir;

   GetTransform(tx);
   tx.GetRotation(mat);
   mat.get(q);
   viewDir = q * osg::Vec3(0,-1,0);

   // translate the player along its current view direction based on current velocity
   osg::Vec3 pos;
   tx.GetTranslation(pos);
   pos = pos + (viewDir*(mVelocity*deltaSimTime));
   
   //particle fun
   if(mDust.valid() && mIsEngineRunning && mVelocity != 0)
   {
      // Get the layer we want
      dtCore::ParticleLayer& pLayerToSet = *mDust->GetSingleLayer("Layer 0");
      
      // make a temp var for changing particle default template.
      osgParticle::Particle& defaultParticle = pLayerToSet.GetParticleSystem().getDefaultParticleTemplate();

      // do our funky changes
      float lifetime = dtUtil::Max(2.0f, abs(mVelocity+1) * .4f);
      defaultParticle.setLifeTime(lifetime);
   }

   // attempt to ground clamp the actor so that he doesn't go through mountains.
   osg::Vec3 intersection;   
   mIsector->Reset(); 
   mIsector->SetStartPosition(osg::Vec3(pos.x(),pos.y(),-10000));
   mIsector->SetDirection(osg::Vec3(0,0,1));
   if (mIsector->Update())
   {
      osgUtil::IntersectVisitor &iv = mIsector->GetIntersectVisitor();
      osg::Vec3 p = iv.getHitList(mIsector->GetLineSegment())[0].getWorldIntersectPoint();
      // make it hover
      pos.z() = p.z() + 2.0f;
   }
   
   osg::Vec3 xyz = GetGameActorProxy().GetRotation();
   xyz[2] += 360.0f * mTurnRate * deltaSimTime;

   tx.SetTranslation(pos);
   SetTransform(tx);
   GetGameActorProxy().SetRotation(xyz);
}

///////////////////////////////////////////////////////////////////////////////
void TankActor::SetVelocity(float velocity)
{
	mVelocity = velocity;

   // Notify the world that our velocity changed, if there is enough difference
   // In a more sophisticated app, you would track acceleration, not just velocity
   // And then you wouldn't have to send velocity but every so often, since acceleration
   // would allow you to dead reckon the position without a network update.
   if ((abs(abs(mLastReportedVelocity) - abs(mVelocity)) > 0.5) || 
      (mLastReportedVelocity != mVelocity && 
         (mVelocity == MAXTANKVELOCITY || mVelocity == 0.0f || mVelocity == -MAXTANKVELOCITY )))
   {
      mLastReportedVelocity = mVelocity;
      GetGameActorProxy().NotifyFullActorUpdate();
   }
}

///////////////////////////////////////////////////////////////////////////////
void TankActor::SetTurnRate(float rate)
{
   if (mTurnRate != rate)
   {
      mTurnRate = rate;
      // Notify the world that our turn rate changed. Only changes on keypress
      GetGameActorProxy().NotifyFullActorUpdate();
   }
}

///////////////////////////////////////////////////////////////////////////////
void TankActor::OnEnteredWorld()
{
   // add our dust particle
	mDust = new dtCore::ParticleSystem();
	mDust->LoadFile("Particles/dust.osg",true);
	mDust->SetEnabled(false);
	AddChild(mDust.get());

   dtActors::GameMeshActor::OnEnteredWorld();

   GetTransform(mOriginalPosition);

   mIsector->SetScene(&(GetGameActorProxy().GetGameManager()->GetScene()));
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

   // "Velocity" property
   AddProperty(new dtDAL::FloatActorProperty("Velocity","Velocity",
      dtDAL::MakeFunctor(actor, &TankActor::SetVelocity),
      dtDAL::MakeFunctorRet(actor, &TankActor::GetVelocity),
      "Sets/gets the hover tank's velocity.", GROUP));

   // "Turnrate" property
   AddProperty(new dtDAL::FloatActorProperty("Turnrate","Turn Rate",
      dtDAL::MakeFunctor(actor, &TankActor::SetTurnRate),
      dtDAL::MakeFunctorRet(actor, &TankActor::GetTurnRate),
      "Sets/gets the hover tank's turn rate in degrees per second.", GROUP));
}

///////////////////////////////////////////////////////////////////////////////
void TankActorProxy::CreateActor()
{
   mActor = new TankActor(*this);
}

///////////////////////////////////////////////////////////////////////////////
void TankActorProxy::OnEnteredWorld()
{
   // Note we did not create any of these Invokables.  ProcessMessage(), TickLocal(), 
   // and TickRemote() are created for us in GameActorProxy::BuildInvokables().

   // Register an invokable for tick messages. Local or Remote only, not both!
   // TUTORIAL - REGISTER FOR TICK_REMOTE IF YOUR ACTOR IS REMOTE, OTHERWISE
   //            REGITER FOR TICK_LOCAL (HINT: CHECK IsRemote() )

   dtActors::GameMeshActorProxy::OnEnteredWorld();
}
