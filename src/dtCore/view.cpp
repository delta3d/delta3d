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
#include <dtCore/refptr.h>
#include <dtCore/databasepager.h>

#include <cassert>
#include <osgUtil/LineSegmentIntersector>

#include <osgViewer/View>

#include <map>
#include <set>

namespace dtCore
{

class ViewImpl
{
public:
   ViewImpl(osgViewer::View& view)
      : mOsgViewerView(&view)
      , mTargetFrameRate(60.0)
      , mRenderOrder(0)
      , mPager(NULL)
   {
      assert(mOsgViewerView.valid());
   }

   KeyboardMouseHandler* GetKeyboardMouseHandler() { return mKeyboardMouseHandler.get(); }

   ///Get the list of Intersections given the mouse position and traversal mask
   bool GetMouseIntersections(osgUtil::LineSegmentIntersector::Intersections& hitList,
                              const osg::Vec2& mousePos,
                              unsigned int traversalMask);

   typedef std::set< dtCore::RefPtr<dtCore::Camera> > CameraSet;
   /// set of osg::Camera wrapping the mView camera slave list
   CameraSet mCameraSlave;

   /// osgViewer::View wrapped
   RefPtr<osgViewer::View> mOsgViewerView;

   /// osg::Camera wrapping the mView master camera
   RefPtr<dtCore::Camera> mCamera;
   /// osgViewer::Scene wrapping the mView scene
   RefPtr<dtCore::Scene> mScene;

   /// KeyboardMouseHandler to handle osgViewer::View event, intern variable provide access to Mouse and Keyboard
   RefPtr<dtCore::KeyboardMouseHandler> mKeyboardMouseHandler;

   double mTargetFrameRate;
   unsigned int mRenderOrder;  //higher numbers get rendered FIRST

   RefPtr<DatabasePager> mPager; ///< The pager this View will use (could be NULL)
};

////////////////////////////////////////////////////////////////////////////////
bool ViewImpl::GetMouseIntersections(osgUtil::LineSegmentIntersector::Intersections& hitList,
                                     const osg::Vec2& mousePos,
                                     unsigned int traversalMask)
{
   bool success = false;

   if (mCamera.valid() && mCamera->GetWindow() != NULL)
   {
      // lower left screen has (0, 0)
      osg::Vec2 windowCoord(0.0 , 0.0);
      mCamera->GetWindow()->CalcPixelCoords(mousePos, windowCoord);

      if (mOsgViewerView->computeIntersections(windowCoord.x(), windowCoord.y(),
         hitList, traversalMask))
      {
         success = true;
      }
   }

   return success;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_MANAGEMENT_LAYER(View)

////////////////////////////////////////////////////////////////////////////////
View::View(const std::string& name, bool useSceneLight)
   : Base(name)
   , mImpl(new ViewImpl(*new osgViewer::View))
{
   Ctor(useSceneLight);
}

////////////////////////////////////////////////////////////////////////////////
View::View(osgViewer::View* view, const std::string& name, bool useSceneLight)
   : Base(name)
   , mImpl(new ViewImpl(*view))
{
   Ctor(useSceneLight);
}

////////////////////////////////////////////////////////////////////////////////
void View::Ctor(bool useSceneLight)
{
   RegisterInstance(this);

   if (useSceneLight)
   {
      mImpl->mOsgViewerView->setLightingMode(osg::View::SKY_LIGHT);
   }
   else
   {
      mImpl->mOsgViewerView->setLightingMode(osg::View::NO_LIGHT);
   }

   CreateKeyboardMouseHandler();

   if (mImpl->mOsgViewerView->getDatabasePager() != NULL)
   {
      SetDatabasePager(new DatabasePager(*mImpl->mOsgViewerView->getDatabasePager()));
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
   delete mImpl;
   mImpl = NULL;
}

////////////////////////////////////////////////////////////////////////////////
DEPRECATE_FUNC void View::SetFrameBin( unsigned int frameBin )
{
   DEPRECATE("View::SetFrameBin", "View::SetRenderOrder");

   SetRenderOrder(frameBin);
}

////////////////////////////////////////////////////////////////////////////////
DEPRECATE_FUNC unsigned int View::GetFrameBin() const
{
   DEPRECATE("View::GetFrameBin", "View::GetRenderOrder");

   return GetRenderOrder();
}

////////////////////////////////////////////////////////////////////////////////
void View::SetRenderOrder(unsigned int rOrder)
{
   mImpl->mRenderOrder = rOrder;
   GetCamera()->GetOSGCamera()->setRenderOrder(osg::Camera::NESTED_RENDER, rOrder);
}

////////////////////////////////////////////////////////////////////////////////
unsigned int View::GetRenderOrder() const
{
   return mImpl->mRenderOrder;
}

////////////////////////////////////////////////////////////////////////////////
bool View::AddSlave(Camera* camera)
{
   if (camera == NULL)
   {
      throw dtUtil::Exception(ExceptionEnum::INVALID_PARAMETER,
         "Supplied dtCore::Camera is invalid", __FILE__, __LINE__);
   }

   if (camera->GetOSGCamera() == NULL)
   {
      throw dtUtil::Exception(ExceptionEnum::INVALID_PARAMETER,
         "Supplied dtCore::Camera::GetOsgCamera() is invalid", __FILE__, __LINE__);
   }

   mImpl->mCameraSlave.insert(camera);

   return mImpl->mOsgViewerView->addSlave(camera->GetOSGCamera());
}

////////////////////////////////////////////////////////////////////////////////
bool View::RemoveSlave(Camera* camera)
{
   assert(camera);
   assert(camera->GetOSGCamera());

   return mImpl->mCameraSlave.erase(camera) > 0;
}

////////////////////////////////////////////////////////////////////////////////
void View::SetCamera(Camera* camera)
{
   if (mImpl->mCamera != camera)
   {
      mImpl->mCamera = camera;

      if (mImpl->mCamera.valid())
      {
         mImpl->mOsgViewerView->setCamera(camera->GetOSGCamera());
         mImpl->mOsgViewerView->assignSceneDataToCameras();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void View::SetScene(Scene* scene)
{
   if (mImpl->mScene != scene)
   {
      if (mImpl->mScene.valid())
      {
         mImpl->mScene->RemoveView(*this);
         mImpl->mScene->SetDatabasePager(NULL);
      }

      mImpl->mScene = scene;

      if (mImpl->mScene.valid())
      {
         mImpl->mScene->AddView(*this);
         mImpl->mScene->SetDatabasePager(mImpl->mPager.get());
         UpdateFromScene();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
Camera* View::GetCamera()
{
   return (mImpl->mCamera.get());
}

////////////////////////////////////////////////////////////////////////////////
const Camera* View::GetCamera() const
{
   return (mImpl->mCamera.get());
}

////////////////////////////////////////////////////////////////////////////////
Scene* View::GetScene()
{
   return (mImpl->mScene.get());
}

////////////////////////////////////////////////////////////////////////////////
const Scene* View::GetScene() const
{
   return (mImpl->mScene.get());
}

////////////////////////////////////////////////////////////////////////////////
osgViewer::View* View::GetOsgViewerView()
{
   return mImpl->mOsgViewerView.get();
}

////////////////////////////////////////////////////////////////////////////////
const osgViewer::View* View::GetOsgViewerView() const
{
   return mImpl->mOsgViewerView.get();
}

////////////////////////////////////////////////////////////////////////////////
void View::SetMouse(Mouse* mouse)
{
   mImpl->mKeyboardMouseHandler->SetMouse(mouse);
}

////////////////////////////////////////////////////////////////////////////////
void View::SetKeyboard(Keyboard* keyboard)
{
   mImpl->mKeyboardMouseHandler->SetKeyboard(keyboard);
}

////////////////////////////////////////////////////////////////////////////////
Keyboard* View::GetKeyboard()
{
   return mImpl->mKeyboardMouseHandler.valid() ? mImpl->mKeyboardMouseHandler->GetKeyboard() : NULL;
}

////////////////////////////////////////////////////////////////////////////////
const Keyboard* View::GetKeyboard() const
{
   return mImpl->mKeyboardMouseHandler.valid() ? mImpl->mKeyboardMouseHandler->GetKeyboard() : NULL;
}

////////////////////////////////////////////////////////////////////////////////
Mouse* View::GetMouse()
{
   return mImpl->mKeyboardMouseHandler.valid() ? mImpl->mKeyboardMouseHandler->GetMouse() : NULL;
}

////////////////////////////////////////////////////////////////////////////////
const Mouse* View::GetMouse() const
{
   return mImpl->mKeyboardMouseHandler.valid() ? mImpl->mKeyboardMouseHandler->GetMouse() : NULL;
}

////////////////////////////////////////////////////////////////////////////////
void View::UpdateFromScene()
{
   mImpl->mOsgViewerView->setSceneData(mImpl->mScene->GetSceneNode());
   mImpl->mOsgViewerView->assignSceneDataToCameras();
}

////////////////////////////////////////////////////////////////////////////////
bool View::GetMousePickPosition(osg::Vec3& position, unsigned int traversalMask)
{
   const Mouse* mouse = GetMouse();
   bool picked = false;

   if (mouse != NULL)
   {
      picked = GetPickPosition(position, mouse->GetPosition(), traversalMask);
   }

   return picked;
}

////////////////////////////////////////////////////////////////////////////////
bool View::GetPickPosition(osg::Vec3& intersectionPoint,
                           const osg::Vec2& mousePos,
                           unsigned int traversalMask)
{
   osgUtil::LineSegmentIntersector::Intersections hitList;

   bool success = false;

   if (mImpl->GetMouseIntersections(hitList, mousePos, traversalMask))
   {
      std::multiset<osgUtil::LineSegmentIntersector::Intersection>::iterator itr = hitList.begin();
      osgUtil::LineSegmentIntersector::Intersection hit = *itr;

      intersectionPoint = hit.getWorldIntersectPoint();

      success = true;
   }

   return success;
}

////////////////////////////////////////////////////////////////////////////////
DeltaDrawable* View::GetMousePickedObject(unsigned int traversalMask)
{
   DeltaDrawable* result = NULL;
   const Mouse* mouse = GetMouse();

   if (mouse != NULL)
   {
      const osg::Vec2& mousePos = mouse->GetPosition();
      result = GetPickedObject(mousePos, traversalMask);
   }

   return result;
}

////////////////////////////////////////////////////////////////////////////////
DeltaDrawable* View::GetPickedObject(const osg::Vec2& mousePos, unsigned int traversalMask)
{
   Scene* scene = GetScene();
   if (scene == NULL) { return NULL; }

   osgUtil::LineSegmentIntersector::Intersections hitList;

   if (mImpl->GetMouseIntersections(hitList, mousePos, traversalMask) == false)
   {
      return NULL;
   }

   // For every isector hit, loop through all of the Nodes in it's NodePath and
   // try to find a corresponding DeltaDrawable that's in the Scene.
   // Will return back the first match found.
   // Note: using a reverse iterator to loop through the NodePath.  This should cause
   // it to return the child-most DeltaDrawable and not the DeltaDrawable parents.
   std::vector<DeltaDrawable*> drawables = scene->GetAllDrawablesInTheScene();

   for (std::multiset<osgUtil::LineSegmentIntersector::Intersection>::const_iterator hitItr = hitList.begin();
        hitItr != hitList.end();
        ++hitItr)
   {
      for (osg::NodePath::const_reverse_iterator nodeItr = hitItr->nodePath.rbegin();
           nodeItr != hitItr->nodePath.rend();
           ++nodeItr)
      {
         for (std::vector<DeltaDrawable*>::iterator drawableItr = drawables.begin();
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

   // nothing hit or no match found, return NULL
   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::View::SetDatabasePager(DatabasePager* pager)
{
   if (mImpl->mPager.get() != pager)
   {
      mImpl->mPager = pager;

      if (mImpl->mPager.valid())
      {
         mImpl->mOsgViewerView->setDatabasePager(mImpl->mPager->GetOsgDatabasePager());
      }
      else
      {
         mImpl->mOsgViewerView->setDatabasePager(NULL);
      }


      if (mImpl->mScene.valid())
      {
         mImpl->mScene->SetDatabasePager(mImpl->mPager.get());
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
DatabasePager* dtCore::View::GetDatabasePager()
{
   return mImpl->mPager.get();
}

////////////////////////////////////////////////////////////////////////////////
const DatabasePager* dtCore::View::GetDatabasePager() const
{
   return mImpl->mPager.get();
}

////////////////////////////////////////////////////////////////////////////////
KeyboardMouseHandler* View::CreateKeyboardMouseHandler()
{
    mImpl->mKeyboardMouseHandler = new dtCore::KeyboardMouseHandler(this);
    mImpl->mOsgViewerView->addEventHandler(mImpl->mKeyboardMouseHandler.get());
    return mImpl->mKeyboardMouseHandler.get();
}

////////////////////////////////////////////////////////////////////////////////
}
