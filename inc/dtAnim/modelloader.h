
#ifndef __DELTA_MODELLOADER_H__
#define __DELTA_MODELLOADER_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/export.h>
#include <dtAnim/basemodelwrapper.h>
#include <dtAnim/attachmentcontroller.h>
#include <dtCore/sigslot.h>
#include <dtCore/resourcedescriptor.h>
#include <osg/Referenced>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace dtCore
{
   class ResourceDescriptor;
}

namespace dtAnim
{
   class BaseModelData;
   class Cal3DModelData;
   


   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT ModelLoader : public osg::Referenced, sigslot::has_slots<>
   {
   public:
      enum LoadingState
      {
         IDLE,
         LOADING,
         FAILED,
         COMPLETE
      };
      ModelLoader();

      sigslot::signal2<dtAnim::BaseModelWrapper*, LoadingState> ModelLoaded;

      /**
       * Loads the model into the database.
       * it will emit the signal ModelLoaded when it completes.
       * @param async pass true if this should load in the background.
       */
      void LoadModel(const dtCore::ResourceDescriptor& resource, bool async = false);

      const dtCore::ResourceDescriptor& GetResourceDescriptor() const { return mResource; }

      /**
       * This will return the loading state for an async load.
       * @param reset  If the state is FAILED or COMPLETE, it will set it back to IDLE if this is true.
       */
      LoadingState GetLoadingState(bool reset = true);

      dtCore::RefPtr<dtAnim::BaseModelWrapper> CreateModel();

      dtAnim::BaseModelData* GetLoadedModelData();

      // Set and get the attachment controller that is to receive attachments.
      void SetAttachmentController(dtAnim::AttachmentController* controller);
      dtAnim::AttachmentController* GetAttachmentController();

      void CreateAttachments(dtAnim::BaseModelData& modelData, AttachmentController& attachmentController);

   protected:
      virtual ~ModelLoader();

   private:
      dtCore::RefPtr<dtAnim::AttachmentController> mAttachmentController;
      LoadingState mLoadState;
      dtCore::ResourceDescriptor mResource;
   };

}



#endif
