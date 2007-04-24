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

#include <dtAnim/sequencemixer.h>
#include <dtAnim/animationsequence.h>
#include <dtAnim/animatable.h>
#include <dtAnim/animationcontroller.h>
#include <dtUtil/log.h>


namespace dtAnim
{

SequenceMixer::SequenceMixer()
{
}


SequenceMixer::~SequenceMixer()
{
}

void SequenceMixer::Update(float dt)
{
   mRootSequence->Update(dt, 1.0f);
}

void SequenceMixer::PlayAnimation(Animatable* pAnim)
{
   mRootSequence->AddAnimation(pAnim);
}

void SequenceMixer::PlayAnimation(const std::string& pAnim)
{
   Animatable* anim = Lookup(pAnim);
   if(anim)
   {
      PlayAnimation(anim);
   }
   else
   {
      LOG_ERROR("Cannot play animation '" + pAnim + "' because it has not been registered with the SequenceMixer.")
   }
}

void SequenceMixer::FadeOutActiveAnimations(float time)
{
   mRootSequence->ForceFadeOut(time);
}

void SequenceMixer::ClearRegisteredAnimations()
{
   mAnimatables.clear();
}

void SequenceMixer::RemoveActiveAnimation(const std::string& pAnim)
{
   mRootSequence->RemoveAnimation(pAnim);
}

void SequenceMixer::RemoveRegisteredAnimation(const std::string& pAnim)
{
   AnimationTable::iterator iter = mAnimatables.find(pAnim);
   if(iter == mAnimatables.end())
   {
      LOG_WARNING("Cannot find registered animation '" + pAnim+ "' in SequenceMixer." )
   }

   mAnimatables.erase(iter);
}

Animatable* SequenceMixer::GetActiveAnimation(const std::string& pAnim)
{
   return mRootSequence->GetAnimation(pAnim);
}

const Animatable* SequenceMixer::GetActiveAnimation(const std::string& pAnim) const
{
   return mRootSequence->GetAnimation(pAnim);
}

Animatable* SequenceMixer::GetRegisteredAnimation(const std::string& pAnim)
{
   return Lookup(pAnim);
}

const Animatable* SequenceMixer::GetRegisteredAnimation(const std::string& pAnim) const
{
   return Lookup(pAnim);
}

void SequenceMixer::RegisterAnimation(const std::string& pAnim, Animatable* pAnimation)
{
   AnimationTable::iterator iter = mAnimatables.find(pAnim);
   if(iter != mAnimatables.end())
   {
      LOG_WARNING("Animation '" + pAnim+ "' already added to SequenceMixer." )
   }
   else
   {   
      mAnimatables.insert(TableKey(pAnim, pAnimation));
   }
}


Animatable* SequenceMixer::Lookup(const std::string& pAnimation)
{
   AnimationTable::iterator iter = mAnimatables.find(pAnimation);
   if(iter == mAnimatables.end())
   {
      return 0;
   }
   else
   {
      return (*iter).second.get();
   }
}

const Animatable* SequenceMixer::Lookup(const std::string& pAnimation) const
{
   AnimationTable::const_iterator iter = mAnimatables.find(pAnimation);

   if(iter == mAnimatables.end())
   {
      return 0;
   }
   else
   {
      return (*iter).second.get();
   }
}


}//namespace dtAnim
