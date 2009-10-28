#include <dtCore/windowresizecontainer.h>
#include <dtCore/windowresizecallback.h>
#include <dtCore/deltawin.h>

//////////////////////////////////////////////////////////////////////////
dtCore::WindowResizeContainer::WindowResizeContainer(DeltaWin& win)
: mWindow(&win)
{
}

//////////////////////////////////////////////////////////////////////////
void dtCore::WindowResizeContainer::resizedImplementation(osg::GraphicsContext* gc, int x, int y, int width, int height)
{
   std::vector<RefPtr<WindowResizeCallback> >::const_iterator itr = mCallbacks.begin();

   while (itr != mCallbacks.end())
   {
      (*(*itr))(*mWindow, x, y, width, height);
      ++itr;
   }
}

//////////////////////////////////////////////////////////////////////////
dtCore::WindowResizeContainer::~WindowResizeContainer()
{
   mWindow = NULL;
   mCallbacks.clear();
}

//////////////////////////////////////////////////////////////////////////
void dtCore::WindowResizeContainer::AddCallback(WindowResizeCallback& cb)
{
   mCallbacks.push_back(&cb);
}

//////////////////////////////////////////////////////////////////////////
void dtCore::WindowResizeContainer::RemoveCallback(dtCore::WindowResizeCallback &cb)
{
   std::vector<RefPtr<WindowResizeCallback> >::iterator itr = mCallbacks.begin();

   while (itr != mCallbacks.end())
   {
      if ((*itr) == &cb)
      {
         mCallbacks.erase(itr);
         break;
      }
      ++itr;
   }
}

//////////////////////////////////////////////////////////////////////////
std::vector<dtCore::RefPtr<dtCore::WindowResizeCallback> > dtCore::WindowResizeContainer::GetCallbacks() const
{
   return mCallbacks;
}
