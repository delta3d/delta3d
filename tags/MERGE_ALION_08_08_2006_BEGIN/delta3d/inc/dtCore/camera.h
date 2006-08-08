/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
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

#include <Producer/Camera>
#include <dtCore/stats.h>
#include <dtCore/timer.h>
#include <dtCore/refptr.h>
#include <dtCore/transformable.h>
#include <osg/Vec4>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class FrameStamp;
}

namespace osgUtil
{
   class SceneView;
}
/// @endcond

namespace dtCore
{
   class CameraGroup;
   class DeltaWin;
   class Scene;

   /**
    * A dtCore::Camera is a view into the Scene.  It requires a dtCore::DeltaWin to 
    * render the the Scene into.  If no DeltaWin is supplied, a default DeltaWin 
    * will be created and will be overridden when a valid DeltaWin is supplied
    * using SetWindow().
    * 
    * The method SetScene() supplies the geometry to be rendered from the 
    * Camera's point of view.
    *
    * Any part of the Scene that doesn't contain renderable geometry will be
    * drawn a solid color using the values supplied to SetClearColor().
    *
    * The Frame() method must be called once per frame.  This will update the
    * scene, then cull and draw the visual objects.
    */
   class DT_CORE_EXPORT Camera : public Transformable
   {
      DECLARE_MANAGEMENT_LAYER(Camera)

   public:

      class DT_CORE_EXPORT _SceneHandler : public Producer::Camera::SceneHandler
      {
      public:
         _SceneHandler(bool useSceneLight=true);
      protected:
         virtual ~_SceneHandler();
      public:
         osgUtil::SceneView* GetSceneView() { return mSceneView.get(); }

         virtual void clear(Producer::Camera& cam);

         void ClearImplementation( Producer::Camera& cam );

         /** 
         *  Prepare the scene by sorting, and
         *  ordering for optimal rendering
         */
         virtual void cull( Producer::Camera &cam );

         void CullImplementation( Producer::Camera &cam );

         /** 
         *  The draw() method must be implemented by
         *  the derived class for rendering the scene
         */
         virtual void draw( Producer::Camera &cam );

         void DrawImplementation( Producer::Camera &cam );

         Stats *mStats; ///<The statistics display

      protected:
         RefPtr<osgUtil::SceneView> mSceneView;
         dtCore::Timer mTimer;
      private:
         RefPtr<osg::FrameStamp> mFrameStamp;
         dtCore::Timer_t mStartTime;
      };

      Camera( const std::string& name = "camera" );      

   protected:

      virtual ~Camera();

   public:

      void SetFrameBin( unsigned int frameBin );
      unsigned int GetFrameBin() const { return mFrameBin; }

      static CameraGroup* GetCameraGroup() { return mCameraGroup; }

      ///Enabled or disable this Camera. Disabled Cameras will not render.
      void SetEnabled( bool enabled );

      ///Is this Camera enabled?
      bool GetEnabled() const;
      
      ///Use the supplied DeltaWin to draw into
	   void SetWindow( DeltaWin *win );

      ///Get the supplied DeltaWin (could be NULL)
      DeltaWin* GetWindow();
      /** 
       *  Redraw the view.
       *  @param lastCamera Pass true if this is the last camera
       *  rendered this frame number, otherwise false.
       */
	   void Frame( bool lastCamera );

      ///Supply the Scene this Camera should render
      void SetScene( Scene *scene );

      ///Get a non-const version of the supplied Scene
      Scene* GetScene() { return mScene.get(); }

      ///Get a const version of the supplied Scene
      const Scene* GetScene() const { return mScene.get(); }

      ///Set the color non-geometry in the Scene should be drawn (0.0 - 1.0)
      void SetClearColor( float r, float g, float b, float a);

      ///Set the color non-geometry in the Scene should be drawn (0.0 - 1.0)
      void SetClearColor(const osg::Vec4& v);
      
      ///Get the color that non-geometry in the Scene should be rendered
      void GetClearColor( float& r, float& g, float& b, float& a);
      
      ///Get the color that non-geometry in the Scene should be rendered
      void GetClearColor(osg::Vec4& color) { color = mClearColor; }
      
      ///Set Perspective of camera lens
      void SetPerspective( double hfov, double vfov, double nearClip, double farClip );

      ///Set view frustrum of camera lens
      void SetFrustum(  double left, double right,
                        double bottom, double top,
                        double nearClip, double farClip );

      ///set orthographic mode
      void SetOrtho(    double left, double right,
                        double bottom, double top,
                        double nearClip, double farClip );

      ///enable orthographic mode
      void ConvertToOrtho( float d );

      ///enable perspective mode
      bool ConvertToPerspective( float d );

      ///@return HOV
      float GetHorizontalFov();

      ///@return FOV
      float GetVerticalFov();

      void SetAutoAspect( bool ar );

      bool GetAutoAspect();

      ///takes a number from 0-1 to set as the aspect ratio
      void SetAspectRatio( double aspectRatio );

      ///@return aspect ratio
      double GetAspectRatio();

      ///Get a handle to the Producer Lens that this Camera uses
      Producer::Camera::Lens *GetLens() { return mCamera->getLens(); }

      ///Get a non-const handle to the underlying Producer::Camera
      Producer::Camera* GetCamera() { return mCamera.get(); }
      
      ///Get a const handle to the underlying Producer::Camera
      const Producer::Camera* GetCamera() const { return mCamera.get(); }

      _SceneHandler *GetSceneHandler() { return mSceneHandler.get(); }

      ///Display the next statistics mode
      void SetNextStatisticsType() { mSceneHandler->mStats->SelectNextType(); }     

      ///Display the supplied statistics type
      void SetStatisticsType(osgUtil::Statistics::statsType type) 
      {
        mSceneHandler->mStats->SelectType(type);
      }

      /** 
      * Supply the Scene this Camera has been added to. Normally this
      * is done inside dtCore::Scene. So you should probably never have 
      * to call this.
      *
      * @param scene The Scene to which this Camera is being added to.
      */
      virtual void AddedToScene( Scene* scene );


   private:

      // Disallowed to prevent compile errors on VS2003. It apparently
      // creates this functions even if they are not used, and if
      // this class is forward declared, these implicit functions will
      // cause compiler errors for missing calls to "ref".
      Camera& operator=( const Camera& ); 
      Camera( const Camera& );

      unsigned int mFrameBin;
      static CameraGroup* mCameraGroup;

      RefPtr<Producer::Camera> mCamera; // Handle to the Producer camera
      RefPtr<DeltaWin> mWindow; // The currently assigned DeltaWin
      RefPtr<Scene> mScene;
      osg::Vec4 mClearColor; // The current clear color
      bool mAddedToSceneGraph;

      Producer::RenderSurface* mDefaultRenderSurface;
      RefPtr<_SceneHandler> mSceneHandler;

   };
}

#endif // DELTA_CAMERA
