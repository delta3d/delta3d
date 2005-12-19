// camera.cpp: implementation of the Camera class.
//
//////////////////////////////////////////////////////////////////////

#include <Producer/RenderSurface>

#include <dtCore/camera.h>
#include <dtCore/cameragroup.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <osg/NodeVisitor>
#include <dtUtil/log.h>

#include <osg/Matrix>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(Camera)
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Camera::_SceneHandler::_SceneHandler(bool useSceneLight):
mSceneView(new osgUtil::SceneView()),
mFrameStamp(new osg::FrameStamp())
{
   mSceneView->init();
   mSceneView->setDefaults(); //osg::SceneView

   if(useSceneLight)
      mSceneView->setLightingMode(osgUtil::SceneView::SKY_LIGHT);
   else
      mSceneView->setLightingMode(osgUtil::SceneView::NO_SCENEVIEW_LIGHT);

   mSceneView->setFrameStamp(mFrameStamp.get());

   mStats = new Stats( mSceneView.get() );
   mStats->Init( mSceneView.get()->getRenderStage() );

   mStartTime = mTimer.tick();
}

Camera::_SceneHandler::~_SceneHandler()
{
   LOG_DEBUG("Destroying _SceneHandler");
}

void Camera::_SceneHandler::clear(Producer::Camera& cam)
{
   ClearImplementation( cam );
}

void Camera::_SceneHandler::ClearImplementation( Producer::Camera &cam )
{
   //Override the Producer::Camera::clear() because the 
   //  OSGUtil::SceneView::draw() does it for us.

   //So lets not do anything clearing here, ok?
}

void Camera::_SceneHandler::cull( Producer::Camera &cam ) 
{
   //call osg cull here         
   CullImplementation( cam );
}

void Camera::_SceneHandler::CullImplementation(Producer::Camera &cam)
{
   mStats->SetTime(Stats::TIME_BEFORE_CULL);

   mFrameStamp->setFrameNumber(mFrameStamp->getFrameNumber()+1);
   mFrameStamp->setReferenceTime( mTimer.delta_s( mStartTime, mTimer.tick() ) );

   //copy the Producer Camera's position to osg::SceneView  
   mSceneView->getProjectionMatrix().set(cam.getProjectionMatrix());
   mSceneView->getViewMatrix().set(cam.getPositionAndAttitudeMatrix());

   //Copy the Producer Camera's viewport info to osg::SceneView
   int x, y;
   unsigned int w, h;
   cam.getProjectionRectangle( x, y, w, h );

   mSceneView->setViewport( x, y, w, h );

   //Now tell SceneView to cull
   mSceneView->cull();

   mStats->SetTime(Stats::TIME_AFTER_CULL);
}

void Camera::_SceneHandler::draw( Producer::Camera &cam ) 
{
   //call osg draw here
   DrawImplementation( cam );
};


void Camera::_SceneHandler::DrawImplementation( Producer::Camera &cam )
{
   mStats->SetTime(Stats::TIME_BEFORE_DRAW);

   mSceneView->draw();
   mStats->SetTime(Stats::TIME_AFTER_DRAW);
   mStats->Draw();
}


CameraGroup* Camera::mCameraGroup = new CameraGroup;

Camera::Camera( const std::string& name )
   :  Transformable(name),
      mFrameBin(0),
      mWindow(0),
      mScene(0)
{
   RegisterInstance(this);

   mCamera = new Producer::Camera();

   mSceneHandler = new _SceneHandler(false);
   mCamera->setSceneHandler( mSceneHandler.get() );

   //A Producer Camera has a default RenderSurface (a "window") so lets
   //set its "default" values in case the user doesn't supply their own
   //later on with SetWindow().
   mDefaultRenderSurface = new Producer::RenderSurface;
  
   mDefaultRenderSurface->setWindowRectangle( 100, 100, 640, 480 );
   mDefaultRenderSurface->setWindowName("defaultWin");

   SetClearColor( 0.2f, 0.2f, 0.6f, 1.f );

   // Default collision category = 1
   SetCollisionCategoryBits( UNSIGNED_BIT(1) );

   mCameraGroup->AddCamera(this);
}

Camera::~Camera()
{
   mCameraGroup->RemoveCamera(this);
   DeregisterInstance(this);
}

void Camera::SetFrameBin( unsigned int frameBin )
{ 
   mCameraGroup->RemoveCamera(this);

   mFrameBin = frameBin; 

   mCameraGroup->AddCamera(this);
}

void Camera::SetEnabled( bool enabled )
{
   if( enabled )
   {
      mCamera->enable();
   }
   else
   {
      mCamera->disable();
   }
}

///\todo Do not need to const_cast with OSG 1.0
bool Camera::GetEnabled() const
{
   return const_cast<Producer::Camera*>(mCamera.get())->isEnabled();
}

/*!
 * Render the next frame.  This will update the scene graph, cull then geometry,
 * then draw the geometry.
 * @param lastCamera Pass true if this is the last camera drawn this frame,
 * otherwise false.
 */
void Camera::Frame( bool lastCamera )
{
   // Only do our normal Camera stuff if it is enabled.
   // If Producer::Camera::frame is never called, our cull callback
   // will never be called either.
   if( !GetEnabled() )
   {
      return;
   }

   // We also do not want to perform frame if we do not have a valid
   // window.
   if( !mWindow.valid() )
   {
      // This is a special case for dtABC::Widget. Normally a Window will
      // be set via SetWindow (wow). But when a Widget is configured it
      // creates a DeltaWin, but does not directly call SetWindow. Instead
      // it creates its own Producer::RenderSurface and never tells poor
      // ol' Camera. However it does set the Producer::Window handle on
      // the RenderSurface we are using. So if SetWindow is never called
      // (i.e. !mWindow.valid()) let's check if we have a valid
      // Producer::Window.
      Producer::Window pWindow(0);
      
      if( Producer::RenderSurface* rs = mCamera->getRenderSurface() )
      {
         pWindow = rs->getWindow();
      }

      if( pWindow == 0 )
      {
         return;
      }
   }
      
   if( mScene.valid() && !System::Instance()->GetPause() )
   {
      // TODO: Investigate double updates when we have multiple camera.
      // Anything with an update callback may be called twice!
      GetSceneHandler()->GetSceneView()->update(); //osgUtil::SceneView update
   }
   
   //Get our Camera's position, up vector, and look-at vector and pass them
   //to the Producer Camera
   osg::Matrix mat = GetMatrixNode()->getMatrix();
   Transform absXform;
   
   osg::Matrix absMat;
   GetAbsoluteMatrix( GetMatrixNode(), absMat );
   
   //choose Z as the up vector
   osg::Vec3 eye(-absMat(3,0), -absMat(3,1), -absMat(3,2));
   osg::Vec3 UP(absMat(2,0), absMat(2,1), absMat(2,2));
   osg::Vec3 F = UP ^ osg::Vec3(absMat(0,0), absMat(0,1), absMat(0,2));
   F.normalize();
   UP.normalize();
   
   osg::Vec3 s = F ^ UP;
   osg::Vec3 u = s ^ F;
   F = -F;
   
   Producer::Matrix m(s[0], u[0], F[0], 0.0,
                      s[1], u[1], F[1], 0.0,
                      s[2], u[2], F[2], 0.0,
                      s*eye, u*eye, F*eye, 1.0);
   mCamera->setViewByMatrix(m);
   
   mCamera->frame(lastCamera);
}

void Camera::SetWindow( DeltaWin* win )
{
   mWindow = win;

   if( mWindow == 0 )
   {
      mCamera->setRenderSurface( mDefaultRenderSurface );
   }
   else
   {
      mCamera->setRenderSurface( mWindow->GetRenderSurface() );
   }
}

void Camera::SetScene(Scene *scene)
{
   mScene = scene;

   if(mScene == 0)
   {
      mCamera->setSceneHandler(0);
   }
   else
   {
      //Copy our camera's clear color into the scene handler cause thats where
      //  the screen actually gets cleared.      
      GetSceneHandler()->GetSceneView()->setClearColor( mClearColor );

      //assign the supplied scene to the SceneView
      GetSceneHandler()->GetSceneView()->setSceneData( scene->GetSceneNode() );
   }
}


void Camera::SetClearColor(float r, float g, float b, float a)
{
   mClearColor.set(r, g, b, a);

   //tell the scene handler about the change
   GetSceneHandler()->GetSceneView()->setClearColor(mClearColor);
}

void Camera::SetClearColor(const osg::Vec4& color)
{
   mClearColor = color;
   
   //tell the scene handler about the change
   GetSceneHandler()->GetSceneView()->setClearColor(mClearColor);
}

void Camera::GetClearColor( float& r, float& g, float& b, float& a )
{
   r = mClearColor[0];
   g = mClearColor[1];
   b = mClearColor[2];
   a = mClearColor[3];
}

void Camera::SetPerspective(double hfov, double vfov, double nearClip, double farClip)
{
	mCamera->getLens()->setPerspective(hfov, vfov, nearClip, farClip);

}

void Camera::SetFrustum(double left, double right, double bottom, double top, double nearClip, double farClip)
{
   mCamera->getLens()->setFrustum(left, right, bottom, top, nearClip, farClip);

}

void Camera::SetOrtho( double left, double right, double bottom, double top, double nearClip, double farClip )
{
   mCamera->getLens()->setOrtho(left, right, bottom, top, nearClip, farClip);
}

void Camera::ConvertToOrtho( float d )
{
   mCamera->getLens()->convertToOrtho(d);
}

bool Camera::ConvertToPerspective( float d )
{
   bool t;
   t = mCamera->getLens()->convertToPerspective(d);
   return t;
}

float Camera::GetHorizontalFov()
{
   float hfov = 0.0f;
   hfov = mCamera->getLens()->getHorizontalFov();
   return hfov;
}

float Camera::GetVerticalFov()
{
   float vfov = 0.0f;
   vfov = mCamera->getLens()->getVerticalFov();
   return vfov;
}

void Camera::SetAutoAspect( bool ar )
{
   mCamera->getLens()->setAutoAspect(ar);
}

bool Camera::GetAutoAspect()
{
   bool ar;
   ar = mCamera->getLens()->getAutoAspect();
   return ar;
}

void Camera::SetAspectRatio( double aspectRatio )
{
	mCamera->getLens()->setAspectRatio(aspectRatio);
}

double Camera::GetAspectRatio()
{
	double aspectRatio;
	aspectRatio = mCamera->getLens()->getAspectRatio();
	return aspectRatio;
}
