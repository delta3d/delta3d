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
#include <dtCore/refptr.h>

namespace dtAnim
{
   class Animatable;
   class AnimationSequence;


class	DT_ANIM_EXPORT AnimationController: public osg::Referenced
{

public:
   /**
   * Constructor
   * @param the parent AnimatationSequence
   */
   AnimationController(AnimationSequence*);

   ///copy constructor
   AnimationController(const AnimationController&);
   ///operator equal
   AnimationController& operator=(const AnimationController&);

   ///classes derived from this should implement a custom Clone()
   virtual dtCore::RefPtr<AnimationController> Clone() const;

   ///Sets the parent AnimationSequence
   void SetParent(AnimationSequence*);

   /**
   * The Update function is responsible for updating all the parent
   * sequences children.  This update consists of incrementing their 
   * elapsed time, calculating their current weight, and call update
   * on them if they are active.
   *
   * @param delta time
   */
   virtual void Update(float dt);

   /**
   * The Recalculate function is responsible for calculating the start times
   * of all the parent sequences children and then calling Recalculate() on 
   * them so they can calculate their end times.
   */
   virtual void Recalculate();

protected:
   virtual ~AnimationController();

   /**
   * Helper function to compute the weight of a child animatable
   * using the parents weight.
   */
   void SetComputeWeight(Animatable* pAnim);

   /**
   * Helper function to compute the speed of a child animatable
   * using the parents speed.
   */
   void SetComputeSpeed(Animatable* pAnim);


   //////////////////////////////////////////////////////////////////////////////////////
   /**
   * This functor is used to update the start and end times of the parent sequences children
   */
   class RecalcFunctor
   {
   public:

      RecalcFunctor(float pStart) : mStart(pStart), mEnd(0.0f) {}

      float GetEnd()const
      {
         return mEnd;
      }

      template <typename T>
      void operator()(T& pChild)
      {
         float delay = pChild->GetStartDelay();
         pChild->SetStartTime(mStart + delay);
         pChild->Recalculate();

         //keep track of the maximum end time
         float end = pChild->GetEndTime();
         if(end > mEnd) mEnd = end;
      }

   private:
      float mStart;
      float mEnd;
   };
   ///////////////////////////////////////////////////////////////////////////////////////

private:

   dtCore::ObserverPtr<AnimationSequence> mParent;
};

}//namespace dtAnim

#endif // __DELTA_ANIMATIONCONTROLLER_H__

