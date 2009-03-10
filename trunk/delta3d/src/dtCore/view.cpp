#include <prefix/dtcoreprefix-src.h>
#include <dtCore/view.h>
#include <dtCore/camera.h>
#include <dtCore/mouse.h>
#include <dtCore/keyboard.h>
#include <dtCore/scene.h>
#include <dtCore/deltawin.h>
#include <dtCore/keyboardmousehandler.h>
#include <dtCore/exceptionenum.h>
#include <dtUtil/exception.h>
#include <cassert>
#include <osgUtil/LineSegmentIntersector>

#include <osgViewer/View>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(View)


////////////////////////////////////////////////////////////////////////////////
View::View(const std::string& name, bool useSceneLight)
   : Base(name)
   , mOsgViewerView(new osgViewer::View)
   , mTargetFrameRate(60.0)
   , mFrameBin(0)
   , mPager(NULL)
{
   Ctor(useSceneLight);
}

////////////////////////////////////////////////////////////////////////////////
View::View(osgViewer::View* view, const std::string& name, bool useSceneLight)
   : Base(name)
   , mOsgViewerView(view)
   , mTargetFrameRate(60.0)
   , mFrameBin(0)
   , mPager(NULL)
{
   assert(mOsgViewerView.valid());
   Ctor(useSceneLight);
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::View::Ctor(bool useSceneLight)
{
   RegisterInstance(this);

   if (useSceneLight)
   {
      mOsgViewerView->setLightingMode(osg::View::SKY_LIGHT);
   }
   else
   {
      mOsgViewerView->setLightingMode(osg::View::NO_LIGHT);
   }

   CreateKeyboardMouseHandler();

   if (mOsgViewerView->getDatabasePager() != NULL)
   {
      SetDatabasePager(new DatabasePager(*mOsgViewerView->getDatabasePager()));
   }
   else
   {
      SetDatabasePager(new DatabasePager());
   }
}

////////////////////////////////////////////////////////////////////////////////
View::~View()
{
    DeregisterInstance(this);
}

////////////////////////////////////////////////////////////////////////////////
void View::SetFrameBin( unsigned int frameBin )
{
   mFrameBin = frameBin;
   
   GetCamera()->GetOSGCamera()->setRenderOrder(osg::Camera::NESTED_RENDER, frameBin);
}

////////////////////////////////////////////////////////////////////////////////
bool View::AddSlave(Camera* camera)
{
   if (camera == NULL)
   {
      throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
         "Supplied dtCore::Camera is invalid", __FILE__, __LINE__);
   }

   if (camera->GetOSGCamera() == NULL)
   {
      throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
         "Supplied dtCore::Camera::GetOsgCamera() is invalid", __FILE__, __LINE__);
   }

   mCameraSlave.insert(camera);

   return mOsgViewerView->addSlave(camera->GetOSGCamera());
}

////////////////////////////////////////////////////////////////////////////////
bool View::RemoveSlave(Camera* camera)
{
   assert(camera);
   assert(camera->GetOSGCamera());

   return mCameraSlave.erase(camera) > 0;
}

////////////////////////////////////////////////////////////////////////////////
void View::SetCamera(Camera* camera)
{
   if (mCamera == camera)
   {
      return;
   }

   mCamera = camera;

   if (mCamera.valid())
   {
      mOsgViewerView->setCamera(camera->GetOSGCamera());
      mOsgViewerView->assignSceneDataToCameras();
   }
}

////////////////////////////////////////////////////////////////////////////////
void View::SetScene(Scene* scene)
{
   if (mScene == scene)
   {
      return;
   }

   if (mScene.valid())
   {
      mScene->RemoveView(*this);
      mScene->SetDatabasePager(NULL);
   }

   mScene = scene;

   if (mScene.valid())
   {
      mScene->AddView(*this);
      mScene->SetDatabasePager(mPager.get());
      UpdateFromScene();
   }
}

////////////////////////////////////////////////////////////////////////////////
void View::SetMouse(Mouse* mouse)
{
   if (mouse == 0)
   {
      throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
         "Supplied dtCore::Mouse is invalid", __FILE__, __LINE__);
   }

   mKeyboardMouseHandler->SetMouse(mouse);
}

////////////////////////////////////////////////////////////////////////////////
void View::SetKeyboard(Keyboard* keyboard)
{
   if (keyboard == 0)
   {
      throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
         "Supplied dtCore::Keyboard is invalid", __FILE__, __LINE__);
   }

   mKeyboardMouseHandler->SetKeyboard(keyboard);
}

////////////////////////////////////////////////////////////////////////////////
Keyboard* View::GetKeyboard()
{
   return mKeyboardMouseHandler.valid() ? mKeyboardMouseHandler->GetKeyboard() : NULL;
}

////////////////////////////////////////////////////////////////////////////////
const Keyboard* View::GetKeyboard() const
{
   return mKeyboardMouseHandler.valid() ? mKeyboardMouseHandler->GetKeyboard() : NULL;
}

////////////////////////////////////////////////////////////////////////////////
Mouse* View::GetMouse()
{
   return mKeyboardMouseHandler.valid() ? mKeyboardMouseHandler->GetMouse() : NULL;
}

////////////////////////////////////////////////////////////////////////////////
const Mouse* View::GetMouse() const
{
   return mKeyboardMouseHandler.valid() ? mKeyboardMouseHandler->GetMouse() : NULL;
}

////////////////////////////////////////////////////////////////////////////////
void View::UpdateFromScene()
{
   mOsgViewerView->setSceneData(mScene->GetSceneNode());
   mOsgViewerView->assignSceneDataToCameras();
}

////////////////////////////////////////////////////////////////////////////////
dtCore::KeyboardMouseHandler* View::CreateKeyboardMouseHandler()
{
    mKeyboardMouseHandler = new dtCore::KeyboardMouseHandler(this);
    mOsgViewerView->addEventHandler(mKeyboardMouseHandler.get());
    return mKeyboardMouseHandler.get();
}

////////////////////////////////////////////////////////////////////////////////
bool View::GetMousePickPosition(osg::Vec3& position, unsigned int traversalMask)
{
   const Mouse* mouse = GetMouse();

   if (mouse == NULL)
   {
      return false;
   }

   return GetPickPosition(position, mouse->GetPosition(), traversalMask);
}

////////////////////////////////////////////////////////////////////////////////
bool View::GetPickPosition(osg::Vec3& intersectionPoint,
                           const osg::Vec2& mousePos,
                           unsigned int traversalMask)
{
   osgUtil::LineSegmentIntersector::Intersections hitList;

   if (GetMouseIntersections(hitList, mousePos, traversalMask))
   {
      std::multiset<osgUtil::LineSegmentIntersector::Intersection>::iterator itr = hitList.begin();
      osgUtil::LineSegmentIntersector::Intersection hit = *itr;

      intersectionPoint = hit.getWorldIntersectPoint();

      return true;
   }
   else
   {
      return false;
   }
}

////////////////////////////////////////////////////////////////////////////////
bool dtCore::View::GetMouseIntersections(osgUtil::LineSegmentIntersector::Intersections& hitList,
                                         const osg::Vec2& mousePos,
                                         unsigned int traversalMask)
{
   if (GetCamera() == NULL)
   {
      return false;
   }

   if (GetCamera()->GetWindow() == NULL)
   {
      return false;
   }

   // lower left screen has (0, 0)
   osg::Vec2 windowCoord(0.0 , 0.0);
   GetCamera()->GetWindow()->CalcPixelCoords(mousePos, windowCoord);

   if (GetOsgViewerView()->computeIntersections(windowCoord.x(), windowCoord.y(),
                                                hitList, traversalMask))
   {
      return true;
   }
   else
   {
      return false;
   }
}

////////////////////////////////////////////////////////////////////////////////
dtCore::DeltaDrawable* View::GetMousePickedObject(unsigned int traversalMask)
{
   const Mouse* mouse = GetMouse();
   if (mouse == NULL) { return NULL; }
   return GetPickedObject(mouse->GetPosition(), traversalMask);
}

////////////////////////////////////////////////////////////////////////////////
dtCore::DeltaDrawable* View::GetPickedObject(const osg::Vec2& mousePos, unsigned int traversalMask)
{
   dtCore::Scene* scene = GetScene();
   if (scene == NULL) { return NULL; }

   osgUtil::LineSegmentIntersector::Intersections hitList ;

   if (GetMouseIntersections(hitList, mousePos, traversalMask) == false)
   {
      return NULL;
   }

   //For every isector hit, loop through all of the Nodes in it's NodePath and
   //try to find a corresponding DeltaDrawable that's in the Scene.
   //Will return back the first match found.
   //Note: using a reverse iterator to loop through the NodePath.  This should cause
   //it to return the child-most DeltaDrawable and not the DeltaDrawable parents.
   std::vector<dtCore::DeltaDrawable*> drawables = scene->GetAllDrawablesInTheScene();

   for (std::multiset<osgUtil::LineSegmentIntersector::Intersection>::const_iterator hitItr = hitList.begin();
        hitItr != hitList.end();
        ++hitItr)
   {
      for (osg::NodePath::const_reverse_iterator nodeItr = hitItr->nodePath.rbegin();
           nodeItr != hitItr->nodePath.rend();
           ++nodeItr)
      {
         for (std::vector<dtCore::DeltaDrawable*>::iterator drawableItr = drawables.begin();
              drawableItr != drawables.end();
              ++drawableItr)
         {
            if ((*nodeItr) == (*drawableItr)->GetOSGNode())
            {
               return (*drawableItr);
            }
         }
      }
   }

   //nothing hit or no match found, return NULL
   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::View::SetDatabasePager(dtCore::DatabasePager* pager)
{
   if (mPager.get() == pager)
   {
      return;
   }

   mPager = pager;

   if (mPager.valid())
   {
      mOsgViewerView->setDatabasePager(mPager->GetOsgDatabasePager());
   }
   else
   {
      mOsgViewerView->setDatabasePager(NULL);
   }


   if (mScene.valid())
   {
      mScene->SetDatabasePager(mPager.get());
   }
}

////////////////////////////////////////////////////////////////////////////////
dtCore::DatabasePager* dtCore::View::GetDatabasePager()
{
   return mPager.get();
}

////////////////////////////////////////////////////////////////////////////////
const dtCore::DatabasePager* dtCore::View::GetDatabasePager() const
{
   return mPager.get();
}

////////////////////////////////////////////////////////////////////////////////
