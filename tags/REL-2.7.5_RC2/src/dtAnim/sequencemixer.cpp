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

#include <dtAnim/animatable.h>
#include <dtAnim/sequencemixer.h>
#include <dtUtil/log.h>



namespace dtAnim
{
////////////////////////////////////////////////////////////////////////////////
SequenceMixer::SequenceMixer()
   : mRootSequence(new AnimationSequence())
{
}

////////////////////////////////////////////////////////////////////////////////
SequenceMixer::~SequenceMixer()
{
}

////////////////////////////////////////////////////////////////////////////////
const AnimationSequence& SequenceMixer::GetRootSequence() const
{
   return *mRootSequence;
}

////////////////////////////////////////////////////////////////////////////////
void SequenceMixer::Update(float dt)
{
   const AnimationSequence::AnimationContainer& childAnims
      = mRootSequence->GetChildAnimations();

   // Update the root sequence's elapsed time since its update method does not.
   if ( ! childAnims.empty())
   {
      mRootSequence->SetElapsedTime(mRootSequence->GetElapsedTime() + dt);
   }

   mRootSequence->Update(dt);

   // If all children are gone, set the time to 0.
   if (childAnims.empty())
   {
      mRootSequence->SetElapsedTime(0.0);
   }
}

////////////////////////////////////////////////////////////////////////////////
void SequenceMixer::PlayAnimation(Animatable* anim)
{
   size_t numAnims = mRootSequence->GetChildAnimations().size();
   if(numAnims == 0)
   {
      mRootSequence->SetElapsedTime(0.0);
   }

   mRootSequence->AddAnimation(anim);
}

////////////////////////////////////////////////////////////////////////////////
void SequenceMixer::ClearAnimation(const std::string& pAnim, float time)
{
   mRootSequence->ClearAnimation(pAnim, time);
}

////////////////////////////////////////////////////////////////////////////////
void SequenceMixer::ClearActiveAnimations(float time)
{
   mRootSequence->ForceFadeOut(time);
}

////////////////////////////////////////////////////////////////////////////////
void SequenceMixer::ClearRegisteredAnimations()
{
   mAnimatables.clear();
}

////////////////////////////////////////////////////////////////////////////////
void SequenceMixer::RemoveRegisteredAnimation(const std::string& pAnim)
{
   AnimationTable::iterator iter = mAnimatables.find(pAnim);
   if (iter == mAnimatables.end())
   {
      LOG_WARNING("Cannot find registered animation '" + pAnim+ "' in SequenceMixer.");
   }

   mAnimatables.erase(iter);
}

////////////////////////////////////////////////////////////////////////////////
Animatable* SequenceMixer::GetActiveAnimation(const std::string& pAnim)
{
   return mRootSequence->GetAnimation(pAnim);
}

////////////////////////////////////////////////////////////////////////////////
const Animatable* SequenceMixer::GetActiveAnimation(const std::string& pAnim) const
{
   return mRootSequence->GetAnimation(pAnim);
}

////////////////////////////////////////////////////////////////////////////////
const Animatable* SequenceMixer::GetRegisteredAnimation(const std::string& pAnim) const
{
   return Lookup(pAnim);
}

////////////////////////////////////////////////////////////////////////////////
void SequenceMixer::GetRegisteredAnimations(std::vector<const Animatable*>& toFill) const
{
   toFill.clear();
   toFill.reserve(mAnimatables.size());

   AnimationTable::const_iterator i = mAnimatables.begin();
   AnimationTable::const_iterator end = mAnimatables.end();
   for (; i != end; ++i)
   {
      toFill.push_back(i->second.get());
   }
}

////////////////////////////////////////////////////////////////////////////////
void SequenceMixer::RegisterAnimation(const Animatable* pAnimation)
{
   AnimationTable::iterator iter = mAnimatables.find(pAnimation->GetName());
   if (iter != mAnimatables.end())
   {
      LOG_WARNING("Animation '" + pAnimation->GetName() + "' already added to SequenceMixer.");
   }
   else
   {
      mAnimatables.insert(TableKey(pAnimation->GetName(), pAnimation));
   }
}

////////////////////////////////////////////////////////////////////////////////
const Animatable* SequenceMixer::Lookup(const std::string& pAnimation) const
{
   AnimationTable::const_iterator iter = mAnimatables.find(pAnimation);

   if (iter == mAnimatables.end())
   {
      return 0;
   }
   else
   {
      return (*iter).second.get();
   }
}


////////////////////////////////////////////////////////////////////////////////
void SequenceMixer::ForceRecalculate()
{
   //somehow we only want to recalculate active animations
   //this case does not account for a single animation channel who needs to get
   //its start time recomputed
   //for now we will just update the entire root

   //AnimationSequence::AnimationContainer::iterator iter = mRootSequence->GetChildAnimations().begin();
   //AnimationSequence::AnimationContainer::iterator end = mRootSequence->GetChildAnimations().end();
   //
   //for (;iter != end; ++iter)
   //{
   //   Animatable* anim = (*iter).get();
   //   if (!anim->IsActive())
   //   {
   //      anim->Recalculate();
   //   }
   //}

   mRootSequence->Recalculate();

}

////////////////////////////////////////////////////////////////////////////////
bool SequenceMixer::IsAnimationPlaying(const std::string& pAnim) const
{
   return mRootSequence->IsAnimationPlaying(pAnim);
}

////////////////////////////////////////////////////////////////////////////////
// Helper Function
void GetActiveAnimations_Recursive(AnimationSequence& seq, SequenceMixer::AnimatableArray& toFill)
{
   if (seq.IsActive())
   {
      toFill.push_back(&seq);
   }

   Animatable* anim = NULL;
   AnimationSequence* curSeq = NULL;
   AnimationSequence::AnimationContainer& children = seq.GetChildAnimations();
   AnimationSequence::AnimationContainer::iterator curIter = children.begin();
   AnimationSequence::AnimationContainer::iterator endIter = children.end();
   for (; curIter != endIter; ++curIter)
   {
      anim = *curIter;
      if (anim->IsActive())
      {
         curSeq = dynamic_cast<AnimationSequence*>(anim);
         if (curSeq != NULL)
         {
            GetActiveAnimations_Recursive(*curSeq, toFill);
         }
         else
         {
            toFill.push_back(anim);
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void SequenceMixer::GetActiveAnimations(SequenceMixer::AnimatableArray& toFill)
{
   GetActiveAnimations_Recursive(*mRootSequence, toFill);
}

} // namespace dtAnim
