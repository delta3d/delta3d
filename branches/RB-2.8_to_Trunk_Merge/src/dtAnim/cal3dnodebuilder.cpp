
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA3D
#include <dtAnim/cal3dnodebuilder.h>
#include <dtAnim/charactershaderbuilder.h>
#include <dtCore/shaderprogram.h>
#include <dtUtil/log.h>
// OSG
#include <osg/Geode>

DT_DISABLE_WARNING_ALL_START
#include <cal3d/global.h>
#include <cal3d/hardwaremodel.h>

//For the bounding box class, who knew.
#include <cal3d/vector.h>
DT_DISABLE_WARNING_END



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   Cal3dBoundingSphereCalculator::Cal3dBoundingSphereCalculator(dtAnim::Cal3DModelWrapper& wrapper)
      : mWrapper(&wrapper)
   {}

   osg::BoundingSphere Cal3dBoundingSphereCalculator::computeBound(const osg::Node&) const
   {
      osg::BoundingSphere bSphere(mWrapper->GetBoundingBox());
      if (bSphere.radius2() <= FLT_EPSILON)
      {
         bSphere.expandBy(osg::Vec3(3.0f, 3.0f, 3.0f));
      }
      return bSphere;
   }



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   Cal3dNodeBuilder::Cal3dNodeBuilder()
   {}

   Cal3dNodeBuilder::~Cal3dNodeBuilder()
   {}
   
   dtCore::RefPtr<osg::Node> Cal3dNodeBuilder::CreateSoftware(dtAnim::Cal3DModelWrapper* wrapper)
   {
      return CreateSoftwareInternal(wrapper, true);
   }

   dtCore::RefPtr<osg::Node> Cal3dNodeBuilder::CreateSoftwareNoVBO(dtAnim::Cal3DModelWrapper* wrapper)
   {
      return CreateSoftwareInternal(wrapper, false);
   }

   dtCore::RefPtr<osg::Node> Cal3dNodeBuilder::CreateSoftwareInternal(Cal3DModelWrapper* wrapper, bool vbo)
   {
      if (wrapper == NULL)
      {
         LOG_ERROR("Invalid parameter to CreateGeode.");
         return NULL;
      }

      dtCore::RefPtr<osg::Geode> geode = new osg::Geode();

      geode->setComputeBoundingSphereCallback(new Cal3dBoundingSphereCalculator(*wrapper));

      if (wrapper->BeginRenderingQuery())
      {
         // Compute this only once
         osg::BoundingBox boundingBox = wrapper->GetBoundingBox();
      
         dtAnim::MeshArray meshes;
         wrapper->GetMeshes(meshes);

         dtAnim::MeshInterface* curMesh = NULL;
         dtAnim::MeshArray::iterator curIter = meshes.begin();
         dtAnim::MeshArray::iterator endIter = meshes.end();
         for ( ; curIter != endIter; ++curIter)
         {
            curMesh = curIter->get();

            dtAnim::SubmeshArray submeshes;
            curMesh->GetSubmeshes(submeshes);

            dtAnim::SubmeshInterface* curSubmesh = NULL;
            dtAnim::SubmeshArray::iterator curSubmeshIter = submeshes.begin();
            dtAnim::SubmeshArray::iterator endSubmeshIter = submeshes.end();
            for (int submeshID = 0; curSubmeshIter != endSubmeshIter; ++submeshID, ++curSubmeshIter)
            {
               curSubmesh = curSubmeshIter->get();

               dtCore::RefPtr<dtAnim::SubmeshDrawable> submesh
                  = new dtAnim::SubmeshDrawable(wrapper, curMesh->GetID(), submeshID);
               submesh->SetBoundingBox(boundingBox);
               submesh->setUseVertexBufferObjects(vbo);
               geode->addDrawable(submesh);

               dtAnim::Cal3dSubmesh* calSubmesh = dynamic_cast<dtAnim::Cal3dSubmesh*>(curSubmesh);
               if (calSubmesh != NULL)
               {
                  calSubmesh->SetDrawable(submesh);
               }
            }
         }

         wrapper->EndRenderingQuery();
      }

      wrapper->UpdateAnimations(0.0f);

      return geode;
   }

   dtCore::RefPtr<osg::Node> Cal3dNodeBuilder::CreateHardware(Cal3DModelWrapper* wrapper)
   {
      if (wrapper == NULL)
      {
         LOG_ERROR("Invalid parameter to CreateGeode.");
         return NULL;
      }

      Cal3DModelData* modelData = wrapper->GetCalModelData();

      if (modelData == NULL)
      {
         LOG_ERROR("Model does not have model data.  Unable to create hardware submesh.");
         return NULL;
      }

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

      return geode;
   }

   void Cal3dNodeBuilder::CalcNumVertsAndIndices(Cal3DModelWrapper* pWrapper,
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

}
