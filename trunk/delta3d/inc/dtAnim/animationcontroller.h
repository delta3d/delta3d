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

#ifndef __DELTA_ANIMATIONCONTROLLER_H__
#define __DELTA_ANIMATIONCONTROLLER_H__

#include <dtAnim/export.h>
#include <osg/Referenced>
#include <dtCore/observerptr.h>

namespace dtAnim
{
   class Animatable;
   class AnimationSequence;


class	DT_ANIM_EXPORT AnimationController: public osg::Referenced
{

public:
   AnimationController(AnimationSequence*);

   void SetParent(AnimationSequence*);

   virtual void Update(float dt);
   virtual void Recalculate();

protected:
   virtual ~AnimationController();

   void SetComputeWeight(Animatable* pAnim);
   void SetComputeSpeed(Animatable* pAnim);


private:
   AnimationController(const AnimationController&);  //not implemented
   AnimationController& operator=(const AnimationController&); //not implemented

   dtCore::ObserverPtr<AnimationSequence> mParent;
};

}//namespace dtAnim

#endif // __DELTA_ANIMATIONCONTROLLER_H__

