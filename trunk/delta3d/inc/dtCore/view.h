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

#include <map>
#include <set>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Camera;
}

namespace osgViewer
{
   class View;
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
