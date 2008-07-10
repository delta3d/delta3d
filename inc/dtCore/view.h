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
#include <dtCore/databasepager.h>
#include <dtUtil/deprecationmgr.h>
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

      
       /** Supply a DatabasePager instance to overwrite the internal default pager.
         * @param pager : A custom DatabasePager (or NULL to disable database paging)
         */
       void SetDatabasePager( dtCore::DatabasePager *pager );

       /** Get the DatabasePager instance this View is using.  
         * @return The internal DatabasePager (could be NULL)
         */
       dtCore::DatabasePager* GetDatabasePager();

       /** Get the DatabasePager instance this View is using.  
       * @return The internal DatabasePager (could be NULL)
       */
       const dtCore::DatabasePager* GetDatabasePager() const;


       ///Deprecated 07/01/08
       void EnablePaging()
       {
          DEPRECATE("dtCore::View::EnablePaging()",
                    "void dtCore::View::SetDatabasePager( dtCore::DatabasePager *pager )");
       }

       ///Deprecated 07/01/08
       void DisablePaging()
       {
          DEPRECATE("dtCore::View::DisablePaging()",
                    "void dtCore::View::SetDatabasePager( dtCore::DatabasePager *pager )");
       }

       ///Deprecated 07/01/08
       void SetTargetFrameRate(double pTargetFR)
       { 
          DEPRECATE("dtCore::View::SetTargetFrameRate(double)",
             "dtCore::DatabasePager::SetTargetFrameRate(double)");

          GetDatabasePager()->SetTargetFrameRate(pTargetFR);
       }

       ///Deprecated 07/01/08
       double GetTargetFrameRate()
       { 
          DEPRECATE("double dtCore::View::GetTargetFrameRate()",
             "double dtCore::DatabasePager::GetTargetFrameRate() const");

          return GetDatabasePager()->GetTargetFrameRate();
       }

   protected:

      virtual ~View();

      
      friend class Scene;
      void UpdateFromScene();
           
      
   private:
 
      ///constructor implementation
      void Ctor(bool useSceneLight);

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

      RefPtr<DatabasePager> mPager; ///< The pager this View will use (could be NULL)
   };
}

#endif //DELTA_VIEW
