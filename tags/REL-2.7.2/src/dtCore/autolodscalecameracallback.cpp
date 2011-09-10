#include <prefix/dtcoreprefix.h>
#include <dtCore/autolodscalecameracallback.h>
#include <dtCore/camera.h>
#include <dtCore/system.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/log.h>
#include <osg/Math>

using namespace dtCore;

//////////////////////////////////////////////////////////////
AutoLODScaleCameraCallback::AutoLODScaleCameraCallback(dtCore::Camera* camera)
: mTargetFrameTimeMS(33.333)
, mTargetFrameTimeEpsilon(0.3)
, mMinLODScale(1.0f)
, mMaxLODScale(2.0f)
, mChangeFactor(0.001f)
, mCamera(camera)
, mCameraWasNull(camera == NULL)
{
}

//////////////////////////////////////////////////////////////
AutoLODScaleCameraCallback::~AutoLODScaleCameraCallback()
{
}

//////////////////////////////////////////////////////////////
void AutoLODScaleCameraCallback::Update(Camera& camera)
{
   if (mCameraWasNull || mCamera == &camera)
   {
      double frameTime = GetFrameTimeMS();
      // Check to see if they are different by more than a tenth of a millisecond
      if (!osg::equivalent(mTargetFrameTimeMS, frameTime, mTargetFrameTimeEpsilon))
      {
         float oldScale = camera.GetLODScale();
         float newScale = oldScale;

         if (frameTime < mTargetFrameTimeMS)
         {
            newScale = dtUtil::Max(camera.GetLODScale() / (1 + mChangeFactor), mMinLODScale);
         }
         else // frame time is greater
         {
            newScale = dtUtil::Min(camera.GetLODScale() * (1 + mChangeFactor), mMaxLODScale);
         }

         if (!dtUtil::Equivalent(oldScale, newScale))
         {
            dtUtil::Log& log = dtUtil::Log::GetInstance("camera.cpp");
            if (log.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               log.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "Auto-LOD: Changing LOD scale from \"%f\" to \"%f\".  FrameTime \"%lf\" ", oldScale, newScale, frameTime);
            }
            camera.SetLODScale(newScale);
         }
      }
   }
}

//////////////////////////////////////////////////////////////
double AutoLODScaleCameraCallback::GetFrameTimeMS() const
{
   return dtCore::System::GetInstance().GetSystemStageTime(System::STAGES_ALL);
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
DT_IMPLEMENT_ACCESSOR(AutoLODScaleCameraCallback, double, TargetFrameTimeMS);
DT_IMPLEMENT_ACCESSOR(AutoLODScaleCameraCallback, double, TargetFrameTimeEpsilon)
DT_IMPLEMENT_ACCESSOR(AutoLODScaleCameraCallback, float, MinLODScale);
DT_IMPLEMENT_ACCESSOR(AutoLODScaleCameraCallback, float, MaxLODScale);
DT_IMPLEMENT_ACCESSOR(AutoLODScaleCameraCallback, float, ChangeFactor);
