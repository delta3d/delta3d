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
 * Bradley Anderegg 03/28/2007
 */
#include <dtAnim/animnodebuilder.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/submesh.h>
#include <dtAnim/hardwaresubmesh.h>
#include <dtAnim/cal3ddatabase.h>
#include <dtAnim/cal3dmodeldata.h>

#include <dtCore/globals.h>
#include <dtCore/shaderprogram.h>
#include <dtCore/shadermanager.h>
#include <dtCore/shadergroup.h>
#include <dtUtil/log.h>

#include <osg/Geode>
#include <osg/BoundingSphere>
#include <osg/BoundingBox>
#include <osg/Texture2D>

#include <cal3d/hardwaremodel.h>

//For the bounding box class, who knew.
#include <cal3d/vector.h>

namespace dtAnim
{

template <typename T>
class Array
{
   public:
      typedef T value_type;

      Array(size_t size = 0): mArray(NULL)
      {
         if (size > 0)
            mArray = new T[size];
      }

      ~Array()
      {
         delete[] mArray;
      }

      T& operator[](size_t index)
      {
         return mArray[index];
      }

      T* mArray;
};

AnimNodeBuilder::AnimNodeBuilder()
{
   SetCreate(CreateFunc(this, &AnimNodeBuilder::CreateHardware));
}

AnimNodeBuilder::AnimNodeBuilder(const CreateFunc& pCreate)
: mCreateFunc(pCreate)
{
}


AnimNodeBuilder::~AnimNodeBuilder()
{
}

AnimNodeBuilder::CreateFunc& AnimNodeBuilder::GetCreate()
{
   return mCreateFunc;
}

void AnimNodeBuilder::SetCreate(const CreateFunc& pCreate)
{
   mCreateFunc = pCreate;
}


dtCore::RefPtr<osg::Geode> AnimNodeBuilder::CreateGeode(Cal3DModelWrapper* pWrapper)
{
   return mCreateFunc(pWrapper);
}


dtCore::RefPtr<osg::Geode> AnimNodeBuilder::CreateSoftware(Cal3DModelWrapper* pWrapper)
{
   if(pWrapper == NULL)
   {
      LOG_ERROR("Invalid parameter to CreateGeode.");
      return NULL;
   }

   osg::Geode* geode = new osg::Geode();
   geode->setComputeBoundingSphereCallback(new Cal3DBoundingSphereCalculator(*pWrapper));

   if(pWrapper->BeginRenderingQuery()) 
   {
      int meshCount = pWrapper->GetMeshCount();

      for(int meshId = 0; meshId < meshCount; meshId++) 
      {
         int submeshCount = pWrapper->GetSubmeshCount(meshId);

         for(int submeshId = 0; submeshId < submeshCount; submeshId++) 
         {
            dtAnim::SubMeshDrawable *submesh = new dtAnim::SubMeshDrawable(pWrapper, meshId, submeshId);
            geode->addDrawable(submesh);
         }
      }

      pWrapper->EndRenderingQuery();
   }

   pWrapper->Update(0);

   return geode;
}

dtCore::RefPtr<osg::Geode> AnimNodeBuilder::CreateHardware(Cal3DModelWrapper* pWrapper)
{
   if(pWrapper == NULL)
   {
      LOG_ERROR("Invalid parameter to CreateGeode.");
      return NULL;
   }

   Cal3DModelData* modelData = Cal3DDatabase::GetInstance().GetModelData(*pWrapper);
   
   if (modelData == NULL)
   {
      LOG_ERROR("Model does not have model data.  Unable to create hardware submesh.");
      return NULL;
   }
   
   //TODO: query for the maximum number of bones through opengl
   static const int MAX_BONES = 72;
// Apple OpenGL reports the names of the array uniforms differently
#ifdef __APPLE__
   static const std::string BONE_TRANSFORM_UNIFORM("boneTransforms[0]");
#else
   static const std::string BONE_TRANSFORM_UNIFORM("boneTransforms");
#endif
   dtCore::RefPtr<osg::Geode> geode = new osg::Geode();

   pWrapper->SetLODLevel(1);
   pWrapper->Update(0);

   if(pWrapper && pWrapper->BeginRenderingQuery()) 
   {
      CalCoreModel* model = pWrapper->GetCalModel()->getCoreModel();
      
      unsigned numVerts = 0;
      unsigned numIndices = 0;

      int meshCount = model->getCoreMeshCount();

      for(int meshId = 0; meshId < meshCount; meshId++) 
      {
         CalCoreMesh* calMesh = model->getCoreMesh(meshId);
         int submeshCount = calMesh->getCoreSubmeshCount();

         for(int submeshId = 0; submeshId < submeshCount; submeshId++) 
         {
            CalCoreSubmesh* subMesh = calMesh->getCoreSubmesh(submeshId);
            numVerts += subMesh->getVertexCount();
            numIndices += 3 * subMesh->getFaceCount();
         }
      }
      const size_t stride = 18;
      const size_t strideBytes = stride * sizeof(float);

      Array<CalIndex> indexArray(numIndices);

      CalHardwareModel* hardwareModel = new CalHardwareModel(model);


      osg::Drawable::Extensions* glExt = osg::Drawable::getExtensions(0, true);
      GLuint vbo[2];
      if (modelData->GetVertexVBO() == 0)
      {
         glExt->glGenBuffers(1, &vbo[0]);
         glExt->glBindBuffer(GL_ARRAY_BUFFER_ARB, vbo[0]);
         glExt->glBufferData(GL_ARRAY_BUFFER_ARB, strideBytes * numVerts, NULL, GL_STATIC_DRAW_ARB);
         modelData->SetVertexVBO(vbo[0]);
      }
      else
      {
         vbo[0] = modelData->GetVertexVBO();
         glExt->glBindBuffer(GL_ARRAY_BUFFER_ARB, vbo[0]);
      }

      bool newIndexBuffer = false;
      if (modelData->GetIndexVBO() == 0)
      {
         glExt->glGenBuffers(1, &vbo[1]);
         modelData->SetIndexVBO(vbo[1]);
         newIndexBuffer = true;
      }
      else
      {
         vbo[1] = modelData->GetIndexVBO();
      }

      hardwareModel->setIndexBuffer(indexArray.mArray);

      float* vboVertexAttr = static_cast<float*>(glExt->glMapBuffer(GL_ARRAY_BUFFER_ARB, GL_READ_WRITE_ARB));

      hardwareModel->setVertexBuffer(reinterpret_cast<char*>(vboVertexAttr), strideBytes);
      hardwareModel->setNormalBuffer(reinterpret_cast<char*>(vboVertexAttr + 3), strideBytes);

      hardwareModel->setTextureCoordNum(2);
      hardwareModel->setTextureCoordBuffer(0, reinterpret_cast<char*>(vboVertexAttr + 6), strideBytes);
      hardwareModel->setTextureCoordBuffer(1, reinterpret_cast<char*>(vboVertexAttr + 8), strideBytes);

      hardwareModel->setWeightBuffer(reinterpret_cast<char*>(vboVertexAttr + 10), strideBytes);
      hardwareModel->setMatrixIndexBuffer(reinterpret_cast<char*>(vboVertexAttr + 14), strideBytes);

      if(hardwareModel->load(0, 0, MAX_BONES))
      {
         numVerts = hardwareModel->getTotalVertexCount();
         numIndices = 3 * hardwareModel->getTotalFaceCount();

         //invert texture coordinates.
         for(unsigned i = 0; i < numVerts * stride; i += stride)
         {
            vboVertexAttr[i + 7] = 1.0f - vboVertexAttr[i + 7]; //the odd texture coordinates in cal3d are flipped, not sure why
            vboVertexAttr[i + 9] = 1.0f - vboVertexAttr[i + 7]; //the odd texture coordinates in cal3d are flipped, not sure why
         }

         for(int meshCount = 0; meshCount < hardwareModel->getHardwareMeshCount(); ++meshCount)
         {
            hardwareModel->selectHardwareMesh(meshCount);

            for(int face = 0; face < hardwareModel->getFaceCount(); ++face) 
            {
               for(int index = 0; index < 3; ++index)
               {
                  indexArray[face * 3 + index + hardwareModel->getStartIndex()] += hardwareModel->getBaseVertexIndex();
               }
            }
         }

         glExt->glUnmapBuffer(GL_ARRAY_BUFFER_ARB);
         if (newIndexBuffer)
         {
            glExt->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, vbo[1]);
            glExt->glBufferData(GL_ELEMENT_ARRAY_BUFFER_ARB, numIndices * sizeof(CalIndex), (const void*) indexArray.mArray, GL_STATIC_DRAW_ARB);
         }
         
         //todo- pull shader name out of character xml
         LoadShaders(*modelData, *geode);

         int numMeshes = hardwareModel->getHardwareMeshCount();
         for(int meshCount = 0; meshCount < numMeshes; ++meshCount)
         {
            HardwareSubMeshDrawable* drawable = new HardwareSubMeshDrawable(pWrapper, hardwareModel, BONE_TRANSFORM_UNIFORM, MAX_BONES, meshCount, vbo[0], vbo[1]);
            geode->addDrawable(drawable);
         }

         geode->setComputeBoundingSphereCallback(new Cal3DBoundingSphereCalculator(*pWrapper));
      }
      else
      {
         glExt->glUnmapBuffer(GL_ARRAY_BUFFER_ARB);
         LOG_ERROR("Unable to create a hardware mesh.");
      }

      glExt->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

      pWrapper->EndRenderingQuery();
   }


   return geode;
}


dtCore::ShaderProgram* AnimNodeBuilder::LoadShaders(Cal3DModelData& modelData, osg::Geode& geode) const
{
   static const std::string hardwareSkinningSPGroup = "HardwareSkinning";
   dtCore::ShaderManager& shaderManager = dtCore::ShaderManager::GetInstance();
   dtCore::ShaderProgram* shaderProgram = NULL;
   if (!modelData.GetShaderGroupName().empty())
   {
      dtCore::ShaderGroup* spGroup = shaderManager.FindShaderGroupPrototype(modelData.GetShaderGroupName());
      if (spGroup != NULL)
      {
         if (!modelData.GetShaderName().empty())
         {
            shaderProgram = spGroup->FindShader(modelData.GetShaderName());
            if (shaderProgram == NULL)
            {
               LOG_ERROR("Shader program \"" + modelData.GetShaderName() + "\" from group \"" 
                     + modelData.GetShaderGroupName() + "\" was not found, using the default from the group.");
               shaderProgram = spGroup->GetDefaultShader();
               
               if (shaderProgram == NULL)
               {
                  LOG_ERROR("Shader Group \""  + modelData.GetShaderGroupName() 
                        + "\" was not found, overriding to use the default group.");
               }
            }
         }
         else
         {
            shaderProgram = spGroup->GetDefaultShader();
            if (shaderProgram == NULL)
            {
               LOG_ERROR("Shader Group \""  + modelData.GetShaderGroupName() 
                     + "\" was not found, overriding to use the default group.");
            }
         }
      }
   }
   
   //If no shader group is setup, create one.
   if (shaderProgram == NULL)
   {
      dtCore::ShaderGroup* defSPGroup = shaderManager.FindShaderGroupPrototype(hardwareSkinningSPGroup);
      if (defSPGroup == NULL)
      {
         defSPGroup = new dtCore::ShaderGroup(hardwareSkinningSPGroup);
         shaderProgram = new dtCore::ShaderProgram("Default");
         shaderProgram->SetVertexShaderSource("shaders/HardwareCharacter.vert");
         defSPGroup->AddShader(*shaderProgram, true);
         shaderManager.AddShaderGroupPrototype(*defSPGroup);
      }
      else
      {
         shaderProgram = defSPGroup->GetDefaultShader();
      }
      modelData.SetShaderGroupName(hardwareSkinningSPGroup);
   }
   
   return shaderManager.AssignShaderFromPrototype(*shaderProgram, geode);
}

AnimNodeBuilder::Cal3DBoundingSphereCalculator::Cal3DBoundingSphereCalculator(Cal3DModelWrapper& wrapper)
   : mWrapper(&wrapper)
{
}

osg::BoundingSphere AnimNodeBuilder::Cal3DBoundingSphereCalculator::computeBound(const osg::Node&) const
{ 
   CalBoundingBox& calBBox = mWrapper->GetCalModel()->getBoundingBox(false);
   osg::BoundingBox bBox(-calBBox.plane[0].d, -calBBox.plane[2].d, -calBBox.plane[4].d,
         calBBox.plane[1].d, calBBox.plane[3].d, calBBox.plane[5].d);

   osg::BoundingSphere bSphere(bBox);
   return bSphere;
}


}//namespace dtAnim
