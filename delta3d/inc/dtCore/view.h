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

#ifndef DELTA_VIEW
#define DELTA_VIEW

#include <dtCore/export.h>
#include <dtCore/base.h>
#include <dtCore/refptr.h>


#include <osg/Referenced>
#include <osgViewer/View>

#include <map>
#include <set>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Camera;
}
/// @endcond

namespace dtCore
{
   class Camera;
   class Scene;

   class KeyboardMouseHandler;
   class Keyboard;
   class Mouse;
   
   class DT_CORE_EXPORT View : public Base
   {
      DECLARE_MANAGEMENT_LAYER(View)

   public:

      /**
       * Configuration property.
       * <br>
       * Set to true or false
       * <br>
       * This will set the database pager for the default view to precompile gl objects or not.
       * If they are not precomplied, they will be compiled when they are first viewed, which can
       * cause major frame hiccups in some cases.  It defaults to true.
       * @see osgDB::DatabasePager
       */
      static const std::string DATABASE_PAGER_PRECOMPILE_OBJECTS;

      /**
       * Configuration property.
       * <br>
       * Unsigned integer
       * <br>
       * The max number of GL objects to compile per frame.  This number should be low.  The default is
       * 2.  Making it higher will make tiles page in sooner, but it will also cause more of a frame
       * hiccup.
       * @see osgDB::DatabasePager
       */
      static const std::string DATABASE_PAGER_MAX_OBJECTS_TO_COMPILE_PER_FRAME;

      /**
       * Configuration property.
       * <br>
       * floating point number
       * <br>
       * The minimum amount of time to allocate from pre-compiling GL objects in a paged database
       * @see osgDB::DatabasePager
       */
      static const std::string DATABASE_PAGER_MIN_TIME_FOR_OBJECT_COMPILE;

      /**
       * Configuration property.
       * <br>
       * floating point in frames per second.
       * <br>
       * The target frame rate.  The pager uses this to time certain operations.  
       * If the system is set to use a fixed time step, the target frame rate is set to match the
       * fixed frame rate (SIM_FRAME_RATE). Otherwise, it defaults to 100.
       * Turning this number down can improve paging preformance somewhat.
       * @see osgDB::DatabasePager
       */
      static const std::string DATABASE_PAGER_TARGET_FRAMERATE;

      /**
       * Configuration property.
       * <br>
       * DoNotModify, DisplayList, VBO or VertexArrays
       * <br>
       * This defaults to DoNotModify, but changing it will tell the pager to alter the drawing settings
       * on loaded pages.  DisplayList will force drawables to draw using display lists.  VBO will tell
       * drawables to draw using Vertex Buffer Objects.  VertexArrays will tell the drawables to draw using
       * standard vertex arrays with no performance tricks.
       * @see osgDB::DatabasePager
       */
      static const std::string DATABASE_PAGER_DRAWABLE_POLICY;

      /**
       * Configuration property.
       * <br>
       * Set the thread priority to DEFAULT, MIN, LOW, NOMINAL, HIGH or MAX.
       * @see osgDB::DatabasePager
       */
      static const std::string DATABASE_PAGER_THREAD_PRIORITY;

      /**
       * Configuration property.
       * <br>
       * floating point in seconds.
       * <br>
       * the time in seconds of not being rendered before the database pager deletes pages.
       * @see osgDB::DatabasePager
       */
      static const std::string DATABASE_PAGER_EXPIRY_DELAY;

      View(const std::string& name = "view", bool useSceneLight = true);
      View(osgViewer::View * view, const std::string& name = "view", bool useSceneLight = true);

      /*
      *  This function is used by dtCore::Scene to take all the 
      *  existing cameras that reference that scene and reset its scene root.
      *  This is done only when the scene's 'SceneNode' has changed.
      */
//      void ResetCameraScenes(Scene* sceneRootChanged);

      void SetFrameBin( unsigned int frameBin ) { mFrameBin = frameBin; }
      unsigned int GetFrameBin() const { return mFrameBin; }
      
      
      bool AddSlave( Camera* camera );
      bool RemoveSlave( Camera* camera );
      
      void SetCamera( Camera* camera );
      Camera* GetCamera() { return mCamera.get(); }
      const Camera* GetCamera() const { return mCamera.get(); }
      
      ///Supply the Scene this Camera should render
      void SetScene( Scene *scene );

      ///Get a non-const version of the supplied Scene
      Scene* GetScene() { return (mScene.get()); }
      const Scene* GetScene() const { return (mScene.get()); }
      
      ///Get the osgViewer::View wrapped
      osgViewer::View * GetOsgViewerView() { return mOsgViewerView.get(); }
      
      ///Get the osgViewer::View wrapped
      const osgViewer::View * GetOsgViewerView() const { return mOsgViewerView.get(); }

      /** 
       * Supply an instance of a Keyboard to be used instead of the default, 
       * internal Keyboard, or the one supplied in the constructor.
       * @param keyboard : instance of a valid Keyboard to use
       * @pre keyboard != 0
       * @exception dtCore::ExceptionEnum::INVALID_PARAMETER The supplied instance
       * is NULL.  The original Keyboard will still be used.
       */
      void SetKeyboard( Keyboard* keyboard );
      
      ///Get a handle to the Keyboard associated with the DeltaWin
      Keyboard* GetKeyboard();
      const Keyboard* GetKeyboard() const;
      
//      /// Turns off/on key repeat. It only allows turning it off for X11.
//      void SetKeyRepeat(bool on);
//      /// @return true if key repeat is on.  On windows and mac this just returns true.
//      bool GetKeyRepeat() const;
      
      /** Supply an instance of a Mouse to be used instead of the default internal
       *  Mouse, or the one supplied in the constructor.
       *  @param mouse : Instance of a valid Mouse
       *  @pre mouse != 0
       *  @exception dtCore::ExceptionEnum::INVALID_PARAMETER The supplied instance
       *  is NULL.  The original Mouse will still be used.
       */
      void SetMouse( Mouse* mouse );

      ///Get a handle to the Mouse associated with the DeltaWin
      Mouse* GetMouse();
      const Mouse* GetMouse() const;
      
      
      //this method is commented out in the implementation
      //void Frame();
      
      /**
       * Enables paging when called ONLY AFTER a page-able
       * node has been added to the scene
       * @note all settings must be made before this call
       */
      void EnablePaging();
      
      /**
       *  Disables Paging, after enabled
       *  called on scene cleanup
       */
      void DisablePaging();

      /**
        * Sets target frame rate for database pager, default 60
        * @param target framerate for paging thread in Frames / Sec
        */
       void SetTargetFrameRate(double pTargetFR){ mTargetFrameRate = pTargetFR; }

       /**
         * return target frame rate for database pager, default 60
         */
       double GetTargetFrameRate(){ return mTargetFrameRate; }

       /** Use the mouse cursor position to do an intersection into the view.
         * @param intersectionPoint : the world coordinate intersection point
         * @param traversalMask : the bit mask to use for the intersection traversal
         * @return true if geometry was intersected, false otherwise
         */
       bool GetMousePickPosition( osg::Vec3 &intersectionPoint,
                                  unsigned int traversalMask = 0xffffffff);

       /** Use the supplied mouse position to do an intersection into the view.
         * @param intersectionPoiont : the world coordinate intersection point
         * @param mousePos : the mouse position to use (-1..1), (-1..1)
         * @param traversalMask : the bit mask to use for the intersection traversal
         * @return true if geometry was intersected, false otherwise
         */
       bool GetPickPosition( osg::Vec3 &intersectionPoint, 
                             const osg::Vec2 &mousePos, 
                             unsigned int traversalMask = 0xffffffff );

      
   protected:

      virtual ~View();

      
      friend class Scene;
      void UpdateFromScene();
           
      
   private:


      KeyboardMouseHandler * GetKeyboardMouseHandler() { return mKeyboardMouseHandler.get(); }

      dtCore::KeyboardMouseHandler * CreateKeyboardMouseHandler();
            
      typedef std::set<dtCore::RefPtr<dtCore::Camera> > CameraSet;
      /// set of osg::Camera wrapping the mView camera slave list
      CameraSet mCameraSlave;

      /// osgViewer::View wrapped
      RefPtr<osgViewer::View> mOsgViewerView;
      
      /// osg::Camera wrapping the mView master camera
      RefPtr<dtCore::Camera> mCamera;
      /// osgViewer::Scene wrapping the mView scene
      RefPtr<dtCore::Scene> mScene;
      
      /// KeyboardMouseHandler to handle osgViewer::View event, intern variable provide access to Mouse and Keyboard
      RefPtr<dtCore::KeyboardMouseHandler> mKeyboardMouseHandler;
      
      double mTargetFrameRate;
      unsigned int mFrameBin;
   };
}

#endif //DELTA_VIEW
