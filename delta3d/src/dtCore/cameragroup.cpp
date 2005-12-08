#include <dtCore/cameragroup.h>
#include <dtCore/camera.h>
#include <cassert>

using namespace dtCore;

CameraGroup::CameraGroup()
{
}

CameraGroup::~CameraGroup()
{
}

bool CameraGroup::AddCamera( Camera* camera )
{
   // A 0x0 pointer should never be passed here.
   assert(camera);

   // Find/Create the FrameBin index in the map, insert the camera in the
   // bin's set, and return whether or the insertion was sucessful. It 
   // will only fail if the camera has already been added to that bin.
   return mFrameBinMap[ camera->GetFrameBin() ].insert(camera).second;
}
 
bool CameraGroup::RemoveCamera( Camera* camera )
{
   assert(camera);

   // Attempt to remove the camera from its FrameBin. This will
   // fail if the Camera has never bee added.
   return mFrameBinMap[ camera->GetFrameBin() ].erase(camera) != 0;
}

void CameraGroup::Frame()
{
   mRenderSurfaceSet.clear();

   // Loop all over map entries
   for(  IntCameraSetMap::iterator frameKeyIter = mFrameBinMap.begin();
         frameKeyIter != mFrameBinMap.end();
         ++frameKeyIter )
   {
      CameraSet& currentCameraSet = frameKeyIter->second;

      // For each bin number, loop over all cameras in the bin
      for(  CameraSet::iterator cameraIter = currentCameraSet.begin();
         cameraIter != currentCameraSet.end();
         ++cameraIter )
      {
         // Call Frame on each camera but do not swap buffers.
         (*cameraIter)->Frame(false);

         // Insert the associated RenderSurface into a set. This ensures there
         // are no duplicates.
         mRenderSurfaceSet.insert( (*cameraIter)->GetCamera()->getRenderSurface() );

      }
   }

   // Swap the buffers on each RenderSurface once.
   for(  RenderSurfaceSet::iterator renderSurfaceIter = mRenderSurfaceSet.begin();
         renderSurfaceIter != mRenderSurfaceSet.end();
         ++renderSurfaceIter )
   {
      (*renderSurfaceIter)->swapBuffers();
   }
}
