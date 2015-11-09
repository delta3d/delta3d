
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA3D
#include <dtAnim/cal3dnodebuilder.h>
#include <dtAnim/animnodebuilder.h>
#include <dtAnim/constants.h>
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
   Cal3DBoundingSphereCalculator::Cal3DBoundingSphereCalculator(dtAnim::Cal3DModelWrapper& wrapper)
      : mWrapper(&wrapper)
   {}

   osg::BoundingSphere Cal3DBoundingSphereCalculator::computeBound(const osg::Node&) const
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
   
   dtCore::RefPtr<osg::Node> Cal3dNodeBuilder::CreateSoftware(osg::RenderInfo* renderInfo, dtAnim::Cal3DModelWrapper* wrapper)
   {
      return CreateSoftwareInternal(renderInfo, wrapper, true);
   }

   dtCore::RefPtr<osg::Node> Cal3dNodeBuilder::CreateSoftwareNoVBO(osg::RenderInfo* renderInfo, dtAnim::Cal3DModelWrapper* wrapper)
   {
      return CreateSoftwareInternal(renderInfo, wrapper, false);
   }

   dtCore::RefPtr<osg::Node> Cal3dNodeBuilder::CreateSoftwareInternal(osg::RenderInfo* renderInfo, Cal3DModelWrapper* wrapper, bool vbo)
   {
      if (wrapper == NULL)
      {
         LOG_ERROR("Invalid parameter to CreateGeode.");
         return NULL;
      }

      dtCore::RefPtr<osg::Geode> geode = new osg::Geode();

      geode->setComputeBoundingSphereCallback(new Cal3DBoundingSphereCalculator(*wrapper));

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

      wrapper->UpdateAnimation(0.0f);

      return geode;
   }

   dtCore::RefPtr<osg::Node> Cal3dNodeBuilder::CreateHardware(osg::RenderInfo* renderInfo, Cal3DModelWrapper* wrapper)
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

      dtCore::RefPtr<osg::Geode> geode = new osg::Geode();

      wrapper->SetLODLevel(1);
      wrapper->UpdateAnimation(0);

      if (wrapper->BeginRenderingQuery() == false)
      {
         LOG_ERROR("Can't begin the rendering query.");
         return NULL;
      }

      int numVerts = 0;
      int numIndices = 0;

      CalcNumVertsAndIndices(wrapper, numVerts, numIndices);

      CalHardwareModel* hardwareModel = modelData->GetOrCreateCalHardwareModel();

      osg::IntArray* indexArray = modelData->GetSourceIndexArray();
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

         dtCore::ShaderProgram* shadProg
            = AnimNodeBuilder::LoadShaders(*modelData, *geode);

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

         osg::Program::PerContextProgram* pcp = prog->getPCP(*renderInfo->getState());

         std::string boneTransformUniform = Constants::BONE_TRANSFORM_UNIFORM;

         if (pcp != NULL && pcp->getUniformLocation(boneTransformUniform) == -1)
         {
            if (pcp != NULL && pcp->getUniformLocation(boneTransformUniform + "[0]") == -1)
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
         osg::BoundingBox boundingBox = wrapper->GetBoundingBox();

         int boneTransformLocation = pcp->getUniformLocation(boneTransformUniform);
         int boneWeightsLocation = pcp->getAttribLocation(Constants::BONE_WEIGHTS_ATTRIB);
         int boneIndicesLocation = pcp->getAttribLocation(Constants::BONE_INDICES_ATTRIB);
         int tangentsLocation = pcp->getAttribLocation(Constants::TANGENT_SPACE_ATTRIB);

         if (boneTransformLocation == -1) {
             LOG_ERROR("Can't find uniform named \"" + Constants::BONE_TRANSFORM_UNIFORM
                 + "\" which is required for skinning.");
         }
         else if (boneWeightsLocation== -1) {
             LOG_ERROR("Can't find attribute named \"" + Constants::BONE_WEIGHTS_ATTRIB
                 + "\" which is required for skinning.");
         }
         else if (boneIndicesLocation == -1) {
             LOG_ERROR("Can't find attribute named \"" + Constants::BONE_INDICES_ATTRIB
                 + "\" which is required for skinning.");
         }
         else
         {
             for (int meshCount = 0; meshCount < hardwareModel->getHardwareMeshCount(); ++meshCount)
             {
                 HardwareSubmeshDrawable* drawable = new HardwareSubmeshDrawable(wrapper, hardwareModel,
                     Constants::BONE_TRANSFORM_UNIFORM, modelData->GetShaderMaxBones(),
                     meshCount, vertexVBO, indexEBO,
                     boneWeightsLocation,
                     boneIndicesLocation,
                     tangentsLocation);
                 drawable->SetBoundingBox(boundingBox);
                 geode->addDrawable(drawable);
             }
         }

         geode->setComputeBoundingSphereCallback(new Cal3DBoundingSphereCalculator(*wrapper));
      }

      wrapper->EndRenderingQuery();

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
