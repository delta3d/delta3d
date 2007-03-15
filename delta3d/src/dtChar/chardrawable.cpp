#include <osg/NodeCallback>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Timer>
#include <dtChar/submesh.h>
#include <dtChar/chardrawable.h>
#include <dtChar/coremodel.h>
#include <dtCore/system.h>
#include <dtUtil/log.h>
#include <cassert>

using namespace dtChar;

CharDrawable::CharDrawable()
   : mGeode(new osg::Geode)
   , mCalModel(NULL)   
{
   GetMatrixNode()->addChild(mGeode.get()); 
   AddSender(&dtCore::System::GetInstance());
}

CharDrawable::~CharDrawable()
{
   
}


void CharDrawable::OnMessage(Base::MessageData *data)
{
   // tick the animation
   if( data->message == "preframe" )
   {
      double dt = *static_cast<double*>(data->userData);      
      mCalModel->update(dt);
   }
}


void CharDrawable::Create(CoreModel *core) 
{
   // Create a new cal model and an associated update callback
   if (!mCalModel)
   {     
      mCalModel  = new CalModel(core->get());      
   }   

   osg::NodeCallback* nodeCallback = mGeode->getUpdateCallback();
   
   // attach all meshes to the model
   for(int meshId = 0; meshId < core->get()->getCoreMeshCount(); meshId++)
   {
      mCalModel->attachMesh(meshId);
   }

   mCalModel->setMaterialSet(0);

   CalRenderer *pCalRenderer = mCalModel->getRenderer();
   if(pCalRenderer->beginRendering()) 
   {
      int meshCount = pCalRenderer->getMeshCount();

      for(int meshId = 0; meshId < meshCount; meshId++) 
      {
         int submeshCount = pCalRenderer->getSubmeshCount(meshId);
      
         for(int submeshId = 0; submeshId < submeshCount; submeshId++) 
         {
            SubMesh *submesh = new SubMesh(mCalModel, meshId, submeshId);
            mGeode->addDrawable(submesh);
         }
      }
      pCalRenderer->endRendering();
   }

   /// Force generation of first mesh
   mCalModel->update(0);
   mCoreModel = core;
}

void CharDrawable::StartAction(unsigned id, float delay_in, float delay_out)
{
   assert(mCalModel);

   if (!mCalModel) 
   {
      LOG_ERROR("Error!  Trying to call StartAction without a valid model");   
      return;
   }

   if (mCalModel->getMixer() != 0)
   {
      mCalModel->getMixer()->executeAction(id, delay_in, delay_out);
   }
   else
   {
      LOG_ERROR("Model::action: ERROR: The model doesn't have a mixer");      
   }
}

void CharDrawable::StopAction(unsigned id) 
{
   assert(mCalModel);

   if (!mCalModel) 
   {
      LOG_ERROR("Error!  Trying to call StopAction without a valid model");    
      return;
   }

   if (mCalModel->getMixer() != 0)
   {
      mCalModel->getMixer()->removeAction(id);
   }
   else
   {
      LOG_ERROR("Model::action: ERROR: The model doesn't have a mixer");      
   }
}

void CharDrawable::StartLoop(unsigned id, float weight, float delay) 
{
   if (!mCalModel) 
   {
      LOG_ERROR("Error!  Trying to call StartLoop without a valid model");    
      return;
   }
   
   if (mCalModel->getMixer() != 0)
   {
      mCalModel->getMixer()->blendCycle(id, weight, delay);
   }
   else
   {
     LOG_ERROR("Model::action: ERROR: The model doesn't have a mixer");  
   }
}

void CharDrawable::StopLoop(unsigned id, float delay) 
{
   if (!mCalModel) 
   {
      LOG_ERROR("Error!  Trying to call StartLoop without a valid model");    
      return;
   }

   if (mCalModel->getMixer() != 0)
   {
      mCalModel->getMixer()->clearCycle(id, delay);
   }
   else
   {
      LOG_ERROR("Model::action: ERROR: The model doesn't have a mixer");  
   }
}
