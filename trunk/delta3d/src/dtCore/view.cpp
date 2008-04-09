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

/////////////////////
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

/////////////////////
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

/////////////////////
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
   
   if( camera->GetOSGCamera() == NULL )
   {
      throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
         "Supplied dtCore::Camera::GetOsgCamera() is invalid", __FILE__, __LINE__);
   }
   
   mCameraSlave.insert(camera);
   
   return mOsgViewerView->addSlave(camera->GetOSGCamera()); 
}
////////////////////////////
bool View::RemoveSlave( Camera* camera )
{
   assert(camera);
   assert(camera->GetOSGCamera());
   
   return mCameraSlave.erase(camera) > 0;
}

////////////////////////////
void View::SetCamera( Camera* camera )
{
   if (mCamera == camera) return;
   
   mCamera = camera;
   
   if (mCamera.valid())
   {
      mOsgViewerView->setCamera(camera->GetOSGCamera());
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
Keyboard* View::GetKeyboard() 
{ 
   return mKeyboardMouseHandler.valid() ? mKeyboardMouseHandler->GetKeyboard() : NULL; 
}
/////////////////////
const Keyboard* View::GetKeyboard() const 
{ 
   return mKeyboardMouseHandler.valid() ? mKeyboardMouseHandler->GetKeyboard() : NULL; 
}
/////////////////////
Mouse* View::GetMouse() 
{ 
   return mKeyboardMouseHandler.valid() ? mKeyboardMouseHandler->GetMouse() : NULL; 
}

/////////////////////
const Mouse* View::GetMouse() const 
{ 
   return mKeyboardMouseHandler.valid() ? mKeyboardMouseHandler->GetMouse() : NULL; 
}

/////////////////////
void View::UpdateFromScene()
{
   mOsgViewerView->setSceneData(mScene->GetSceneNode());
   if (mScene->IsPagingEnabled())
   {
      EnablePaging();
   }
   mOsgViewerView->assignSceneDataToCameras();
}

///////////////////// 
dtCore::KeyboardMouseHandler * View::CreateKeyboardMouseHandler()
{
    mKeyboardMouseHandler = new dtCore::KeyboardMouseHandler(this);
    mOsgViewerView->addEventHandler(mKeyboardMouseHandler.get());
    return mKeyboardMouseHandler.get();
}


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



bool View::GetMousePickPosition( osg::Vec3 &position )
{
   osgUtil::LineSegmentIntersector::Intersections hitList ;

   const Camera *cam = GetCamera();
   if (cam == NULL)  return false;

   const osg::Viewport* vp = cam->GetOSGCamera()->getViewport();
   if (vp == NULL) return false;

   const float scr_width  = vp->width () ;
   const float scr_height = vp->height() ;

   const Mouse *mouse = GetMouse();
   if (mouse == NULL) return false;

   const osg::Vec2 pos = mouse->GetPosition() ;

   // lower left screen has ( 0, 0 )
   osg::Vec2 scr_map_coord( 0.0 , 0.0 ) ;
   scr_map_coord[ 0 ] =  0.5 * ( pos.x() + 1.0 ) * scr_width;
   scr_map_coord[ 1 ] =  0.5 * ( pos.y() + 1.0 ) * scr_height;


   if( GetOsgViewerView()->computeIntersections( scr_map_coord.x() , scr_map_coord.y() , hitList ) )
   {
      std::multiset< osgUtil::LineSegmentIntersector::Intersection >::iterator itr = hitList.begin() ;
      osgUtil::LineSegmentIntersector::Intersection hit = *itr ;
      
      position = hit.getWorldIntersectPoint() ;

      return true ;
   }
   else
   {
      return false ;
   }
}
