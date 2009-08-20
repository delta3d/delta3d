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
 * Jeffrey P Houde
 */
#ifndef DELTA_EDITOR_VIEWPORT
#define DELTA_EDITOR_VIEWPORT

#include <dtEditQt/viewport.h>
#include <dtEditQt/stageobjectmotionmodel.h>

namespace dtEditQt {

   /**
    * This class is the base class for all viewports that will be used to
    * edit the maps.
    */
   class EditorViewport : public Viewport
   {
      Q_OBJECT

   public:
      /**
       * Sets whether or not an actor should be linked to the camera when
       * in translate mode.
       * @param value True if the actor should be linked to the camera.
       * @note
       *  This interaction behavior allows the user to move actors in the scene
       *  by merely moving the camera.  If this property is true, setting the
       *  current interaction move to actor translation mode will enable this
       *  behavior.
       */
      void setMoveActorWithCamera(bool value) { mAttachActorToCamera = value; }

      /**
       * Gets whether or not an actor should be linked to the camera when
       * in translate mode.
       * @return True if enabled.
       */
      bool getMoveActorWithCamera() const { return mAttachActorToCamera; }

      /**
       * Moves the camera.
       * @par
       *  The camera's movement is determined by the current mode of the
       *  perspective viewport:<br>
       *      CAMERA_TRANSLATE - The camera is moved by up/down (dy) and
       *          left/right (dx). <br>
       *      CAMERA_NAVIGATE - The camera is moved forward/backward (dy) and
       *          can be rotated about its up axis. (dx). <br>
       *      CAMERA_LOOK - The camera is stationary and can look in any direction. <br>
       * @param dx
       * @param dy
       */
      virtual bool moveCamera(float dx, float dy);

      /**
       * Sets the scene to be rendered by this viewport.
       * @param scene The new scene to be rendered.
       */
      virtual void setScene(dtCore::Scene* scene);

      /**
       * Refreshes the viewport with a new set of selected actors.
       *
       * @param[in]  actors  The list of all actors being selected.
       */
      virtual void refreshActorSelection(const std::vector< dtCore::RefPtr<dtDAL::ActorProxy> >& actors);

      /**
       * Tells the viewport to repaint itself.
       */
      virtual void refresh();

      /**
      * Retrieves the ghost proxy.
      */
      dtDAL::ActorProxy* GetGhostProxy() {return mGhostProxy.get();}

      /**
      * Retrieves the current mouse button pressed.
      */
      Qt::MouseButton& GetMouseButton() {return mMouseButton;}
      Qt::MouseButtons& GetMouseButtons() {return mMouseButtons;}

      /**
      * Retrieves the current key modifiers pressed.
      */
      Qt::KeyboardModifiers& GetKeyMods() {return mKeyMods;}

      /**
      * Retrieves the object motion model.
      */
      STAGEObjectMotionModel* GetObjectMotionModel() {return mObjectMotionModel.get();}

      /**
      * Called when the user presses a mouse button in the viewport.  Based on
      * the combination of buttons pressed, the viewport's current mode will
      * be set.
      * @param e
      * @see ModeType
      */
      virtual void mousePressEvent(QMouseEvent* e);

      /**
      * Called when the user releases a mouse button in the viewport.  Based on
      * the buttons released, the viewport's current mode is updated
      * accordingly.
      * @param e
      */
      virtual void mouseReleaseEvent(QMouseEvent* e);

      /**
      * Called when the user double clicks the mouse button in the viewport.
      */
      virtual void mouseDoubleClickEvent(QMouseEvent *e);

      /**
      * Called when the user moves the wheel on a mouse containing a scroll wheel.
      * This causes the scene to be zoomed in and out.
      * @param e
      */
      virtual void wheelEvent(QWheelEvent* e);

      /**
      * Drag events.
      */
      void dragEnterEvent(QDragEnterEvent* event);
      void dragLeaveEvent(QDragLeaveEvent* event);
      void dragMoveEvent(QDragMoveEvent* event);
      void dropEvent(QDropEvent* event);

   public slots:
      /**
       * Puts the perspective viewport options in sync with the editor preferences.
       */
      void onEditorPreferencesChanged();

      /**
      * Toggles the gizmos into local or world space.
      *
      * @param[in]  enabled  True for local space, false for world.
      */
      void setLocalSpace(bool enabled);

      /**
      * Sets the translation snap increment.
      *
      * @param[in]  increment  The size of the increment.
      */
      void setSnapTranslation(float increment);

      /**
      * Sets the angular snap increment.
      *
      * @param[in]  increment  The size of the increment.
      */
      void setSnapRotation(float increment);

      /**
      * Sets the scalar snap increment.
      *
      * @param[in]  increment  The size of the increment.
      */
      void setSnapScale(float increment);

      /**
      * Sets the enabled status of each snap tool.
      */
      void setSnapEnabled(bool translation, bool rotation, bool scale);

   protected:
      /**
       * Constructs the perspective viewport.
       * @param name
       * @param parent
       * @param shareWith
       */
      EditorViewport(ViewportManager::ViewportType& type,
         const std::string& name, QWidget* parent = NULL,
         osg::GraphicsContext* shareWith = NULL);

      /**
       * Destroys the viewport.
       */
      virtual ~EditorViewport() {}


      /**
      * Clears the current Ghost Proxy.
      */
      void ClearGhostProxy();

      /**
       * Called when the user moves the mouse while pressing any combination of
       * mouse buttons.  Based on the current mode, the camera is updated.
       * @param dx the adjusted change in x that the mouse moved.
       * @param dy the adjusted change in y that the mouse moved.
       */
      virtual void onMouseMoveEvent(QMouseEvent* e, float dx, float dy);

   public:
      /**
       * Called from the mousePressEvent handler.  This sets the viewport state
       * to properly respond to mouse movement events when in camera mode.
       * @param e
       */
      virtual bool beginCameraMode(QMouseEvent* e);

      /**
       * Called from the mouseReleaseEvent handler.  This restores the state of
       * the viewport as it was before camera mode was entered.
       * @param e
       */
      virtual bool endCameraMode(QMouseEvent* e);

      /**
      * Determines whether a mouse press at the given position should activate
      * the actor mode.
      *
      * @param[in]  position  The mouse position.
      *
      * @return     True to begin actor mode.
      */
      virtual bool shouldBeginActorMode(osg::Vec2 position);

      /**
       * Called from the mousePressEvent handler.  Depending on what modifier
       * key is pressed, this puts the viewport state into a mode that enables
       * actor manipulation.
       * @param e
       */
      virtual bool beginActorMode(QMouseEvent* e);

      /**
       * Called from the mouseReleaseEvent handler.  This restores the state of
       * the viewport as it was before actor mode was entered.
       * @param e
       */
      virtual bool endActorMode(QMouseEvent* e);

      /**
      * Called from the mouse ReleaseEvent handler.  This handles the
      * selection of actors.
      * @param e
      */
      virtual bool selectActors(QMouseEvent* e);

      /**
       * Sets up this viewport to listen for global UI events being generated by
       * the level editor.
       */
      virtual void connectInteractionModeSlots();

      /**
       * Stops listening to global UI events being generated by the level
       * editor.
       */
      virtual void disconnectInteractionModeSlots();

      /**
      * Duplicates the selected actors.
      */
      virtual bool DuplicateActors();

      /**
      * Called when the user presses a key on the keyboard in the viewport.
      * Based on the combination of keys pressed, the viewport's current
      * mode will be set.
      * @param e
      */
      void keyPressEvent(QKeyEvent* e);

      /**
      * Called when the user releases a key on the keyboard in the viewport.
      * Based on the keys released, the viewport's current mode is
      * updated accordingly.
      * @param e
      */
      void keyReleaseEvent(QKeyEvent* e);

      /** 
      * Enable or disable this Viewport.  High level control over if this
      * Viewport is considered enabled or not.
      * @param enabled: true to enabled, false to disable
      */
      void SetEnabled(bool enabled);

      /** 
      * Is this Viewport considered to be enabled or not
      * @return true if enabled, false otherwise
      */
      bool GetEnabled() const;

   protected:

      /**
      * Renders the scene as is viewed from the viewport's currently assigned
      * camera.
      */
      virtual void renderFrame();

      bool mAttachActorToCamera;

      dtCore::RefPtr<STAGEObjectMotionModel> mObjectMotionModel;

      dtCore::RefPtr<dtDAL::ActorProxy>      mGhostProxy;

      Qt::MouseButton                        mMouseButton;
      Qt::MouseButtons                       mMouseButtons;
      Qt::KeyboardModifiers                  mKeyMods;

      bool                                   mSkipUpdateForCam;

   private:
      bool                                   mEnabled;  //is this Viewport Enabled?
   };

} // namespace dtEditQt

#endif //DELTA_EDITOR_VIEWPORT
