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

#ifndef __DELTA_BONEINTERFACE_H__
#define __DELTA_BONEINTERFACE_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA3D
#include <dtAnim/export.h>
#include <dtAnim/animationinterface.h>
#include <dtCore/refptr.h>
#include <dtUtil/referencedinterface.h>
// OSG
#include <osg/Matrix>
#include <osg/Quat>
#include <osg/Vec3>
// STL
#include <string>
#include <vector>



///////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
///////////////////////////////////////////////////////////////////////////////////
namespace dtAnim
{
   class BoneInterface;



   ////////////////////////////////////////////////////////////////////////////////
   // TYPE DEFINITIONS
   ////////////////////////////////////////////////////////////////////////////////
   typedef std::vector<dtCore::RefPtr<dtAnim::BoneInterface> > BoneArray;



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT BoneInterface : virtual public dtUtil::ReferencedInterface
   {
   public:
      virtual int GetID() const = 0;

      virtual const std::string& GetName() const = 0;

      virtual BoneInterface* GetParentBone() const = 0;

      virtual int GetChildBones(dtAnim::BoneArray& outBones) const = 0;
      
      virtual void GetAbsoluteMatrix(osg::Matrix& outMatrix) const = 0;

      virtual osg::Quat GetAbsoluteRotation() const = 0;
      virtual osg::Vec3 GetAbsoluteTranslation() const = 0;

      virtual osg::Quat GetRelativeRotation() const = 0;
      
      virtual osg::Quat GetAbsoluteRotationForKeyframe(const dtAnim::AnimationInterface& anim, dtAnim::Keyframe keyframe) const = 0;
      
   protected:
      virtual ~BoneInterface() {}
   };
}

#endif
