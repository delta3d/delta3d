#ifndef DELTA_COMPASS
#define DELTA_COMPASS

#include <osg/ref_ptr>
#include <osg/MatrixTransform>

#include <macros.h>
#include <transformable.h>
#include <deltadrawable.h>
#include <deltawin.h>
#include <camera.h>



namespace dtCore
{

   ///A special visual Object which constantly aligns with the world axes

   /** The Compass represents visual axes in the Scene.  The Compass is positioned
    *  by screen space and is set in it's own renderbin to render after everything else.
    *  The compass is derived from Object so the user could manipulate it's translations,
    *  however, this is discouraged.
    *
    * The Compass must be added to a Scene to be viewed using Scene::AddObject().
    */

   class DT_EXPORT Compass :   public   Transformable, public   DeltaDrawable
   {
      DECLARE_MANAGEMENT_LAYER(Compass)

      private:
         static   const float       MAX_SCREEN_X;
         static   const float       MAX_SCREEN_Y;
         static   const float       MIN_SCREEN_X;
         static   const float       MIN_SCREEN_Y;
         static   const float       DEF_SCREEN_X;
         static   const float       DEF_SCREEN_Y;
         static   const float       DEF_SCREEN_W;
         static   const float       DEF_SCREEN_H;
         static   const float       DEF_AXIS_SIZE;

      public:
                                    Compass( dtCore::Camera* cam );
         virtual                    ~Compass();
         
         ///Get a handle to the OSG Node
         virtual  osg::Node*        GetOSGNode( void );

         virtual  void              GetScreenPosition( float& x, float& y )   const;
         virtual  void              SetScreenPosition( float x, float y );

         virtual  dtCore::Camera*      GetCamera( void );
         virtual  void              SetCamera( dtCore::Camera* cam );

      private:
         inline   void              ctor( void );
         inline   void              SetWindow( dtCore::DeltaWin* win );

      private:
                  osg::ref_ptr<osg::MatrixTransform>  mNode;      /// contains the actual model
                  osg::ref_ptr<dtCore::Camera>           mCamera;    /// camera who's window we place the model
                  float                               mScreenX;   /// screen position of model
                  float                               mScreenY;   /// screen position of model
                  float                               mScreenW;   /// screen width
                  float                               mScreenH;   /// screen height
   };

};

#endif // DELTA_COMPASS
