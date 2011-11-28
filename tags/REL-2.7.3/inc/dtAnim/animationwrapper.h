/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * Bradley Anderegg 04/20/2007
 */

#ifndef __DELTA_ANIMATIONWRAPPER_H__
#define __DELTA_ANIMATIONWRAPPER_H__

#include <dtAnim/export.h>
#include <osg/Referenced>

#include <string>

namespace dtAnim
{

/**
 * The AnimationWrapper is meant to be a wrapper around a Cal3D animation.
 * The wrapper is used to keep our API from passing Cal3D specific objects
 * and to attach additional information to the animation.
 */
class DT_ANIM_EXPORT AnimationWrapper: public osg::Referenced
{

public:
   AnimationWrapper(const std::string& pName, int pAnimationID);

   /**
    * The ID of the Animation Wrapper refers to the Cal3D animation ID
    * @return the Cal3D animation ID
    */
   int GetID() const;

   /**
    * Set the ID of the animation that this wrapper refers to.
    * NOTE: This does not change the id of the referenced animation.
    *    and should only be set if the Cal3D animation ID has changed
    *    outside the control of this object.
    */
   void SetID(int id);

   /**
    * The duration of an animation is defined to be the amount of time
    * it takes to play through a single cycle
    */
   float GetDuration() const;
   void SetDuration(float duration);

   /**
    * The speed of an animation is defined to be a percentage of the
    * export speed, defaulting to 1.0
    */
   float GetSpeed() const;
   void SetSpeed(float pSpeed);


   /**
    *  The name of this animation as it is defined in the XML file
    */
   const std::string& GetName() const;
   void SetName(const std::string& pName);


protected:
   virtual ~AnimationWrapper();


private:

   int mAnimationID;
   float mDuration;

   float mSpeed;

   std::string mName;
};

} // namespace dtAnim

#endif // __DELTA_ANIMATIONWRAPPER_H__

