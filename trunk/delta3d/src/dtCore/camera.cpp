// camera.cpp: implementation of the Camera class.
//
//////////////////////////////////////////////////////////////////////

#include "Producer/RenderSurface"

#include "dtCore/camera.h"
#include "dtCore/scene.h"
#include <dtUtil/matrixutil.h>
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

void Camera::_SceneHandler::cull( Producer::Camera &cam) 
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

void Camera::_SceneHandler::draw( Producer::Camera &cam) 
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


Camera::Camera(string name) :
mWindow(NULL),
mScene(NULL)
{
   RegisterInstance(this);
   SetName(name);

   mCamera = new Producer::Camera();

   mSceneHandler = new _SceneHandler(false);
   mCamera->setSceneHandler( mSceneHandler.get() );

   //A Producer Camera has a default RenderSurface (a "window") so lets
   //set its "default" values in case the user doesn't supply their own
   //later on with SetWindow().

   //mDefaultRenderSurface = mCamera->getRenderSurface();
   mDefaultRenderSurface = new Producer::RenderSurface;
  
   mDefaultRenderSurface->setWindowRectangle( 100, 100, 640, 480 );
   mDefaultRenderSurface->setWindowName("defaultWin");

   SetClearColor( 0.2f, 0.2f, 0.6f, 1.f );
}

Camera::~Camera()
{
   DeregisterInstance(this);
}



/*!
 * Render the next frame.  This will update the scene graph, cull then geometry,
 * then draw the geometry.
 *
 */
void Camera::Frame()
{
   if (mScene != NULL)
   {
      GetSceneHandler()->GetSceneView()->update(); //osgUtil::SceneView update
   }

   //Get our Camera's position, up vector, and look-at vector and pass them
   //to the Producer Camera
   osg::Matrix mat = GetMatrixNode()->getMatrix();
   Transform absXform;

   osg::Matrix absMat;
   GetAbsoluteMatrix( GetMatrixNode(), absMat );

   osg::Vec3 eyePoint(absMat(3,0), absMat(3,1), absMat(3,2));

   //choose Z as the up vector
   osg::Vec3 upVec(absMat(2,0), absMat(2,1), absMat(2,2));

   osg::Vec3 lookVec = upVec ^ osg::Vec3(absMat(0,0), absMat(0,1), absMat(0,2));
   osg::Vec3 centerPoint = eyePoint + lookVec * 10.f;

   //sync up Producer's camera with the world matrix for this node
   mCamera->setViewByLookat( eyePoint[0], eyePoint[1], eyePoint[2],
                             centerPoint[0], centerPoint[1], centerPoint[2],
                             upVec[0], upVec[1], upVec[2] );
   
   //TODO should only call frame(true) if this camera is the last camera assigned to this RenderSurface
   //Might cause a problem with multi camera's sharing one RenderSurface
   mCamera->frame(true);
}

void Camera::SetWindow(DeltaWin *win)
{
   mWindow = win;
   if (mWindow == NULL)
   {
      mCamera.get()->setRenderSurface( mDefaultRenderSurface );
   }
   else
   {
      mCamera.get()->setRenderSurface( mWindow.get()->GetRenderSurface() );
   }
}

void Camera::SetScene(Scene *scene)
{
   mScene = scene;
   if (mScene == NULL)
   {
      mCamera->setSceneHandler( NULL);
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

void Camera::GetClearColor( float *r, float *g, float *b, float *a)
{
   *r = mClearColor[0];
   *g = mClearColor[1];
   *b = mClearColor[2];
   *a = mClearColor[3];
}

void Camera::SetPerspective(double hfov, double vfov, double nearClip, double farClip)
{
	mCamera.get()->getLens()->setPerspective(hfov, vfov, nearClip, farClip);

}

void Camera::SetFrustum(double left, double right, double bottom, double top, double nearClip, double farClip)
{
   mCamera.get()->getLens()->setFrustum(left, right, bottom, top, nearClip, farClip);

}

void Camera::SetOrtho( double left, double right, double bottom, double top, double nearClip, double farClip )
{
   mCamera.get()->getLens()->setOrtho(left, right, bottom, top, nearClip, farClip);
}

void Camera::ConvertToOrtho( float d )
{
   mCamera.get()->getLens()->convertToOrtho(d);
}

bool Camera::ConvertToPerspective( float d )
{
   bool t;
   t = mCamera.get()->getLens()->convertToPerspective(d);
   return t;
}

/*void Camera::Apply( float xshear=0.0f, float yshear=0.0 )
{
   mCamera.get()->getLens()->apply(xshear, yshear);
}*/

/*void Camera::GenerateMatrix( float xshear, float yshear, Matrix::value_type matrix[16] )
{
   mCamera.get()->getLens()->generateMatrix(xshear, yshear, Matrix);
}*/

/*void Camera::GetParams( double &left, double &right, 
                double &bottom, double &top, 
                double &nearClip, double &farClip )
{
	mCamera.get()->getLens()->getParams(&left, &right, &bottom, &top, &nearClip, &farClip);
}*/

float Camera::GetHorizontalFov()
{
   float hfov = 0.0f;
   hfov = mCamera.get()->getLens()->getHorizontalFov();
   return hfov;
}

float Camera::GetVerticalFov()
{
   float vfov = 0.0f;
   vfov = mCamera.get()->getLens()->getVerticalFov();
   return vfov;
}

void Camera::SetAutoAspect( bool ar )
{
   mCamera.get()->getLens()->setAutoAspect(ar);
}

bool Camera::GetAutoAspect()
{
   bool ar;
   ar = mCamera.get()->getLens()->getAutoAspect();
   return ar;
}

void Camera::SetAspectRatio( double aspectRatio )
{
	mCamera.get()->getLens()->setAspectRatio(aspectRatio);
}

double Camera::GetAspectRatio()
{
	double aspectRatio;
	aspectRatio = mCamera.get()->getLens()->getAspectRatio();
	return aspectRatio;
}
