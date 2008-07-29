// camera.cpp: implementation of the Camera class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix-src.h>
#include <dtCore/camera.h>


#include <dtCore/deltawin.h>
#include <dtCore/scene.h>
#include <dtCore/keyboardmousehandler.h> //due to including scene.h
#include <dtCore/system.h>
#include <dtCore/collisioncategorydefaults.h>
#include <dtUtil/datetime.h>
#include <dtUtil/log.h>

#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osgDB/WriteFile>

#include <osg/Version>


#include <cassert>

using namespace dtUtil;

namespace dtCore
{

   class ScreenShotCallback : public osg::Camera::DrawCallback
   {
      public:
         ScreenShotCallback() : mTakeScreenShotNextFrame(false){}
         
         void SetNameToOutput(const std::string& name)
         {
            mTakeScreenShotNextFrame  = true;
            mNameOfScreenShotToOutput = name;
         }

         const std::string GetNameToOutput() const { return mNameOfScreenShotToOutput; };

         virtual void operator()(const osg::Camera &camera) const
         {
            if(mTakeScreenShotNextFrame)
            {
               mTakeScreenShotNextFrame = false;
               osg::ref_ptr<osg::Image> image = new osg::Image;
               
               int x = static_cast<int>(camera.getViewport()->x());
               int y = static_cast<int>(camera.getViewport()->y());
               unsigned int width = static_cast<unsigned int>(camera.getViewport()->width());
               unsigned int height = static_cast<unsigned int>(camera.getViewport()->height());
               
               image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
               image->readPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE);
               bool status = osgDB::writeImageFile( *image.get(), mNameOfScreenShotToOutput.c_str() ); // jpg, rgb, png, bmp
               if (status == false)
               {
                  LOG_ERROR("Can't write out screenshot file: " + mNameOfScreenShotToOutput +
                            ". Does the osgDB plugin exist?");
               }
            }
         }

      private:
         mutable bool  mTakeScreenShotNextFrame;
         std::string mNameOfScreenShotToOutput;
   };


   IMPLEMENT_MANAGEMENT_LAYER(Camera)

   //////////////////////////////////////////////////////////////////////
   // Construction/Destruction
   //////////////////////////////////////////////////////////////////////

   Camera::Camera(const std::string& name)
   :  Transformable(name),
      mAddedToSceneGraph(false),
      mEnable(true),
      mEnabledNodeMask(0xffffffff),
      mOsgCamera(new osg::Camera)
   {
      mOsgCamera->setName(GetName());

      double height = osg::DisplaySettings::instance()->getScreenHeight();
      double width = osg::DisplaySettings::instance()->getScreenWidth();
      double distance = osg::DisplaySettings::instance()->getScreenDistance();
      double vfov = osg::RadiansToDegrees(atan2(height/2.0f,distance)*2.0);
      mOsgCamera->setProjectionMatrixAsPerspective( vfov, width/height, 1.0f,10000.0f);

      Ctor();
   }


   ////////////////////////////////////////// 
   Camera::Camera(osg::Camera& osgCamera, const std::string& name)
      :  Transformable(name),
         mAddedToSceneGraph(false),
         mEnable(true),
         mEnabledNodeMask(0xffffffff),
         mOsgCamera(&osgCamera)
   {
      Ctor();
   }
   
   //////////////////////////////////////////////////////////////////////////
   void Camera::Ctor()
   {
      RegisterInstance(this);

      System* sys = &dtCore::System::GetInstance();
      AddSender(sys);

      SetClearColor(0.2f, 0.2f, 0.6f, 1.f);

      SetCollisionCategoryBits(COLLISION_CATEGORY_MASK_CAMERA);

      mScreenShotTaker = new ScreenShotCallback;
      mOsgCamera->setPostDrawCallback(mScreenShotTaker.get());
   }


   ////////////////////////////////////////// 
   Camera::~Camera()
   {
      DeregisterInstance(this);
      RemoveSender( &dtCore::System::GetInstance() );
   }

   ////////////////////////////////////////// 
   const std::string Camera::TakeScreenShot(const std::string& namePrefix)
   {
      std::string timeString = dtUtil::DateTime::ToString(dtUtil::DateTime(dtUtil::DateTime::TimeOrigin::LOCAL_TIME),
                                                          dtUtil::DateTime::TimeFormat::CALENDAR_DATE_AND_TIME_FORMAT);
      for(unsigned int i = 0 ; i < timeString.length(); ++i)
      {
         char c = timeString[i];
         if(c == '.'
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

   ////////////////////////////////////////// 
   void Camera::SetEnabled( bool enabled )
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

   ////////////////////////////////////////// 
   bool Camera::GetEnabled() const
   {
       return (mEnable);
   }

   ////////////////////////////////////////// 
   void Camera::OnMessage( MessageData* data )
   {

      if( data->message == "framesynch" )
      {
         FrameSynch( *static_cast<const double*>(data->userData) );
      }
   }
   
   ////////////////////////////////////////// 
   void Camera::SetNearFarCullingMode( AutoNearFarCullingMode mode )
   {
      osg::CullSettings::ComputeNearFarMode osgMode;

      switch(mode)
      {
      case NO_AUTO_NEAR_FAR: osgMode = osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR;	break;
      case BOUNDING_VOLUME_NEAR_FAR: osgMode = osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES; break;
      case PRIMITIVE_NEAR_FAR: osgMode = osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES; break;

      default: osgMode = osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES; break;
      }

      GetOSGCamera()->setComputeNearFarMode(osgMode);
   }

   ////////////////////////////////////////// 
   void Camera::FrameSynch( const double /*deltaFrameTime*/ )
   {
      // Only do our normal Camera stuff if it is enabled
      if(GetEnabled() == false)
      {
         return;
      }

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
      if( mAddedToSceneGraph )
      {
         // Find the transform in World coordinates, but leave out
         // the osg::CameraNode.
         Transformable::GetAbsoluteMatrix( GetMatrixNode(), absMat );
      }
      else
      {
         osg::NodePathList nodePathList = GetMatrixNode()->getParentalNodePaths();

         if( !nodePathList.empty() )
         {
            osg::NodePath nodePath = nodePathList[0];

            // Find the transform in World coorindates, but leave
            // on the osg::CameraNode.

            #if defined(OSG_VERSION_MAJOR) && defined(OSG_VERSION_MINOR) && OSG_VERSION_MAJOR == 1 && OSG_VERSION_MINOR == 1
            // In OSG 1.1, there is a default second paramter to computeLocalToWorld which
            // ignore the CameraNode at the top of the scene graph. Normally this is what we
            // want, but for the Camera, we want it included (so pass false).
            absMat.set( osg::computeLocalToWorld(nodePath, false) );
            #else
            absMat.set( osg::computeLocalToWorld(nodePath) );
            #endif
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
   void Camera::SetPerspectiveParams(double vfov, double aspectRatio, double nearClip, double farClip)
   {
      mOsgCamera->setProjectionMatrixAsPerspective(vfov, aspectRatio , nearClip, farClip);
   }

   ////////////////////////////////////////// 
   void Camera::GetPerspectiveParams(double &vfov, double &aspectRatio, double &nearClip, double &farClip)
   {
      mOsgCamera->getProjectionMatrixAsPerspective(vfov, aspectRatio, nearClip, farClip);
   }

   ////////////////////////////////////////// 
   void Camera::SetFrustum(double left, double right, double bottom, double top, 
            double nearClip, double farClip)
   {
      mOsgCamera->setProjectionMatrixAsFrustum(left, right, bottom, top, nearClip, farClip);
   }

   ////////////////////////////////////////// 
   void Camera::GetFrustum(double& left, double& right, double& bottom, double& top, 
            double& nearClip, double& farClip)
   {
      mOsgCamera->getProjectionMatrixAsFrustum(left, right, bottom, top, nearClip, farClip);
   }

   ////////////////////////////////////////// 
   void Camera::SetOrtho( double left, double right, double bottom, double top, double nearClip, double farClip )
   {
      mOsgCamera->setProjectionMatrixAsOrtho(left, right, bottom, top, nearClip, farClip);
   }

   ////////////////////////////////////////// 
   void Camera::SetClearColor(float r, float g, float b, float a)
   {
      osg::Vec4 color(r, g, b, a);

      SetClearColor(color);
   }

   ////////////////////////////////////////// 
   void Camera::SetClearColor(const osg::Vec4& color)
   {
       mOsgCamera->setClearColor(color);
   }

   ////////////////////////////////////////// 
   void Camera::GetClearColor( float& r, float& g, float& b, float& a )
   {
      osg::Vec4 color = mOsgCamera->getClearColor();
       
      r = color[0];
      g = color[1];
      b = color[2];
      a = color[3];
   }
   
   ////////////////////////////////////////// 
   void Camera::GetClearColor(osg::Vec4& color)
   {
      color = mOsgCamera->getClearColor();
   }

   ////////////////////////////////////////// 
   void Camera::SetLODScale(float newScale)
   {
      GetOSGCamera()->setLODScale(newScale);
   }

   ////////////////////////////////////////// 
   float Camera::GetLODScale() const
   {
      return GetOSGCamera()->getLODScale();
   }

   ////////////////////////////////////////// 
   float Camera::GetHorizontalFov()
   {
      double vfov, aspectRatio, nearClip, farClip;
      GetPerspectiveParams(vfov, aspectRatio, nearClip, farClip);
      return float(vfov * aspectRatio);
   }

   //////////////////////////////////////////
   float Camera::GetVerticalFov()
   {
      double vfov, aspectRatio, nearClip, farClip;
      GetPerspectiveParams(vfov, aspectRatio, nearClip, farClip);
      return float(vfov);
   }


   //////////////////////////////////////////
   void Camera::SetAspectRatio( double aspectRatio )
   {
      double vfov, oldAspectRatio, nearClip, farClip;
      GetPerspectiveParams(vfov, oldAspectRatio, nearClip, farClip);
      SetPerspectiveParams(vfov, aspectRatio, nearClip, farClip);
   }

   //////////////////////////////////////////
   double Camera::GetAspectRatio()
   {
      double vfov, aspectRatio, nearClip, farClip;
      GetPerspectiveParams(vfov, aspectRatio, nearClip, farClip);
      return aspectRatio;
   }

   //////////////////////////////////////////
   void Camera::AddedToScene( Scene* scene )
   {
      mAddedToSceneGraph = bool(scene != NULL);

      Transformable::AddedToScene(scene);
   }

   //////////////////////////////////////////
   void Camera::SetWindow( DeltaWin* win )
   {
      if (mWindow == win) return;
            
      mWindow = win;

      OnWindowChanged();
   }

   ////////////////////////////////////////// 
   void Camera::OnWindowChanged()
   {
      if (mWindow.valid())
      {
         osgViewer::GraphicsWindow * gw = mWindow->GetOsgViewerGraphicsWindow();
         mOsgCamera->setGraphicsContext(gw);

         const osg::GraphicsContext::Traits * traits = gw->getTraits();
         
         double fovy, aspectRatio, zNear, zFar;
         mOsgCamera->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);

         double newAspectRatio = double(traits->width) / double(traits->height);
         double aspectRatioChange = newAspectRatio / aspectRatio;
         if (aspectRatioChange != 1.0)
         {
            mOsgCamera->getProjectionMatrix() *= osg::Matrix::scale(1.0/aspectRatioChange, 1.0, 1.0);
         }

         // HACK -- this is a temporary solution to get FLTK guis w/ menu bars playing nice
         // set the dimensions of the viewport, initialized to the size of the window
         int x      = 0;
         int y      = 0;
         int width  = traits->width;
         int height = traits->height;

         // offset by toolbar dimensions to help FLTK out
         width  += 12;
         height += 31;

         // apply viewport dimensions for camera
         mOsgCamera->setViewport(new osg::Viewport(x, y, width, height));
         // end HACK

         GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;

         mOsgCamera->setDrawBuffer(buffer);
         mOsgCamera->setReadBuffer(buffer);
      }
   }

}

