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
#include <dtAnim/geometrybuilder.h>

#include <dtCore/shaderprogram.h>
#include <dtCore/shadermanager.h>
#include <dtCore/shadergroup.h>
#include <dtCore/project.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/log.h>

#include <osg/Geode>
#include <osg/State>
#include <osg/BoundingSphere>
#include <osg/BoundingBox>
#include <osg/Texture2D>
#include <osg/GLExtensions>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>

#include <cal3d/hardwaremodel.h>

#include <cal3d/error.h>

//For the bounding box class, who knew.
#include <cal3d/vector.h>

namespace dtAnim
{
   GeometryBuilder AnimNodeBuilder::mGeometryBuilder;

   ///Used to delay the building of the animated characters geometry until
   ///it is first rendered, at which point a valid OpenGL context should be valid
   class CreateGeometryDrawCallback : public osg::Drawable::DrawCallback
   {
   public:
      CreateGeometryDrawCallback(AnimNodeBuilder::CreateFunc& func,
         Cal3DModelWrapper* wrapper)
         : mCreatedNode(NULL)
         , mCreateFunc(func)
         , mWrapper(wrapper)
      {
      };

      ~CreateGeometryDrawCallback()
      {
      };

      virtual void drawImplementation(osg::RenderInfo& renderInfo, const osg::Drawable*) const
      {
         if (!mCreatedNode.valid())
         {
            CreateGeometryDrawCallback* const_this = const_cast<CreateGeometryDrawCallback*>(this);
            const_this->mCreatedNode = mCreateFunc(&renderInfo, mWrapper);
         }
      }

      dtCore::RefPtr<osg::Node> mCreatedNode;

   private:
      AnimNodeBuilder::CreateFunc mCreateFunc;
      Cal3DModelWrapper* mWrapper;
   };

   ///Used to grab the created geometry from the CreateGeometryDrawCallback
   ///and add it as a child to the supplied Group
   class UpdateCallback : public osg::NodeCallback
   {
   public:
      UpdateCallback(CreateGeometryDrawCallback* callback, osg::Group& group)
         : mCreateCB(callback)
         , mGroupToAddTo(&group)
      {
      }

      ~UpdateCallback()
      {
      }

      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      {
         //wait until the create draw-callback has a valid node created
         if (mCreateCB->mCreatedNode.valid())
         {
            //then add it, remove the temp geometry, and remove this callback
            mGroupToAddTo->addChild(mCreateCB->mCreatedNode.get());
            mGroupToAddTo->removeChild(0, 1);
            mGroupToAddTo->setUpdateCallback(NULL);
         }
         else
         {
            traverse(node,nv);
         }
      }

   private:
      CreateGeometryDrawCallback* mCreateCB;
      osg::observer_ptr<osg::Group> mGroupToAddTo;
   };

////////////////////////////////////////////////////////////////////////////////
AnimNodeBuilder::Cal3DBoundingSphereCalculator::Cal3DBoundingSphereCalculator(Cal3DModelWrapper& wrapper)
   : mWrapper(&wrapper)
{
}

////////////////////////////////////////////////////////////////////////////////
osg::BoundingSphere AnimNodeBuilder::Cal3DBoundingSphereCalculator::computeBound(const osg::Node&) const
{
   osg::BoundingSphere bSphere(mWrapper->GetBoundingBox());
   return bSphere;
}

////////////////////////////////////////////////////////////////////////////////
AnimNodeBuilder::AnimNodeBuilder(bool useDeprecatedHardwareModel)
   : mUseDeprecatedHardwareModel(useDeprecatedHardwareModel)
{
   if (SupportsHardware())
   {
      if(mUseDeprecatedHardwareModel)
      {
         SetCreate(CreateFunc(this, &AnimNodeBuilder::CreateHardware));
      }
      else
      {
         SetCreate(CreateFunc(&mGeometryBuilder, &GeometryBuilder::CreateGeometry));
      }
   }
   else
   if (SupportsSoftware())
   {
      SetCreate(CreateFunc(this, &AnimNodeBuilder::CreateSoftware));
   }
   else
   {
      LOG_WARNING("dtAnim could not deduce the hardware capability, no geometry will be rendered.")
      SetCreate(CreateFunc(this, &AnimNodeBuilder::CreateNULL));
   }
}

////////////////////////////////////////////////////////////////////////////////
AnimNodeBuilder::AnimNodeBuilder(const CreateFunc& pCreate)
: mCreateFunc(pCreate)
, mUseDeprecatedHardwareModel(false)
{
}

////////////////////////////////////////////////////////////////////////////////
AnimNodeBuilder::~AnimNodeBuilder()
{

}

////////////////////////////////////////////////////////////////////////////////
AnimNodeBuilder::CreateFunc& AnimNodeBuilder::GetCreate()
{
   return mCreateFunc;
}

////////////////////////////////////////////////////////////////////////////////
void AnimNodeBuilder::SetCreate(const CreateFunc& pCreate)
{
   mCreateFunc = pCreate;
}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Node> AnimNodeBuilder::CreateNode(Cal3DModelWrapper* pWrapper, bool immediate)
{
   dtCore::RefPtr<osg::Node> result;
   if(!immediate)
   {
      ///Add a temporary rendered shape with a draw callback to a Group.  The callback
      ///will postpone the creation of the real geometry until a valid openGL
      ///context is available.
      CreateGeometryDrawCallback* createCallback = new CreateGeometryDrawCallback(mCreateFunc, pWrapper);
      osg::Group* rootNode = new osg::Group();
      rootNode->setUpdateCallback(new UpdateCallback(createCallback, *rootNode));

      osg::Geode* defaultGeode = new osg::Geode();
      osg::Cylinder* shape = new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f), 2.0f, 4.0f);
      osg::ShapeDrawable* defaultDrawable = new osg::ShapeDrawable(shape);
      defaultDrawable->setDrawCallback(createCallback);

      defaultGeode->addDrawable(defaultDrawable);
      rootNode->addChild(defaultGeode);

      result = rootNode;
   }
   else
   {
      result = mCreateFunc(0, pWrapper);
   }

   float scale = pWrapper->GetScale();
   if (result.valid() && dtUtil::Abs(1.0f - scale) > FLT_EPSILON)
   {
      osg::Matrix scaleXform;
      scaleXform.makeScale(scale, scale, scale);
      dtCore::RefPtr<osg::MatrixTransform> scaleTransform = new osg::MatrixTransform(scaleXform);
      scaleTransform->setName("Scale Transform");
      scaleTransform->addChild(result);
      result = scaleTransform.get();
   }
   return result;
}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Node> AnimNodeBuilder::CreateSoftwareInternal(Cal3DModelWrapper* pWrapper, bool vbo)
{
   if (pWrapper == NULL)
   {
      LOG_ERROR("Invalid parameter to CreateGeode.");
      return NULL;
   }

   dtCore::RefPtr<osg::Geode> geode = new osg::Geode();

   geode->setComputeBoundingSphereCallback(new Cal3DBoundingSphereCalculator(*pWrapper));

   if (pWrapper->BeginRenderingQuery())
   {
      // Compute this only once
      osg::BoundingBox boundingBox = pWrapper->GetBoundingBox();

      int meshCount = pWrapper->GetMeshCount();

      for (int meshId = 0; meshId < meshCount; meshId++)
      {
         int submeshCount = pWrapper->GetSubmeshCount(meshId);

         for (int submeshId = 0; submeshId < submeshCount; submeshId++)
         {
            dtAnim::SubmeshDrawable* submesh = new dtAnim::SubmeshDrawable(pWrapper, meshId, submeshId);
            submesh->SetBoundingBox(boundingBox);
            submesh->setUseVertexBufferObjects(vbo);
            geode->addDrawable(submesh);
         }
      }

      pWrapper->EndRenderingQuery();
   }

   pWrapper->Update(0);

   return geode;
}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Node> AnimNodeBuilder::CreateSoftware(osg::RenderInfo* renderInfo, Cal3DModelWrapper* pWrapper)
{
   return AnimNodeBuilder::CreateSoftwareInternal(pWrapper, true);
}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Node> AnimNodeBuilder::CreateSoftwareNoVBO(osg::RenderInfo* renderInfo, Cal3DModelWrapper* pWrapper)
{
   return AnimNodeBuilder::CreateSoftwareInternal(pWrapper, false);
}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Node> AnimNodeBuilder::CreateHardware(osg::RenderInfo* renderInfo, Cal3DModelWrapper* pWrapper)
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

   dtCore::RefPtr<osg::Geode> geode = new osg::Geode();

   static const std::string BONE_TRANSFORM_UNIFORM("boneTransforms");
   static const std::string BONE_WEIGHTS_ATTRIB("boneWeights");
   static const std::string BONE_INDICES_ATTRIB("boneIndices");
   static const std::string TANGENT_SPACE_ATTRIB("tangentSpace");

   pWrapper->SetLODLevel(1);
   pWrapper->Update(0);

   if (pWrapper->BeginRenderingQuery() == false)
   {
      LOG_ERROR("Can't begin the rendering query.");
      return NULL;
   }

   int numVerts = 0;
   int numIndices = 0;

   CalcNumVertsAndIndices(pWrapper, numVerts, numIndices);

   CalHardwareModel* hardwareModel = modelData->GetOrCreateCalHardwareModel();

   CalIndex* indexArray = modelData->GetSourceIndexArray();
   osg::FloatArray* vertexArray = modelData->GetSourceVertexArray();

   // Create GPU resources from our source data
   {
      dtCore::RefPtr<osg::VertexBufferObject> vertexVBO = modelData->GetVertexBufferObject();
      dtCore::RefPtr<osg::ElementBufferObject> indexEBO = modelData->GetElementBufferObject();

      if (!vertexVBO.valid() || !indexEBO.valid())
      {
         vertexVBO = new osg::VertexBufferObject;
         indexEBO = new osg::ElementBufferObject;

         vertexVBO->addArray(vertexArray);

         // Store the buffers with the model data for possible re-use later
         modelData->SetVertexBufferObject(vertexVBO);
         modelData->SetElementBufferObject(indexEBO);

         osg::DrawElements* drawElements = NULL;

         // Allocate the draw elements for the element size that CalIndex defines
         if (sizeof(CalIndex) < 4)
         {
            drawElements = new osg::DrawElementsUShort(GL_TRIANGLES, numIndices, (GLushort*)indexArray);
         }
         else
         {
            drawElements = new osg::DrawElementsUInt(GL_TRIANGLES, numIndices, (GLuint*)indexArray);
         }

         modelData->SetDrawElements(drawElements);
      }

      dtCore::ShaderProgram* shadProg = LoadShaders(*modelData, *geode);


      /* Begin figure out if this open gl implementation uses [0] on array uniforms
       * This seems to be an ATI/NVIDIA thing.
       * This requires me to force the shader to compile.
       * The latest developer version of Open Scene Graph has a workaround
       * in the shader program class for this issue, but the current stable
       * osg release as of this writing (3.0.1) lacks the fix so
       * include this hack here.
       */
      osg::Program* prog = shadProg->GetShaderProgram();
      prog->compileGLObjects(*renderInfo->getState());

      std::string boneTransformUniform = BONE_TRANSFORM_UNIFORM;

      if (prog->getPCP(renderInfo->getContextID()) != NULL && prog->getPCP(renderInfo->getContextID())->getUniformLocation(boneTransformUniform) == -1)
      {
         if (prog->getPCP(renderInfo->getContextID()) != NULL && prog->getPCP(renderInfo->getContextID())->getUniformLocation(boneTransformUniform + "[0]") == -1)
         {
            LOG_ERROR("Can't find uniform named \"" + boneTransformUniform
                      + "\" which is required for skinning.");
         }
         else
         {
            boneTransformUniform.append("[0]");
         }
      }
      //End check.

      // Compute this only once
      osg::BoundingBox boundingBox = pWrapper->GetBoundingBox();

      int boneTransformLocation = prog->getPCP(renderInfo->getContextID())->getUniformLocation(boneTransformUniform);
      int boneWeightsLocation = prog->getPCP(renderInfo->getContextID())->getAttribLocation(BONE_WEIGHTS_ATTRIB);
      int boneIndicesLocation = prog->getPCP(renderInfo->getContextID())->getAttribLocation(BONE_INDICES_ATTRIB);
      int tangentsLocation = prog->getPCP(renderInfo->getContextID())->getAttribLocation(TANGENT_SPACE_ATTRIB);

      if (boneTransformLocation == -1) {
          LOG_ERROR("Can't find uniform named \"" + BONE_TRANSFORM_UNIFORM
              + "\" which is required for skinning.");
      }
      else if (boneWeightsLocation== -1) {
          LOG_ERROR("Can't find attribute named \"" + BONE_WEIGHTS_ATTRIB
              + "\" which is required for skinning.");
      }
      else if (boneIndicesLocation == -1) {
          LOG_ERROR("Can't find attribute named \"" + BONE_INDICES_ATTRIB
              + "\" which is required for skinning.");
      }
      else {
          for (int meshCount = 0; meshCount < hardwareModel->getHardwareMeshCount(); ++meshCount) {
              HardwareSubmeshDrawable* drawable = new HardwareSubmeshDrawable(pWrapper, hardwareModel,
                  BONE_TRANSFORM_UNIFORM, modelData->GetShaderMaxBones(),
                  meshCount, vertexVBO, indexEBO,
                  boneWeightsLocation,
                  boneIndicesLocation,
                  tangentsLocation);
              drawable->SetBoundingBox(boundingBox);
              geode->addDrawable(drawable);
          }
      }

      geode->setComputeBoundingSphereCallback(new Cal3DBoundingSphereCalculator(*pWrapper));
   }

   pWrapper->EndRenderingQuery();

   return geode;
}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Node> AnimNodeBuilder::CreateNULL(osg::RenderInfo* renderInfo, Cal3DModelWrapper* pWrapper)
{
   DTUNREFERENCED_PARAMETER(pWrapper);

   dtCore::RefPtr<osg::Geode> geode = new osg::Geode();

   //NULL create function.  Used if hardware and software create functions fail.
   return geode;
}

////////////////////////////////////////////////////////////////////////////////
dtCore::ShaderProgram* AnimNodeBuilder::LoadShaders(Cal3DModelData& modelData, osg::Geode& geode) const
{
   static const std::string hardwareSkinningSPGroup = "HardwareSkinningLegacy";
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

////////////////////////////////////////////////////////////////////////////////
void AnimNodeBuilder::CalcNumVertsAndIndices(Cal3DModelWrapper* pWrapper,
                                             int& numVerts, int& numIndices)
{
   CalCoreModel* model = pWrapper->GetCalModel()->getCoreModel();


   const int meshCount = model->getCoreMeshCount();

   for (int meshId = 0; meshId < meshCount; meshId++)
   {
      CalCoreMesh* calMesh = model->getCoreMesh(meshId);
      int submeshCount = calMesh->getCoreSubmeshCount();

      for (int submeshId = 0; submeshId < submeshCount; submeshId++)
      {
         CalCoreSubmesh* subMesh = calMesh->getCoreSubmesh(submeshId);
         numVerts += subMesh->getVertexCount();
         numIndices += 3 * subMesh->getFaceCount();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
bool AnimNodeBuilder::SupportsHardware() const
{
   //check if hardware supports our requirements
   return SupportsVertexBuffers();
}

////////////////////////////////////////////////////////////////////////////////
bool AnimNodeBuilder::SupportsSoftware() const
{
   //check if hardware supports our requirements
   return SupportsVertexBuffers();
}

////////////////////////////////////////////////////////////////////////////////
bool AnimNodeBuilder::SupportsVertexBuffers() const
{
   //see if we can support vertex buffer objects
   osg::Drawable::getExtensions(0, true);

   return (osg::isGLExtensionSupported(0, "GL_ARB_vertex_buffer_object"));
}

////////////////////////////////////////////////////////////////////////////////


}//namespace dtAnim
