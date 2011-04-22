#ifndef cameradrawcallback_h__
#define cameradrawcallback_h__

#include <osg/Referenced>

namespace osg
{
   class RenderInfo;
}

namespace dtCore
{
   class Camera;

   /** Abstract base class for Camera draw callbacks.  Derive and implement
     * the () operator.
     * @see dtCore::Camera::AddPostDrawCallback()
     */
   class CameraDrawCallback : public osg::Referenced
   {
   public:
      CameraDrawCallback() {};

      virtual void operator () (const dtCore::Camera& camera, osg::RenderInfo& renderInfo) const = 0;

   protected:
     virtual ~CameraDrawCallback() {};

   };
}
#endif // cameradrawcallback_h__