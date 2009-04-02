#include <dtCore/cameracallbackcontainer.h>
#include <dtCore/camera.h>
#include <dtCore/cameradrawcallback.h>

using namespace dtCore;

//////////////////////////////////////////////////////////////////////////
CameraCallbackContainer::CameraCallbackContainer(Camera& camera):
mCamera(&camera)
{

}

//////////////////////////////////////////////////////////////////////////
CameraCallbackContainer::~CameraCallbackContainer()
{
   mCamera = NULL;
   mCallbacks.clear();
}

//////////////////////////////////////////////////////////////////////////
void CameraCallbackContainer::operator()(osg::RenderInfo& renderInfo) const
{
   std::vector<RefPtr<CameraDrawCallback> >::const_iterator itr = mCallbacks.begin();

   while (itr != mCallbacks.end())
   {
      (*(*itr))(*mCamera, renderInfo);
      ++itr;
   }
}

//////////////////////////////////////////////////////////////////////////
void CameraCallbackContainer::AddCallback(CameraDrawCallback& cb)
{
   mCallbacks.push_back(&cb);
}

//////////////////////////////////////////////////////////////////////////
void CameraCallbackContainer::RemoveCallback(CameraDrawCallback &cb)
{
   std::vector<RefPtr<CameraDrawCallback> >::iterator itr = mCallbacks.begin();

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
std::vector<RefPtr<CameraDrawCallback> > CameraCallbackContainer::GetCallbacks() const
{
   return mCallbacks;
}
