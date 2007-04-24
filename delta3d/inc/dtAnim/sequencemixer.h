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

#ifndef __DELTA_SEQUENCEMIXER_H__
#define __DELTA_SEQUENCEMIXER_H__

#include <dtAnim/export.h>
#include <osg/Referenced>
#include <dtCore/refptr.h>

#include <string>
#include <map>

namespace dtAnim
{
 
   class AnimationSequence;
   class Animatable;

class	DT_ANIM_EXPORT SequenceMixer: public osg::Referenced
{

public:
   typedef std::map<std::string, dtCore::RefPtr<Animatable> > AnimationTable;
   typedef AnimationTable::allocator_type::value_type TableKey;

public:
   SequenceMixer();

   void Update(float dt);
   
   void PlayAnimation(Animatable* pAnim);
   void PlayAnimation(const std::string& pAnim);

   void FadeOutActiveAnimations(float time);
   void ClearRegisteredAnimations();

   void RemoveActiveAnimation(const std::string& pAnim);
   void RemoveRegisteredAnimation(const std::string& pAnim);

   Animatable* GetActiveAnimation(const std::string& pAnim);
   const Animatable* GetActiveAnimation(const std::string& pAnim) const;

   Animatable* GetRegisteredAnimation(const std::string& pAnim);
   const Animatable* GetRegisteredAnimation(const std::string& pAnim) const;

   void RegisterAnimation(const std::string& pAnim, Animatable* pAnimation);


protected:
   virtual ~SequenceMixer();


private:

   Animatable* Lookup(const std::string& pAnimation);
   const Animatable* Lookup(const std::string& pAnimation) const;

   AnimationTable mAnimatables;
   dtCore::RefPtr<AnimationSequence> mRootSequence;

};

}//namespace dtAnim

#endif // __DELTA_SEQUENCEMIXER_H__

