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

#include <dtAnim/animationsequence.h>
#include <dtAnim/animatable.h>
#include <dtAnim/animationcontroller.h>
#include <dtUtil/log.h>

#include <algorithm>

namespace dtAnim
{

////////////////////////////////////////////////////////////////////////////////////////////
//functors
struct AnimSequenceUpdater
{
   AnimSequenceUpdater(float dt, float parent_weight): mDT(dt), mParentWeight(parent_weight){}
   template<typename T>
   void operator()(T& pChild)
   {
      pChild.second->Update(mDT, mParentWeight);
   }

private:
   float mDT, mParentWeight;
};

struct AnimSeqForceFade
{
   AnimSeqForceFade(float time): mTime(time){}
   template<typename T>
   void operator()(T& pChild)
   {
      pChild.second->ForceFadeOut(mTime);
   }

private:
   float mTime;
};

////////////////////////////////////////////////////////////////////////////////////////////

AnimationSequence::AnimationSequence()
: mName()
, mController(0)
, mActiveAnimations()
{
}


AnimationSequence::~AnimationSequence()
{
}

const AnimationController* AnimationSequence::GetController() const
{
   return mController.get();
}

AnimationController* AnimationSequence::GetController()
{
   return mController.get();
}

void AnimationSequence::SetController(AnimationController* pController) 
{
   mController = pController;
}


void AnimationSequence::AddAnimation(Animatable* pAnimation)
{
   Insert(pAnimation);
}

void AnimationSequence::RemoveAnimation(const std::string& pAnimName)
{
   Remove(pAnimName);
}

Animatable* AnimationSequence::GetAnimation(const std::string& pAnimName)
{
   AnimationContainer::iterator iter = mActiveAnimations.find(pAnimName);
   if(iter == mActiveAnimations.end())
   {
      LOG_WARNING("Unable to find animation '" + pAnimName + "' in AnimationSequence '" + GetName() + "'." )
   }

   return (*iter).second.get();
}

const Animatable* AnimationSequence::GetAnimation(const std::string& pAnimName) const
{
   AnimationContainer::const_iterator iter = mActiveAnimations.find(pAnimName);
   if(iter == mActiveAnimations.end())
   {
      LOG_WARNING("Unable to find animation '" + pAnimName + "' in AnimationSequence '" + GetName() + "'." )
   }

   return (*iter).second.get();
}

AnimationSequence::AnimationContainer& AnimationSequence::GetActiveAnimations()
{
   return mActiveAnimations;
}

const AnimationSequence::AnimationContainer& AnimationSequence::GetActiveAnimations() const
{
   return mActiveAnimations;
}


void AnimationSequence::Update(float dt, float parent_weight)
{
   PruneChildren();
   CalculateBaseWeight();
   SetCurrentWeight(GetBaseWeight() * parent_weight);

   if(mController.valid())
   {
      mController->Update(dt, parent_weight);
   }

   //update all children
   std::for_each(mActiveAnimations.begin(), mActiveAnimations.end(), AnimSequenceUpdater(dt, GetCurrentWeight()));
}

//TODO
void AnimationSequence::CalculateBaseWeight()
{
   //calculate weight based on fade in's and out's
}

void AnimationSequence::ForceFadeOut(float time)
{
   std::for_each(mActiveAnimations.begin(), mActiveAnimations.end(), AnimSeqForceFade(time));

   SetPrune(true);
}

const std::string& AnimationSequence::GetName() const
{
   return mName;
}

void AnimationSequence::SetName(const std::string& pName)
{
   mName = pName;
}

void AnimationSequence::PruneChildren()
{
   AnimationContainer::iterator iter = mActiveAnimations.begin();
   AnimationContainer::iterator end = mActiveAnimations.end();

   for(;iter != end; ++iter)
   {
      if((*iter).second->Prune())
      {
         //erasing from a map doesnt invalidate the iterator.. hurray!!!
         mActiveAnimations.erase(iter);
      }
   }
}

void AnimationSequence::Insert(Animatable* pAnimation)
{
   AnimationContainer::iterator iter = mActiveAnimations.find(pAnimation->GetName());
   if(iter != mActiveAnimations.end())
   {
      LOG_WARNING("Trying to add already existing animation '" + pAnimation->GetName() + "' to AnimationSequence '" + GetName() + "'." )
   }

   mActiveAnimations.insert(ContainerMapping(pAnimation->GetName(), pAnimation));
}

void AnimationSequence::Remove(const std::string& pAnim)
{
   AnimationContainer::iterator iter = mActiveAnimations.find(pAnim);
   if(iter == mActiveAnimations.end())
   {
      LOG_WARNING("Unable to remove animation '" + pAnim + "' from AnimationSequence '" + GetName() + "'." )
   }

   mActiveAnimations.erase(iter);
}

}//namespace dtAnim
