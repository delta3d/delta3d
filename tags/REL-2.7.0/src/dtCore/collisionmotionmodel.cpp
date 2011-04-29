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
 */

#include <prefix/dtcoreprefix.h>
#include <dtCore/collisionmotionmodel.h>

#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/inputdevice.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/transformable.h>
#include <dtCore/transform.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/matrixutil.h>


namespace dtCore
{

IMPLEMENT_MANAGEMENT_LAYER(CollisionMotionModel);

/**
 * Constructor.
 *
 * @param keyboard the keyboard instance, or 0 to
 * avoid creating default input mappings
 * @param mouse the mouse instance, or 0 to avoid
 * creating default input mappings
 */
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
{
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

/**
* Destructor.
*/
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
   osg::Vec3 xyz, hpr, newXYZ;

   // query initial status (to change from)
   GetTarget()->GetTransform(transform);
   transform.GetTranslation(xyz);
   transform.GetRotation(hpr);

   // calculate x/y delta
   osg::Vec3 translation(0.0f, 0.0f, 0.0f);
   translation[0] = GetSidestepFactor()    * GetMaximumSidestepSpeed();
   translation[1] = GetForwardBackFactor() * GetMaximumWalkSpeed();

   // transform our x/y delta by our new heading
   osg::Matrix mat;
   const float heading = hpr[0];
   mat.makeRotate(osg::DegreesToRadians(heading), osg::Vec3(0.0f, 0.0f, 1.0f));
   translation = translation * mat;

   // limit maximum lateral speed to be the maximum walk speed
   /*
   if (translation.length() > GetMaximumWalkSpeed())
   {
      translation.normalize();
      translation.set(translation[0] * GetMaximumWalkSpeed(), translation[1] * GetMaximumWalkSpeed(), 0.0f);
   }
   //*/

   // perform integration step, physically constraining translation path to environment
   newXYZ = mCollider.Update(xyz, translation, deltaTime, mCanJump ? GetKeyboard()->GetKeyState(' ') : false);
   //newXYZ = xyz + translation * deltaTime;

   // apply changes (new position)
   transform.SetTranslation(newXYZ);
   GetTarget()->SetTransform(transform);
}

/////////////////////////////////////////////////////////////////////////////

}
