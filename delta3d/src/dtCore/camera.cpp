// camera.cpp: implementation of the Camera class.
//
//////////////////////////////////////////////////////////////////////

#include "Producer/RenderSurface"

#include "dtCore/camera.h"

#include "dtCore/notify.h"

using namespace dtCore;


IMPLEMENT_MANAGEMENT_LAYER(Camera)
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Camera::Camera(string name) :
mWindow(NULL),
mScene(NULL)
{
   RegisterInstance(this);
   SetName(name);

   mCamera = new Producer::Camera();

   //A Producer Camera has a default RenderSurface (a "window") so lets
   //set its "default" values in case the user doesn't supply their own
   //later on with SetWindow().
   mDefaultRenderSurface = mCamera->getRenderSurface();
   mDefaultRenderSurface->setWindowRectangle( 100, 100, 640, 480 );
   mDefaultRenderSurface->setWindowName("Default Window");

   SetClearColor( 0.2f, 0.2f, 0.6f, 1.f );
}

Camera::~Camera()
{
   DeregisterInstance(this);
   Notify(DEBUG_INFO, "destroying Camera ref count:%d", this->referenceCount() );
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
      mScene->GetSceneHandler()->GetSceneView()->update(); //osgUtil::SceneView update
   }


   //Get our Camera's position, up vector, and look-at vector and pass them
   //to the Producer Camera
   sgMat4 mat;
   Transform absXform;
   //mAbsTransform->Get(mat);
   GetTransform(&absXform, ABS_CS);
   absXform.Get(mat);
   
   osg::Vec3 eyePoint(mat[3][0], mat[3][1], mat[3][2]);   
   osg::Vec3 upVec(mat[2][0], mat[2][1], mat[2][2]);
   osg::Vec3 lookVec = upVec ^ osg::Vec3(mat[0][0], mat[0][1], mat[0][2]);
   osg::Vec3 centerPoint = eyePoint + lookVec * 10.f;

   mCamera->setViewByLookat( eyePoint[0], eyePoint[1], eyePoint[2],
                             centerPoint[0], centerPoint[1], centerPoint[2],
                             upVec[0], upVec[1], upVec[2] );

   mCamera->frame();
}

void Camera::SetWindow(DeltaWin *win)
{
   mWindow = win;
   if (mWindow == NULL)
   {
      mCamera.get()->setRenderSurface( mDefaultRenderSurface.get() );
   }
   else
      mCamera.get()->setRenderSurface( mWindow.get()->GetRenderSurface() );
}

void Camera::SetScene(Scene *scene)
{
   mScene = scene;
   if (mScene == NULL)
      mCamera->setSceneHandler( NULL);
   else
   {
      mCamera->setSceneHandler( scene->GetSceneHandler() );

      //Copy our camera's clear color into the scene handler cause thats where
      //  the screen actually gets cleared.      
      osg::Vec4 clearColor;
      sgCopyVec4(clearColor._v, mClearColor);
      mScene->GetSceneHandler()->GetSceneView()->setClearColor( clearColor );
   }
}


void Camera::SetClearColor(float r, float g, float b, float a)
{
   sgVec4 color = {r,g,b,a};
   SetClearColor(color);
}

void Camera::SetClearColor(sgVec4 color)
{
   sgCopyVec4(mClearColor, color);
   
   //tell the scene handler about the change
   osg::Vec4 clearColor;
   sgCopyVec4(clearColor._v, mClearColor);
   if (mScene.get()) mScene->GetSceneHandler()->GetSceneView()->setClearColor( clearColor );
}

void Camera::GetClearColor( float *r, float *g, float *b, float *a)
{
   *r = mClearColor[0];
   *g = mClearColor[1];
   *b = mClearColor[2];
   *a = mClearColor[3];
}
