/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
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

#include "Producer/Camera"
#include "dtCore/deltawin.h"
#include "dtCore/base.h"
#include "dtCore/scene.h"
#include "dtUtil/deprecationmgr.h"
#include "dtCore/transformable.h"
#include <osg/FrameStamp>
#include <osgUtil/SceneView>
#include <osg/Vec4>

namespace dtCore
{
   
   ///A dtCore::Camera

   /** A dtCore::Camera is a view into the Scene.  It requires a dtCore::DeltaWin to 
    *  render the the Scene into.  If no DeltaWin is supplied, a default DeltaWin 
    *  will be created and will be overridden when a valid DeltaWin is supplied
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

      class DT_EXPORT _SceneHandler : public Producer::Camera::SceneHandler
      {
      public:
         _SceneHandler(bool useSceneLight=true);
         virtual ~_SceneHandler();

         osgUtil::SceneView* GetSceneView() { return mSceneView.get(); }

         virtual void clear(Producer::Camera& cam);

         void ClearImplementation( Producer::Camera& cam);

         /** 
         *  Prepare the scene by sorting, and
         *  ordering for optimal rendering
         */
         virtual void cull( Producer::Camera &cam);

         void CullImplementation( Producer::Camera &cam );

         /** 
         *  The draw() method must be implemented by
         *  the derived class for rendering the scene
         */
         virtual void draw( Producer::Camera &cam);

         void DrawImplementation( Producer::Camera &cam );

         Stats *mStats; ///<The statistics display

      protected:
         RefPtr<osgUtil::SceneView> mSceneView;
         Timer mTimer;
         //osg::Timer mTimer;
      private:
         RefPtr<osg::FrameStamp> mFrameStamp;
         //osg::Timer_t mStartTime;
         dtCore::Timer_t mStartTime;
      };

      Camera(std::string name = "camera");
      virtual ~Camera();
      
      ///Use the supplied DeltaWin to draw into
	   void SetWindow( DeltaWin *win );

      ///Get the supplied DeltaWin (could be NULL)
      DeltaWin *GetWindow(void) {return mWindow.get();}

      ///Redraw the view
	   void Frame( void );

      ///Supply the Scene this Camera should render
      void SetScene( Scene *scene );

      ///Get the supplied Scene
      Scene *GetScene(void) const {return (Scene*)mScene.get();}

      ///Set the color non-geometry in the Scene should be drawn (0.0 - 1.0)
      void SetClearColor( float r, float g, float b, float a);

      ///Set the color non-geometry in the Scene should be drawn (0.0 - 1.0)
      void SetClearColor(const osg::Vec4& v);
      
      ///Get the color that non-geometry in the Scene should be rendered
      void GetClearColor( float *r, float *g, float *b, float *a);
      
      ///Get the color that non-geometry in the Scene should be rendered
      void GetClearColor(osg::Vec4& color) {color = mClearColor;}
      
      ///Set Perspective of camera lens
      void SetPerspective( double hfov, double vfov, double nearClip, double farClip );

      ///Set view frustrum of camera lens
      void SetFrustum( double left, double right,
                        double bottom, double top,
                        double nearClip, double farClip );

      void SetOrtho( double left, double right,
                        double bottom, double top,
                        double nearClip, double farClip );

      void ConvertToOrtho( float d );

      bool ConvertToPerspective( float d );

//      void Apply( float xshear=0.0f, float yshear=0.0 );

//      void GenerateMatrix( float xshear, float yshear, Matrix::value_type matrix[16] );

//      void GetParams( double &left, double &right, 
//                      double &bottom, double &top, 
//                      double &nearClip, double &farClip );

      float GetHorizontalFov();

      float GetVerticalFov();

      void SetAutoAspect( bool ar );

      bool GetAutoAspect();

      void SetAspectRatio( double aspectRatio );

      double GetAspectRatio();

      ///Get a handle to the Producer Lens that this Camera uses
      Producer::Camera::Lens *GetLens(void) {return mCamera.get()->getLens();}
      
      ///Get a handle to the underlying Producer::Camera
      Producer::Camera *GetCamera(void)const {return (Producer::Camera*)mCamera.get();};

      _SceneHandler *GetSceneHandler(void) {return mSceneHandler.get();}

      ///Display the next statistics mode
      void SetNextStatisticsType() {mSceneHandler->mStats->SelectNextType();}     

      ///Display the supplied statistics type
      void SetStatisticsType(osgUtil::Statistics::statsType type) 
      {
        mSceneHandler->mStats->SelectType(type);
      }

   private:
      RefPtr<Producer::Camera> mCamera; // Handle to the Producer camera
      RefPtr<DeltaWin> mWindow; // The currently assigned DeltaWin
      RefPtr<Scene> mScene;
      osg::Vec4 mClearColor; // The current clear color

      Producer::RenderSurface* mDefaultRenderSurface;
      RefPtr<_SceneHandler> mSceneHandler;

   };
   
};


#endif // DELTA_CAMERA
