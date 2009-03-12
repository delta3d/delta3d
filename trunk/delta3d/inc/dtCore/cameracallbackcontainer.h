#ifndef cameracallbackcontainer_h__
#define cameracallbackcontainer_h__

#include <dtCore/export.h>
#include <dtCore/refptr.h>
#include <osg/Camera>
#include <dtCore/observerptr.h>

#include <vector>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class RenderInfo;
}
/// @endcond

namespace dtCore
{
   class Camera;
   class CameraDrawCallback;

   /** Class used to manage a container of CameraDrawCallbacks.  Used
     * by the Camera to allow for multiple camera draw callbacks.
     */
   class DT_CORE_EXPORT CameraCallbackContainer : public osg::Camera::DrawCallback
   {
   public:
      CameraCallbackContainer(dtCore::Camera& camera);

      virtual void operator() (osg::RenderInfo& renderInfo) const;

      /** Add a CameraDrawCallback to the container.  Callbacks will be 
        * triggered in the order they are added.
        * @param cb The callback to add to this container.
        */
      void AddCallback(dtCore::CameraDrawCallback& cb);

      /** Remove an existing CameraDrawCallback from the container.
        * @param cb The Callback to remove from this container.
        */
      void RemoveCallback(dtCore::CameraDrawCallback &cb);

      /** Get a copy of all the callbacks in this container.
       * @return a copy of the internal Callback container
       */
      std::vector<RefPtr<CameraDrawCallback> > GetCallbacks() const;     

   protected:
      virtual ~CameraCallbackContainer();

      dtCore::ObserverPtr<dtCore::Camera> mCamera; ///<observer pointer to avoid circular dependency
   
   private:
      std::vector<RefPtr<CameraDrawCallback> > mCallbacks;
   };
}
#endif // cameracallbackcontainer_h__
