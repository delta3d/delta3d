#include <prefix/dtcoreprefix.h>
#include <dtCore/cameracallbackcontainer.h>
#include <dtCore/camera.h>
#include <dtCore/cameradrawcallback.h>
#include <iterator>

using namespace dtCore;

//////////////////////////////////////////////////////////////////////////
CameraCallbackContainer::CameraCallbackContainer(Camera& camera)
   : mCamera(&camera)
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
   // conceptually, this function should be locked because osg calls post draw callbacks on multiple threads
   // but since this is only every added to a single camera that is only used at the top level, then
   // it's only going to get called once per frame.
   std::vector<ObserverPtr<CameraDrawCallback> >::iterator itr = mCallbacks.begin();

   // Fire all recurring callbacks
   while (itr != mCallbacks.end())
   {
      if (itr->valid())
      {
         (*(*itr))(*mCamera, renderInfo);
         ++itr;
      }
      else
      {
         itr = mCallbacks.erase(itr);
      }
   }

   // Fire and remove all single shot callbacks
   if (!mSingleFireCallbacks.empty())
   {
      std::vector<RefPtr<CameraDrawCallback> >::const_iterator itr = mSingleFireCallbacks.begin();

      while (itr != mSingleFireCallbacks.end())
      {
         (*(*itr))(*mCamera, renderInfo);
         ++itr;
      }

      mSingleFireCallbacks.clear();
   }
}

//////////////////////////////////////////////////////////////////////////
void CameraCallbackContainer::AddCallback(CameraDrawCallback& cb, bool singleFire)
{
   if (!singleFire)
   {
      mCallbacks.push_back(&cb);
   }
   else
   {
      mSingleFireCallbacks.push_back(&cb);
   }
}

//////////////////////////////////////////////////////////////////////////
void CameraCallbackContainer::RemoveCallback(CameraDrawCallback &cb)
{
   std::vector<ObserverPtr<CameraDrawCallback> >::iterator itr = mCallbacks.begin();

   while (itr != mCallbacks.end())
   {
      if ((*itr) == &cb)
      {
         mCallbacks.erase(itr);
         break;
      }
      ++itr;
   }

   if (!mSingleFireCallbacks.empty())
   {
      std::vector<RefPtr<CameraDrawCallback> >::iterator itr = mSingleFireCallbacks.begin();

      while (itr != mSingleFireCallbacks.end())
      {
         if ((*itr) == &cb)
         {
            mSingleFireCallbacks.erase(itr);
            break;
         }
         ++itr;
      }
   }
}

//////////////////////////////////////////////////////////////////////////
std::vector<RefPtr<CameraDrawCallback> > CameraCallbackContainer::GetCallbacks() const
{
   typedef std::vector<RefPtr<CameraDrawCallback> > CallbackList;

   CallbackList allCallbacks;
   std::back_insert_iterator<CallbackList> cbIter(allCallbacks);

   std::vector<ObserverPtr<CameraDrawCallback> >::const_iterator itr = mCallbacks.begin();


   // Have to use a loop on these because the types don't quite match.
   while (itr != mCallbacks.end())
   {
      allCallbacks.push_back(itr->get());
      ++itr;
   }

   std::copy(mSingleFireCallbacks.begin(), mSingleFireCallbacks.end(), cbIter);

   return allCallbacks;
}
