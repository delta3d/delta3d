/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Matthew W. Campbell
 * David Guthrie
 */

#ifndef DELTA_STAGE_VIEWPORT
#define DELTA_STAGE_VIEWPORT

#include <QtOpenGL/QGLWidget>
#include <QtGui/QWidget>
#include <QtGui/QCursor>

#include <map>

#include <dtUtil/enumeration.h>
#include <dtCore/actorproxy.h>
#include <dtEditQt/stagecamera.h>
#include <dtEditQt/viewportmanager.h>
#include <dtQt/typedefs.h>
#include <dtCore/refptr.h>
#include <dtCore/deltawin.h>
#include <dtCore/view.h>
#include <dtEditQt/export.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class StateSet;
   class ClearNode;
}
/// @endcond

namespace dtCore
{
   class Isector;
   class Scene;
}

namespace dtEditQt
{

   /**
    * This is the base viewport class.  The viewport class acts as the
    * primary mechanism for viewing any Delta3D scene from within the
    * level editor.  It provides mechanisms for picking objects in the
    * scene and controls the styles by which the scene is rendered.
    * @note
    *  The ViewportManager must be used to create viewport objects.  This
    *  way, the graphics context may be shared between them.
    * @note
    *  Since this is the base viewport class, specific behavior, such as
    *  perspective and orthographic views are sub-classes.
    * @see ViewportManager
    * @see PerspectiveViewport
    * @see OrthoViewport
    */
   class DT_EDITQT_EXPORT Viewport : public QObject
   {
      Q_OBJECT

   public:
      /**
       * The RenderStyle enumeration contains the different ways in which
       * a particular viewport can render its scene.
       */
      class DT_EDITQT_EXPORT RenderStyle : public dtUtil::Enumeration
      {
         DECLARE_ENUM(RenderStyle);

      public:
         static const RenderStyle WIREFRAME;
         static const RenderStyle LIT;
         static const RenderStyle TEXTURED;
         static const RenderStyle LIT_AND_TEXTURED;

      private:
         RenderStyle(const std::string& name)
            : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
      };

      /**
       * The InteractionMode enumeration defines the different "modes"
       * that may be active for a particular viewport.  These modes
       * determine the way a viewport responds to user input as well as
       * how the scene is manipulated.
       */
      class DT_EDITQT_EXPORT InteractionMode : public dtUtil::Enumeration
      {
         DECLARE_ENUM(InteractionMode);

      public:
         /**
          * Default interaction mode when mouse buttons are not pressed.
          */
         static const InteractionMode NOTHING;

         /**
          * If in this mode, user input is translated into camera movement,
          * thus allowing the user to navigate the current scene.
          */
         static const InteractionMode CAMERA;

         /**
          * This mode allows selected actors to be moved.
          */
         static const InteractionMode ACTOR;

         /**
          * This mode allows the user to select actor(s) in the scene.  When
          * they are selected, the property viewer allows the actor(s) properties
          * to be edited.
          */
         static const InteractionMode SELECT_ACTOR;


      private:
         InteractionMode(const std::string& name)
            : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
      };

      /**
       * Gets the name assigned to this viewport.  This cannot be changed
       * and is assigned to it by the ViewportManager when the viewport is
       * created.
       * @return The viewport's name.
       */
      const std::string& getName() const { return mName; }

      /**
       * Gets this viewport's type.  This is assigned by the ViewportManager
       * when the viewport is created.
       * @return The type of viewport.
       */
      ViewportManager::ViewportType& getType() const { return mViewPortType; }

      /**
       * Tells the viewport whether or not to keep in sync with the rest
       * of the editor UI.
       * @param on If true, use auto interaction mode.
       * @par
       *  By default, this property is true.  This means that the viewport
       *  responds to global UI messages that are sent from various components
       *  in the level editor.  For example, when the camera button is selected
       *  in the toolbar, all viewports that have this property set to true
       *  will put themselves into the CAMERA interaction mode.
       */
      void setAutoInteractionMode(bool on);

      /**
       * Gets the auto interaction mode property.
       * @return True if auto interaction mode is enabled on this viewport.
       */
      bool getAutoInteractionMode() const {
         return mUseAutoInteractionMode;
      }

      /**
       * Sets whether or not this viewport wants to be notified when the current
       * map or project has changed.
       * @param on If true then receive notifications.
       * @note
       *  This should be turned off if the user wishes to not view the currently
       *  loaded scene.  For example, the static mesh browser has this property
       *  set to false.
       */
      void setAutoSceneUpdate(bool on)  { mAutoSceneUpdate = on; }

      /**
       * Gets the auto scene update property.
       * @return
       */
      bool getAutoSceneUpdate() const { return mAutoSceneUpdate; }

      /**
       * Sets whether or not the hot keys for actor manipulation and selection
       * are enabled.
       * @param enable If true, key bindings (hot keys) are enabled, else they
       *  are disabled.
       * @note
       *  Key bindings are used for actor manipulation and selection.  Some viewports
       *  may or may not have key bindings.  However, if key bindings are present,
       *  this method allows the user to disable them if desired.
       */
      void setEnableKeyBindings(bool enable) { mEnableKeyBindings = enable; }

      /**
       * Gets whether or not key bindings (hot keys) are enabled for this
       * viewport.
       * @return True if enabled.
       */
      bool getEnableKeyBindings() const { return mEnableKeyBindings; }

      /**
       * Projects the 2D window coordinates into the current scene and determines
       * if a ray whose origin is at the projected point intersects any actors
       * in the current scene.  Window coordinates are such that the origin is
       * in the upper left corner of the window.
       * @param x Horizonal window coordinate.
       * @param y Vertical window coordinate.
       */
      virtual void pick(int x, int y);

      /**
      * Converts pixel mouse position to delta normalized format.
      *
      * @param[in]  pixelPos  The original position of the mouse.
      *
      * @return               The converted mouse position.
      */
      virtual osg::Vec2 convertMousePosition(QPoint pixelPos);

      /**
      * Calculates the 3D collision line that represents the mouse.
      *
      * @param[in]  mousePos  The position of the mouse in screen coords.
      * @param[in]  start     The start position of the line.
      * @param[in]  end       The end position of the line.
      */
      virtual void GetMouseLine(osg::Vec2 mousePos, osg::Vec3& start, osg::Vec3& end);

      /**
      * Projects the 2D window coordinates into the current scene and determines
      * if a ray whose origin is at the projected point intersects any actors
      * in the current scene.  Window coordinates are such that the origin is
      * in the upper left corner of the window.
      * @param x Horizonal window coordinate.
      * @param y Vertical window coordinate.
      * @return True if there was a collision.
      */
      virtual bool calculatePickISector(int x, int y);

      /**
      * Projects a line into the current scene and does a collision test.
      * @param nearPoint The near point of the line.
      * @param farPoint The far point of the line.
      * @return True if there was a collision.
      */
      virtual bool calculatePickISector(osg::Vec3 nearPoint, osg::Vec3 farPoint);

      /**
      * Projects the 2D window coordinates into the current scene and determines
      * if a ray whose origin is at the projected point intersects any actors
      * in the current scene.  Window coordinates are such that the origin is
      * in the upper left corner of the window.
      * @param x Horizonal window coordinate.
      * @param y Vertical window coordinate.
      * @param ignoredDrawable A drawable to ignore.
      */
      virtual bool getPickPosition(int x, int y, osg::Vec3& position, std::vector<dtCore::DeltaDrawable*> ignoredDrawables = std::vector<dtCore::DeltaDrawable*>());

      /**
      * Projects a line into the current scene and returns the nearest point of collision.
      * @param nearPoint The near point of the line.
      * @param farPoint The far point of the line.
      * @param position The returned closest point of collision.
      * @param ignoredDrawable A drawable to ignore.
      */
      virtual bool getPickPosition(osg::Vec3 nearPoint, osg::Vec3 farPoint, osg::Vec3& position, std::vector<dtCore::DeltaDrawable*> ignoredDrawables = std::vector<dtCore::DeltaDrawable*>());

      /**
      * Retrieves the last pick position calculated.
      * @param ignoredDrawable A drawable to ignore.
      */
      virtual bool getPickPosition(osg::Vec3& position, std::vector<dtCore::DeltaDrawable*> ignoredDrawables = std::vector<dtCore::DeltaDrawable*>());

      /**
       * Projects the 2D window coordinates into the current scene and determines
       * if a ray whose origin is at the projected point intersects any actors
       * in the current scene.  Window coordinates are such that the origin is
       * in the upper left corner of the window.
       * @param x Horizonal window coordinate.
       * @param y Vertical window coordinate.
       */
      virtual dtCore::DeltaDrawable* getPickDrawable(int x, int y);
      virtual dtCore::DeltaDrawable* getPickDrawable();

      /**
       * After each mouse move event, this method will reset the cursor position to
       * the center of the viewport.  This is useful for tracking mouse delta movements
       * when the actual position of the mouse cursor is not relavent.
       * @note
       *  This method also hides the mouse cursor.
       */
      void trapMouseCursor();

      /**
       * Releases a mouse cursor that has been trapped.  Also makes the cursor
       * visible again.
       */
      void releaseMouseCursor(const QPoint& mousePosition = QPoint(-1,-1));

      /**
       * Sets the scene to be rendered by this viewport.
       * @param scene The new scene to be rendered.
       */
      virtual void setScene(dtCore::Scene* scene);

      /**
       * Gets the scene currently being rendered by this viewport.
       * @return A pointer to the scene.
       */
      dtCore::Scene* getScene() { return mScene.get(); }

      /**
       * Sets the viewport's camera.  The camera determines the point of view
       * from which the current scene is rendered.
       * @param cam The new camera to use.
       */
      void setCamera(StageCamera* cam);

      /**
       * Gets this viewport's camera.
       * @return
       */
      StageCamera* getCamera() { return mCamera.get(); }

      /**
       * Sets the mouse sensitivity for this viewport.  All mouse input into
       * this viewport is them scaled by the sensitivity factor.  A higher value
       * implies that mouse movement will have less impact on the viewport.
       * The default is 1.f.
       * @param value
       */
      void setMouseSensitivity(float value) { mMouseSensitivity = value; }

      /**
       * Gets this viewport's mouse sensitivity.
       * @return
       */
      float getMouseSensitivity() const { return mMouseSensitivity; }

      /**
       * Sets the current render style of this viewport.
       * @param style The new style.
       * @param refreshView If this is true, the viewport will be told to
       * redraw itself.  Note, by default this is true, however, if this
       * method is called before the viewport is initialized an exception
       * will be thrown.
       */
      void setRenderStyle(const RenderStyle& style, bool refreshView = true);

      /**
       * Gets the current render style of this viewport.
       * @return
       */
      const RenderStyle& getRenderStyle() const { return *mRenderStyle; }

      /**
       * Gets the interaction mode currently in use by this viewport.
       * @return The current interaction mode.
       */
      const InteractionMode& getInteractionMode() const { return *mInteractionMode; }

      /**
       * Sets the current interaction mode.
       */
      void setInteractionMode(const InteractionMode& mode) { mInteractionMode = &mode; }

      /**
       * Returns whether or not the viewport has been initialized.
       * @return True if the viewport has been initialized.
       */
      bool isInitialized() const { return mInitialized; }

      /**
       * Refreshes the viewport with a new set of selected actors.
       *
       * @param[in]  actors  The list of all actors being selected.
       */
      virtual void refreshActorSelection(const std::vector< dtCore::RefPtr<dtCore::BaseActorObject> >& actors);

      /**
       * Tells the viewport to repaint itself.
       */
      virtual void refresh();

      /**
       * Sets the background color of this viewport.
       * The default color is a darkish blue.
       * @param color The new color of the viewport background.
       */
      void setClearColor(const osg::Vec4& color);

      QGLWidget* GetQGLWidget();


      /**
      * Returns the underlying scene view that is attached to this viewport.
      * @return
      */
      dtCore::View* GetView() { return mView.get(); }

      dtCore::DeltaWin* GetWindow();

      /**
      * Called when the user moves the mouse while pressing any combination of
      * mouse buttons.  Based on the current mode, the camera is updated.
      */
      virtual void mouseMoveEvent(QMouseEvent* e);

      /**
      * Called when the viewport needs to redraw itself.
      */
      virtual void paintGL();

      /**
      * Called by the Qt windowing system when the viewport is to be initialized.
      */
      virtual void initializeGL();

      /**
      * Called by the Qt windowing system when the viewport is resized by the user.
      */
      virtual void resizeGL(int width, int height);

      osg::Group* GetRootNode() { return mRootNodeGroup.get(); }

      /**
      * Updates a property on the current actor selection.
      * @param propName The name of the property to update.
      * @note
      *  This method sends an actorPropertyChange event for each actor proxy
      *  selected.  This method is used in the ortho and perspective viewports
      *  to update the current selection after it has been translated, rotated, etc.
      * @see EditorEvents::emitActorPropertyChanged
      */
      void updateActorSelectionProperty(const std::string& propName);

      /**
      * Saves the original version of the actor rotation or translation values.
      * This is used so that undo/redo can track the old value of a property.
      * @param propName The name of the property to update.
      * @note
      *  It clears the list of any previous old values.
      */
      void saveSelectedActorOrigValues(const std::string& propName);

   public slots:
      ///Moves the camera such that the actor is clearly visible.
      virtual void onGotoActor(dtCore::ActorPtr proxy);

      virtual void onGotoPosition(double x, double y, double z);

      // starts a larger transaction to prevent excessive refreshes
      void onBeginChangeTransaction();

      // ends a transaction and does a final refresh viewports
      void onEndChangeTransaction();

   signals:
      ///This signal is emitted when the render style is changed.
      void renderStyleChanged();

   protected:
      /**
       * Constructs the viewport.
       * @param type Type of viewport to create.
       * @param name Name to assign to this viewport.
       * @param parent Parent UI widget with which the viewport resides.
       * @param shareWith If true, the graphics context will be shared
       * with the specified QGLWidget.
       */
      Viewport(ViewportManager::ViewportType& type, const std::string& name,
         QWidget* parent = NULL, osg::GraphicsContext* shareWith = NULL);

      /**
       * Destroys this viewport.
       */
      virtual ~Viewport();



      /**
       * Renders the scene as is viewed from the viewport's currently assigned
       * camera.
       */
      virtual void renderFrame();


      /**
       * Returns the state set for this viewport.  This determines how the scene is
       * rendered.
       * @return The global state set for this viewport.
       */
      osg::StateSet* getGlobalStateSet() { return mGlobalStateSet.get(); }

   protected:
      /**
       * Tells this viewport to listen to global UI events.
       */
      virtual void connectInteractionModeSlots();

      /**
       * Tells the viewport to stop listening to global UI events.
       */
      virtual void disconnectInteractionModeSlots();

      /**
       * Spins through the actor proxies currently in the map and if they
       * are using billboards to represent themselves, orient the billboards
       * to be inline with the camera.
       * @note The billboard behavior of OpenSceneGraph was not suitable to
       *  to the needs of the actor proxy billboards, therefore, a different
       *  implementation is required.
       */
      virtual void updateActorProxyBillboards();


      /// Called by the mouse move event with the adjusted x and y so that subclasses can do what they need.
      virtual void onMouseMoveEvent(QMouseEvent* e, float dx, float dy) = 0;

      /**
       * Camera attached to this viewport.
       * @note
       *  This is not created by this viewport.  It must be created by
       *  classes extending the base viewport.
       */
      dtCore::RefPtr<StageCamera> mCamera;

      /**
       * Tracks whether we are currently in a batch change transaction
       */
      bool mInChangeTransaction;


      dtCore::RefPtr<dtCore::DeltaWin> mWindow;

   private:
      ///Sets up the initial render state of this viewport.
      void setupInitialRenderState();

      ///Allow the viewport manager to have access to the viewport so it can create it.
      friend class ViewportManager;

      float                          mMouseSensitivity;
      std::string                    mName;
      ViewportManager::ViewportType& mViewPortType;
      const RenderStyle*             mRenderStyle;
      const InteractionMode*         mInteractionMode;

      bool mUseAutoInteractionMode;
      bool mAutoSceneUpdate;
      bool mInitialized;
      bool mEnableKeyBindings;

      QCursor mOldMouseCursor;
      QPoint  mOldMouseLocation;
      bool    mIsMouseTrapped;

      QPoint mLastMouseUpdateLocation;

      // holds the original values of translation and/or rotation.  This should
      // be set in BeginEdit and cleared in EndEdit
      std::map< std::string, std::vector<std::string> > mSelectedActorOrigValues;

      dtCore::RefPtr<dtCore::Scene>    mScene;
      dtCore::RefPtr<dtCore::View>     mView;
      osg::ref_ptr<osg::StateSet>      mGlobalStateSet;
      osg::ref_ptr<osg::ClearNode>     mClearNode;

      /**
       * The root node group holds to branches.  The first branch is the scene
       * currently assigned to this viewport.  The other branch is reserved for
       * overlays and other objects that are editor specific and are not
       * a part of the actual scene.
       */
      osg::ref_ptr<osg::Group>         mRootNodeGroup;

      dtCore::RefPtr<dtCore::Isector>  mIsector;
   };

} // namespace dtEditQt

#endif // DELTA_STAGE_VIEWPORT
