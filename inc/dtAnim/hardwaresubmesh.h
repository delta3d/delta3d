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

#include <dtAnim/export.h>
#include <dtCore/refptr.h>
#include <osg/Drawable>


class CalHardwareModel;

 /// @cond IGNORE
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
      typedef osg::Drawable BaseClass;

      static const unsigned int VBO_OFFSET_POSITION = 0;
      static const unsigned int VBO_OFFSET_NORMAL = 3;
      static const unsigned int VBO_OFFSET_TEXCOORD0 = 6;
      static const unsigned int VBO_OFFSET_TEXCOORD1 = 8;
      static const unsigned int VBO_OFFSET_WEIGHT = 10;
      static const unsigned int VBO_OFFSET_BONE_INDEX = 14;
      static const unsigned int VBO_OFFSET_TANGENT_SPACE = 18;
      static const unsigned int VBO_STRIDE = 22;
      static const unsigned int VBO_STRIDE_BYTES = VBO_STRIDE * sizeof(float);

      HardwareSubmeshDrawable(Cal3DModelWrapper* wrapper, CalHardwareModel* model,
            const std::string& boneUniformName, unsigned numBones,
            unsigned mesh, osg::VertexBufferObject* vertexVBO, osg::ElementBufferObject* indexEBO,
            int boneWeightsLocation,
            int boneIndicesLocation,
            int tangentSpaceLocation);

      void SetBoundingBox(const osg::BoundingBox& boundingBox);

      virtual void drawImplementation(osg::RenderInfo& renderInfo) const;

      virtual osg::Object* cloneType() const;
      virtual osg::Object* clone(const osg::CopyOp&) const;

   protected:
      ~HardwareSubmeshDrawable();

   private:
      void SetUpMaterial();

      dtCore::RefPtr<Cal3DModelWrapper> mWrapper;
      CalHardwareModel* mHardwareModel;
      dtCore::RefPtr<osg::Uniform> mScale;
      dtCore::RefPtr<osg::Uniform> mBoneTransforms;
      std::string mBoneUniformName;
      osg::BoundingBox mBoundingBox;
      unsigned int mNumBones, mMeshID;
      dtCore::RefPtr<osg::VertexBufferObject> mVertexVBO;
      dtCore::RefPtr<osg::ElementBufferObject> mIndexEBO;
      int mBoneWeightsLocation;
      int mBoneIndicesLocation;
      int mTangentSpaceLocation;
      mutable OpenThreads::Mutex mUpdateMutex; ///Used to support rendering with multiple threads
   };

}; //namespace dtAnim

#endif //DELTA_HARDWARE_SUBMESH
