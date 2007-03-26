#include <dtAnim/cal3dmodelwrapper.h>
#include <cal3d/cal3d.h>
#include <assert.h>

using namespace dtAnim;

Cal3DModelWrapper::Cal3DModelWrapper(CalModel *model):
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

Cal3DModelWrapper::~Cal3DModelWrapper()
{
   delete mCalModel;
}


void Cal3DModelWrapper::SetCalModel( CalModel *model )
{
   mCalModel = model;
   assert(mCalModel != NULL);
   mRenderer = mCalModel->getRenderer();
   mMixer = mCalModel->getMixer();
}
