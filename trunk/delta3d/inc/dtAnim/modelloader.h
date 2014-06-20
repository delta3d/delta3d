
#ifndef __DELTA_MODELLOADER_H__
#define __DELTA_MODELLOADER_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/export.h>
#include <dtAnim/basemodelwrapper.h>
#include <dtAnim/attachmentcontroller.h>
#include <dtCore/sigslot.h>
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
   class DT_ANIM_EXPORT ModelLoader : public osg::Referenced
   {
   public:
      ModelLoader();

      dtCore::RefPtr<dtAnim::BaseModelWrapper> LoadModel(const std::string& filename);
      void LoadModelAsynchronously(const std::string& filename);

      dtCore::RefPtr<dtAnim::BaseModelWrapper> CreateModel(dtAnim::BaseModelData& modelData);

      dtAnim::BaseModelWrapper* GetLoadedModel();

      dtAnim::BaseModelData* GetLoadedModelData();

      // Set and get the attachment controller that is to receive attachments.
      void SetAttachmentController(dtAnim::AttachmentController* controller);
      dtAnim::AttachmentController* GetAttachmentController();

      void CreateAttachments(dtAnim::BaseModelData& modelData, AttachmentController& attachmentController);

   protected:
      virtual ~ModelLoader();

   private:
      dtCore::RefPtr<dtAnim::BaseModelWrapper> mLoadedModel;
      dtCore::RefPtr<dtAnim::AttachmentController> mAttachmentController;
      std::string mFile;
   };
}

#endif
