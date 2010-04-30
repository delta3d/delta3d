// camera.cpp: implementation of the Camera class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtCore/camera.h>

#include <dtCore/deltawin.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/collisioncategorydefaults.h>
#include <dtCore/cameracallbackcontainer.h>
#include <dtCore/screenshotcallback.h>
#include <dtCore/transform.h>
#include <dtUtil/datetime.h>
#include <dtUtil/log.h>
#include <dtUtil/functor.h>
#include <dtUtil/mathdefines.h>

#include <osg/Matrix>
#include <osg/MatrixTransform>

#include <osg/Version>
#include <osg/GraphicsContext>
#include <osgViewer/GraphicsWindow>

#include <cassert>
#include <algorithm>
#include <cmath>

using namespace dtUtil;

namespace dtCore
{
   typedef std::vector<std::pair<dtCore::ObserverPtr<osg::Referenced>, Camera::CameraSyncCallback> > CallbackListType;
   static CallbackListType staticCameraSyncCallbacks;


   IMPLEMENT_MANAGEMENT_LAYER(Camera)

   /////////////////////////////////////////////////////////////////////////////
   Camera::Camera(const std::string& name)
   : Transformable(name)
   , mOsgCamera(new osg::Camera)
   , mAddedToSceneGraph(false)
   , mEnable(true)
   , mEnableAutoLODScaleCallback(false)
   , mEnabledNodeMask(0xffffffff)
   , mCallbackContainer(NULL)

   {
      mOsgCamera->setName(GetName());

      double height = osg::DisplaySettings::instance()->getScreenHeight();
      double width = osg::DisplaySettings::instance()->getScreenWidth();
      double distance = osg::DisplaySettings::instance()->getScreenDistance();
      double vfov = osg::RadiansToDegrees(atan2(height / 2.0f,distance) * 2.0);
      mOsgCamera->setProjectionMatrixAsPerspective(vfov, width / height, 1.0f, 10000.0f);

      Ctor();
   }

   /////////////////////////////////////////////////////////////////////////////
   Camera::Camera(osg::Camera& osgCamera, const std::string& name)
      : Transformable(name)
      , mOsgCamera(&osgCamera)
      , mAddedToSceneGraph(false)
      , mEnable(true)
      , mEnabledNodeMask(0xffffffff)
      , mCallbackContainer(NULL)
   {
      Ctor();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::Ctor()
   {
      RegisterInstance(this);

      System* sys = &dtCore::System::GetInstance();
      AddSender(sys);

      SetClearColor(0.2f, 0.2f, 0.6f, 1.f);

      SetCollisionCategoryBits(COLLISION_CATEGORY_MASK_CAMERA);
   }

   /////////////////////////////////////////////////////////////////////////////
   Camera::~Camera()
   {
      DeregisterInstance(this);
      RemoveSender(&dtCore::System::GetInstance());

      if (mCallbackContainer.valid())
      {
         if (mScreenShotTaker.valid())
         {
            mCallbackContainer->RemoveCallback(*mScreenShotTaker);
            mScreenShotTaker = NULL;
         }
         mCallbackContainer = NULL;
      }

   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string Camera::TakeScreenShot(const std::string& namePrefix)
   {
      if (mScreenShotTaker.valid() == false)
      {
         //create our screen shot taker and add it to the container
         mScreenShotTaker = new ScreenShotCallback();
         if (AddPostDrawCallback(*mScreenShotTaker) == false)
         {
            //could not add our callback
            mScreenShotTaker = NULL;
            return std::string();
         }
      }

      std::string timeString = dtUtil::DateTime::ToString(dtUtil::DateTime(dtUtil::DateTime::TimeOrigin::LOCAL_TIME),
                                                          dtUtil::DateTime::TimeFormat::CALENDAR_DATE_AND_TIME_FORMAT);
      for (unsigned int i = 0 ; i < timeString.length(); ++i)
      {
         char c = timeString[i];
         if (c == '.'
            || c == ':'
            || c == '-')
         {
            timeString[i] = '_';
         }
      }
      const std::string outputName = namePrefix + "_" + timeString + ".jpg";
      mScreenShotTaker->SetNameToOutput(outputName);
      return outputName;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::SetEnabled(bool enabled)
   {
      if (mEnable == enabled)
      {
         return; //nothing to do here
      }

      mEnable = enabled;

      if (mEnable == true)
      {
         if (GetOSGCamera()->getNodeMask() != 0x0)
         {
            //error: the user must have set the node mask while the
            //camera was disabled. tsk tsk
            LOG_ERROR("User supplied camera node mask will be overwritten.");
         }

         GetOSGCamera()->setNodeMask(mEnabledNodeMask);
      }
      else
      {
         //save off the existing, theoretically enabled, node mask
         mEnabledNodeMask = GetOSGCamera()->getNodeMask();
         GetOSGCamera()->setNodeMask(0x0);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Camera::GetEnabled() const
   {
       return (mEnable);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::OnMessage(MessageData* data)
   {

      if (data->message == dtCore::System::MESSAGE_CAMERA_SYNCH)
      {
         CameraSynch(*static_cast<const double*>(data->userData));
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::SetNearFarCullingMode(AutoNearFarCullingMode mode)
   {
      osg::CullSettings::ComputeNearFarMode osgMode;

      switch (mode)
      {
         case NO_AUTO_NEAR_FAR:         osgMode = osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR; break;
         case BOUNDING_VOLUME_NEAR_FAR: osgMode = osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES; break;
         case PRIMITIVE_NEAR_FAR:       osgMode = osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES; break;
         default:                       osgMode = osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES; break;
      }

      GetOSGCamera()->setComputeNearFarMode(osgMode);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::UpdateViewMatrixFromTransform()
   {
      //Get our Camera's position, up vector, and look-at vector and pass them
      //to the Producer Camera
      osg::Matrix mat = GetMatrixNode()->getMatrix();
      Transform absXform;

      // Get the absolute matrix but make sure to use the CameraNode in
      // computing it.
      osg::Matrix absMat;

      // If this dtCore::Camera has been added to a dtCore::Scene, we must not
      // use the top osg::CameraNode in our matrix calculations, otherwise it
      // will be applied twice. If this instance of dtCore::Camera is NOT in
      // a dtCore::Scene, we must apply the osg::CameraNode matrix here.
      if (mAddedToSceneGraph)
      {
         // Find the transform in World coordinates, but leave out
         // the osg::CameraNode.
         Transformable::GetAbsoluteMatrix(GetMatrixNode(), absMat);
      }
      else
      {
         osg::NodePathList nodePathList = GetMatrixNode()->getParentalNodePaths();

         if (!nodePathList.empty())
         {
            osg::NodePath nodePath = nodePathList[0];

            // Find the transform in World coorindates, but leave
            // on the osg::CameraNode.
            absMat.set(osg::computeLocalToWorld(nodePath));
         }
         else
         {
            LOG_WARNING("Finding NodePath of Camera's subgraph failed.")
         }
      }

      //choose Z as the up vector
      osg::Vec3d eye(-absMat(3,0), -absMat(3,1), -absMat(3,2));
      osg::Vec3d UP(absMat(2,0), absMat(2,1), absMat(2,2));
      osg::Vec3d F = UP ^ osg::Vec3d(absMat(0,0), absMat(0,1), absMat(0,2));

      F.normalize();
      UP.normalize();

      osg::Vec3d s = F ^ UP;
      osg::Vec3d u = s ^ F;
      F = -F;

      osg::Matrix m(s[0], u[0], F[0], 0.0,
                    s[1], u[1], F[1], 0.0,
                    s[2], u[2], F[2], 0.0,
                    s*eye, u*eye, F*eye, 1.0);

      mOsgCamera->setViewMatrix(m);
   }

   //////////////////////////////////////////
   void Camera::CameraSynch( const double /*deltaFrameTime*/ )
   {
      // Only do our normal Camera stuff if it is enabled
      if (GetEnabled() == false)
      {
         return;
      }

      UpdateViewMatrixFromTransform();
      CallCameraSyncCallbacks();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::SetPerspectiveParams(double vfov, double aspectRatio, double nearClip, double farClip)
   {
      mOsgCamera->setProjectionMatrixAsPerspective(vfov, aspectRatio , nearClip, farClip);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::GetPerspectiveParams(double& vfov, double& aspectRatio, double& nearClip, double& farClip)
   {
      mOsgCamera->getProjectionMatrixAsPerspective(vfov, aspectRatio, nearClip, farClip);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::SetFrustum(double left, double right, double bottom, double top, double nearClip, double farClip)
   {
      mOsgCamera->setProjectionMatrixAsFrustum(left, right, bottom, top, nearClip, farClip);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::GetFrustum(double& left, double& right, double& bottom, double& top, double& nearClip, double& farClip)
   {
      mOsgCamera->getProjectionMatrixAsFrustum(left, right, bottom, top, nearClip, farClip);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::SetOrtho( double left, double right, double bottom, double top, double nearClip, double farClip )
   {
      mOsgCamera->setProjectionMatrixAsOrtho(left, right, bottom, top, nearClip, farClip);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::SetClearColor(float r, float g, float b, float a)
   {
      osg::Vec4 color(r, g, b, a);

      SetClearColor(color);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::SetClearColor(const osg::Vec4& color)
   {
       mOsgCamera->setClearColor(color);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::GetClearColor(float& r, float& g, float& b, float& a)
   {
      osg::Vec4 color = mOsgCamera->getClearColor();

      r = color[0];
      g = color[1];
      b = color[2];
      a = color[3];
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::GetClearColor(osg::Vec4& color)
   {
      color = mOsgCamera->getClearColor();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::SetLODScale(float newScale)
   {
      GetOSGCamera()->setLODScale(newScale);
   }

   /////////////////////////////////////////////////////////////////////////////
   float Camera::GetLODScale() const
   {
      return GetOSGCamera()->getLODScale();
   }
   /////////////////////////////////////////////////////////////////////////////
   void Camera::SetAutoLODScaleEnabled(bool enable)
   {
      if (enable && !mEnableAutoLODScaleCallback)
      {
         if (!mAutoLODScaleCameraCallback.valid())
         {
            mAutoLODScaleCameraCallback = new AutoLODScaleCameraCallback(this);
         }
         AddCameraSyncCallback(*mAutoLODScaleCameraCallback, CameraSyncCallback(mAutoLODScaleCameraCallback, &AutoLODScaleCameraCallback::Update));
      }
      else if (!enable && mEnableAutoLODScaleCallback)
      {
         // this check SHOULD be unnecessary but I'll do it anyway.
         if (mAutoLODScaleCameraCallback.valid())
         {
            RemoveCameraSyncCallback(*mAutoLODScaleCameraCallback);
         }

      }
      mEnableAutoLODScaleCallback = enable;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Camera::IsAutoLODScaleEnabled() const
   {
      return mEnableAutoLODScaleCallback;
   }

   /////////////////////////////////////////////////////////////////////////////
   AutoLODScaleCameraCallback* Camera::GetAutoLODScaleCallback()
   {
      return mAutoLODScaleCameraCallback;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::SetAutoLODScaleCallback(AutoLODScaleCameraCallback* callback)
   {
      bool wasEnabled = mEnableAutoLODScaleCallback;
      SetAutoLODScaleEnabled(false);
      mAutoLODScaleCameraCallback = callback;
      SetAutoLODScaleEnabled(wasEnabled);
   }

   /////////////////////////////////////////////////////////////////////////////
   double Camera::ComputeAspectFromFOV(double hfov, double vfov)
   {
      //aspect ratio is the ratio of the tangents of the half angles.
      return std::tan(osg::DegreesToRadians(hfov / 2)) /
         std::tan(osg::DegreesToRadians(vfov / 2));
   }

   /////////////////////////////////////////////////////////////////////////////
   float Camera::GetHorizontalFov()
   {
      double vfov, aspectRatio, nearClip, farClip;
      GetPerspectiveParams(vfov, aspectRatio, nearClip, farClip);

      // aspect ratio is the ration of the tangents of the half angles, so need twice the arctan to get the angle
      return 2 * osg::RadiansToDegrees(std::atan(std::tan(osg::DegreesToRadians(vfov / 2)) * aspectRatio));
   }

   /////////////////////////////////////////////////////////////////////////////
   float Camera::GetVerticalFov()
   {
      double vfov, aspectRatio, nearClip, farClip;
      GetPerspectiveParams(vfov, aspectRatio, nearClip, farClip);
      return float(vfov);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::SetAspectRatio(double aspectRatio)
   {
      double vfov, oldAspectRatio, nearClip, farClip;
      GetPerspectiveParams(vfov, oldAspectRatio, nearClip, farClip);
      SetPerspectiveParams(vfov, aspectRatio, nearClip, farClip);
   }

   /////////////////////////////////////////////////////////////////////////////
   double Camera::GetAspectRatio()
   {
      double vfov, aspectRatio, nearClip, farClip;
      GetPerspectiveParams(vfov, aspectRatio, nearClip, farClip);
      return aspectRatio;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::AddedToScene(Scene* scene)
   {
      mAddedToSceneGraph = bool(scene != NULL);

      Transformable::AddedToScene(scene);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Camera::ConvertWorldCoordinateToScreenCoordinate(const osg::Vec3d& worldPos, osg::Vec3d& outScreenPos) const
   {
      // DEBUG: Print the incoming 3D point.
      // std::cout << "t\World XYZ [" << worldPos << "]\n" << std::endl;

      const osg::Camera& cam = *GetOSGCamera();
      // Start with a Vec4 version of the specified world coordinate.
      osg::Vec4d screenXYZ(worldPos.x(), worldPos.y(), worldPos.z(), 1.0);

      // Go from world space to view space.
      screenXYZ = cam.getViewMatrix().preMult(screenXYZ);

      // Go from view space to screen space (with perspective).
      screenXYZ = cam.getProjectionMatrix().preMult(screenXYZ);

      // Capture the XY coordinates
      outScreenPos.set(screenXYZ.x(), screenXYZ.y(), screenXYZ.z());

      // Remove perspective.
      outScreenPos /= screenXYZ.w();

      // Center the resulting normalized coordinates.
      outScreenPos += osg::Vec3d(1.0, 1.0, 1.0);
      outScreenPos *= 0.5;

      // If W is negative, flip the Z value to negative to
      // flag this point as being behind the camera view.
      if (screenXYZ.w() < 0.0)
      {
         outScreenPos.z() *= -1.0;
      }

      return outScreenPos.z() > 0.0
         && outScreenPos.x() >= 0.0 && outScreenPos.x() <= 1.0
         && outScreenPos.y() >= 0.0 && outScreenPos.y() <= 1.0;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::SetWindow(DeltaWin* win)
   {
      if (mWindow == win)
      {
         return;
      }

      mWindow = win;

      OnWindowChanged();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::OnWindowChanged()
   {
      if (mWindow.valid())
      {
         osgViewer::GraphicsWindow* gw = mWindow->GetOsgViewerGraphicsWindow();
         mOsgCamera->setGraphicsContext(gw);

         const osg::GraphicsContext::Traits* traits = gw->getTraits();

         double fovy, aspectRatio, zNear, zFar;
         mOsgCamera->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);

         double newAspectRatio = double(traits->width) / double(traits->height);
         double aspectRatioChange = newAspectRatio / aspectRatio;
         if (aspectRatioChange != 1.0)
         {
            mOsgCamera->getProjectionMatrix() *= osg::Matrix::scale(1.0/aspectRatioChange, 1.0, 1.0);
         }

         // apply viewport dimensions for camera
         mOsgCamera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));

         GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;

         mOsgCamera->setDrawBuffer(buffer);
         mOsgCamera->setReadBuffer(buffer);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   class CameraCallbackHandler
   {
   public:
      CameraCallbackHandler(const osg::Referenced* keyObject, dtCore::Camera* camera = NULL, bool callCallback = false)
         : mKeyObject(keyObject)
         , mCamera(camera)
         , mCallCallback(callCallback)
      {

      }

      bool operator()(CallbackListType::value_type& item)
      {
         if (mCallCallback && mCamera != NULL && item.first.valid())
         {
            item.second(*mCamera);
         }

         return item.first == mKeyObject;
      }

      const osg::Referenced* mKeyObject;
      dtCore::Camera* mCamera;
      bool mCallCallback;
   };

   /////////////////////////////////////////////////////////////////////////////
   void Camera::CallCameraSyncCallbacks()
   {
      staticCameraSyncCallbacks.erase(
               std::remove_if(staticCameraSyncCallbacks.begin(), staticCameraSyncCallbacks.end(), CameraCallbackHandler(NULL, this, true)),
               staticCameraSyncCallbacks.end());
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::AddCameraSyncCallback(osg::Referenced& keyObject, CameraSyncCallback callback)
   {
      staticCameraSyncCallbacks.push_back(std::make_pair(&keyObject, callback));
   }

   /////////////////////////////////////////////////////////////////////////////
   void Camera::RemoveCameraSyncCallback(osg::Referenced& keyObject)
   {
      staticCameraSyncCallbacks.erase(
               std::remove_if(staticCameraSyncCallbacks.begin(), staticCameraSyncCallbacks.end(), CameraCallbackHandler(&keyObject)),
               staticCameraSyncCallbacks.end());
   }

   //////////////////////////////////////////////////////////////////////////
   bool Camera::VerifyCallbackContainer()
   {
      if (GetOSGCamera()->getPostDrawCallback() == NULL)
      {
         // 1) No callback yet
         if (mCallbackContainer.valid() == false)
         {
            mCallbackContainer = new CameraCallbackContainer(*this);
         }
         GetOSGCamera()->setPostDrawCallback(mCallbackContainer.get());

         return true;
      }

      if (GetOSGCamera()->getPostDrawCallback() == mCallbackContainer.get())
      {
         //2) its our callback
         return true;
      }
      else
      {
         //3) its not our callback
         LOG_ERROR("The OSG Camera already has a PostDrawCallback assigned.  Can't replace it.");
         return false;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool Camera::AddPostDrawCallback(dtCore::CameraDrawCallback& cb, bool singleFire)
   {
      if (VerifyCallbackContainer() == false) { return false; }

      mCallbackContainer->AddCallback(cb, singleFire);

      return true;
   }

   //////////////////////////////////////////////////////////////////////////
   bool Camera::RemovePostDrawCallback(dtCore::CameraDrawCallback& cb)
   {
      if (VerifyCallbackContainer() == false) { return false; }

      mCallbackContainer->RemoveCallback(cb);

      return true;
   }

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
         if (!dtUtil::Equivalent(mTargetFrameTimeMS, frameTime, mTargetFrameTimeEpsilon))
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
               printf("changing LOD scale from \"%f\" to \"%f\"\n", oldScale, newScale);
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
   IMPLEMENT_PROPERTY(AutoLODScaleCameraCallback, double, TargetFrameTimeMS);
   IMPLEMENT_PROPERTY(AutoLODScaleCameraCallback, double, TargetFrameTimeEpsilon)
   IMPLEMENT_PROPERTY(AutoLODScaleCameraCallback, float, MinLODScale);
   IMPLEMENT_PROPERTY(AutoLODScaleCameraCallback, float, MaxLODScale);
   IMPLEMENT_PROPERTY(AutoLODScaleCameraCallback, float, ChangeFactor);

}

/////////////////////////////////////////////////////////////////////////////

