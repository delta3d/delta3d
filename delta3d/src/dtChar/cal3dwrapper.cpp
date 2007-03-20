
#include <dtChar/Cal3DWrapper.h>
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