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

#include <dtAnim/hardwaresubmesh.h>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/PolygonMode>
#include <dtAnim/cal3dmodelwrapper.h>
#include <osg/Uniform>
#include <osg/PrimitiveSet>
#include <osg/Vec3>
#include <osg/BoundingBox>
#include <dtUtil/matrixutil.h>
#include <cal3d/hardwaremodel.h>

namespace dtAnim
{
   class HardwareSubmeshComputeBound : public osg::Drawable::ComputeBoundingBoxCallback
   {
      public:
         HardwareSubmeshComputeBound()
         {
         }
         
         /*virtual*/ osg::BoundingBox computeBound(const osg::Drawable& drawable) const  
         {
            return drawable.getInitialBound();
         }
         
   };

   class HardwareSubmeshCallback : public osg::Drawable::UpdateCallback
   {
      public:
         HardwareSubmeshCallback(Cal3DModelWrapper& wrapper, CalHardwareModel& model, 
               osg::Uniform& boneTrans, unsigned mesh)
            : mWrapper(&wrapper)
            , mHardwareModel(&model)
            , mBoneTransforms(&boneTrans)
            , mMeshID(mesh)
         {
         }

         /** do customized update code.*/
         virtual void update(osg::NodeVisitor*, osg::Drawable* drawable)
         {
            //select the proper hardware mesh
            mHardwareModel->selectHardwareMesh(mMeshID);
            
            //spin through the bones in the hardware mesh
            int numBones = mHardwareModel->getBoneCount();
            for(int bone = 0; bone < numBones; ++bone)
            {
               
               CalSkeleton* skel = mWrapper->GetCalModel()->getSkeleton();
               const CalQuaternion& quat = mHardwareModel->getRotationBoneSpace(bone, skel);
               const CalVector& vec = mHardwareModel->getTranslationBoneSpace(bone, skel);
                              
               //compute matrices
               osg::Matrix matRot(osg::Quat(quat.x, quat.y, quat.z, quat.w));

               osg::Vec4 rotX, rotY, rotZ;
               rotX = dtUtil::MatrixUtil::GetRow4(matRot, 0);
               rotY = dtUtil::MatrixUtil::GetRow4(matRot, 1);
               rotZ = dtUtil::MatrixUtil::GetRow4(matRot, 2);

               rotX[3] = vec.x;
               rotY[3] = vec.y;
               rotZ[3] = vec.z;

               //set data on uniform
               mBoneTransforms->setElement(bone * 3 + 0, rotX);
               mBoneTransforms->setElement(bone * 3 + 1, rotY);
               mBoneTransforms->setElement(bone * 3 + 2, rotZ);
            }
         }

      private:
         dtCore::RefPtr<Cal3DModelWrapper> mWrapper;
         CalHardwareModel* mHardwareModel;
         dtCore::RefPtr<osg::Uniform> mBoneTransforms;
         unsigned mMeshID;
   };


HardwareSubMeshDrawable::HardwareSubMeshDrawable(Cal3DModelWrapper *wrapper, CalHardwareModel* model, 
      osg::Program* shader, const std::string& boneUniformName, unsigned numBones, unsigned mesh, 
      unsigned vertexVBO, unsigned indexVBO)
: mWrapper(wrapper)
, mHardwareModel(model)
, mProgram(shader)
, mBoneTransforms(new osg::Uniform(osg::Uniform::FLOAT_VEC4, boneUniformName, numBones))
, mBoneUniformName(boneUniformName)
, mNumBones(numBones)
, mMeshID(mesh)
, mVertexVBO(vertexVBO)
, mIndexVBO(indexVBO)
{ 
	setUseDisplayList(false);
   setUseVertexBufferObjects(true);

   osg::StateSet* ss = getOrCreateStateSet();

   ss->setAttributeAndModes(mProgram.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
   ss->addUniform(mBoneTransforms.get());

   //get selected textures
   std::vector<CalCoreMaterial::Map>& vectorMap = mHardwareModel->getVectorHardwareMesh()[mMeshID].pCoreMaterial->getVectorMap();

   std::vector<CalCoreMaterial::Map>::iterator iter = vectorMap.begin();
   std::vector<CalCoreMaterial::Map>::iterator endIter = vectorMap.end();

   for(int i = 0; iter != endIter; ++iter, ++i)
   {
      osg::Texture2D *texture = (osg::Texture2D*)((*iter).userData);
      if(texture != NULL) 
      {
         ss->setTextureAttributeAndModes(i, texture, osg::StateAttribute::ON);
      }
   }

   //set our update callback which will update the bone transforms
   setUpdateCallback(new HardwareSubmeshCallback(*mWrapper, *mHardwareModel, *mBoneTransforms, mMeshID));
   setComputeBoundingBoxCallback(new HardwareSubmeshComputeBound());
}

HardwareSubMeshDrawable::~HardwareSubMeshDrawable(void)
{
}

void HardwareSubMeshDrawable::drawImplementation(osg::State& state) const 
{
   //select the appropriate mesh
   mHardwareModel->selectHardwareMesh(mMeshID);

   //bind the VBO's
   state.disableAllVertexArrays();

   const Extensions* glExt = getExtensions(state.getContextID(),true);

   glExt->glBindBuffer(GL_ARRAY_BUFFER_ARB, mVertexVBO);

   unsigned stride = 18 * sizeof(float);

   #define BUFFER_OFFSET(x)((GLvoid*) (0 + (x * sizeof(float))))

   state.setVertexPointer(3, GL_FLOAT, stride, BUFFER_OFFSET(0));

   state.setNormalPointer(GL_FLOAT, stride, BUFFER_OFFSET(3));
   state.setTexCoordPointer(0, 2, GL_FLOAT, stride, BUFFER_OFFSET(6));
   state.setTexCoordPointer(1, 2, GL_FLOAT, stride, BUFFER_OFFSET(8));

	state.setTexCoordPointer(2, 4, GL_FLOAT, stride, BUFFER_OFFSET(10));
   state.setTexCoordPointer(3, 4, GL_FLOAT, stride, BUFFER_OFFSET(14));

   //make the call to render
   glExt->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, mIndexVBO);

   glDrawElements(GL_TRIANGLES,  mHardwareModel->getFaceCount() * 3, (sizeof(CalIndex) < 4) ? 
         GL_UNSIGNED_SHORT: GL_UNSIGNED_INT, (void*)(sizeof(CalIndex) * mHardwareModel->getStartIndex()));

   glExt->glBindBuffer(GL_ARRAY_BUFFER_ARB, NULL);
   glExt->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, NULL);
}

osg::Object* HardwareSubMeshDrawable::clone(const osg::CopyOp&) const 
{
   return new HardwareSubMeshDrawable(mWrapper.get(), mHardwareModel, mProgram.get(), mBoneUniformName, 
         mNumBones, mMeshID, mVertexVBO, mIndexVBO);
}

osg::Object* HardwareSubMeshDrawable::cloneType() const
{
   return new HardwareSubMeshDrawable(mWrapper.get(), mHardwareModel, mProgram.get(), 
         mBoneUniformName, mNumBones, mMeshID, mVertexVBO, mIndexVBO);
}

} //namespace dtAnim

