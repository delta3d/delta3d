#include <osg/NodeCallback>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Timer>
#include <dtChar/submesh.h>
#include <dtChar/chardrawable.h>
#include <dtChar/coremodel.h>
#include <dtChar/cal3dwrapper.h>
#include <dtCore/system.h>
#include <dtUtil/log.h>
#include <cassert>

using namespace dtChar;

CharDrawable::CharDrawable()
   : mGeode(new osg::Geode)
   ,mCal3DWrapper(NULL)
{

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
      mCal3DWrapper->Update(dt);
   }
}


void CharDrawable::Create(CoreModel *core) 
{
   // Create a new cal model and an associated update callback

   CalModel *model  = new CalModel(core->get());
   mCal3DWrapper = new Cal3DWrapper(model);

   osg::NodeCallback* nodeCallback = mGeode->getUpdateCallback();
   
   // attach all meshes to the model
   for(int meshId = 0; meshId < core->get()->getCoreMeshCount(); meshId++)
   {
      mCal3DWrapper->AttachMesh(meshId);
   }

   mCal3DWrapper->SetMaterialSet(0);

   if(mCal3DWrapper->BeginRenderingQuery()) 
   {
      int meshCount = mCal3DWrapper->GetMeshCount();

      for(int meshId = 0; meshId < meshCount; meshId++) 
      {
         int submeshCount = mCal3DWrapper->GetSubmeshCount(meshId);
      
         for(int submeshId = 0; submeshId < submeshCount; submeshId++) 
         {
            SubMesh *submesh = new SubMesh(mCal3DWrapper.get(), meshId, submeshId);
            mGeode->addDrawable(submesh);
         }
      }
      mCal3DWrapper->EndRenderingQuery();
   }

   /// Force generation of first mesh
   mCal3DWrapper->Update(0);
   mCoreModel = core;

   GetMatrixNode()->addChild(mGeode.get()); 

}

void CharDrawable::StartAction(unsigned id, float delay_in, float delay_out)
{
   assert(mCal3DWrapper.get());

   if (!mCal3DWrapper) 
   {
      LOG_ERROR("Error!  Trying to call StartAction without a valid model");   
      return;
   }


   mCal3DWrapper->ExecuteAction(id, delay_in, delay_out);
}

void CharDrawable::StopAction(unsigned id) 
{
   assert(mCal3DWrapper.get());

   if (!mCal3DWrapper) 
   {
      LOG_ERROR("Error!  Trying to call StopAction without a valid model");    
      return;
   }


   mCal3DWrapper->RemoveAction(id);
}

void CharDrawable::StartLoop(unsigned id, float weight, float delay) 
{
   if (!mCal3DWrapper.get()) 
   {
      LOG_ERROR("Error!  Trying to call StartLoop without a valid model");    
      return;
   }
   

   mCal3DWrapper->BlendCycle(id, weight, delay);
}

void CharDrawable::StopLoop(unsigned id, float delay) 
{
   if (!mCal3DWrapper.get()) 
   {
      LOG_ERROR("Error!  Trying to call StartLoop without a valid model");    
      return;
   }

   mCal3DWrapper->ClearCycle(id, delay);
}
