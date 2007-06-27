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
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtCore/refptr.h>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>

#include <algorithm>

namespace dtAnim
{

////////////////////////////////////////////////////////////////////////////////////////////
//functors
struct AnimSequenceUpdater
{
public:
   AnimSequenceUpdater(float dt, float parent_weight): mDT(dt), mParentWeight(parent_weight){}
   template<typename T>
   void operator()(T& pChild)
   {
      pChild->Update(mDT, mParentWeight);
   }

private:
   float mDT, mParentWeight;
};

class AnimSeqForceFade
{
public:
   AnimSeqForceFade(float time): mTime(time){}
   template<typename T>
   void operator()(T& pChild)
   {
      pChild->ForceFadeOut(mTime);
   }

private:
   float mTime;
};

class CloneFunctor
{
public:
   CloneFunctor(AnimationSequence* pSeq, Cal3DModelWrapper* pWrapper): mSequence(pSeq), mWrapper(pWrapper) {}
   template<typename T>
   void operator()(T& pChild)
   {
      dtCore::RefPtr<Animatable> pAnim = pChild->Clone(mWrapper);
      mSequence->AddAnimation(pAnim.get());
   }

private:
   AnimationSequence* mSequence;
   Cal3DModelWrapper* mWrapper;
};


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


/////////////////////////////////////////////////////////////////////////////////
//Animation Controller
/////////////////////////////////////////////////////////////////////////////////
AnimationSequence::AnimationController::AnimationController(AnimationSequence& pParent)
: mParent(&pParent)
{
}

/////////////////////////////////////////////////////////////////////////////////
AnimationSequence::AnimationController::~AnimationController()
{
}

/////////////////////////////////////////////////////////////////////////////////
AnimationSequence::AnimationController::AnimationController(const AnimationController& pCont)
: mParent(pCont.mParent)
{
}

/////////////////////////////////////////////////////////////////////////////////
AnimationSequence::AnimationController& AnimationSequence::AnimationController::operator=(const AnimationController& pCont)
{
   mParent = pCont.mParent;
   return *this;
}

/////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<AnimationSequence::AnimationController> AnimationSequence::AnimationController::Clone() const
{
   return new AnimationController(*this);
}

/////////////////////////////////////////////////////////////////////////////////
AnimationSequence& AnimationSequence::AnimationController::GetParent()
{
   return *mParent;
}

/////////////////////////////////////////////////////////////////////////////////
const AnimationSequence& AnimationSequence::AnimationController::GetParent() const
{
   return *mParent;
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationSequence::AnimationController::SetParent(AnimationSequence& pParent)
{
   mParent = &pParent;
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationSequence::AnimationController::Update(float dt)
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

/////////////////////////////////////////////////////////////////////////////////
void AnimationSequence::AnimationController::Recalculate()
{
   float start = mParent->GetStartTime();
   AnimationSequence::AnimationContainer& pCont = mParent->GetChildAnimations();

   RecalcFunctor func(start);
   std::for_each(pCont.begin(), pCont.end(), func);

   mParent->SetEndTime(func.GetEnd());
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationSequence::AnimationController::SetComputeWeight(Animatable* pAnim)
{
   //Compute Child Weight using fade in and out
   float weight = mParent->GetCurrentWeight();

   float fade = 1.0f;

   if(pAnim->GetElapsedTime() < pAnim->GetStartTime() + pAnim->GetFadeIn())
   {
      //compute fade in- we linearly interpolate from weight 0 to weight base weight
      //over time specified by FadeIn
      float percent = (pAnim->GetElapsedTime() - pAnim->GetStartTime()) / pAnim->GetFadeIn();
      fade = percent * pAnim->GetBaseWeight();

   }
   else if(pAnim->GetEndTime() > 0.0f && (pAnim->GetElapsedTime() > (pAnim->GetEndTime() - pAnim->GetFadeOut())))
   {
      //compute fade out- we linearly interpolate from base weight to 0
      //over time specified by FadeOut
      float percent = (pAnim->GetEndTime() - pAnim->GetElapsedTime()) / pAnim->GetFadeOut();
      fade = percent * pAnim->GetBaseWeight();         
   }

   dtUtil::Clamp(fade, 0.0f, 1.0f);
  
   pAnim->SetCurrentWeight(fade * weight * pAnim->GetBaseWeight());
}

void AnimationSequence::AnimationController::SetComputeSpeed(Animatable* pAnim)
{
   float speed = mParent->GetSpeed();
   pAnim->SetSpeed(speed * pAnim->GetSpeed());
}


////////////////////////////////////////////////////////////////////////////////////////////
//Animation Sequence
////////////////////////////////////////////////////////////////////////////////////////////
AnimationSequence::AnimationSequence()
: mActiveAnimations()
{
   // vs complains about using this in initializer list
   SetController(new AnimationController(*this));
}

/////////////////////////////////////////////////////////////////////////////////
AnimationSequence::AnimationSequence(AnimationController* pController)
: mActiveAnimations()
{
   SetController(pController);
}

/////////////////////////////////////////////////////////////////////////////////
AnimationSequence::~AnimationSequence()
{
}

/////////////////////////////////////////////////////////////////////////////////
AnimationSequence::AnimationSequence(const AnimationSequence& pSeq, Cal3DModelWrapper* wrapper)
: Animatable(pSeq)
, mActiveAnimations()
{
   if (pSeq.GetController() != NULL)
   {
      SetController(pSeq.GetController()->Clone().get());
   }
      
   std::for_each(pSeq.mActiveAnimations.begin(), pSeq.mActiveAnimations.end(), CloneFunctor(this, wrapper));
}

/////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<Animatable> AnimationSequence::Clone(Cal3DModelWrapper* wrapper) const
{
   return new AnimationSequence(*this, wrapper); 
}

/////////////////////////////////////////////////////////////////////////////////
const AnimationSequence::AnimationController* AnimationSequence::GetController() const
{
   return mController.get();
}

/////////////////////////////////////////////////////////////////////////////////
AnimationSequence::AnimationController* AnimationSequence::GetController()
{
   return mController.get();
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationSequence::SetController(AnimationController* pController) 
{
   mController = pController;
   mController->SetParent(*this);
}


/////////////////////////////////////////////////////////////////////////////////
void AnimationSequence::AddAnimation(Animatable* pAnimation)
{
   Insert(pAnimation);
   Recalculate();
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationSequence::ClearAnimation(const std::string& pAnimName, float fadeTime)
{
   Animatable* anim = GetAnimatable(pAnimName);
   if(anim)
   {
      anim->ForceFadeOut(fadeTime);
   }
   else
   {
      LOG_ERROR("Unable to ClearAnimation '" + pAnimName + "'");
   }
}

/////////////////////////////////////////////////////////////////////////////////
Animatable* AnimationSequence::GetAnimation(const std::string& pAnimName)
{
   Animatable* anim = GetAnimatable(pAnimName);
   if(anim == NULL)
   {
      LOG_WARNING("Unable to find animation '" + pAnimName + "' in AnimationSequence '" + GetName() + "'." )
      return NULL;
   }

   return anim;
}

/////////////////////////////////////////////////////////////////////////////////
const Animatable* AnimationSequence::GetAnimation(const std::string& pAnimName) const
{
   const Animatable* anim = GetAnimatable(pAnimName);
   if(anim == NULL)
   {
      LOG_WARNING("Unable to find animation '" + pAnimName + "' in AnimationSequence '" + GetName() + "'." )
      return NULL;
   }

   return anim;
}

/////////////////////////////////////////////////////////////////////////////////
AnimationSequence::AnimationContainer& AnimationSequence::GetChildAnimations()
{
   return mActiveAnimations;
}

/////////////////////////////////////////////////////////////////////////////////
const AnimationSequence::AnimationContainer& AnimationSequence::GetChildAnimations() const
{
   return mActiveAnimations;
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationSequence::Update(float dt)
{
   if(mController.valid())
   {
      mController->Update(dt);
   }

   PruneChildren();

   if(mActiveAnimations.empty())
   {
      SetPrune(true);
   }
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationSequence::Recalculate()
{
   mController->Recalculate();
}


/////////////////////////////////////////////////////////////////////////////////
//TODO
void AnimationSequence::CalculateBaseWeight()
{
   //calculate weight based on fade in's and out's
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationSequence::ForceFadeOut(float time)
{
   std::for_each(mActiveAnimations.begin(), mActiveAnimations.end(), AnimSeqForceFade(time));

   SetPrune(true);
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationSequence::Prune()
{
   AnimationContainer::iterator iter = mActiveAnimations.begin();
   AnimationContainer::iterator end = mActiveAnimations.end();

   for(;iter != end; ++iter)
   {
      (*iter)->Prune();
      iter = mActiveAnimations.erase(iter);      
   }
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationSequence::PruneChildren()
{
   AnimationContainer::iterator iter = mActiveAnimations.begin();
   AnimationContainer::iterator end = mActiveAnimations.end();

   for(;iter != end;)
   {
      if((*iter)->ShouldPrune())
      {
        (*iter)->Prune();
        AnimationContainer::iterator toDelete = iter;
        ++iter;
        mActiveAnimations.erase(toDelete);
      }
      else
      {
         ++iter;
      }
   }
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationSequence::Insert(Animatable* pAnimation)
{
   if(GetAnimatable(pAnimation->GetName()) != NULL)
   {
      LOG_WARNING("Trying to add already existing animation '" + pAnimation->GetName() + "' to AnimationSequence '" + GetName() + "'." )
   }
   else
   {
      mActiveAnimations.push_back(pAnimation);
   }
}

/////////////////////////////////////////////////////////////////////////////////
Animatable* AnimationSequence::GetAnimatable(const std::string& pAnim)
{
   AnimationContainer::iterator iter = mActiveAnimations.begin();
   AnimationContainer::iterator end = mActiveAnimations.end();

   for(;iter != end; ++iter)
   {
      if((*iter)->GetName() == pAnim)
      {
        return (*iter).get();
      }
   }
   return NULL;
}

/////////////////////////////////////////////////////////////////////////////////
const Animatable* AnimationSequence::GetAnimatable(const std::string& pAnim) const
{
   AnimationContainer::const_iterator iter = mActiveAnimations.begin();
   AnimationContainer::const_iterator end = mActiveAnimations.end();

   for(;iter != end; ++iter)
   {
      if((*iter)->GetName() == pAnim)
      {
        return (*iter).get();
      }
   }
   return NULL;
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationSequence::Remove(const std::string& pAnim)
{
   AnimationContainer::iterator iter = mActiveAnimations.begin();
   AnimationContainer::iterator end = mActiveAnimations.end();

   for(;iter != end; ++iter)
   {
      if((*iter)->GetName() == pAnim)
      {
         iter = mActiveAnimations.erase(iter);
         return;
      }
   }

   LOG_WARNING("Unable to remove animation '" + pAnim + "' from AnimationSequence '" + GetName() + "'." )
}

}//namespace dtAnim
