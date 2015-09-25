
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
#include <dtUtil/threadpool.h>

namespace dtAnim
{

   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT LoadTask : public dtUtil::ThreadPoolTask
   {
   public:
      LoadTask(dtAnim::ModelLoader& loader, const dtCore::ResourceDescriptor& resource);

      virtual void operator()();

   private:
      dtCore::RefPtr<dtAnim::ModelLoader> mLoader;
      dtCore::ResourceDescriptor mResource;
   };

   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   ModelLoader::ModelLoader()
   : mLoadState(IDLE)
   {
   }
      
   /////////////////////////////////////////////////////////////////////////////////
   ModelLoader::~ModelLoader()
   {}

   /////////////////////////////////////////////////////////////////////////////////
   ModelLoader::LoadingState ModelLoader::GetLoadingState(bool reset)
   {
      ModelLoader::LoadingState result = mLoadState;
      if (reset && (mLoadState == COMPLETE || mLoadState == FAILED))
      {
         mLoadState = IDLE;
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void ModelLoader::LoadModel(const dtCore::ResourceDescriptor& resource, bool async)
   {
      LoadingState tempLoadingState = mLoadState = LOADING;
      mResource = resource;

      dtCore::RefPtr<dtAnim::BaseModelWrapper> result;

      dtAnim::ModelDatabase& database = dtAnim::ModelDatabase::GetInstance();
      // Already loaded.
      dtAnim::BaseModelData* modelData = database.GetModelData(resource);
      
      if (modelData != NULL)
      {
         tempLoadingState = COMPLETE;
         result = CreateModel();
      }
      else if (!async)
      {
         if (database.Load(resource))
         {
            modelData = database.GetModelData(resource);
            if (modelData == NULL)
            {
               LOG_ERROR("Invalid state.  Character Database Load returned true but the model data for '" + resource.GetResourceIdentifier() + "' is not found.");
               tempLoadingState = FAILED;
            }
            else
            {
               tempLoadingState = COMPLETE;
               result = CreateModel();
            }
         }
         else
         {
            LOG_ERROR("ModelData not found for Character XML '" + resource.GetResourceIdentifier() + "'");
            tempLoadingState = FAILED;
         }
      }
      else
      {
         dtCore::RefPtr<LoadTask> loadTask = new LoadTask(*this, resource);
         dtUtil::ThreadPool::AddTask(*loadTask, dtUtil::ThreadPool::IO);
         tempLoadingState = LOADING;
      }

      if (tempLoadingState != LOADING)
      {
         ModelLoaded.emit_signal(result, tempLoadingState);
      }
      // Set the state at the end so that the callback is done before external could ever see
      // loading as COMPLETE or FAILED
      mLoadState = tempLoadingState;
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtAnim::BaseModelWrapper> ModelLoader::CreateModel()
   {
      if (GetLoadedModelData() == NULL)
      {
         return NULL;
      }

      dtCore::RefPtr<dtAnim::BaseModelWrapper> newWrapper
         = dtAnim::ModelDatabase::GetInstance().CreateModelWrapper(*GetLoadedModelData());
      
      if (!mAttachmentController.valid())
      {
         mAttachmentController = new dtAnim::AttachmentController;
      }
      CreateAttachments(*GetLoadedModelData(), *mAttachmentController);
      
      return newWrapper;
   }

   /////////////////////////////////////////////////////////////////////////////////
   dtAnim::BaseModelData* ModelLoader::GetLoadedModelData()
   {
      dtAnim::ModelDatabase& database = dtAnim::ModelDatabase::GetInstance();

      // See if the data is ready yet
      return database.GetModelData(mResource);
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
                  LOG_ERROR(std::string("Loading attached resource for \"") + modelData.GetResource().GetResourceIdentifier() + "\" character file: " + i->second  + " : " + pfe.ToString());
               }
               catch (const dtCore::ProjectInvalidContextException& pice)
               {
                  LOG_ERROR(std::string("Project invalid exception while loading attached resource  \"") + modelData.GetResource().GetResourceIdentifier()  + "\" character file: " + i->second  + " : " + pice.ToString());
               }
            }
            dtCore::RefPtr<dtCore::Object> obj = new dtCore::Object(hsd.mName);
            obj->LoadFile(pathToLoad, true);
            newAttachment->AddChild(obj);
         }

         attachmentController.AddAttachment(*newAttachment, i->first);
      }
   }


   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   LoadTask::LoadTask(dtAnim::ModelLoader& loader, const dtCore::ResourceDescriptor& resource)
   : mLoader(&loader)
   , mResource(resource)
   {
      SetName(resource.GetResourceIdentifier());
   }

   void LoadTask::operator()()
   {
      try
      {
         mLoader->LoadModel(mResource, false);
      }
      catch(const dtUtil::Exception& ex)
      {
         ex.LogException(dtUtil::Log::LOG_WARNING);
      }
      catch(...)
      {
         LOG_ERROR("Unknown exception thrown while loading animation model: " + mResource.GetResourceIdentifier());
      }
   }

}
