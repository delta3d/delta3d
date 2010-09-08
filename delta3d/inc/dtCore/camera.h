/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#ifndef DELTA_CAMERA
#define DELTA_CAMERA

// camera.h: interface for the Camera class.
//
//////////////////////////////////////////////////////////////////////


#include <dtUtil/generic.h>
#include <dtCore/refptr.h>
#include <dtCore/transformable.h>
#include <dtCore/observerptr.h>
#include <osg/Vec4>

#include <dtUtil/getsetmacros.h>

#include <osg/Camera>

namespace dtUtil
{
   template <typename R, class TList, unsigned int size>
   class Functor;
}

namespace dtCore
{
   class DeltaWin;
   class Scene;
   class ScreenShotCallback;
   class CameraCallbackContainer;
   class CameraDrawCallback;
   class AutoLODScaleCameraCallback;

   /**
    * A dtCore::Camera is a view into the Scene.  It requires a dtCore::DeltaWin to
    * render the the Scene into.  If no DeltaWin is supplied, a default DeltaWin
    * will be created and will be overridden when a valid DeltaWin is supplied
    * using SetWindow().
    *
    * Any part of the Scene that doesn't contain renderable geometry will be
    * drawn a solid color using the values supplied to SetClearColor().
    *
    * The Frame() method must be called once per frame.  This will update the
    * scene, then cull and draw the visual objects.
    */
   class DT_CORE_EXPORT Camera : public Transformable
   {
      DECLARE_MANAGEMENT_LAYER(Camera)

   public:

      Camera(const std::string& name = "camera");
      Camera(osg::Camera& osgCamera, const std::string& name = "camera");

   protected:

      virtual ~Camera();

   public:

      enum AutoNearFarCullingMode
      {
         NO_AUTO_NEAR_FAR = 0,      ///<no auto calculation
         BOUNDING_VOLUME_NEAR_FAR,  ///<use the scene graph bounding spheres
         PRIMITIVE_NEAR_FAR         ///<use the scene graph primitives
      };

      ///Change how the near/far culling planes get automatically calculated
      void SetNearFarCullingMode(AutoNearFarCullingMode mode);

      //Enabled or disable this Camera. Disabled Cameras will not render.
      void SetEnabled(bool enabled);

      ///Is this Camera enabled?
      bool GetEnabled() const;


      ///Get the supplied DeltaWin (could be NULL)
      DeltaWin* GetWindow()             { return (mWindow.get()); }
      const DeltaWin* GetWindow() const { return (mWindow.get()); }

      ///Use the supplied DeltaWin to draw into
      void SetWindow(DeltaWin *win);

      /**
       * Take a screen shot at end of next frame
       * @param namePrefix the prefix of the screenshot file to write
       * @return the final file name of the screenshot that will be written next frame.
       */
      const std::string TakeScreenShot(const std::string& namePrefix);

      /**
       * Set the perspective parameters of this Camera.
       * @param vfov : the vertical field of view in degrees
       * @param aspectRatio : the aspect ratio of the camera (width/height)
       * @param nearClip : the distance from the Camera to the near clipping plane
       * @param farClip : the distance from the Camera to the far clipping plane
       */
      void SetPerspectiveParams(double vfov, double aspectRatio, double nearClip, double farClip);

      /**
      * Get the perspective parameters of this Camera.
      * @param vfov : the vertical field of view in degrees
      * @param aspectRatio : the aspect ratio of the camera (width/height)
      * @param nearClip : the distance from the Camera to the near clipping plane
      * @param farClip : the distance from the Camera to the far clipping plane
      * @return false if the Camera is not setup as a perspective camera
      */
      bool GetPerspectiveParams(double& vfov, double& aspectRatio, double& nearClip, double& farClip);

      /** 
       * Set the perspective parameters of the camera using a frustum.
       * @param left Coordinate for the left vertical clipping plane
       * @param right Coordinate for the right vertical clipping plane
       * @param bottom Coordinate for the bottom horizontal clipping plane
       * @param top Coordinate for the top horizontal clipping plane
       * @param nearClip Distance to the near depth clipping plane (must be positive)
       * @param farClip Distance to the far depth clipping plane (must be positive)
       */
      void SetFrustum(double left, double right,
                      double bottom, double top,
                      double nearClip, double farClip);

      /** 
       * Get the perspective parameters of the camera using a frustum.
       * @param left Coordinate for the left vertical clipping plane
       * @param right Coordinate for the right vertical clipping plane
       * @param bottom Coordinate for the bottom horizontal clipping plane
       * @param top Coordinate for the top horizontal clipping plane
       * @param nearClip Distance to the near depth clipping plane (must be positive)
       * @param farClip Distance to the far depth clipping plane (must be positive)
       * @return false if the Camera isn't setup using frustum values
       */
      bool GetFrustum(double& left, double& right,
                      double& bottom, double& top,
                      double& nearClip, double& farClip);

      /** 
       * Set the orthographic parameters of the camera.
       * @param left Coordinate for the left vertical clipping plane
       * @param right Coordinate for the right vertical clipping plane
       * @param bottom Coordinate for the bottom horizontal clipping plane
       * @param top Coordinate for the top horizontal clipping plane
       * @param nearClip Distance to the near depth clipping plane
       * @param farClip Distance to the far depth clipping plane
       */
      void SetOrtho(double left, double right,
                    double bottom, double top,
                    double nearClip, double farClip);

      /** 
       * Get the orthographic parameters of the camera.
       * @param left Coordinate for the left vertical clipping plane
       * @param right Coordinate for the right vertical clipping plane
       * @param bottom Coordinate for the bottom horizontal clipping plane
       * @param top Coordinate for the top horizontal clipping plane
       * @param nearClip Distance to the near depth clipping plane
       * @param farClip Distance to the far depth clipping plane
       * @return false if the Camera isn't setup using orthographic values
       */
      bool GetOrtho(double& left, double& right,
                    double& bottom, double& top,
                    double& nearClip, double& farClip) const;

      static double ComputeAspectFromFOV(double hfov, double vfov);

      ///@return HOV
      float GetHorizontalFov();

      ///@return FOV
      float GetVerticalFov();

      void SetProjectionResizePolicy( osg::Camera::ProjectionResizePolicy prp )
      {
         mOsgCamera->setProjectionResizePolicy(prp);
      }

      osg::Camera::ProjectionResizePolicy GetProjection()
      {
         return (mOsgCamera->getProjectionResizePolicy());
      }

      ///takes a number (generally width / height) to allow projections to scale correctly
      void SetAspectRatio(double aspectRatio);

      ///@return aspect ratio
      double GetAspectRatio();

      void SetClearColor(float r, float g, float b, float a);
      void SetClearColor(const osg::Vec4& color);
      void GetClearColor(float& r, float& g, float& b, float& a);
      void GetClearColor(osg::Vec4& color);

      ///Get a non-const handle to the underlying Producer::Camera
      osg::Camera* GetOSGCamera() { return mOsgCamera.get(); }

      ///Get a const handle to the underlying Producer::Camera
      const osg::Camera* GetOSGCamera() const { return mOsgCamera.get(); }

      /**
       * Changes the multiplier applied to the Level of Detail distance calculation when rendering.
       * For example, setting this to 2 would double the distance used when picking the LOD.  This means
       * the detail will decrease twice as fast.
       */
      void SetLODScale(float newScale);

      /// @return the multiplier applied to the Level of Detail distance calculation when rendering.
      float GetLODScale() const;

      /**
       *  Enables or disables the auto LOD scale support.
       *  Auto LOD scale will attempt to increase or decrease the LOD scale in an attempt to keep the app at a decent framerate.
       *  Setting this to true will, if none has been assigned, make the camera create a default instance.
       */
      void SetAutoLODScaleEnabled(bool enable);
      /// @return true if the auto lod scale callback is enabled.
      bool IsAutoLODScaleEnabled() const;

      /**
       * Call this to get the auto lod scale callback so you can set values on it.  It will return NULL if none has been set
       * and SetAutoLODScaleEnabled is set to false.  If you set the callback, this will return the instance or subclass you assigned.
       */
      AutoLODScaleCameraCallback* GetAutoLODScaleCallback();

      /**
       * Sets the callback for handling auto lod scaling. You only need to set this if you want to subclass and
       * override behavior for your callback.  If you want to make a single callback work for all of the cameras, you should
       * create an instance of AutoLODScaleCameraCallback or a subclass, and assign it here.  If you set this to null, it will
       * go back to using the default class by creating a new instance as necessary.
       */
      void SetAutoLODScaleCallback(AutoLODScaleCameraCallback* callback);

      /**
      * Supply the Scene this Camera has been added to. Normally this
      * is done inside dtCore::Scene. So you should probably never have
      * to call this.
      *
      * @param scene The Scene to which this Camera is being added to.
      */
      virtual void AddedToScene(Scene* scene);

      /**
       * Gets the position on the screen into the outScreenPos and returns whether or not the position is on the screen
       * in normalized coordinates.  outScreenPos.x is the horizontal position 0 - 1 from left to right.  outScreenPos.y
       * is the veritcal position O - 1 from bottom to top.  outScreenPos.z is the depth from 0 - 1 from the near clipping
       * plane to the far clipping plane.  The values for anything values may be greater or less of the position
       * given is off the screen or behind the camera..
       */
      bool ConvertWorldCoordinateToScreenCoordinate(const osg::Vec3d& worldPos, osg::Vec3d& outScreenPos) const;

      typedef dtUtil::Functor<void, TYPELIST_1(dtCore::Camera&), 4 * sizeof(void*)> CameraSyncCallback;

      /**
       * Adds a callback that fires whenever the this camera is updated on CameraSync.  This makes it easy to
       * add code that will add camera specific uniforms, or do operations in screen space since the camera will be in its
       * final state prior to rendering.
       *
       * An example call from inside class "A" with a method with signature "void OnCameraSync(Camera& camera)" would be:
       *
       * Camera::AddCameraSyncCallback(this, CameraSyncCallback(this, &A::OnCameraSync))
       *
       * @param keyObject this object will be held onto with an observer pointer.
       *                  When this object is deleted, the callback will be removed.
       *                  It's also handy when removing callbacks directly.
       * @param callback a functor to call. See the typedef above CameraSyncCallback.
       */
      static void AddCameraSyncCallback(osg::Referenced& keyObject, CameraSyncCallback callback);
      static void RemoveCameraSyncCallback(osg::Referenced& keyObject);

      // Updates the view matrix via the transformed position.  This is called from CameraSynch.
      void UpdateViewMatrixFromTransform();

      /** Add a postdraw callback to the Camera.
        * @param cb The callback to trigger during post-draw
        * @return true if cb was added correctly
        */
      bool AddPostDrawCallback(dtCore::CameraDrawCallback& cb, bool singleFire = false);

      /** Remove a postdraw callback from the Camera.
        * @param cb The callback to remove
        * @return true if cb was removed correctly
        */
      bool RemovePostDrawCallback(dtCore::CameraDrawCallback& cb);

   protected:

      ///Override for CameraSynch
      virtual void CameraSynch(const double deltaFrameTime);

      /// Base override to receive messages.
      /// This method should be called from derived classes
      /// @param data the message to receive
      virtual void OnMessage(MessageData* data);

      /// Call all of the static frame sync callbacks using this camera.
      void CallCameraSyncCallbacks();

   private:

      // Disallowed to prevent compile errors on VS2003. It apparently
      // creates this functions even if they are not used, and if
      // this class is forward declared, these implicit functions will
      // cause compiler errors for missing calls to "ref".
      Camera& operator=(const Camera&);
      Camera(const Camera&);

      void OnWindowChanged();

      ///Common constructor functionality
      void Ctor();

      /** Verify the CameraCallbackContainer is created and added to the OSG Camera.
        * If it's not, try to create and add it.
        * @return true if the container has been created and added to the OSG Camera
        */
      bool VerifyCallbackContainer();

      RefPtr<osg::Camera> mOsgCamera; // Handle to the osg Camera
      RefPtr<DeltaWin> mWindow; // The currently assigned DeltaWin

      bool mAddedToSceneGraph;
      bool mEnable;
      bool mEnableAutoLODScaleCallback;
      RefPtr<ScreenShotCallback> mScreenShotTaker;
      RefPtr<AutoLODScaleCameraCallback> mAutoLODScaleCameraCallback;

      osg::Node::NodeMask mEnabledNodeMask; ///<The last known node mask corresponding to "enabled"
      RefPtr<CameraCallbackContainer> mCallbackContainer;
   };

   /**
    * @brief implements the auto level of detail (LOD) scale feature.
    *
    * The LOD scale is a multiplier that is applied to the distance from the view point to
    * the bounding sphere of an LOD node when it is traversed in the culling phase of rendering.
    * the.  The LOD node has several children that are assigned ranges of distance where they will be rendered,
    * and each child is expected to have a different level of detail.  At some distance, no child will be selected and
    * nothing will be rendered. The scale with either move those transitions closer or farther depending on if the value is
    * greater or lesser than 1.0 respectively.  This class will take into account the time it takes to render the scene
    * using the system frame time by default and decide whether to increase or decrease the scale to make the rendering
    * time approach the target.  Minimum and maximum scales values exist to so the rendering quality can be controlled as well.
    *
    * The Update and GetFrameTimeMS methods can be overridden to change the behavior if needed.
    */
   class DT_CORE_EXPORT AutoLODScaleCameraCallback : public osg::Referenced
   {
   public:
      /**
       * Main constructor.
       * @param camera The camera use a target. If you want to use a specific camera, you set this, if you want to
       *               apply it to all cameras, pass NULL.
       */
      AutoLODScaleCameraCallback(Camera* camera = NULL);
      virtual ~AutoLODScaleCameraCallback();

      /// The main update method. Override this to change the behavior.
      virtual void Update(Camera&);
      /// @return the frame time in milliseconds.  You may override this to use a different time value that you either lookup or calculate.
      virtual double GetFrameTimeMS() const;

      /**
       * The target frame time.  It will increase the LOD scale if the frame time is larger than this,
       * or decrease it if it's smaller. Default is 33.33 or 30Hz
       */
      DT_DECLARE_ACCESSOR(double, TargetFrameTimeMS)

      /// The amount of slop to allow when comparing the frame time to the target frame time.  Defaults to 0.3
      DT_DECLARE_ACCESSOR(double, TargetFrameTimeEpsilon)

      /**
       * The minimum LOD scale to set even if the framerate is really high.  You should probably set this to 1.0 (Default)
       * but if you want it to go lower than 1.0 so it will show better LOD if you have the performance, then you can.
       */
      DT_DECLARE_ACCESSOR(float, MinLODScale)

      /**
       * The maximum LOD scale to set even if the framerate is really low.  You should probably set this to something greater than 1.0.
       * The default is 2.0.
       */
      DT_DECLARE_ACCESSOR(float, MaxLODScale)

      /**
       * Each time it needs to change the number, this is the scalar factor of how much to change it.  0-1 work.  Something
       * around 0.001 probably makes the most sense so you don't get huge changes in one frame, which can lead to oscillations.
       */
      DT_DECLARE_ACCESSOR(float, ChangeFactor)

   private:
      dtCore::ObserverPtr<Camera> mCamera;
      // bool to see if it should just exit if the camera is/becomes null.
      bool mCameraWasNull;
   };
}

#endif // DELTA_CAMERA
