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

#ifndef __DELTA_ANIMATIONCHANNEL_H__
#define __DELTA_ANIMATIONCHANNEL_H__

#include <dtAnim/export.h>
#include <dtCore/refptr.h>
#include <dtAnim/animatable.h>

namespace dtAnim
{
   class AnimationWrapper;
   class Cal3DModelWrapper;

/**
*  AnimationChannel derives from Animatable and holds an AnimationWrapper, and 
*  contains semantics for playing an animation using the Cal3DModelWrapper API.
*/
class	DT_ANIM_EXPORT AnimationChannel: public Animatable
{

public:
   /**
   * @param the model wrapper used to play animations with
   * @param the animation wrapper to specify which animation to play
   */
   AnimationChannel(Cal3DModelWrapper* pModelWrapper, AnimationWrapper* pAnimationWrapper);

   /**
   * The per frame update function.
   * @param delta time
   * @param the parents weight
   */
   /*virtual*/ void Update(float dt, float parent_weight);

   /**
   * ForceFadeOut will ignore the EndTime and automatically fade out
   * this animation over the time specified.
   * 
   * @param the time to fade out over
   */

   /*virtual*/ void ForceFadeOut(float time);

   /**
   * @return the name of this animation
   */
   /*virtual*/ const std::string& GetName() const;
   
   /**
   * @param the name to set this animation to
   */
   /*virtual*/ void SetName(const std::string& pName);

protected:
   /*virtual*/ ~AnimationChannel();


private:

   //todo, verify holding a refptr to the model wrapper is ok
   dtCore::RefPtr<Cal3DModelWrapper> mModelWrapper;
   dtCore::RefPtr<AnimationWrapper> mAnimationWrapper;


};

}//namespace dtAnim

#endif // __DELTA_ANIMATIONCHANNEL_H__

