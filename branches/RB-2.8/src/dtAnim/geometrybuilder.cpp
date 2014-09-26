/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2014, Caper Holdings LLC
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
 * Bradley Anderegg
 */

#include <dtAnim/geometrybuilder.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/submesh.h>
#include <dtAnim/hardwaresubmesh.h>
#include <dtAnim/cal3ddatabase.h>
#include <dtAnim/cal3dmodeldata.h>

#include <dtCore/shaderprogram.h>
#include <dtCore/shadermanager.h>
#include <dtCore/shadergroup.h>
#include <dtCore/project.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/matrixutil.h>

#include <dtUtil/log.h>

#include <osg/Geode>
#include <osg/State>
#include <osg/Material>
#include <osg/BoundingSphere>
#include <osg/BoundingBox>
#include <osg/Texture2D>
#include <osg/GLExtensions>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/BlendFunc>
#include <osg/CullFace>
#include <osg/Drawable>
#include <osgUtil/TangentSpaceGenerator>
#include <cal3d/hardwaremodel.h>

//For the bounding box class, who knew.
#include <cal3d/vector.h>

namespace dtAnim
{

   class UpdateSkeletonCallback : public osg::Drawable::UpdateCallback
   {
   public:
      UpdateSkeletonCallback(CalSkeleton& skel, CalHardwareModel& model,
         osg::Uniform& boneTrans, unsigned mesh)
         : mSkeleton(&skel)
         , mHardwareModel(&model)
         , mBoneTransforms(&boneTrans)
         , mHardwareMeshID(mesh)
      {
      }
      ~UpdateSkeletonCallback()
      {
         //todo  - does the hardware model need to be delted here?
      }

      /** do customized update code.*/
      virtual void update(osg::NodeVisitor*, osg::Drawable*)
      {
         //traverse(node,nv);

         // select the proper hardware mesh
         if (mHardwareModel->selectHardwareMesh(mHardwareMeshID))
         {
            // spin through the bones in the hardware mesh
            const int numBones = mHardwareModel->getBoneCount();
            for (int bone = 0; bone < numBones; ++bone)
            {
               
               const CalQuaternion& quat = mHardwareModel->getRotationBoneSpace(bone, mSkeleton);
               const CalVector& vec = mHardwareModel->getTranslationBoneSpace(bone, mSkeleton);

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
            }
         }

      }

   private:
      CalSkeleton* mSkeleton;
      CalHardwareModel* mHardwareModel;
      dtCore::RefPtr<osg::Uniform> mBoneTransforms;
      unsigned mHardwareMeshID;
   };

////////////////////////////////////////////////////////////////////////////////
GeometryBuilder::MeshCacheData::MeshCacheData()
   : mId(0,0)
   , mName("")
   , mGeometry()
{
}

////////////////////////////////////////////////////////////////////////////////
GeometryBuilder::MeshCacheData::~MeshCacheData()
{
   mGeometry = NULL;
}


////////////////////////////////////////////////////////////////////////////////
GeometryBuilder::GeometryCache::GeometryCache()
{
}

////////////////////////////////////////////////////////////////////////////////
GeometryBuilder::GeometryCache::~GeometryCache()
{
   mLoadedModels.clear();
}

osg::ref_ptr<osg::Geometry> GeometryBuilder::GeometryCache::GetOrCreateHardwareMeshSubMesh(CalHardwareModel* hardwareModel, Cal3DModelWrapper* pWrapper, int meshId, int subMeshId, int vertexCount, int faceCount, int boneCount, int baseIndex, int startIndex)
{
      if (pWrapper == NULL)
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



      osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();

      geom->setSupportsDisplayList(false);
      geom->setUseDisplayList(false);
      geom->setUseVertexBufferObjects(true);

      int numIndices = faceCount * 3;
      CalIndex* indexArray = new CalIndex[numIndices];

      osg::Vec3Array* vertArray = new osg::Vec3Array();
      vertArray->resizeArray(vertexCount);

      osg::Vec3Array* normalArray = new osg::Vec3Array();
      normalArray->resizeArray(vertexCount);

      osg::Vec4Array* tangentArray = new osg::Vec4Array();
      tangentArray->resizeArray(vertexCount);

      osg::Vec2Array* texCoordArray = new osg::Vec2Array();
      texCoordArray->resizeArray(vertexCount);

      osg::Vec4Array* boneInfluenceArray = new osg::Vec4Array();
      boneInfluenceArray->resizeArray(vertexCount);

      osg::Vec4Array* boneIndexArray = new osg::Vec4Array();
      boneIndexArray->resizeArray(vertexCount);

      CalIndex* sourceIndex = modelData->GetSourceIndexArray();
      osg::FloatArray& sourceVertex = *modelData->GetSourceVertexArray();

      int stride = dtAnim::HardwareSubmeshDrawable::VBO_STRIDE;
      int vertexNum = 0;
      for (int i = baseIndex * stride; i < (baseIndex + vertexCount) * stride; i+= stride)
      {
         osg::Vec3 pos(sourceVertex[i], sourceVertex[i + 1], sourceVertex[i + 2]);
         osg::Vec3 normal(sourceVertex[i + 3], sourceVertex[i + 4], sourceVertex[i + 5]);
         osg::Vec2 tx0(sourceVertex[i + 6], sourceVertex[i + 7]);
         osg::Vec2 tx1(sourceVertex[i + 8], sourceVertex[i + 9]);
         osg::Vec4 weight(sourceVertex[i + 10], sourceVertex[i + 11], sourceVertex[i + 12], sourceVertex[i + 13]);
         osg::Vec4 bones(sourceVertex[i + 14], sourceVertex[i + 15], sourceVertex[i + 16], sourceVertex[i + 17]);
         osg::Vec4 tangent(sourceVertex[i + 18], sourceVertex[i + 19], sourceVertex[i + 20], sourceVertex[i + 21]);

         (*vertArray)[vertexNum] = pos;
         (*normalArray)[vertexNum] = normal;
         (*texCoordArray)[vertexNum] = tx0;
         (*boneInfluenceArray)[vertexNum] = weight;
         (*boneIndexArray)[vertexNum] = bones;
         (*tangentArray)[vertexNum] = tangent;
         ++vertexNum;
      }

      //set vertex array data
      geom->setVertexArray(vertArray);
      geom->setNormalArray(normalArray);
      geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

      //geom->setVertexAttribArray(3, boneInfluenceArray);
      //geom->setVertexAttribBinding(3, osg::Geometry::BIND_PER_VERTEX);

      //geom->setVertexAttribArray(4, boneIndexArray);
      //geom->setVertexAttribBinding(4, osg::Geometry::BIND_PER_VERTEX);

      //geom->setVertexAttribArray(5, tangentArray);
      //geom->setVertexAttribBinding(5, osg::Geometry::BIND_PER_VERTEX);
      
      geom->setTexCoordArray(0, texCoordArray);
      geom->setTexCoordArray(1, boneInfluenceArray);
      geom->setTexCoordArray(2, boneIndexArray);
      geom->setTexCoordArray(3, tangentArray);

      for (int face = 0; face < faceCount; ++face)
      {
         for (int index = 0; index < 3; ++index)
         {
            indexArray[face * 3 + index ] = sourceIndex[face * 3 + index + startIndex] - baseIndex;
         }
      }

      //create draw triangle data
      osg::DrawElements* drawElements = NULL;

      if (sizeof(CalIndex) < 4)
      {
         drawElements = new osg::DrawElementsUShort(GL_TRIANGLES, numIndices, (GLushort*)indexArray);
      }
      else
      {
         drawElements = new osg::DrawElementsUInt(GL_TRIANGLES, numIndices, (GLuint*)indexArray);
      }

      geom->addPrimitiveSet(drawElements);

      
      delete [] indexArray;

      return geom;
}

osg::ref_ptr<osg::Geode> GeometryBuilder::GeometryCache::CreateSubmeshGeode(Cal3DModelWrapper* pWrapper, CalHardwareModel* hardwareModel, GeometryBuilder::MeshCacheData& mcd)
{
   osg::ref_ptr<osg::Geode> geode = new osg::Geode();
   osg::StateSet* geodeSS = mcd.mGeometry->getOrCreateStateSet();

   dtCore::RefPtr<osg::Uniform> boneTransforms = new osg::Uniform(osg::Uniform::FLOAT_VEC4, "boneTransforms", hardwareModel->getBoneCount() * 3);
   geodeSS->addUniform(boneTransforms.get());
   geodeSS->setDataVariance(osg::Object::DYNAMIC);

   geode->addDrawable(mcd.mGeometry.get());

   mcd.mGeometry->setUpdateCallback(new UpdateSkeletonCallback(*pWrapper->GetCalModel()->getSkeleton(), *hardwareModel, *boneTransforms, mcd.mId.second));

   return geode;
}

////////////////////////////////////////////////////////////////////////////////
osg::ref_ptr<osg::Geometry> GeometryBuilder::GeometryCache::GetOrCreateMeshSubMesh(Cal3DModelWrapper* pWrapper, int meshId, int subMeshId)
{
     osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();

     geom->setSupportsDisplayList(false);
     geom->setUseDisplayList(false);
     geom->setUseVertexBufferObjects(true);


     // select mesh and submesh for further data access
     if(pWrapper->SelectMeshSubmesh(meshId, subMeshId))
     {
        int vertexCount = pWrapper->GetVertexCount();
        int faceCount = pWrapper->GetFaceCount();
        
        osg::Vec3Array* vertArray = new osg::Vec3Array();
        vertArray->resizeArray(vertexCount);

        osg::Vec3Array* normalArray = new osg::Vec3Array();
        normalArray->resizeArray(vertexCount);

        osg::Vec2Array* texCoordArray = new osg::Vec2Array();
        texCoordArray->resizeArray(vertexCount);


        // get the transformed vertices of the submesh
        pWrapper->GetVertices(&(*vertArray)[0][0]);

        // get the transformed normals of the submesh
        pWrapper->GetNormals(&(*normalArray)[0][0]);

        pWrapper->GetTextureCoords(0, &(*texCoordArray)[0][0]);

        // flip vertical coordinates
        for (int i = 1; i < vertexCount; ++i)
        {
           (*texCoordArray)[i][1] = 1.0f - (*texCoordArray)[i][1];
        }

        //set vertex array data
        geom->setVertexArray(vertArray);
        geom->setNormalArray(normalArray);
        geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

        geom->setTexCoordArray(0, texCoordArray);

        //create draw triangle data

        osg::DrawElements* drawElements = NULL;

        int numIndices = 3 * faceCount;
        CalIndex* indexArray = new CalIndex[numIndices];
        pWrapper->GetFaces(indexArray);

        if (sizeof(CalIndex) < 4)
        {
           drawElements = new osg::DrawElementsUShort(GL_TRIANGLES, numIndices, (GLushort*)indexArray);
        }
        else
        {
           drawElements = new osg::DrawElementsUInt(GL_TRIANGLES, numIndices, (GLuint*)indexArray);
        }

        geom->addPrimitiveSet(drawElements);

        //generate tangents
        osg::ref_ptr<osgUtil::TangentSpaceGenerator> tsg = new osgUtil::TangentSpaceGenerator;
        tsg->generate(geom, 0);
        
        geom->setTexCoordArray(1, tsg->getTangentArray());

     }

     return geom;
}

////////////////////////////////////////////////////////////////////////////////
osg::ref_ptr<osg::Node> GeometryBuilder::GeometryCache::GetOrCreateModel(Cal3DModelWrapper* pWrapper)
{

   if (pWrapper == NULL)
   {
      LOG_ERROR("Invalid parameter to CreateGeode.");
      return NULL;
   }

   dtCore::RefPtr<osg::Group> modelRoot = new osg::Group();

   CalCoreModel* model = pWrapper->GetCalModel()->getCoreModel();
   std::string modelName = model->getName();

   Cal3DModelData* modelData = Cal3DDatabase::GetInstance().GetModelData(*pWrapper);

   CalHardwareModel* hardwareModel = modelData->GetOrCreateCalHardwareModel();


   GeometryMap::iterator iterBegin = mLoadedModels.lower_bound(modelName);
   GeometryMap::iterator iterEnd = mLoadedModels.upper_bound(modelName);

   if(false)//iterBegin != iterEnd)
   {
      for (; iterBegin != iterEnd; ++iterBegin)
      {
         MeshCacheData& meshData = (*iterBegin).second;

         hardwareModel->selectHardwareMesh(meshData.mId.second);

         osg::ref_ptr<osg::Geode> geode = CreateSubmeshGeode(pWrapper, hardwareModel, meshData);

         modelRoot->addChild(geode.get());
      }
   }
   else
   {
      pWrapper->SetLODLevel(1);
      pWrapper->Update(0);

      if (pWrapper->BeginRenderingQuery())
      {
         int meshCount = 0;
         int meshId = 0;
         int submeshId = 0;

         CalCoreMesh* calMesh = model->getCoreMesh(0);

#ifdef BUILD_CAL3D_SOFTWARE

         meshCount = model->getCoreMeshCount();

         for (meshId = 0; meshId < meshCount; meshId++)
         {
            calMesh = model->getCoreMesh(meshId);
            int submeshCount = calMesh->getCoreSubmeshCount();

            for (submeshId = 0; submeshId < submeshCount; submeshId++)
            {
               CalCoreSubmesh* subMesh = calMesh->getCoreSubmesh(submeshId);
#else
         
         if (modelData == NULL)
         {
            LOG_ERROR("Model does not have model data.  Unable to create hardware submesh.");
            return NULL;
         }

         meshCount = hardwareModel->getHardwareMeshCount();

         for (submeshId = 0; submeshId < meshCount; submeshId++) {

            hardwareModel->selectHardwareMesh(submeshId);

            int vertexCount = hardwareModel->getVertexCount();
            int faceCount = hardwareModel->getFaceCount();
            int boneCount = hardwareModel->getBoneCount();
            int startIndex = hardwareModel->getStartIndex();
            int baseIndex = hardwareModel->getBaseVertexIndex();
#endif
               MeshCacheData mcd;
               mcd.mId.first = meshId;
               mcd.mId.second = submeshId;
               mcd.mName = calMesh->getName();

#ifdef BUILD_CAL3D_SOFTWARE
               mcd.mGeometry = GetOrCreateMeshSubMesh(pWrapper, meshId, submeshId);
#else

               mcd.mGeometry = GetOrCreateHardwareMeshSubMesh(hardwareModel, pWrapper, meshId, submeshId, vertexCount, faceCount, boneCount, baseIndex, startIndex);

               osg::ref_ptr<osg::Geode> geode = CreateSubmeshGeode(pWrapper, hardwareModel, mcd);
               modelRoot->addChild(geode.get());
#endif

               SetUpMaterial(mcd.mGeometry.get(), hardwareModel, pWrapper, meshId, submeshId);

               mLoadedModels.insert(std::make_pair(modelName, mcd));
            }
         }

         pWrapper->EndRenderingQuery();
      }

   pWrapper->Update(0);

   return modelRoot;
}


////////////////////////////////////////////////////////////////////////////////
GeometryBuilder::GeometryBuilder()
{
}

////////////////////////////////////////////////////////////////////////////////
GeometryBuilder::~GeometryBuilder()
{

}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Node> GeometryBuilder::CreateGeometry( osg::RenderInfo* renderInfo, Cal3DModelWrapper* pWrapper )
{
   dtCore::RefPtr<osg::Node> result = mGeometries.GetOrCreateModel(pWrapper);

   Cal3DModelData* modelData = Cal3DDatabase::GetInstance().GetModelData(*pWrapper);

   if (modelData == NULL)
   {
      LOG_ERROR("Model does not have model data.  Unable to create hardware submesh.");
      return NULL;
   }


   dtCore::ShaderProgram* shadProg = LoadShaders(*modelData, *result);

   /*#ifndef BUILD_CAL3D_SOFTWARE
   static const std::string BONE_WEIGHTS_ATTRIB("boneWeights");
   static const std::string BONE_INDICES_ATTRIB("boneIndices");
   static const std::string TANGENT_SPACE_ATTRIB("tangentSpace");

   shadProg->GetShaderProgram()->addBindAttribLocation(BONE_WEIGHTS_ATTRIB, 3);
   shadProg->GetShaderProgram()->addBindAttribLocation(BONE_INDICES_ATTRIB, 4);
   shadProg->GetShaderProgram()->addBindAttribLocation(TANGENT_SPACE_ATTRIB, 5);
   #endif*/

   return result;
}


////////////////////////////////////////////////////////////////////////////////
dtCore::ShaderProgram* GeometryBuilder::LoadShaders(Cal3DModelData& modelData, osg::Node& geode) const
{
   static const std::string hardwareSkinningSPGroup = "HardwareSkinning";
   dtCore::ShaderManager& shaderManager = dtCore::ShaderManager::GetInstance();
   dtCore::ShaderProgram* shaderProgram = NULL;
   if (!modelData.GetShaderGroupName().empty())
   {
      dtCore::ShaderGroup* spGroup = shaderManager.FindShaderGroupPrototype(modelData.GetShaderGroupName());
      if (spGroup != NULL)
      {
         bool editMode = dtCore::Project::GetInstance().GetEditMode();

         if (editMode)
         {
            shaderProgram = spGroup->GetEditorShader();
         }

         if (shaderProgram == NULL && !modelData.GetShaderName().empty())
         {
            shaderProgram = spGroup->FindShader(modelData.GetShaderName());
            if (shaderProgram == NULL)
            {
               LOG_ERROR("Shader program \"" + modelData.GetShaderName() + "\" from group \""
                  + modelData.GetShaderGroupName() + "\" was not found, using the default from the group.");
            }
         }

         if (shaderProgram == NULL)
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


void GeometryBuilder::GeometryCache::SetUpMaterial(osg::Geometry* geom, CalHardwareModel* hardwareModel, Cal3DModelWrapper* pWrapper, int meshId, int subMeshId)
{
    
   osg::StateSet* ss = geom->getOrCreateStateSet();

   osg::Material* material = new osg::Material();
   ss->setAttributeAndModes(material, osg::StateAttribute::ON);

   if (!hardwareModel->selectHardwareMesh(subMeshId))
   {
      return;
   }

   unsigned char meshColor[4];
   osg::Vec4 materialColor;

   // set the material diffuse color
   hardwareModel->getDiffuseColor(&meshColor[0]);
   materialColor[0] = meshColor[0] / 255.0f;
   materialColor[1] = meshColor[1] / 255.0f;
   materialColor[2] = meshColor[2] / 255.0f;
   materialColor[3] = meshColor[3] / 255.0f;

   bool materialTranslucent = materialColor[3] < 1.0f;
   osg::Material::Face materialFace = materialTranslucent ? osg::Material::FRONT_AND_BACK : osg::Material::FRONT;

   material->setDiffuse(materialFace, materialColor);

   // set the material ambient color
   hardwareModel->getAmbientColor(&meshColor[0]);
   materialColor[0] = meshColor[0] / 255.0f;
   materialColor[1] = meshColor[1] / 255.0f;
   materialColor[2] = meshColor[2] / 255.0f;
   materialColor[3] = meshColor[3] / 255.0f;
   material->setAmbient(materialFace, materialColor);

   // set the material specular color
   hardwareModel->getSpecularColor(&meshColor[0]);
   materialColor[0] = meshColor[0] / 255.0f;
   materialColor[1] = meshColor[1] / 255.0f;
   materialColor[2] = meshColor[2] / 255.0f;
   materialColor[3] = meshColor[3] / 255.0f;
   material->setSpecular(materialFace, materialColor);

   // set the material shininess factor
   float shininess = hardwareModel->getShininess();
   material->setShininess(materialFace, shininess);

   std::vector<CalHardwareModel::CalHardwareMesh>& meshVec = hardwareModel->getVectorHardwareMesh();

   if (subMeshId >= meshVec.size())
   {
      LOG_WARNING("MeshID isn't defined in the list of meshes");
      return;
   }

   CalHardwareModel::CalHardwareMesh& coreMesh = meshVec[subMeshId];

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
}

 
}//namespace dtAnim
