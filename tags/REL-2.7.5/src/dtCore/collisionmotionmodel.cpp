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
 * Bradley Anderegg and Chris Darken 08/21/2006
 * Erik Johnson and Jeff Houde 07/05/2011
 */

#include <prefix/dtcoreprefix.h>
#include <dtCore/collisionmotionmodel.h>

#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/scene.h>
#include <dtCore/transform.h>
#include <dtUtil/mathdefines.h>


namespace dtCore
{

IMPLEMENT_MANAGEMENT_LAYER(CollisionMotionModel);

////////////////////////////////////////////////////////////////////////////////
CollisionMotionModel::CollisionMotionModel(float pHeight, float pRadius, float k, float theta,
                                           dtCore::Scene* pScene,
                                           Keyboard* keyboard,
                                           Mouse* mouse,
                                           float maxWalkSpeed,
                                           float maxTurnSpeed,
                                           float maxSidestepSpeed,                                           
                                           bool useWASD,
                                           bool useArrowKeys)
   : FPSMotionModel(keyboard, mouse, maxWalkSpeed, maxTurnSpeed,
                    maxSidestepSpeed, pHeight, k, useWASD, useArrowKeys)
   , mCollider(pHeight, pRadius, k, theta, pScene)
   , mCanJump(true)
   , mCollisionEnabled(true)
{
   DEPRECATE("CollisionMotionModel::CollisionMotionModel(float, float, float, float, dtCore::Scene*, Keyboard*, Mouse*, float, float, float, bool, bool)",
      "CollisionMotionModel::CollisionMotionModel(float, float, float, dtCore::Scene*, Keyboard*, Mouse*, float, float, float, float, float, float, bool, bool, bool)");

   // parent class FPSMotionModel has set name as "FPSMotionModel" in its constructor;
   // so now manually reset name correctly here
   SetName("CollisionMotionModel");

   // legacy has the CollisionMotionModel requiring mousedown to look
   // override default behavior in FPSMotionModel to this end
   //SetUseMouseButtons(true); // no longer doing; something is causing window to not get mousemove msgs when mouse down...

   if (keyboard != NULL && mouse != NULL)
   {
      SetDefaultMappings(keyboard, mouse);
   }

   // sender adding is already handled in the base class
   RegisterInstance(this);
}

////////////////////////////////////////////////////////////////////////////////
CollisionMotionModel::CollisionMotionModel(float pHeight,
                                           float pRadius,
                                           float stepUpHeight,
                                           dtCore::Scene* pScene,
                                           Keyboard* keyboard,
                                           Mouse* mouse,
                                           float maxWalkSpeed,
                                           float maxTurnSpeed,
                                           float maxSidestepSpeed,   
                                           float jumpSpeed,
                                           float slideSpeed,
                                           float slideThreshold,
                                           bool canJump,
                                           bool useWASD,
                                           bool useArrowKeys)
   : FPSMotionModel(keyboard, mouse, maxWalkSpeed, maxTurnSpeed,
                    maxSidestepSpeed, pHeight, stepUpHeight, useWASD, useArrowKeys)
   , mCollider(pHeight, pRadius, stepUpHeight, 0.0f, pScene)
   , mCanJump(canJump)
   , mCollisionEnabled(true)
{
   // parent class FPSMotionModel has set name as "FPSMotionModel" in its constructor;
   // so now manually reset name correctly here
   SetName("CollisionMotionModel");

   mCollider.SetSlideSpeed(slideSpeed);
   mCollider.SetSlideThreshold(slideThreshold);
   mCollider.SetJumpSpeed(jumpSpeed);

   if (keyboard != NULL && mouse != NULL)
   {
      SetDefaultMappings(keyboard, mouse);
   }

   // sender adding is already handled in the base class
   RegisterInstance(this);
}


////////////////////////////////////////////////////////////////////////////////
CollisionMotionModel::~CollisionMotionModel()
{
   DeregisterInstance(this);
   // sender removal is already handled in the base class
}

FPSCollider& CollisionMotionModel::GetFPSCollider()
{
   return mCollider;
}

void CollisionMotionModel::PerformTranslation(const double deltaTime)
{
   Transform transform;
   osg::Vec3 currentXYZ, hpr;

   // query initial status (to change from)
   GetTarget()->GetTransform(transform);
   transform.GetTranslation(currentXYZ);
   transform.GetRotation(hpr);

   // calculate x/y delta
   osg::Vec3 velocity(0.0f, 0.0f, 0.0f);
   velocity[0] = GetSidestepFactor()    * GetMaximumSidestepSpeed();
   //velocity[0] = 0.25f * GetMaximumSidestepSpeed();
   velocity[1] = GetForwardBackFactor() * GetMaximumWalkSpeed();

   // Clamp the velocity so you cannot go beyond our maximum speed.
   float maxSpeed = dtUtil::Max<float>(GetMaximumSidestepSpeed(), GetMaximumWalkSpeed());
   float curSpeed = velocity.length();
   if (curSpeed > maxSpeed && maxSpeed > 0.0f)
   {
      float mul = maxSpeed / curSpeed;
      velocity *= mul;
   }

   // transform our x/y delta by our new heading
   osg::Matrix mat;
   const float heading = hpr[0];
   mat.makeRotate(osg::DegreesToRadians(heading), osg::Vec3(0.0f, 0.0f, 1.0f));
   velocity = velocity * mat;

   // perform integration step, physically constraining translation path to environment
   if (mCollisionEnabled)
   {
      currentXYZ = mCollider.Update(currentXYZ, velocity, deltaTime, mCanJump ? GetKeyboard()->GetKeyState(' ') : false);
   }

   // apply changes (new position)
   transform.SetTranslation(currentXYZ);
   GetTarget()->SetTransform(transform);
}

/////////////////////////////////////////////////////////////////////////////

}
