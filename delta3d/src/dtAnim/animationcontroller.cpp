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

#include <dtAnim/animationcontroller.h>
#include <dtAnim/animationsequence.h>
#include <algorithm>

namespace dtAnim
{ 

   struct recalcFunctor
   {
      recalcFunctor(float pStart) : mStart(pStart), mEnd(0.0f) {}

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

AnimationController::AnimationController(AnimationSequence* pParent)
: mParent(pParent)
{
}


AnimationController::~AnimationController()
{
}

void AnimationController::SetParent(AnimationSequence* pParent)
{
   mParent = pParent;
}

void AnimationController::Update(float dt)
{
   AnimationSequence::AnimationContainer& pCont = mParent->GetChildAnimations();
   AnimationSequence::AnimationContainer::iterator iterEnd = pCont.end();

   for(AnimationSequence::AnimationContainer::iterator iter = pCont.begin(); iter != iterEnd; ++iter)
   {
      Animatable* child = (*iter).get();

      SetComputeWeight(child);
      SetComputeSpeed(child);

      child->SetElapsedTime(child->GetElapsedTime() + dt);

      if(child->GetElapsedTime() >= child->GetStartTime())
      {
         child->Update(dt);
      }
   }
}

void AnimationController::Recalculate()
{
   float start = mParent->GetStartTime();
   AnimationSequence::AnimationContainer& pCont = mParent->GetChildAnimations();

   recalcFunctor func(start);
   std::for_each(pCont.begin(), pCont.end(), func);

   mParent->SetEndTime(func.GetEnd());
}

//TODO
void AnimationController::SetComputeWeight(Animatable* pAnim)
{
   //Compute Child Weight using fade in and out
   float weight = mParent->GetCurrentWeight();
  
   //if(fade_in) weight * compute_fade_in_weight();
   //else if(fade_out) weight * compute_fade_out_weight();

   pAnim->SetCurrentWeight(weight * pAnim->GetBaseWeight());

}

//TODO
void AnimationController::SetComputeSpeed(Animatable* pAnim)
{

}

}//namespace dtAnim
