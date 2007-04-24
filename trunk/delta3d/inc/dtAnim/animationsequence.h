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

#ifndef __DELTA_ANIMATIONSEQUENCE_H__
#define __DELTA_ANIMATIONSEQUENCE_H__

#include <dtAnim/export.h>
#include <dtAnim/animatable.h>

#include <dtCore/refptr.h>

#include <map>
#include <string>

namespace dtAnim
{
   class AnimationController;

class	DT_ANIM_EXPORT AnimationSequence: public Animatable
{
public:
   typedef std::map<std::string, dtCore::RefPtr<Animatable> > AnimationContainer;
   typedef AnimationContainer::allocator_type::value_type ContainerMapping;

public:
   AnimationSequence();

   void AddAnimation(Animatable* pAnimation);
   void RemoveAnimation(const std::string& pAnimName);

   Animatable* GetAnimation(const std::string& pAnimName);
   const Animatable* GetAnimation(const std::string& pAnimName) const;

   AnimationController* GetController();
   const AnimationController* GetController() const;
   void SetController(AnimationController* pController);

   AnimationContainer& GetActiveAnimations();
   const AnimationContainer& GetActiveAnimations() const;

   /*virtual*/ void Update(float dt, float parent_weight);

   /*virtual*/ void ForceFadeOut(float time);

   /*virtual*/ const std::string& GetName() const;
   /*virtual*/ void SetName(const std::string& pName);

protected:
   /*virtual*/ ~AnimationSequence();


private:

   void Insert(Animatable* pAnimation);
   void Remove(const std::string& pAnim);
   void CalculateBaseWeight();
   void PruneChildren();

   std::string mName;
   dtCore::RefPtr<AnimationController> mController;
   AnimationContainer mActiveAnimations;

};

}//namespace dtAnim

#endif // __DELTA_ANIMATIONSEQUENCE_H__

