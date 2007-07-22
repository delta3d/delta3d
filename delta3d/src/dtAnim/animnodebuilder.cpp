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
#include <osg/BoundingSphere>
#include <osg/BoundingBox>

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
      //unsigned numTextureUnits = 2;

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

      osg::Drawable::Extensions* glExt = osg::Drawable::getExtensions(0, true);
      GLuint vbo[2];
      glExt->glGenBuffers(2, vbo);

      glExt->glBindBuffer(GL_ARRAY_BUFFER_ARB, vbo[0]);
      glExt->glBufferData(GL_ARRAY_BUFFER_ARB, strideBytes * numVerts, NULL, GL_STATIC_DRAW_ARB);
      float* vboVertexAttr = (float *)glExt->glMapBuffer(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);

      CalHardwareModel* hardwareModel = new CalHardwareModel(model);
      hardwareModel->setIndexBuffer(indexArray.mArray);

      hardwareModel->setVertexBuffer((char*) vboVertexAttr, strideBytes);
      hardwareModel->setNormalBuffer((char*) (vboVertexAttr + 3), strideBytes);
      
      hardwareModel->setTextureCoordNum(2);
      hardwareModel->setTextureCoordBuffer(0, (char*) (vboVertexAttr + 6), strideBytes);
      hardwareModel->setTextureCoordBuffer(1, (char*) (vboVertexAttr + 8), strideBytes);

      hardwareModel->setWeightBuffer((char*) (vboVertexAttr + 10), strideBytes);
      hardwareModel->setMatrixIndexBuffer((char*) (vboVertexAttr + 14), strideBytes);

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
         glExt->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, vbo[1]);
         glExt->glBufferData(GL_ELEMENT_ARRAY_BUFFER_ARB, numIndices * sizeof(CalIndex), (const void*) indexArray.mArray, GL_STATIC_DRAW_ARB);

         //todo- pull shader name out of character xml
         osg::Program* shader = LoadShaders("shaders/HardwareCharacter.vert");

         int numMeshes = hardwareModel->getHardwareMeshCount();
         for(int meshCount = 0; meshCount < numMeshes; ++meshCount)
         {
            HardwareSubMeshDrawable* drawable = new HardwareSubMeshDrawable(pWrapper, hardwareModel, shader, BONE_TRANSFORM_UNIFORM, MAX_BONES, meshCount, vbo[0], vbo[1]);
            geode->addDrawable(drawable);
         }
         
         geode->setComputeBoundingSphereCallback(new Cal3DBoundingSphereCalculator(*pWrapper));
      } 
      else
      {
         glExt->glUnmapBuffer(GL_ARRAY_BUFFER_ARB);
         glExt->glDeleteBuffers(2, vbo);
         LOG_ERROR("Unable to create a hardware mesh.");
      }

      glExt->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
      
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
