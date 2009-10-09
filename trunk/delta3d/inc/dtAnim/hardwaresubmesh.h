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
 * Bradley Anderegg 06/27/2007
 */

#ifndef DELTA_HARDWARE_SUBMESH
#define DELTA_HARDWARE_SUBMESH

#include <osg/Drawable>
#include <dtAnim/export.h>
#include <dtCore/refptr.h>


class CalHardwareModel; 

 /// @cond
namespace osg
{
   class Program;
   class PrimitiveFunctor;
}

namespace osgUtil
{
   class GLObjectsVisitor;
}
/// @endcond

namespace dtAnim 
{
   class Cal3DModelWrapper;

   class DT_ANIM_EXPORT HardwareSubmeshDrawable: public osg::Drawable
   {
      public:

         HardwareSubmeshDrawable(Cal3DModelWrapper* wrapper, CalHardwareModel* model,
               const std::string& boneUniformName, unsigned numBones,
               unsigned mesh, osg::VertexBufferObject* vertexVBO, osg::ElementBufferObject* indexEBO);

         void SetBoundingBox(const osg::BoundingBox& boundingBox);

         virtual void drawImplementation(osg::RenderInfo& renderInfo) const;

         virtual osg::Object* cloneType() const;
         virtual osg::Object* clone(const osg::CopyOp&) const;

      protected:
         ~HardwareSubmeshDrawable();

      private:
         HardwareSubmeshDrawable();   ///< not implemented by design
         void SetUpMaterial();

         dtCore::RefPtr<Cal3DModelWrapper> mWrapper;
         CalHardwareModel* mHardwareModel;
         dtCore::RefPtr<osg::Uniform> mBoneTransforms;
         std::string mBoneUniformName;
         osg::BoundingBox mBoundingBox;
         unsigned int mNumBones, mMeshID;
         osg::VertexBufferObject* mVertexVBO;
         osg::ElementBufferObject* mIndexEBO;
   };

}; //namespace dtAnim

#endif //DELTA_HARDWARE_SUBMESH
