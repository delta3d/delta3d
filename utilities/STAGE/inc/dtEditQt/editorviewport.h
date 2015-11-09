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
#include <dtEditQt/stagecameramotionmodel.h>
#include <dtEditQt/export.h>

namespace dtEditQt {

   /**
    * This class is the base class for all viewports that will be used to
    * edit the maps.
    */
   class DT_EDITQT_EXPORT EditorViewport : public Viewport
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
      * Sets the current camera motion model.
      *
      * @param[in]  motion  The camera motion model (use NULL for default).
      */
      void setCameraMotionModel(STAGECameraMotionModel* motion = NULL);

      /**
      * Retrieves the current camera motion mode.
      */
      STAGECameraMotionModel* getCameraMotionModel() {return mCameraMotionModel.get();}

      /**
       * Sets the scene to be rendered by this viewport.
       * @param scene The new scene to be rendered.
       */
      virtual void setScene(dtCore::Scene* scene);

      /**
       *	Creates the compass actor to be shown within the viewport.
       */
      void ShowCompass();

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
      * Retrieves the ghost proxy.
      */
      dtCore::BaseActorObject* GetGhostProxy() {return mGhostProxy.get();}

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
      * Moves the camera to the specified actor.
      *
      * @param[in]  proxy  The actor proxy to look at.
      */
      void onGotoActor(dtCore::ActorPtr proxy);

      /**
      * Moves the camera to a position.
      *
      * @param[in]  x, y, z  The new camera position.
      */
      void onGotoPosition(double x, double y, double z);

      /** Move the selected Actor(s) to the Camera, or the Camera to the selected
       *  Actor, based on the supplied QAction.
       *  @param action Either mAlignActorToCameraAction or mAlignCameraToActorAction
       *  @see EditorActions
       */
      void slotMoveActorOrCamera(QAction* action);

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

      /**
       * Is this QDropEvent something the Viewport supports?
       * @return true if the QDropEvent is supported, false otherwise
       */
      virtual bool IsSupportedDragDropFormat(const QDropEvent* event) const;


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
      * Called when the user presses a key on the keyboard in the viewport.
      * Based on the combination of keys pressed, the viewport's current
      * mode will be set.
      * @param e
      */
      virtual void keyPressEvent(QKeyEvent* e);

      /**
      * Called when the user releases a key on the keyboard in the viewport.
      * Based on the keys released, the viewport's current mode is
      * updated accordingly.
      * @param e
      */
      virtual void keyReleaseEvent(QKeyEvent* e);

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

      /**
      * Whether this viewport is being hidden by the configuration.
      */
      bool GetIsRemoved() const {return mIsRemoved;}
      void RemoveView() {mIsRemoved = true;}

   protected:

      /**
      * Renders the scene as is viewed from the viewport's currently assigned
      * camera.
      */
      virtual void renderFrame();

      bool mAttachActorToCamera;

      dtCore::RefPtr<STAGEObjectMotionModel> mObjectMotionModel;
      dtCore::RefPtr<STAGECameraMotionModel> mCameraMotionModel;
      dtCore::RefPtr<STAGECameraMotionModel> mDefaultCameraMotionModel;

      dtCore::RefPtr<dtCore::BaseActorObject>      mGhostProxy;

      Qt::MouseButton                        mMouseButton;
      Qt::MouseButtons                       mMouseButtons;
      Qt::KeyboardModifiers                  mKeyMods;

      bool                                   mSkipUpdateForCam;

      osg::Node::NodeMask                    mEnabledMask;
      osg::Node::NodeMask                    mDisabledMask;

   private:
      bool                                   mEnabled;
      bool                                   mIsRemoved;

      ///From the QDropEvent, unroll the Prefab actor and add to the Map
      void UnrollPrefab(QDropEvent* event, dtCore::Map* mapPtr);

   };

} // namespace dtEditQt

#endif //DELTA_EDITOR_VIEWPORT
