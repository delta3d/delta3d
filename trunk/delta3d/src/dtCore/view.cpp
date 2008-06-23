#include <prefix/dtcoreprefix-src.h>
#include <dtCore/view.h>
#include <dtCore/camera.h>
#include <dtCore/mouse.h>
#include <dtCore/keyboard.h>
#include <dtCore/scene.h>
#include <dtCore/deltawin.h>
#include <dtCore/keyboardmousehandler.h>
#include <dtCore/exceptionenum.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/exception.h>
#include <dtUtil/configproperties.h>
#include <cassert>

#include <osgViewer/View>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(View)

const std::string View::DATABASE_PAGER_PRECOMPILE_OBJECTS("System.DatabasePager.PrecompileObjects");
const std::string View::DATABASE_PAGER_MAX_OBJECTS_TO_COMPILE_PER_FRAME("System.DatabasePager.MaxObjectsToCompilePerFrame");
const std::string View::DATABASE_PAGER_MIN_TIME_FOR_OBJECT_COMPILE("System.DatabasePager.MinObjectCompileTime");
const std::string View::DATABASE_PAGER_TARGET_FRAMERATE("System.DatabasePager.TargetFrameRate");
const std::string View::DATABASE_PAGER_DRAWABLE_POLICY("System.DatabasePager.DrawablePolicy");
const std::string View::DATABASE_PAGER_THREAD_PRIORITY("System.DatabasePager.ThreadPriority");
const std::string View::DATABASE_PAGER_EXPIRY_DELAY("System.DatabasePager.ExpiryDelay");

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
   assert(mOsgViewerView.valid());

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
      mScene->RemoveView(*this);
   }

   mScene = scene;

   if (mScene.valid())
   {
      mScene->AddView(*this);
      UpdateFromScene();
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
   else
   {
      DisablePaging();
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
static void ReadDBPagerConfig(osgDB::DatabasePager& pager, dtUtil::ConfigProperties* config)
{
   if (config == NULL)
   {
      return;
   }

   std::string value;

   value = config->GetConfigPropertyValue(View::DATABASE_PAGER_PRECOMPILE_OBJECTS);
   if (!value.empty())
   {
      bool precompile = dtUtil::ToType<bool>(value);
      pager.setDoPreCompile(precompile);
   }

   value = config->GetConfigPropertyValue(View::DATABASE_PAGER_MAX_OBJECTS_TO_COMPILE_PER_FRAME);
   if (!value.empty())
   {
      unsigned int maxNum = dtUtil::ToType<unsigned int>(value);
      //Can't be less than 1.  That doesn't make sense.
      maxNum = std::max(maxNum, 1U);
      pager.setMaximumNumOfObjectsToCompilePerFrame(maxNum);
   }

   value = config->GetConfigPropertyValue(View::DATABASE_PAGER_MIN_TIME_FOR_OBJECT_COMPILE);
   if (!value.empty())
   {
      float minTime = dtUtil::ToType<float>(value);
      pager.setMinimumTimeAvailableForGLCompileAndDeletePerFrame(minTime);
   }

   value = config->GetConfigPropertyValue(View::DATABASE_PAGER_TARGET_FRAMERATE);
   if (!value.empty())
   {
      double target = dtUtil::ToType<double>(value);
      pager.setTargetFrameRate(target);
   }
//   else if (dtCore::System::GetInstance().GetUsesFixedTimeStep())
//   {
//      pager.setTargetFrameRate(dtCore::System::GetInstance().GetFrameRate());
//   }

   value = config->GetConfigPropertyValue(View::DATABASE_PAGER_DRAWABLE_POLICY);
   if (!value.empty())
   {
      if (value == "DoNotModify")
      {
          pager.setDrawablePolicy(osgDB::DatabasePager::DO_NOT_MODIFY_DRAWABLE_SETTINGS);
      }
      else if (value == "DisplayList" || value == "DisplayLists" || value == "DL")
      {
          pager.setDrawablePolicy(osgDB::DatabasePager::USE_DISPLAY_LISTS);
      }
      else if (value == "VBO")
      {
          pager.setDrawablePolicy(osgDB::DatabasePager::USE_VERTEX_BUFFER_OBJECTS);
      }
      else if (value == "VertexArrays" || value == "VertexArray"||  value == "VA")
      {
          pager.setDrawablePolicy(osgDB::DatabasePager::USE_VERTEX_ARRAYS);
      }
   }

   value = config->GetConfigPropertyValue(View::DATABASE_PAGER_THREAD_PRIORITY);
   if (!value.empty())
   {
      if (value == "DEFAULT")
      {
          pager.setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_DEFAULT);
      }
      else if (value == "MIN")
      {
         pager.setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_MIN);
      }
      else if (value == "LOW")
      {
         pager.setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_LOW);
      }
      else if (value == "NOMINAL")
      {
         pager.setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_NOMINAL);
      }
      else if (value == "HIGH")
      {
         pager.setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_HIGH);
      } 
      else if (value == "MAX")
      {
         pager.setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_MAX);
      }
   }

   value = config->GetConfigPropertyValue(View::DATABASE_PAGER_EXPIRY_DELAY);
   if (!value.empty())
   {
      double delay = dtUtil::ToType<double>(value);
      pager.setExpiryDelay(delay);
   }
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
      ReadDBPagerConfig(*databasePager, GetScene()->GetConfiguration());
   }
   else
   {
      LOG_INFO("EnablePaging was called when paging was already enabled: Re-reading configuration");
      // If paging is already enabled, we need to re-read because we may have just assigned
      // a new scene to the view that has paging enabled, but different settings.
      ReadDBPagerConfig(*mOsgViewerView->getDatabasePager(), GetScene()->GetConfiguration());
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
      LOG_DEBUG("DisablePaging was called when paging wasn't enabled");
   }
}

//////////////////////////////////////////////////////////////////////////
bool View::GetMousePickPosition( osg::Vec3 &position, unsigned int traversalMask )
{
   const Mouse *mouse = GetMouse();
   if (mouse == NULL) return false;

   return GetPickPosition(position, mouse->GetPosition(), traversalMask);
}

//////////////////////////////////////////////////////////////////////////
bool View::GetPickPosition( osg::Vec3 &intersectionPoint, 
                           const osg::Vec2 &mousePos, 
                           unsigned int traversalMask )
{
   osgUtil::LineSegmentIntersector::Intersections hitList ;

   Camera *cam = GetCamera();
   if (cam == NULL)  return false;

   dtCore::DeltaWin *win = cam->GetWindow();
   if (win == NULL )
   {
      return false;
   }

   // lower left screen has ( 0, 0 )
   osg::Vec2 windowCoord( 0.0 , 0.0 ) ;
   win->CalcPixelCoords(mousePos, windowCoord );

   if( GetOsgViewerView()->computeIntersections( windowCoord.x(), windowCoord.y(),
                                                 hitList, traversalMask ) )
   {
      std::multiset< osgUtil::LineSegmentIntersector::Intersection >::iterator itr = hitList.begin() ;
      osgUtil::LineSegmentIntersector::Intersection hit = *itr ;

      intersectionPoint = hit.getWorldIntersectPoint() ;

      return true ;
   }
   else
   {
      return false ;
   }
}
