#ifndef screenshotcallback_h__
#define screenshotcallback_h__

#include <dtCore/export.h>
#include <dtCore/cameradrawcallback.h>
#include <string>

namespace dtCore
{
   /** Used by the Camera to save a screen shot to the drive.
     */
   class DT_CORE_EXPORT ScreenShotCallback : public dtCore::CameraDrawCallback
   {
   public:
      ScreenShotCallback();

      void SetNameToOutput(const std::string& name);

      const std::string GetNameToOutput() const;;

      virtual void operator()(const dtCore::Camera &camera,
                              osg::RenderInfo& renderInfo) const;

   private:
      mutable bool  mTakeScreenShotNextFrame;
      std::string mNameOfScreenShotToOutput;
   };
}

#endif // screenshotcallback_h__

