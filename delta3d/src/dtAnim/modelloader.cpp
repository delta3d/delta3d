
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/modelloader.h>
#include <dtAnim/basemodeldata.h>
#include <dtAnim/cal3dmodeldata.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/hotspotattachment.h>
#include <dtAnim/modeldatabase.h>
#include <dtCore/object.h>
#include <dtCore/project.h>
#include <dtCore/resourcedescriptor.h>
#include <dtUtil/log.h>

namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   ModelLoader::ModelLoader()
   {
   }
      
   /////////////////////////////////////////////////////////////////////////////////
   ModelLoader::~ModelLoader()
   {}

   /////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtAnim::BaseModelWrapper> ModelLoader::LoadModel(const std::string& filename)
   {
      mFile = filename;

      dtAnim::ModelDatabase& database = dtAnim::ModelDatabase::GetInstance();
      mLoadedModel = database.Load(filename);
      
      if (mLoadedModel.valid())
      {
         dtAnim::BaseModelData* modelData = mLoadedModel->GetModelData();
         if (modelData == NULL)
         {
            LOG_ERROR("ModelData not found for Character XML '" + filename + "'");
            return NULL;
         }

         if (!mAttachmentController.valid())
         {
            mAttachmentController = new dtAnim::AttachmentController;
         }
         CreateAttachments(*modelData, *mAttachmentController);
      }

      return mLoadedModel;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void ModelLoader::LoadModelAsynchronously(const std::string& filename)
   {
      dtAnim::ModelDatabase& database = dtAnim::ModelDatabase::GetInstance();
      database.LoadAsynchronously(filename);
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtAnim::BaseModelWrapper> ModelLoader::CreateModel(dtAnim::BaseModelData& modelData)
   {
      dtAnim::Cal3DModelData* calModelData = dynamic_cast<dtAnim::Cal3DModelData*>(&modelData);
      if (calModelData == NULL)
      {
         LOG_ERROR("Could not cast model data to Cal3DModelData.");
         return NULL;
      }

      dtCore::RefPtr<dtAnim::BaseModelWrapper> newWrapper
         = dtAnim::ModelDatabase::GetInstance().CreateModelWrapper(*calModelData);
      
      mAttachmentController = new dtAnim::AttachmentController;
      CreateAttachments(modelData, *mAttachmentController);
      
      return newWrapper;
   }

   /////////////////////////////////////////////////////////////////////////////////
   dtAnim::BaseModelWrapper* ModelLoader::GetLoadedModel()
   {
      return mLoadedModel.get();
   }

   /////////////////////////////////////////////////////////////////////////////////
   dtAnim::BaseModelData* ModelLoader::GetLoadedModelData()
   {
      dtAnim::ModelDatabase& database = dtAnim::ModelDatabase::GetInstance();

      // See if the data is ready yet
      return database.GetModelData(mFile);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void ModelLoader::SetAttachmentController(dtAnim::AttachmentController* controller)
   {
      mAttachmentController = controller;
   }

   /////////////////////////////////////////////////////////////////////////////////
   dtAnim::AttachmentController* ModelLoader::GetAttachmentController()
   {
      return mAttachmentController.get();
   }

   /////////////////////////////////////////////////////////////////////////////////
   void ModelLoader::CreateAttachments(dtAnim::BaseModelData& modelData, dtAnim::AttachmentController& attachmentController)
   {
      dtAnim::Cal3DModelData* calModelData = dynamic_cast<dtAnim::Cal3DModelData*>(&modelData);
      if (calModelData == NULL)
      {
         LOG_ERROR("Could not cast model data object to Cal3DModelData.");
         return;
      }

      const dtAnim::AttachmentArray& attachments = calModelData->GetAttachments();

      dtAnim::AttachmentArray::const_iterator i,iend;
      i = attachments.begin();
      iend = attachments.end();
      for (; i != iend; ++i)
      {
         dtUtil::HotSpotDefinition hsd = i->first;
         dtCore::RefPtr<dtCore::HotSpotAttachment> newAttachment = new dtCore::HotSpotAttachment(hsd);
         if (!i->second.empty())
         {
            std::string pathToLoad = i->second;
            if (pathToLoad.find(dtCore::ResourceDescriptor::DESCRIPTOR_SEPARATOR) != std::string::npos)
            {
               try
               {
                  pathToLoad = dtCore::Project::GetInstance().GetResourcePath(dtCore::ResourceDescriptor(pathToLoad));
               }
               catch (const dtCore::ProjectFileNotFoundException& pfe)
               {
                  LOG_ERROR(std::string("Error loading attached resource for \"") + modelData.GetFilename() + "\" character file: " + i->second  + " : " + pfe.ToString());
               }
            }
            dtCore::RefPtr<dtCore::Object> obj = new dtCore::Object(hsd.mName);
            obj->LoadFile(pathToLoad, true);
            newAttachment->AddChild(obj);
         }

         attachmentController.AddAttachment(*newAttachment, i->first);
      }
   }

}
