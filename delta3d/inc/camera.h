// camera.h: interface for the Camera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERA_H__45494578_1EF4_493B_86C9_F2E3DB98ED68__INCLUDED_)
#define AFX_CAMERA_H__45494578_1EF4_493B_86C9_F2E3DB98ED68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Producer\camera"
#include "base.h"
#include "window.h"
#include "scene.h"
#include "sg.h"
#include "transformable.h"


namespace dtCore
{
   
   ///A dtCore::Camera

   /** A dtCore::Camera is a view into the Scene.  It requires a dtCore::Window to 
    *  render the the Scene into.  If no Window is supplied, a default Window 
    *  will be created and will be overridden when a valid Window is supplied
    *  using SetWindow().
    * 
    *  The method SetScene() supplies the geometry to be rendered from the 
    *  Camera's point of view.
    *
    *  Any part of the Scene that doesn't contain renderable geometry will be
    *  drawn a solid color using the values supplied to SetClearColor().
    *
    *  The Frame() method must be called once per frame.  This will update the
    *  scene, then cull and draw the visual objects.
    */
   class Camera : public Transformable
   {
      DECLARE_MANAGEMENT_LAYER(Camera)

   public:
      Camera(std::string name = "camera");
      virtual ~Camera();
      
      ///Use the supplied Window to draw into
	   void SetWindow( Window *win );

      ///Get the supplied Window (could be NULL)
      Window *GetWindow(void) {return mWindow.get();}

      ///Redraw the view
	   void Frame( void );

      ///Supply the Scene this Camera should render
      void SetScene( Scene *scene );

      ///Get the supplied Scene
      Scene *GetScene(void) const {return (Scene*)mScene.get();}

      ///Set the color non-geometry in the Scene should be drawn (0.0 - 1.0)
      void SetClearColor( float r, float g, float b, float a);

      ///Set the color non-geometry in the Scene should be drawn (0.0 - 1.0)
      void SetClearColor( sgVec4 color );

      ///Get the color that non-geometry in the Scene should be rendered
      void GetClearColor( float *r, float *g, float *b, float *a);
      
      ///Get the color that non-geometry in the Scene should be rendered
      void GetClearColor( sgVec4 color ) {sgCopyVec4(color, mClearColor);}

      ///Get a handle to the Producer Lens that this Camera uses
      Producer::Camera::Lens *GetLens(void) {return mCamera.get()->getLens();}
      
      ///Get a handle to the underlying Producer::Camera
      Producer::Camera *GetCamera(void)const {return (Producer::Camera*)mCamera.get();};
      

   private:
      osg::ref_ptr<Producer::Camera> mCamera; ///<Handle to the Producer camera
      osg::ref_ptr<Window> mWindow; ///<The currently assigned Window
      osg::ref_ptr<Scene> mScene;
      sgVec4 mClearColor; ///<The current clear color

      /// The Producer "build-in" RenderSurface.
      osg::ref_ptr<Producer::RenderSurface> mDefaultRenderSurface;
   };
   
};


//
// Automatic library inclusion macros that use the #pragma/lib feature
//
#undef _AUTOLIBNAME
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2
#undef _AUTOLIBNAME3
#undef _AUTOLIBNAME4
#if defined(_DEBUG)
   #define _AUTOLIBNAME  "Producerd.lib"
   #define _AUTOLIBNAME4 "OpenThreadsWin32d.lib"  
#else
   #define _AUTOLIBNAME  "Producer.lib"
   #define _AUTOLIBNAME4 "OpenThreadsWin32.lib"  
#endif
#define _AUTOLIBNAME1  "sg.lib"
#define _AUTOLIBNAME2  "ul.lib"
#define _AUTOLIBNAME3  "winmm.lib"


/* You may turn off this include message by defining _NOAUTOLIB */
#ifndef _NOAUTOLIBMSG
   #pragma message( "Will automatically link with " _AUTOLIBNAME )
   #pragma message( "Will automatically link with " _AUTOLIBNAME1 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME2 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME3 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME4 )
#endif

#pragma comment(lib, _AUTOLIBNAME)
#pragma comment(lib, _AUTOLIBNAME1)
#pragma comment(lib, _AUTOLIBNAME2)
#pragma comment(lib, _AUTOLIBNAME4)


#endif // !defined(AFX_CAMERA_H__45494578_1EF4_493B_86C9_F2E3DB98ED68__INCLUDED_)
