#include <dtChar/cal3dwrapper.h>
#include <cal3d/cal3d.h>
#include <assert.h>

using namespace dtChar;

Cal3DWrapper::Cal3DWrapper(CalModel *model):
mCalModel(model),
mRenderer(NULL),
mMixer(NULL)
{
   assert(mCalModel != NULL);
   mRenderer = mCalModel->getRenderer();
   mMixer = mCalModel->getMixer();

   if (model)
   {
      CalCoreModel *coreModel = model->getCoreModel();

      // attach all meshes to the model
      if (coreModel)
      {
         for(int meshId = 0; meshId < coreModel->getCoreMeshCount(); meshId++)
         {
            AttachMesh(meshId);
         }
      }      
   }  

   SetMaterialSet(0);
}

Cal3DWrapper::~Cal3DWrapper()
{
   delete mCalModel;
}


void Cal3DWrapper::SetCalModel( CalModel *model )
{
   mCalModel = model;
   assert(mCalModel != NULL);
   mRenderer = mCalModel->getRenderer();
   mMixer = mCalModel->getMixer();
}
