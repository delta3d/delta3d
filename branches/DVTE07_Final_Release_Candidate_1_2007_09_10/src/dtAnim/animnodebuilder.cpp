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
#include <osg/State>
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
   SetCreate(CreateFunc(this, &AnimNodeBuilder::CreateSoftware));
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
   

   const int maxBones = modelData->GetShaderMaxBones();
   static const std::string BONE_TRANSFORM_UNIFORM("boneTransforms");

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
      const size_t strideCal3D = 18;
      const size_t strideBytesCal3D = strideCal3D * sizeof(float);

      const size_t strideVBO = 12;
      const size_t strideBytesVBO = strideVBO * sizeof(float);


      Array<CalIndex> indexArray(numIndices);

      CalHardwareModel* hardwareModel = new CalHardwareModel(model);

      osg::Drawable::Extensions* glExt = osg::Drawable::getExtensions(0, true);
      GLuint vbo[2];
      if (modelData->GetVertexVBO() == 0)
      {
         glExt->glGenBuffers(1, &vbo[0]);
         glExt->glBindBuffer(GL_ARRAY_BUFFER_ARB, vbo[0]);
         glExt->glBufferData(GL_ARRAY_BUFFER_ARB, strideBytesVBO * numVerts, NULL, GL_STATIC_DRAW_ARB);
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

      float* vboArray = static_cast<float*>(glExt->glMapBuffer(GL_ARRAY_BUFFER_ARB, GL_READ_WRITE_ARB));
      Array<float> vboVertexAttr(strideBytesCal3D * numVerts);

      hardwareModel->setVertexBuffer(reinterpret_cast<char*>(&vboVertexAttr.mArray[0]), strideBytesCal3D);
      hardwareModel->setNormalBuffer(reinterpret_cast<char*>(&vboVertexAttr.mArray[3]), strideBytesCal3D);

      hardwareModel->setTextureCoordNum(2);
      //WARNING: do not check this code into delta on the trunk -bga
      hardwareModel->setTextureCoordBuffer(0, reinterpret_cast<char*>(&vboVertexAttr.mArray[6]), strideBytesCal3D);
      hardwareModel->setTextureCoordBuffer(1, reinterpret_cast<char*>(&vboVertexAttr.mArray[8]), strideBytesCal3D);

      hardwareModel->setWeightBuffer(reinterpret_cast<char*>(&vboVertexAttr.mArray[10]), strideBytesCal3D);
      hardwareModel->setMatrixIndexBuffer(reinterpret_cast<char*>(&vboVertexAttr.mArray[14]), strideBytesCal3D);

      if(hardwareModel->load(0, 0, maxBones))
      {
         numVerts = hardwareModel->getTotalVertexCount();
         numIndices = 3 * hardwareModel->getTotalFaceCount();

         //invert texture coordinates, and compress data
         unsigned int i = 0;
         unsigned int j = 0;
         for(; i < numVerts * strideCal3D; i += strideCal3D, j+= strideVBO)
         {
            //WARNING: do not check this code into delta on the trunk -bga
            //verts
            vboArray[j + 0] = vboVertexAttr.mArray[i + 0];
            vboArray[j + 1] = vboVertexAttr.mArray[i + 1];
            vboArray[j + 2] = vboVertexAttr.mArray[i + 2];

            //packing 3 indices into a single float
            float packedIndices = (10000.0 * vboVertexAttr.mArray[i + 14]) + (100.0 * vboVertexAttr.mArray[i + 15]) + vboVertexAttr.mArray[i + 16];
            vboArray[j + 3] = packedIndices;

            //normals
            vboArray[j + 4] = vboVertexAttr.mArray[i + 3];
            vboArray[j + 5] = vboVertexAttr.mArray[i + 4];
            vboArray[j + 6] = vboVertexAttr.mArray[i + 5];

            //tx 1
            vboArray[j + 7] = vboVertexAttr.mArray[i + 6];
            vboArray[j + 8] = 1.0f - vboVertexAttr.mArray[i + 7];//invert the y tex coord

            //weights
            vboArray[j + 9] = vboVertexAttr.mArray[i + 10];
            vboArray[j + 10] = vboVertexAttr.mArray[i + 11];
            vboArray[j + 11] = vboVertexAttr.mArray[i + 12];
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
         
         dtCore::ShaderProgram* shadProg = LoadShaders(*modelData, *geode);

         /* Begin figure out if this open gl implementation uses [0] on array uniforms 
          * This seems to be an ATI/NVIDIA thing.  
          * This requires me to force the shader to compile.
          */
         osg::Program* prog = shadProg->GetShaderProgram();
         dtCore::RefPtr<osg::State> tmpState = new osg::State;
         tmpState->setContextID(0);
         prog->compileGLObjects(*tmpState);
         const osg::Program::ActiveVarInfoMap& uniformMap = prog->getActiveUniforms(0);

         std::string boneTransformUniform = BONE_TRANSFORM_UNIFORM;
         if (uniformMap.find(boneTransformUniform) == uniformMap.end())
         {
            if (uniformMap.find(boneTransformUniform + "[0]") == uniformMap.end())
            {
               LOG_ERROR("Can't find uniform named \"" + boneTransformUniform 
                     + "\" which is required for skinning.");
            }
            else
            {
               boneTransformUniform.append("[0]");
            }
         }
         // End check.
         
         int numMeshes = hardwareModel->getHardwareMeshCount();
         for(int meshCount = 0; meshCount < numMeshes; ++meshCount)
         {
            HardwareSubMeshDrawable* drawable = new HardwareSubMeshDrawable(pWrapper, hardwareModel, 
                  boneTransformUniform, maxBones, meshCount, vbo[0], vbo[1]);
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
      glExt->glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);

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
         shaderProgram->AddVertexShader("shaders/HardwareCharacter.vert");
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
