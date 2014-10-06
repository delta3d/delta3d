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
 */

#include <dtAnim/skeletaldrawable.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <osg/Vec3>

#include <cstddef>
#include <algorithm>
#include <dtUtil/bits.h>
#include <dtUtil/log.h>

namespace dtAnim
{
   ///@cond DOXYGEN_SHOULD_SKIP_THIS
   namespace details
   {
      template<typename PtrT>
      struct DeletePointer
      {
         void operator ()(PtrT& ptr)
         {
            delete ptr;
         }
      };
   }
   ///@endcond

SkeletalDrawable::IPrimitiveRenderObject::~IPrimitiveRenderObject()
{
}

void SkeletalDrawable::CPrimitiveRenderObject::AddChild(const IPrimitiveRenderObject* prims)
{
   std::ostringstream oss;
   oss << "primitive[" << this << "]: adding child primitive [" << prims << "]" << std::endl;
   LOG_DEBUG(oss.str())

   mChildren.push_back(prims);
}

void SkeletalDrawable::CPrimitiveRenderObject::AddID(int boneId)
{
   std::ostringstream oss;
   oss << "primitive[" << this << "]: adding bone [" << boneId << "]" << std::endl;
   LOG_DEBUG(oss.str())

   mIDs.push_back(boneId);
}

void SkeletalDrawable::CPrimitiveRenderObject::Render(const BaseModelWrapper& model) const
{
   // render my primitive
   if (dtUtil::Bits::Has(mRenderMode,RENDER_MODE_POINTS))
   {
      RenderBoneIDs(GL_POINTS, model);
   }
   if (dtUtil::Bits::Has(mRenderMode,RENDER_MODE_LINESEGMENTS))
   {
      RenderBoneIDs(GL_LINE_STRIP, model);
   }

   // render children
   std::for_each(mChildren.begin(),
                 mChildren.end(),
                 SkeletalDrawable::IPrimitiveRenderObject::RenderPrimitive(&model));
}

void SkeletalDrawable::CPrimitiveRenderObject::RenderBoneIDs(GLenum primitive, const dtAnim::BaseModelWrapper& model) const
{
   glBegin(primitive);
      dtAnim::BoneArray bones;
      model.GetBones(bones);

      dtAnim::BoneArray::iterator curIter = bones.begin();
      dtAnim::BoneArray::iterator endIter = bones.end();

      for ( ; curIter < endIter; ++curIter)
      {
         osg::Vec3 pos = curIter->get()->GetAbsoluteTranslation();
         glVertex3f(pos[0], pos[1], pos[2]);
      }
   glEnd();
}

void SkeletalDrawable::CPrimitiveRenderObject::SetRenderMode(unsigned char bits)
{
}

SkeletalDrawable::CPrimitiveRenderObject::CPrimitiveRenderObject()
   : mChildren()
   , mIDs()
   , mRenderMode(RENDER_MODE_POINTS|RENDER_MODE_LINESEGMENTS)
{
}

SkeletalDrawable::CPrimitiveRenderObject::~CPrimitiveRenderObject()
{
   std::for_each(mChildren.begin(),
                 mChildren.end(),
                 details::DeletePointer<VectorPrimitives::value_type>());
}


// cache the sketeton hierarchy for quick access during draw
SkeletalDrawable::SkeletalDrawable(const dtAnim::BaseModelWrapper* model)
   : mModelWrapper(model)
   , mRootPrimitives()
{
   setSupportsDisplayList(false);
   Init(this);
}

SkeletalDrawable::~SkeletalDrawable()
{
   std::for_each(mRootPrimitives.begin(),
                 mRootPrimitives.end(),
                 details::DeletePointer<VectorPrimitives::value_type>());
}

void SkeletalDrawable::drawImplementation(osg::RenderInfo& /*renderInfo*/) const
{
   glPointSize(4.0f);

   std::for_each(mRootPrimitives.begin(),
                 mRootPrimitives.end(),
                 IPrimitiveRenderObject::RenderPrimitive(mModelWrapper.get()));
}

osg::Object* SkeletalDrawable::cloneType() const
{
   SkeletalDrawable* instance = new SkeletalDrawable(mModelWrapper.get());
   return instance;
}

osg::Object* SkeletalDrawable::clone(const osg::CopyOp& copyop) const
{
   return new SkeletalDrawable(this->mModelWrapper.get());
}

void SkeletalDrawable::Init(SkeletalDrawable* instance)
{
   // start at the root bone,
   dtAnim::BoneArray rootBones;
   dtAnim::SkeletonInterface* skel = const_cast<dtAnim::SkeletonInterface*>(instance->mModelWrapper->GetSkeleton());
   skel->GetRootBones(rootBones);

   // build render objects
   instance->mRootPrimitives.resize(rootBones.size());

   // traverse each parent
   dtAnim::Cal3dBone* curBone = NULL;
   size_t rootSize = rootBones.size();
   for (size_t rootIndex=0; rootIndex<rootSize; ++rootIndex)
   {
      curBone = dynamic_cast<dtAnim::Cal3dBone*>(rootBones[rootIndex].get());
      IPrimitiveRenderObject* rootPrimitive = new CPrimitiveRenderObject();
      rootPrimitive->AddID(curBone->GetID());

      PopulatePrimitive(*(instance->mModelWrapper), *curBone, rootPrimitive);
      instance->mRootPrimitives[rootIndex] = rootPrimitive;
   }
}

void SkeletalDrawable::PopulatePrimitive(const dtAnim::BaseModelWrapper& model,
                                         dtAnim::BoneInterface& bone,
                                         IPrimitiveRenderObject* primitive)
{
   dtAnim::BoneArray childBones;
   bone.GetChildBones(childBones);

   size_t numChildren = childBones.size();

   std::ostringstream boss;
   boss << "bone[" << bone.GetID() << "] has children: ";
   for (size_t childIndex = 0; childIndex < numChildren; ++childIndex)
   {
      boss << ", " << childBones[childIndex];
   }
   boss << std::endl;
   LOG_DEBUG(boss.str())

   if (numChildren == 0)
   {
      return;
   }
   else if (numChildren == 1)
   {
      Cal3dBone* curBone = static_cast<dtAnim::Cal3dBone*>(childBones[0].get());
      primitive->AddID(curBone->GetID());
      PopulatePrimitive(model, *curBone, primitive);
   }
   else  // many children
   {
      dtAnim::Cal3dBone* curBone = NULL;
      for (size_t childIndex=0; childIndex < numChildren; ++childIndex)
      {
         curBone = static_cast<dtAnim::Cal3dBone*>(childBones[childIndex].get());
         
         IPrimitiveRenderObject* newPrimitive = new CPrimitiveRenderObject();
         primitive->AddChild(newPrimitive);

         newPrimitive->AddID(curBone->GetID());
         PopulatePrimitive(model, *curBone, newPrimitive);
      }
   }
}

} // namespace dtAnim
