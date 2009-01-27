#include <dtCore/screenshotcallback.h>
#include <dtUtil/log.h>
#include <osg/RenderInfo>
#include <osgDB/WriteFile>

using namespace dtCore;

//////////////////////////////////////////////////////////////////////////
dtCore::ScreenShotCallback::ScreenShotCallback() 
: mTakeScreenShotNextFrame(false)
{

}

//////////////////////////////////////////////////////////////////////////
void dtCore::ScreenShotCallback::SetNameToOutput(const std::string& name)
{
   mTakeScreenShotNextFrame  = true;
   mNameOfScreenShotToOutput = name;
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ScreenShotCallback::operator()(const dtCore::Camera& camera,
                                            osg::RenderInfo& renderInfo) const
{
   if (mTakeScreenShotNextFrame)
   {
      mTakeScreenShotNextFrame = false;
      osg::ref_ptr<osg::Image> image = new osg::Image;

      const osg::Camera *camera = renderInfo.getCurrentCamera();
      if (camera == NULL)
      {
         return;
      }

      int x = static_cast<int>(camera->getViewport()->x());
      int y = static_cast<int>(camera->getViewport()->y());
      unsigned int width = static_cast<unsigned int>(camera->getViewport()->width());
      unsigned int height = static_cast<unsigned int>(camera->getViewport()->height());

      image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
      image->readPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE);
      bool status = osgDB::writeImageFile(*image.get(), mNameOfScreenShotToOutput.c_str()); // jpg, rgb, png, bmp
      if (status == false)
      {
         LOG_ERROR("Can't write out screenshot file: " + mNameOfScreenShotToOutput +
                    ". Does the osgDB plugin exist?");
      }
   }
}

//////////////////////////////////////////////////////////////////////////
const std::string dtCore::ScreenShotCallback::GetNameToOutput() const
{
   return mNameOfScreenShotToOutput;
}
