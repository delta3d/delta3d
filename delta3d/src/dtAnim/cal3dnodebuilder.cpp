
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

#include <osg/MatrixTransform> // HACK:

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

      dtCore::RefPtr<osg::Geode> geode = new osg::Geode();

      wrapper->SetLODLevel(1);
      wrapper->UpdateAnimations(0.0f);

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
         osg::VertexBufferObject* vertexVBO = modelData->GetVertexBufferObject();
         osg::ElementBufferObject* indexEBO = modelData->GetElementBufferObject();

         if (vertexVBO == NULL)
         {
            // Either both should be NULL, or both non NULL
            assert(indexEBO == NULL);

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
               // WARNING! Ensure that the array pointer is actually of type short.
               drawElements = new osg::DrawElementsUShort(GL_TRIANGLES, numIndices, (GLushort*)indexArray->getDataPointer());
            }
            else
            {
               drawElements = new osg::DrawElementsUInt(GL_TRIANGLES, numIndices, (GLuint*)indexArray->getDataPointer());
            }

            modelData->SetDrawElements(drawElements);
         }

         dtCore::RefPtr<dtAnim::CharacterShaderBuilder> shaderBuilder;
         dtCore::ShaderProgram* shadProg = shaderBuilder->LoadShaders(*modelData, *geode);
         //End check.

         // Compute this only once
         osg::BoundingBox boundingBox = wrapper->GetBoundingBox();

         for (int meshCount = 0; meshCount < hardwareModel->getHardwareMeshCount(); ++meshCount)
         {
            HardwareSubmeshDrawable* drawable = new HardwareSubmeshDrawable(wrapper, hardwareModel,
                                                    CharacterShaderBuilder::BONE_TRANSFORM_UNIFORM, modelData->GetShaderMaxBones(),
                                                    meshCount, vertexVBO, indexEBO);
            drawable->SetBoundingBox(boundingBox);
            geode->addDrawable(drawable);

            // Register the drawable instance with the associated interface object.
            dtAnim::Cal3dHardwareMesh* mesh = dynamic_cast<dtAnim::Cal3dHardwareMesh*>
               (wrapper->GetMeshByIndex(meshCount));
            if (mesh != NULL)
            {
               mesh->SetDrawable(drawable);
            }
         }

         geode->setComputeBoundingSphereCallback(new Cal3dBoundingSphereCalculator(*wrapper));
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
