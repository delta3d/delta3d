#ifndef DELTA_CAMERA
#define DELTA_CAMERA

// camera.h: interface for the Camera class.
//
//////////////////////////////////////////////////////////////////////

#include "Producer/Camera"
#include "window.h"
#include "base.h"
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
   class DT_EXPORT Camera : public Transformable
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


#endif // DELTA_CAMERA
