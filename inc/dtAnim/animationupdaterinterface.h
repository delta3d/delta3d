/*
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2007 MOVES Institute 
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
*/

#ifndef __DELTA_ANIMATIONUPDATERINTERFACE_H__
#define __DELTA_ANIMATIONUPDATERINTERFACE_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA3D
#include <dtAnim/export.h>
#include <dtAnim/animationinterface.h>
#include <dtUtil/referencedinterface.h>



namespace dtAnim
{
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT AnimationUpdaterInterface : virtual public dtUtil::ReferencedInterface
   {
   public:
      virtual void Update(double timeDelta) = 0;

      virtual bool IsUpdatable() const = 0;

      virtual void ClearAll(float delay = 0.0) = 0;

      virtual void SetAnimationTime(float time) = 0;

      virtual float GetAnimationTime() const = 0;

      virtual void SetPaused(bool paused) = 0;

      virtual bool IsPaused() const = 0;

      virtual bool BlendPose(dtAnim::AnimationInterface& anim, float weight, float delay) = 0;
      virtual bool ClearPose(dtAnim::AnimationInterface& anim, float delay) = 0;

   protected:
      virtual ~AnimationUpdaterInterface() {}
   };
}

#endif
