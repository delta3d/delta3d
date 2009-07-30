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
#include <dtUtil/deprecationmgr.h>

#include <osg/Vec3>
#include <osg/Vec2>

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
   class DeltaDrawable;

   class KeyboardMouseHandler;
   class Keyboard;
   class Mouse;

   class DatabasePager;

   class ViewImpl;

   class DT_CORE_EXPORT View : public Base
   {
      DECLARE_MANAGEMENT_LAYER(View)

   public:
      View(const std::string& name = "view", bool useSceneLight = true);
      View(osgViewer::View* view, const std::string& name = "view", bool useSceneLight = true);

      /*
       *  This function is used by dtCore::Scene to take all the
       *  existing cameras that reference that scene and reset its scene root.
       *  This is done only when the scene's 'SceneNode' has changed.
       */
//      void ResetCameraScenes(Scene* sceneRootChanged);

      ///Deprecated Mar 10 2009 in favor of SetRenderOrder
      DEPRECATE_FUNC void SetFrameBin(unsigned int frameBin);

      ///Deprecated Mar 10 2009 in favor of GetRenderOrder
      DEPRECATE_FUNC unsigned int GetFrameBin() const;

      /**
       * SetRenderOrder take an integer argument to designate the order this View
       * is rendered in relation to other Views.
       *
       * Lower numbers get rendered first.  Example:  I have two Views called
       * ViewA and ViewB.  If I call ViewA.SetRenderOrder(0) and then call
       * ViewB.SetRenderOrder(1) then ViewA will get rendered before ViewB.
       *
       * @param rOrderNum : Designates the order that this View get rendered in
       *  relation to other views.
       *
       */
      void SetRenderOrder(unsigned int rOrderNum);

      /**
       * Get the rank of this View's render order.
       * Lower numbers get rendered first (see comments of SetRenderOrder).
       *
       * @return the the View's ranking in the rendering
       * order queue (see comments of SetRenderOrder).
       */
      unsigned int GetRenderOrder() const;


      bool AddSlave(Camera* camera);
      bool RemoveSlave(Camera* camera);

      void SetCamera(Camera* camera);
      Camera* GetCamera();
      const Camera* GetCamera() const;

      ///Supply the Scene this Camera should render
      void SetScene(Scene* scene);

      ///Get a non-const version of the supplied Scene
      Scene* GetScene();
      const Scene* GetScene() const;

      ///Get the osgViewer::View wrapped
      osgViewer::View* GetOsgViewerView();

      ///Get the osgViewer::View wrapped
      const osgViewer::View* GetOsgViewerView() const;

      /**
       * Supply an instance of a Keyboard to be used instead of the default,
       * internal Keyboard, or the one supplied in the constructor.
       * @param keyboard : instance of a valid Keyboard to use
       */
      void SetKeyboard(Keyboard* keyboard);

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
       */
      void SetMouse(Mouse* mouse);

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
      bool GetMousePickPosition(osg::Vec3 &intersectionPoint,
                                unsigned int traversalMask = 0xffffffff);

      /** Use the supplied mouse position to do an intersection into the view.
       * @param intersectionPoiont : the world coordinate intersection point
       * @param mousePos : the mouse position to use (-1..1), (-1..1)
       * @param traversalMask : the bit mask to use for the intersection traversal
       * @return true if geometry was intersected, false otherwise
       */
      bool GetPickPosition(osg::Vec3 &intersectionPoint,
                           const osg::Vec2 &mousePos,
                           unsigned int traversalMask = 0xffffffff);

      /** Use the mouse cursor position to do an intersection into the view.  Return
       * back the first DeltaDrawable if one was intersected.
       * @param traversalMask : the bit mask to use for the intersection traversal
       * @return A DeltaDrawable that was intersected, or NULL
       */
      dtCore::DeltaDrawable* GetMousePickedObject(unsigned int traversalMask = 0xffffffff);

      /** Just like GetMousePickedObject, only that screen coordinates given by position param
       * is used instead of mouse position.
       * @param mousePos The mouse position to use (-1..1), (-1..1)
       * @param traversalMask : the optional bit mask to use for the intersection traversal
       * @return A DeltaDrawable that was intersected, or NULL
       */
      dtCore::DeltaDrawable* GetPickedObject(const osg::Vec2& mousePos, unsigned int traversalMask = 0xffffffff);

      /** Supply a DatabasePager instance to overwrite the internal default pager.
       * @param pager : A custom DatabasePager (or NULL to disable database paging)
       */
      void SetDatabasePager(dtCore::DatabasePager* pager);

      /** Get the DatabasePager instance this View is using.
       * @return The internal DatabasePager (could be NULL)
       */
      dtCore::DatabasePager* GetDatabasePager();

      /** Get the DatabasePager instance this View is using.
       * @return The internal DatabasePager (could be NULL)
       */
      const dtCore::DatabasePager* GetDatabasePager() const;

   protected:
      virtual ~View();

      friend class Scene;
      void UpdateFromScene();

   private:
      ///constructor implementation
      void Ctor(bool useSceneLight);

      KeyboardMouseHandler* CreateKeyboardMouseHandler();

      ViewImpl* mImpl;
   };
} // namespace dtCore

#endif // DELTA_VIEW
