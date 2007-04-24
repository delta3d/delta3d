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

class	DT_ANIM_EXPORT AnimationWrapper: public osg::Referenced
{

public:
   AnimationWrapper(int pAnimationID);

   int GetID() const;

   float GetDuration() const;
   void SetDuration(float duration);

   float GetSpeed() const;
   void SetSpeed(float pSpeed);

   bool IsLooped() const;
   void SetLooping(bool b);

   const std::string& GetName() const;
   void SetName(const std::string& pName);


protected:
   virtual ~AnimationWrapper();


private:

   int mAnimationID;
   float mDuration;

   float mSpeed;
   bool mLooped;
    
   std::string mName;
};

}//namespace dtAnim

#endif // __DELTA_ANIMATIONWRAPPER_H__

