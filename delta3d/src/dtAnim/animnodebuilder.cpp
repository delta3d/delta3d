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
#include <dtCore/globals.h>
#include <dtUtil/log.h>

#include <osg/Geode>

#include <cal3d/hardwaremodel.h>

namespace dtAnim
{

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
   if(!pWrapper)
   {
      LOG_ERROR("Invalid parameter to CreateGeode.");
      return 0;
   }

   osg::Geode* geode = new osg::Geode();

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
   if(!pWrapper)
   {
      LOG_ERROR("Invalid parameter to CreateGeode.");
      return 0;
   }

   //TODO: query for the maximum number of bones through opengl
   static const int MAX_BONES = 72;
// Apple OpenGL reports the names of the array uniforms differently
#ifdef __APPLE__
   static const std::string BONE_TRANSFORM_UNIFORM("boneTransforms[0]");
#else
   static const std::string BONE_TRANSFORM_UNIFORM("boneTransforms");
#endif
   osg::Geode* geode = new osg::Geode();

   pWrapper->SetLODLevel(1);
   pWrapper->Update(0);

   if(pWrapper && pWrapper->BeginRenderingQuery()) 
   {
      CalCoreModel* model = pWrapper->GetCalModel()->getCoreModel();
      
      unsigned numVerts = 0;
      unsigned numIndices = 0;
      unsigned numTextureUnits = 2;

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
      
      float* vertArray = new float[numVerts * 3];
      float* normArray = new float[numVerts * 3];
      float* tex1Array = new float[numVerts * 2];
      float* tex2Array = new float[numVerts * 2];
      float* boneWeightArray = new float[numVerts * 4];
      float* boneIDArray = new float[numVerts * 4];
      CalIndex* indexArray = new CalIndex[numIndices];

      CalHardwareModel* hardwareModel = new CalHardwareModel(model);
      hardwareModel->setVertexBuffer((char*) vertArray, 3 * sizeof(float));
      hardwareModel->setIndexBuffer(indexArray);
      hardwareModel->setNormalBuffer((char*) normArray, 3 * sizeof(float));
      hardwareModel->setWeightBuffer((char*) boneWeightArray, 4 * sizeof(float));
      hardwareModel->setMatrixIndexBuffer((char*) boneIDArray, 4 * sizeof(float));
      hardwareModel->setTextureCoordNum(2);
      hardwareModel->setTextureCoordBuffer(0, (char*) tex1Array, 2 * sizeof(float));
      hardwareModel->setTextureCoordBuffer(1, (char*) tex2Array, 2 * sizeof(float));

      if(hardwareModel->load(0, 0, MAX_BONES))
      {
         numVerts = hardwareModel->getTotalVertexCount();
         numIndices = 3 * hardwareModel->getTotalFaceCount();
      
         osg::Drawable::Extensions* glExt = osg::Drawable::getExtensions(0, true);
         GLuint vbo[2];
         glExt->glGenBuffers(2, vbo);

         float* vboVertexAttr = new float[numVerts * 18];

         for(unsigned i = 0; i < numVerts; ++i)
         {
            unsigned count = i * 18;
            vboVertexAttr[count + 0] = vertArray[(i * 3) + 0];
            vboVertexAttr[count + 1] = vertArray[(i * 3) + 1];
            vboVertexAttr[count + 2] = vertArray[(i * 3) + 2];

            vboVertexAttr[count + 3] = normArray[(i * 3) + 0];
            vboVertexAttr[count + 4] = normArray[(i * 3) + 1];
            vboVertexAttr[count + 5] = normArray[(i * 3) + 2];

            vboVertexAttr[count + 6] = tex1Array[(i * 2) + 0];
            vboVertexAttr[count + 7] = 1.0f - tex1Array[(i * 2) + 1]; //the odd texture coordinates in cal3d are flipped, not sure why

            vboVertexAttr[count + 8] = tex2Array[(i * 2) + 0];
            vboVertexAttr[count + 9] = 1.0f - tex2Array[(i * 2) + 1]; //the odd texture coordinates in cal3d are flipped, not sure why

            vboVertexAttr[count + 10] = boneWeightArray[(i * 4) + 0];
            vboVertexAttr[count + 11] = boneWeightArray[(i * 4) + 1];
            vboVertexAttr[count + 12] = boneWeightArray[(i * 4) + 2];
            vboVertexAttr[count + 13] = boneWeightArray[(i * 4) + 3];
            
            vboVertexAttr[count + 14] = boneIDArray[(i * 4) + 0];           
            vboVertexAttr[count + 15] = boneIDArray[(i * 4) + 1];            
            vboVertexAttr[count + 16] = boneIDArray[(i * 4) + 2];
            vboVertexAttr[count + 17] = boneIDArray[(i * 4) + 3];
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

         glExt->glBindBuffer(GL_ARRAY_BUFFER_ARB, vbo[0]);
         glExt->glBufferData(GL_ARRAY_BUFFER_ARB, 18 * sizeof(float) * numVerts, (const void*) vboVertexAttr, GL_STATIC_DRAW_ARB);

         glExt->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, vbo[1]);
         glExt->glBufferData(GL_ELEMENT_ARRAY_BUFFER_ARB, numIndices * sizeof(CalIndex), (const void*) indexArray, GL_STATIC_DRAW_ARB);

         //todo- pull shader name out of character xml
         osg::Program* shader = LoadShaders("shaders/HardwareCharacter.vert");

         int numMeshes = hardwareModel->getHardwareMeshCount();
         for(int meshCount = 0; meshCount < numMeshes; ++meshCount)
         {
            HardwareSubMeshDrawable* drawable = new HardwareSubMeshDrawable(pWrapper, hardwareModel, shader, BONE_TRANSFORM_UNIFORM, MAX_BONES, meshCount, vbo[0], vbo[1]);
            geode->addDrawable(drawable);
         }
      } 
      else
      {
         LOG_ERROR("Unable to create a hardware mesh.");
      }


      pWrapper->EndRenderingQuery();
   }
   

   return geode;
}


osg::Program* AnimNodeBuilder::LoadShaders(const std::string& shaderFile) const
{
   //we should allow the user to specify this in the character xml
   std::string vertFile = dtCore::FindFileInPathList(shaderFile);
   //std::string fragShader = dtCore::FindFileInPathList("shaders/HardwareCharacter.frag");

   osg::Program* prog = 0;
    
   if(!vertFile.empty())// || fragShader.empty())
   {
      prog = new osg::Program;

	   dtCore::RefPtr<osg::Shader> vertShader = new osg::Shader(osg::Shader::VERTEX);
      vertShader->loadShaderSourceFromFile(vertFile);

	   prog->addShader(vertShader.get());

   }
   else
   {
      LOG_ERROR("Unable to load vertex shader '" + shaderFile + "'.");
   }

   return prog;
}



}//namespace dtAnim
