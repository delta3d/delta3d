// camera.cpp: implementation of the Camera class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix-src.h>
#include <dtCore/camera.h>


#include <dtCore/deltawin.h>
#include <dtCore/scene.h>
#include <dtCore/keyboardmousehandler.h> //due to including scene.h
#include <dtCore/keyboard.h> //due to including scene.h
#include <dtCore/system.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/log.h>
#include <dtCore/exceptionenum.h>
#include <dtUtil/exception.h>

#include <osg/FrameStamp>
#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osgUtil/SceneView>
#include <osgDB/WriteFile>

#include <ctime>
#include <osg/Version>
#include <osgViewer/View>
#include <osgViewer/GraphicsWindow>

#include <cassert>

using namespace dtUtil;

namespace dtCore
{

   class ScreenShotCallback : public osg::Camera::DrawCallback
   {
      public :
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
      mFrameBin(0),
      mAddedToSceneGraph(false),
      mEnable(true)
   {
      RegisterInstance(this);

      System*  sys   = &dtCore::System::GetInstance();
      assert( sys );
      AddSender( sys );
   
      mOsgCamera = new osg::Camera;

      double height = osg::DisplaySettings::instance()->getScreenHeight();
      double width = osg::DisplaySettings::instance()->getScreenWidth();
      double distance = osg::DisplaySettings::instance()->getScreenDistance();
      double vfov = osg::RadiansToDegrees(atan2(height/2.0f,distance)*2.0);
      mOsgCamera->setProjectionMatrixAsPerspective( vfov, width/height, 1.0f,10000.0f);
      
      SetClearColor( 0.2f, 0.2f, 0.6f, 1.f);
   
      // Default collision category = 1
      SetCollisionCategoryBits( UNSIGNED_BIT(1));
   
      mScreenShotTaker = new ScreenShotCallback;
      mOsgCamera->setPostDrawCallback(mScreenShotTaker.get());
   }
   
   
   Camera::Camera(dtCore::View * view, const std::string& name)
      :  Transformable(name),
         mFrameBin(0),
         mView(view),
         mAddedToSceneGraph(false),
         mEnable(true)
   {
      RegisterInstance(this);
      
      System*  sys   = &dtCore::System::GetInstance();
      assert( sys );
      AddSender( sys );
      
      if(view == NULL)
      {
         throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
            "Supplied dtCore::View is NULL", __FILE__, __LINE__);
      }
      
      if(view->GetOsgViewerView() == NULL)
      {
         throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
            "Supplied dtCore::View::GetOsgViewerView() is NULL", __FILE__, __LINE__);
      }
         
      if(view->GetOsgViewerView()->getCamera() == NULL)
      {
         throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
            "Supplied dtCore::View::GetOsgViewerView()->getCamera() is NULL", __FILE__, __LINE__);
      }
      mOsgCamera = view->GetOsgViewerView()->getCamera();
      
      SetClearColor( 0.2f, 0.2f, 0.6f, 1.f);
      
      // Default collision category = 1
      SetCollisionCategoryBits( UNSIGNED_BIT(1) );

      mScreenShotTaker = new ScreenShotCallback;
      mOsgCamera->setPostDrawCallback(mScreenShotTaker.get());
   }

   Camera::~Camera()
   {
      DeregisterInstance(this);
      RemoveSender( &dtCore::System::GetInstance() );
   }

//   void Camera::SetFrameBin( unsigned int frameBin )
//   {
//      mCameraGroup->RemoveCamera(this);
//
//      mFrameBin = frameBin;
//
//      mCameraGroup->AddCamera(this);
//   }

   const std::string Camera::TakeScreenShot(const std::string& namePrefix)
   {
      time_t currTime;
      time(&currTime);

      std::string timeString = dtUtil::TimeAsUTC(currTime);
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
   
   void Camera::SetEnabled( bool enabled )
   {
      mEnable = enabled;      
   }

   bool Camera::GetEnabled() const
   {
       return (mEnable);
//       return mCamera->isEnabled(); 
//       TODO manage the enable disable of Camera or push this in View
   }

   void Camera::OnMessage( MessageData* data )
   {
      if( data->message == "preframe" )
      {
         PreFrame( *static_cast<const double*>(data->userData) );
      }
   }
   
   /*!
    * Render the next frame.  This will update the scene graph, cull then geometry,
    * then draw the geometry.
    * @param lastCamera Pass true if this is the last camera drawn this frame,
    * otherwise false.
    */
   void Camera::PreFrame( const double /*deltaFrameTime*/ )
   {
//      // Only do our normal Camera stuff if it is enabled.
//      // If Producer::Camera::frame is never called, our cull callback
//      // will never be called either.
//      if( !GetEnabled() )
//      {
//         return;
//      }
//
//      // We also do not want to perform frame if we do not have a valid
//      // window.
//      if( !mWindow.valid() )
//      {
//         // This is a special case for dtABC::Widget. Normally a Window will
//         // be set via SetWindow (wow). But when a Widget is configured it
//         // creates a DeltaWin, but does not directly call SetWindow. Instead
//         // it creates its own Producer::RenderSurface and never tells poor
//         // ol' Camera. However it does set the Producer::Window handle on
//         // the RenderSurface we are using. So if SetWindow is never called
//         // (i.e. !mWindow.valid()) let's check if we have a valid
//         // Producer::Window.
//         Producer::Window pWindow(0);
//
//         if( Producer::RenderSurface* rs = mCamera->getRenderSurface() )
//         {
//            pWindow = rs->getWindow();
//         }
//
//         if( pWindow == 0 )
//         {
//            return;
//         }
//      }
//
//      if( mScene.valid() && !System::GetInstance().GetPause() )
//      {
//         // TODO: Investigate double updates when we have multiple camera.
//         // Anything with an update callback may be called twice!
//         GetSceneHandler()->GetSceneView()->update(); //osgUtil::SceneView update
//      }

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
         // Find the transform in World coorindates, but leave out
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


   void Camera::SetPerspective(double hfov, double vfov, double nearClip, double farClip)
   {
      double ratio = 0;
      
      if (!mWindow.valid() || !mWindow->GetOsgViewerGraphicsWindow())
      {
         double height = osg::DisplaySettings::instance()->getScreenHeight();
         double width = osg::DisplaySettings::instance()->getScreenWidth();
         ratio = width / height;
      }
      else
      {
         const osg::GraphicsContext::Traits * traits = mWindow->GetOsgViewerGraphicsWindow()->getTraits();
         ratio = traits->width / traits->height;
      }
      
      mOsgCamera->setProjectionMatrixAsPerspective(hfov, ratio , nearClip, farClip);
   }

   void Camera::SetFrustum(double left, double right, double bottom, double top, double nearClip, double farClip)
   {
      mOsgCamera->setProjectionMatrixAsFrustum(left, right, bottom, top, nearClip, farClip);
   }

   void Camera::SetOrtho( double left, double right, double bottom, double top, double nearClip, double farClip )
   {
      mOsgCamera->setProjectionMatrixAsOrtho(left, right, bottom, top, nearClip, farClip);
   }

   void Camera::SetClearColor(float r, float g, float b, float a)
   {
      osg::Vec4 color(r, g, b, a);

      SetClearColor(color);
   }

   void Camera::SetClearColor(const osg::Vec4& color)
   {
       mOsgCamera->setClearColor(color);
   }

   void Camera::GetClearColor( float& r, float& g, float& b, float& a )
   {
      osg::Vec4 color = mOsgCamera->getClearColor();
       
      r = color[0];
      g = color[1];
      b = color[2];
      a = color[3];
   }
   
   void Camera::GetClearColor(osg::Vec4& color)
   {
      color = mOsgCamera->getClearColor();
   }

//   void Camera::ConvertToOrtho( float d )
//   {
//      mCamera->getLens()->convertToOrtho(d);
//   }
//
//   bool Camera::ConvertToPerspective( float d )
//   {
//      bool t;
//      t = mCamera->getLens()->convertToPerspective(d);
//      return t;
//   }
//
//   float Camera::GetHorizontalFov()
//   {
//      float hfov = 0.0f;
//      hfov = mCamera->getLens()->getHorizontalFov();
//      return hfov;
//   }
//
//   float Camera::GetVerticalFov()
//   {
//      float vfov = 0.0f;
//      vfov = mCamera->getLens()->getVerticalFov();
//      return vfov;
//   }


//   bool Camera::GetAutoAspect()
//   {
//      bool ar;
//      ar = mCamera->getLens()->getAutoAspect();
//      return ar;
//   }
//
//   void Camera::SetAspectRatio( double aspectRatio )
//   {
//   	mCamera->getLens()->setAspectRatio(aspectRatio);
//   }
//
//   double Camera::GetAspectRatio()
//   {
//       return mViewer->getLens()->getAspectRatio();
//   }
   //////////////////////////////////////////
   void Camera::AddedToScene( Scene* scene )
   {
      mAddedToSceneGraph = bool(scene != NULL);

      Transformable::AddedToScene(scene);
   }
   
   //////////////////////////////////////////
   dtCore::DeltaWin * Camera::CreateDeltaWin()
   {
       mWindow = new dtCore::DeltaWin("defaultWindow");
       mWindow->SetCamera(this);
       
       UpdateFromWindow();
       
       return mWindow.get();
   }
   
   //////////////////////////////////////////
   osg::Camera * Camera::CreateOsgCamera()
   {
       mOsgCamera = new osg::Camera();
       return mOsgCamera.get();
   }
   
   //////////////////////////////////////////
   void Camera::SetOsgCamera(osg::Camera * camera) 
   {
      if (camera == mOsgCamera) return;
      
      if (camera == NULL)
      {
         throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
            "Supplied osg::Camera is invalid", __FILE__, __LINE__);
      }
      
      osg::Camera * oldCamera = mOsgCamera.get();
      mOsgCamera = camera;
      
      if (mView.valid())
      {
         if (mView->GetCamera() == this)
         {
            mView->UpdateFromCamera();
         }
         else
         {
            mView->UpdateFromSlave(this, oldCamera);
         }
      }
      
      UpdateFromWindow();
   }
   //////////////////////////////////////////
   void Camera::SetWindow( DeltaWin* win )
   {
      if (mWindow == win) return;
      
      if (mWindow.valid())
      {
         mWindow->SetCamera(NULL);
      }
      
      mWindow = win;

      UpdateFromWindow();
   }
   
   void Camera::UpdateFromWindow()
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

         mOsgCamera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));

         GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;

         mOsgCamera->setDrawBuffer(buffer);
         mOsgCamera->setReadBuffer(buffer);
      }
   }
}

