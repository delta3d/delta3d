#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Timer>

#include <dtAnim/submesh.h>
#include <dtAnim/chardrawable.h>
#include <dtAnim/coremodel.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/cal3dloader.h>
#include <dtCore/system.h>
#include <dtUtil/log.h>
#include <cassert>

#include <cal3d/corematerial.h>

using namespace dtAnim;

CharDrawable::CharDrawable()
   : mGeode(new osg::Geode)
   ,mCal3DWrapper(NULL)
{  

}

CharDrawable::~CharDrawable()
{
  
}


void CharDrawable::OnMessage(Base::MessageData *data)
{
   assert(mCal3DWrapper.get());

   // tick the animation
   if( data->message == "preframe" )
   {
      double dt = *static_cast<double*>(data->userData);      
      mCal3DWrapper->Update(dt);
   }
}


void CharDrawable::Create(const std::string &filename) 
{
   // Create a new cal model and an associated update callback

   Cal3DLoader loader;
   CalModel *model = loader.Load(filename);

   mCal3DWrapper = new Cal3DModelWrapper(model);

   if(mCal3DWrapper->BeginRenderingQuery()) 
   {
      int meshCount = mCal3DWrapper->GetMeshCount();

      for(int meshId = 0; meshId < meshCount; meshId++) 
      {
         int submeshCount = mCal3DWrapper->GetSubmeshCount(meshId);
      
         for(int submeshId = 0; submeshId < submeshCount; submeshId++) 
         {
            SubMeshDrawable *submesh = new SubMeshDrawable(mCal3DWrapper.get(), meshId, submeshId);
            mGeode->addDrawable(submesh);
         }
      }
      mCal3DWrapper->EndRenderingQuery();
   }


   /// Force generation of first mesh
   mCal3DWrapper->Update(0);

   GetMatrixNode()->addChild(mGeode.get()); 
   AddSender(&dtCore::System::GetInstance());
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
