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
#include <dtAnim/lodcullcallback.h> //for the cull callback.
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtUtil/log.h>
#include <dtUtil/matrixutil.h>
#include <osg/BlendFunc>
#include <osg/BoundingBox>
#include <osg/CullFace>
#include <osg/Material>
#include <osg/PolygonMode>
#include <osg/PrimitiveSet>
#include <osg/Texture2D>
#include <osg/Uniform>
#include <osg/Vec3>
#include <osg/Version>
DT_DISABLE_WARNING_ALL_START
#include <cal3d/hardwaremodel.h>
DT_DISABLE_WARNING_END

namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class HardwareSubmeshComputeBound : public osg::Drawable::ComputeBoundingBoxCallback
   {
      public:
         HardwareSubmeshComputeBound()
         {
         }

         /*virtual*/ osg::BoundingBox computeBound(const osg::Drawable& osgD) const
         {
            // temp until a better solution is implemented
            return osgD.getInitialBound();
         }
   };

   class HardwareSubmeshCallback : public osg::Drawable::UpdateCallback
   {
   public:
      HardwareSubmeshCallback(Cal3DModelWrapper& wrapper, CalHardwareModel& model,
            osg::Uniform& scale, osg::Uniform& boneTrans, unsigned mesh, OpenThreads::Mutex& updateMutex)
         : mWrapper(&wrapper)
         , mHardwareModel(&model)
         , mBoneTransforms(&boneTrans)
         , mScale(&scale)
         , mHardwareMeshID(mesh)
         , mUpdateMutex(updateMutex)
      {
      }

      /** do customized update code.*/
      virtual void update(osg::NodeVisitor*, osg::Drawable* drawable)
      {
         OpenThreads::ScopedLock<OpenThreads::Mutex> sl(mUpdateMutex);

         // select the proper hardware mesh
         if (mHardwareModel->selectHardwareMesh(mHardwareMeshID))
         {
            // spin through the bones in the hardware mesh
            const int numBones = mHardwareModel->getBoneCount();
            for (int bone = 0; bone < numBones; ++bone)
            {

               CalSkeleton* skel = mWrapper->GetCalModel()->getSkeleton();
               const CalQuaternion& quat = mHardwareModel->getRotationBoneSpace(bone, skel);
               const CalVector& vec = mHardwareModel->getTranslationBoneSpace(bone, skel);

               // compute matrices
               osg::Matrix matRot(osg::Quat(quat.x, quat.y, quat.z, quat.w));

               osg::Vec4 rotX, rotY, rotZ;
               rotX = dtUtil::MatrixUtil::GetRow4(matRot, 0);
               rotY = dtUtil::MatrixUtil::GetRow4(matRot, 1);
               rotZ = dtUtil::MatrixUtil::GetRow4(matRot, 2);

               rotX[3] = vec.x;
               rotY[3] = vec.y;
               rotZ[3] = vec.z;

               // set data on uniform
               mBoneTransforms->setElement(bone * 3 + 0, rotX);
               mBoneTransforms->setElement(bone * 3 + 1, rotY);
               mBoneTransforms->setElement(bone * 3 + 2, rotZ);

               mScale->set(mWrapper->GetScale());
            }
         }

      }

   private:
      dtCore::RefPtr<Cal3DModelWrapper> mWrapper;
      CalHardwareModel* mHardwareModel;
      dtCore::RefPtr<osg::Uniform> mBoneTransforms;
      dtCore::RefPtr<osg::Uniform> mScale;
      unsigned mHardwareMeshID;
      OpenThreads::Mutex& mUpdateMutex;
   };

////////////////////////////////////////////////////////////////////////////////
HardwareSubmeshDrawable::HardwareSubmeshDrawable(
   Cal3DModelWrapper* wrapper, CalHardwareModel* model,
   const std::string& boneUniformName,
   unsigned numBones, unsigned mesh,
   osg::VertexBufferObject* vertexVBO,
   osg::ElementBufferObject* indexEBO,
   int boneWeightsLocation,
   int boneIndicesLocation,
   int tangentSpaceLocation)
   : BaseClass()
   , mWrapper(wrapper)
   , mHardwareModel(model)
   , mScale(new osg::Uniform(osg::Uniform::FLOAT, "scale", 1))
   , mBoneTransforms(new osg::Uniform(osg::Uniform::FLOAT_VEC4, boneUniformName, numBones*3))
   , mBoneUniformName(boneUniformName)
   , mNumBones(numBones)
   , mMeshID(mesh)
   , mVertexVBO(vertexVBO)
   , mIndexEBO(indexEBO)
   , mBoneWeightsLocation(boneWeightsLocation)
   , mBoneIndicesLocation(boneIndicesLocation)
   , mTangentSpaceLocation(tangentSpaceLocation)
{
   setUseDisplayList(false);
   setUseVertexBufferObjects(true);
   setDataVariance(osg::Object::DYNAMIC);

   osg::StateSet* ss = getOrCreateStateSet();
   ss->addUniform(mBoneTransforms.get());
   ss->addUniform(mScale.get());
   ss->setAttributeAndModes(new osg::CullFace);
   ss->setDataVariance(osg::Object::DYNAMIC);

   mBoneTransforms->setDataVariance(osg::Object::DYNAMIC);
   mScale->setDataVariance(osg::Object::DYNAMIC);

   if (mHardwareModel == NULL)
   {
      return;
   }

   SetUpMaterial();

   int guessedMeshID = mMeshID;
   if (guessedMeshID >= mWrapper->GetMeshCount())
   {
      // this is an ugly hack which attempts to get the correct mesh ID from the hardware mesh ID
      // it is a result of the fact that there is no way to get the original mesh ID from the Cal Hardware Model
      // this only works if there is one submesh per mesh ID
      guessedMeshID = mWrapper->GetMeshCount() - 1;
   }

   // set our update callback which will update the bone transforms
   setUpdateCallback(new HardwareSubmeshCallback(*mWrapper, *mHardwareModel, *mScale, *mBoneTransforms, mMeshID, mUpdateMutex));
   setCullCallback(new LODCullCallback(*mWrapper, guessedMeshID)); //for LOD handling
   setComputeBoundingBoxCallback(new HardwareSubmeshComputeBound);
}

////////////////////////////////////////////////////////////////////////////////
HardwareSubmeshDrawable::~HardwareSubmeshDrawable(void)
{
}

////////////////////////////////////////////////////////////////////////////////
void HardwareSubmeshDrawable::SetBoundingBox(const osg::BoundingBox& boundingBox)
{
   mBoundingBox = boundingBox;
}

////////////////////////////////////////////////////////////////////////////////
void HardwareSubmeshDrawable::drawImplementation(osg::RenderInfo& renderInfo) const
{
   int faceCount = 0;
   int startIndex = 0;
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> sl(mUpdateMutex);
      // select the appropriate mesh
      if (!mHardwareModel->selectHardwareMesh(mMeshID))
      {
         return;
      }
      faceCount = mHardwareModel->getFaceCount();
      startIndex = mHardwareModel->getStartIndex();
   }

   osg::State& state = *renderInfo.getState();

   // Prepare to bind buffer objects
   state.disableAllVertexArrays();

#if defined(OPENSCENEGRAPH_MAJOR_VERSION) && OPENSCENEGRAPH_MAJOR_VERSION >= 3
   state.bindVertexBufferObject(mVertexVBO->getOrCreateGLBufferObject(renderInfo.getContextID()));
#else
   state.bindVertexBufferObject(mVertexVBO);
#endif

   unsigned stride = 18 * sizeof(float);

   #define BUFFER_OFFSET(x)((GLvoid*) (0 + ((x) * sizeof(float))))

   state.setVertexPointer(3, GL_FLOAT, stride, BUFFER_OFFSET(0));

   state.setNormalPointer(GL_FLOAT, stride, BUFFER_OFFSET(3));
   state.setTexCoordPointer(0, 2, GL_FLOAT, stride, BUFFER_OFFSET(6));
   state.setTexCoordPointer(1, 2, GL_FLOAT, stride, BUFFER_OFFSET(8));

   state.setTexCoordPointer(2, 4, GL_FLOAT, stride, BUFFER_OFFSET(10));
   state.setTexCoordPointer(3, 4, GL_FLOAT, stride, BUFFER_OFFSET(14));

#if defined(OPENSCENEGRAPH_MAJOR_VERSION) && OPENSCENEGRAPH_MAJOR_VERSION >= 3
   state.bindElementBufferObject(mIndexEBO->getOrCreateGLBufferObject(renderInfo.getContextID()));
#else
   state.bindElementBufferObject(mIndexEBO);
#endif

   // Make the call to render
   glDrawElements(GL_TRIANGLES,  faceCount * 3,
         (sizeof(CalIndex) < 4) ? GL_UNSIGNED_SHORT: GL_UNSIGNED_INT,
         (void*)(sizeof(CalIndex) * startIndex));

   state.unbindVertexBufferObject();
   state.unbindElementBufferObject();

   // This data could potentially cause problems
   // so we clear it out here (i.e CEGUI incompatible)
   state.setVertexPointer(NULL);

   state.setNormalPointer(NULL);
   state.setTexCoordPointer(0, NULL);
   state.setTexCoordPointer(1, NULL);

   state.setTexCoordPointer(2, NULL);
   state.setTexCoordPointer(3, NULL);
}

////////////////////////////////////////////////////////////////////////////////
osg::Object* HardwareSubmeshDrawable::clone(const osg::CopyOp&) const
{
   return new HardwareSubmeshDrawable(mWrapper.get(), mHardwareModel, mBoneUniformName,
         mNumBones, mMeshID, mVertexVBO, mIndexEBO,
         mBoneWeightsLocation, mBoneIndicesLocation, mTangentSpaceLocation);
}

////////////////////////////////////////////////////////////////////////////////
osg::Object* HardwareSubmeshDrawable::cloneType() const
{
   return new HardwareSubmeshDrawable(mWrapper.get(), mHardwareModel,
         mBoneUniformName, mNumBones, mMeshID, mVertexVBO, mIndexEBO,
         mBoneWeightsLocation, mBoneIndicesLocation, mTangentSpaceLocation);
}

//////////////////////////////////////////////////////////////////////////
void HardwareSubmeshDrawable::SetUpMaterial()
{
   if (!mWrapper.valid())
   {
      return;
   }

   osg::StateSet* ss = this->getOrCreateStateSet();

   osg::Material* material = new osg::Material();
   ss->setAttributeAndModes(material, osg::StateAttribute::ON);

   if (!mHardwareModel->selectHardwareMesh(mMeshID)) {return;}

   unsigned char meshColor[4];
   osg::Vec4 materialColor;

   // set the material diffuse color
   mHardwareModel->getDiffuseColor(&meshColor[0]);
   materialColor[0] = meshColor[0] / 255.0f;
   materialColor[1] = meshColor[1] / 255.0f;
   materialColor[2] = meshColor[2] / 255.0f;
   materialColor[3] = meshColor[3] / 255.0f;

   bool materialTranslucent = materialColor[3] < 1.0f;
   osg::Material::Face materialFace = materialTranslucent ? osg::Material::FRONT_AND_BACK : osg::Material::FRONT;

   material->setDiffuse(materialFace, materialColor);

   // set the material ambient color
   mHardwareModel->getAmbientColor(&meshColor[0]);
   materialColor[0] = meshColor[0] / 255.0f;
   materialColor[1] = meshColor[1] / 255.0f;
   materialColor[2] = meshColor[2] / 255.0f;
   materialColor[3] = meshColor[3] / 255.0f;
   material->setAmbient(materialFace, materialColor);

   // set the material specular color
   mHardwareModel->getSpecularColor(&meshColor[0]);
   materialColor[0] = meshColor[0] / 255.0f;
   materialColor[1] = meshColor[1] / 255.0f;
   materialColor[2] = meshColor[2] / 255.0f;
   materialColor[3] = meshColor[3] / 255.0f;
   material->setSpecular(materialFace, materialColor);

   // set the material shininess factor
   float shininess = mHardwareModel->getShininess();
   material->setShininess(materialFace, shininess);

   std::vector<CalHardwareModel::CalHardwareMesh>& meshVec = mHardwareModel->getVectorHardwareMesh();

   if (mMeshID >= meshVec.size())
   {
      LOG_WARNING("MeshID isn't defined in the list of meshes");
      return;
   }

   CalHardwareModel::CalHardwareMesh& coreMesh = meshVec[mMeshID];

   if (coreMesh.pCoreMaterial != NULL)
   {
      //get selected textures
      std::vector<CalCoreMaterial::Map>& vectorMap = coreMesh.pCoreMaterial->getVectorMap();
      std::vector<CalCoreMaterial::Map>::iterator iter = vectorMap.begin();
      std::vector<CalCoreMaterial::Map>::iterator endIter = vectorMap.end();

      for (int i = 0; iter != endIter; ++iter, ++i)
      {
         osg::Texture2D* texture = reinterpret_cast<osg::Texture2D*>(iter->userData);
         if (texture != NULL)
         {
            // TODO:
            // OSG seems to assume all PNG files have alpha. For now let the code be simple
            // and use material alpha to flag a material as translucent.

            // Mark the mesh as a transparency if the image is found to have alpha values.
            osg::Image* image = texture->getImage();
            /*if(image != NULL && image->isImageTranslucent())
            {
               materialTranslucent = true;
            }*/

            ss->setTextureAttributeAndModes(i, texture, osg::StateAttribute::ON);
         }
      }
   }
   else
   {
      ss->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
   }

   if(materialTranslucent)
   {
      osg::BlendFunc* bf = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      ss->setMode(GL_BLEND, osg::StateAttribute::ON);
      ss->setAttributeAndModes(bf, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
      ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
   }

   mWrapper->EndRenderingQuery();
}

} //namespace dtAnim

