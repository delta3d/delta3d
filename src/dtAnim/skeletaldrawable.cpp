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

void SkeletalDrawable::CPrimitiveRenderObject::Render(const Cal3DModelWrapper& model) const
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

void SkeletalDrawable::CPrimitiveRenderObject::RenderBoneIDs(GLenum primitive, const dtAnim::Cal3DModelWrapper& model) const
{
   glBegin(primitive);
      size_t idsSize = mIDs.size();
      for (size_t idIndex=0; idIndex<idsSize; ++idIndex)
      {
         osg::Vec3 pos = model.GetBoneAbsoluteTranslation(mIDs[idIndex]);
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
SkeletalDrawable::SkeletalDrawable(const Cal3DModelWrapper* model)
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
   std::vector<int> rootBones;
   instance->mModelWrapper->GetRootBoneIDs(rootBones);

   // build render objects
   instance->mRootPrimitives.resize(rootBones.size());

   // traverse each parent
   size_t rootSize = rootBones.size();
   for (size_t rootIndex=0; rootIndex<rootSize; ++rootIndex)
   {
      IPrimitiveRenderObject* rootPrimitive = new CPrimitiveRenderObject();
      rootPrimitive->AddID(rootBones[rootIndex]);

      PopulatePrimitive(*(instance->mModelWrapper), rootBones[rootIndex], rootPrimitive);
      instance->mRootPrimitives[rootIndex] = rootPrimitive;
   }
}

void SkeletalDrawable::PopulatePrimitive(const Cal3DModelWrapper& model,
                                         int boneID,
                                         IPrimitiveRenderObject* primitive)
{
   VectorInt childrenBones;
   model.GetCoreBoneChildrenIDs(boneID, childrenBones);

   size_t childrenSize = childrenBones.size();

   std::ostringstream boss;
   boss << "bone[" << boneID << "] has children: ";
   for (size_t bonechildindex = 0; bonechildindex<childrenSize; ++bonechildindex)
   {
      boss << ", " << childrenBones[bonechildindex];
   }
   boss << std::endl;
   LOG_DEBUG(boss.str())

   if (childrenSize == 0)
   {
      return;
   }
   else if (childrenSize == 1)
   {
      primitive->AddID(childrenBones[0]);
      PopulatePrimitive(model,childrenBones[0], primitive);
   }
   else  // many children
   {
      for (size_t childIndex=0; childIndex<childrenSize; ++childIndex)
      {
         IPrimitiveRenderObject* newPrimitive = new CPrimitiveRenderObject();
         primitive->AddChild(newPrimitive);

         newPrimitive->AddID(childrenBones[childIndex]);
         PopulatePrimitive(model,childrenBones[childIndex], newPrimitive);
      }
   }
}

} // namespace dtAnim
