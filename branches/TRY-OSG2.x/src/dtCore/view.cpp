#include <prefix/dtcoreprefix-src.h>
#include <dtCore/view.h>
#include <dtCore/camera.h>
#include <dtCore/mouse.h>
#include <dtCore/keyboard.h>
#include <dtCore/scene.h>
#include <dtCore/keyboardmousehandler.h>
#include <dtCore/exceptionenum.h>
#include <dtUtil/exception.h>
#include <cassert>

#include <osgViewer/View>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(View)

View::View(const std::string& name, bool useSceneLight) :
   Base(name),
   mOsgViewerView(new osgViewer::View),
   mTargetFrameRate(60.0),
   mFrameBin(0)
{
   RegisterInstance(this);

   if (useSceneLight)
      mOsgViewerView->setLightingMode(osg::View::SKY_LIGHT);
   else
      mOsgViewerView->setLightingMode(osg::View::NO_LIGHT);
   
   CreateKeyboardMouseHandler();
}

View::View(osgViewer::View * view, const std::string& name, bool useSceneLight) :
   Base(name),
   mOsgViewerView(view),
   mTargetFrameRate(60.0),
   mFrameBin(0)
{
   assert(mOsgViewerView.get());
   
   RegisterInstance(this);
   
   if (useSceneLight)
      mOsgViewerView->setLightingMode(osg::View::SKY_LIGHT);
   else
      mOsgViewerView->setLightingMode(osg::View::NO_LIGHT);
   
   CreateKeyboardMouseHandler();
}

View::~View()
{
    DeregisterInstance(this);
}

////////////////////////////
bool View::AddSlave( Camera* camera )
{
   if( camera == NULL )
   {
      throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
         "Supplied dtCore::Camera is invalid", __FILE__, __LINE__);
   }
   
   if( camera->GetOsgCamera() == NULL )
   {
      throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
         "Supplied dtCore::Camera::GetOsgCamera() is invalid", __FILE__, __LINE__);
   }
   
   camera->SetView(this);
   mCameraSlave.insert(camera);
   
   return mOsgViewerView->addSlave(camera->GetOsgCamera()); 
}
////////////////////////////
bool View::RemoveSlave( Camera* camera )
{
   assert(camera);
   assert(camera->GetOsgCamera());
   
   mCameraSlave.erase(camera);
   camera->SetView(NULL);
   
   return (false);
}

////////////////////////////
void View::SetCamera( Camera* camera )
{
   if (mCamera == camera) return;
   
   if (mCamera.valid())
   {
      mCamera->SetView(NULL);
   }
   
   mCamera = camera;
   
   if (mCamera.valid())
   {
      mCamera->SetView(this);
      mOsgViewerView->setCamera(camera->GetOrCreateOsgCamera());
      mOsgViewerView->assignSceneDataToCameras();
   }
}

////////////////////////////
void View::SetScene(Scene *scene)
{   
   if (mScene == scene) return;
   
   if (mScene.valid())
   {
      mScene->RemoveView(this);
   }
   
   mScene = scene;
   
   if (mScene.valid())
   {
      mScene->AddView(this);
   
      mOsgViewerView->setSceneData(mScene->GetSceneNode());
   }
}


/////////////////////////////////////////////////
void View::SetMouse( Mouse* mouse )
{
   if( mouse == 0 )
   {
      throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
         "Supplied dtCore::Mouse is invalid", __FILE__, __LINE__);
   }

   mKeyboardMouseHandler->SetMouse(mouse);
}
///////////////////// 
void View::SetKeyboard( Keyboard* keyboard )
{
   if( keyboard == 0 )
   {
      throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
         "Supplied dtCore::Keyboard is invalid", __FILE__, __LINE__);
   }

   mKeyboardMouseHandler->SetKeyboard(keyboard);
}
///////////////////// 
Keyboard * View::GetOrCreateKeyboard() 
{ 
   return GetOrCreateKeyboardMouseHandler()->GetOrCreateKeyboard(); 
}
Keyboard* View::GetKeyboard() 
{ 
   return mKeyboardMouseHandler.valid() ? mKeyboardMouseHandler->GetKeyboard() : NULL; 
}
const Keyboard* View::GetKeyboard() const 
{ 
   return mKeyboardMouseHandler.valid() ? mKeyboardMouseHandler->GetKeyboard() : NULL; 
}
///////////////////// 
Mouse * View::GetOrCreateMouse() 
{ 
   return GetOrCreateKeyboardMouseHandler()->GetOrCreateMouse(); 
}
Mouse* View::GetMouse() 
{ 
   return mKeyboardMouseHandler.valid() ? mKeyboardMouseHandler->GetMouse() : NULL; 
}
const Mouse* View::GetMouse() const 
{ 
   return mKeyboardMouseHandler.valid() ? mKeyboardMouseHandler->GetMouse() : NULL; 
}

///////////////////// 
dtCore::Camera * View::CreateCamera()
{
    mCamera = new dtCore::Camera(this, "defaultCamera");
    mOsgViewerView->setCamera(mCamera->GetOsgCamera());
    return mCamera.get();
}

void View::UpdateFromScene()
{
   mOsgViewerView->setSceneData(mScene->GetSceneNode());
   if (mScene->IsPagingEnabled())
   {
      EnablePaging();      
   }
}
void View::UpdateFromCamera()
{
   mOsgViewerView->setCamera(mCamera->GetOsgCamera());
}

void View::UpdateFromSlave(Camera * camera, osg::Camera * oldOsgCam)
{  
   unsigned int i = mOsgViewerView->findSlaveIndexForCamera(oldOsgCam);
   mOsgViewerView->removeSlave(i);
   mOsgViewerView->addSlave(camera->GetOsgCamera());
}

dtCore::KeyboardMouseHandler * View::CreateKeyboardMouseHandler()
{
    mKeyboardMouseHandler = new dtCore::KeyboardMouseHandler(this);
    mOsgViewerView->addEventHandler(mKeyboardMouseHandler.get());
    return mKeyboardMouseHandler.get();
}


// ????
//void View::ResetCameraScenes(dtCore::Scene* sceneRootChanged)
//{
//   // Loop all over map entries
//   for(  IntCameraSetMap::iterator frameKeyIter = mFrameBinMap.begin();
//      frameKeyIter != mFrameBinMap.end();
//      ++frameKeyIter )
//   {
//      CameraSet& currentCameraSet = frameKeyIter->second;
//
//      // For each bin number, loop over all cameras in the bin
//      for(  CameraSet::iterator cameraIter = currentCameraSet.begin();
//         cameraIter != currentCameraSet.end();
//         ++cameraIter )
//      {
//         if((*cameraIter)->GetScene() == sceneRootChanged)
//         {
//            (*cameraIter)->SetScene(sceneRootChanged);
//         }
//      }
//
//   }
//
//}

//void View::Frame()
//{
//    mRenderSurfaceSet.clear();
//
//   // Loop all over map entries
//   for(  IntCameraSetMap::iterator frameKeyIter = mFrameBinMap.begin();
//         frameKeyIter != mFrameBinMap.end();
//         ++frameKeyIter )
//   {
//      CameraSet& currentCameraSet = frameKeyIter->second;
//
//      // For each bin number, loop over all cameras in the bin
//      for(  CameraSet::iterator cameraIter = currentCameraSet.begin();
//         cameraIter != currentCameraSet.end();
//         ++cameraIter )
//      {
//         // Call Frame on each camera but do not swap buffers.
//         (*cameraIter)->Frame(false);
//
//         // Insert the associated RenderSurface into a set. This ensures there
//         // are no duplicates.
//         mRenderSurfaceSet.insert( (*cameraIter)->GetCamera()->getRenderSurface() );
//
//      }
//   }
//
//   // Swap the buffers on each RenderSurface once.
//   for(  RenderSurfaceSet::iterator renderSurfaceIter = mRenderSurfaceSet.begin();
//         renderSurfaceIter != mRenderSurfaceSet.end();
//         ++renderSurfaceIter )
//   {
//      (*renderSurfaceIter)->swapBuffers();
//   }
//}


/////////////////////////////////////////////
void View::EnablePaging()
{
   if (mOsgViewerView.valid() == false)
   {
      return;
   }
   
   if (mOsgViewerView->getDatabasePager() == NULL)
   {
      osgDB::DatabasePager* databasePager = osgDB::DatabasePager::create();
      databasePager->setTargetFrameRate(mTargetFrameRate);
      databasePager->registerPagedLODs(mOsgViewerView->getSceneData());

      mOsgViewerView->setDatabasePager(databasePager);
   }
   else
   {
      LOG_ERROR("EnablePaging was called when paging was already enabled");
   }
}
/////////////////////////////////////////////
void View::DisablePaging()
{
   if (mOsgViewerView.valid() == false)
   {
      return;
   }
   
   if(mOsgViewerView->getDatabasePager())
   {
      mOsgViewerView->setDatabasePager(NULL);
   }
   else
   {
      LOG_ERROR("DisablePaging was called when paging wasn't enabled");
   }
}


