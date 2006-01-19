/*
* Delta3D Open Source Game and Simulation Engine 
* Simulation, Training, and Game Editor (STAGE)
* Copyright (C) 2005, BMH Associates, Inc.
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2 of the License, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* @author Matthew W. Campbell
*/
#ifndef DELTA_STAGE_VIEWPORT
#define DELTA_STAGE_VIEWPORT

#include <QtOpenGL/QGLWidget>
#include <QtGui/QCursor>

#include <map>

#include <dtCore/base.h>
#include <dtCore/system.h>
#include <dtCore/transformable.h>
#include <dtUtil/enumeration.h>
#include <dtDAL/actorproxy.h>
#include "dtEditQt/camera.h"
#include "dtEditQt/viewportmanager.h"
#include "dtEditQt/typedefs.h"

namespace osg
{
    class FrameStamp;
    class StateSet;
    class ClearNode;
}

namespace dtCore
{
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
    class Viewport : public QGLWidget
    {
        Q_OBJECT

    public:

        /**
         * The RenderStyle enumeration contains the different ways in which
         * a particular viewport can render its scene.
         */
        class RenderStyle : public dtUtil::Enumeration
        {
            DECLARE_ENUM(RenderStyle);
        public:
            static const RenderStyle WIREFRAME;
            static const RenderStyle LIT;
            static const RenderStyle TEXTURED;
            static const RenderStyle LIT_AND_TEXTURED;
        private:
            RenderStyle(const std::string &name) : dtUtil::Enumeration(name)
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
        class InteractionMode : public dtUtil::Enumeration
        {
            DECLARE_ENUM(InteractionMode);
        public:
            /**
             * If in this mode, user input is translated into camera movement,
             * thus allowing the user to navigate the current scene.
             */
            static const InteractionMode CAMERA;

            /**
             * This mode allows the user to select actor(s) in the scene.  When
             * they are selected, the property viewer allows the actor(s) properties
             * to be edited.
             */
            static const InteractionMode SELECT_ACTOR;

            /**
             * This mode allows the selected actor(s) to be repositioned in the
             * scene by using the mouse and dragging them to a new position.
             */
            static const InteractionMode TRANSLATE_ACTOR;

            /**
             * This mode allows the selected actor(s) to be rotated about own
             * local axis or pivot point.
             */
            static const InteractionMode ROTATE_ACTOR;

            /**
             * This mode allows the selected actor(s) to be either uniformly scaled,
             * or scaled along either the X,Y or Z axis.
             */
            static const InteractionMode SCALE_ACTOR;

        private:
            InteractionMode(const std::string &name) : dtUtil::Enumeration(name)
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
        const std::string &getName() const {
            return this->name;
        }

        /**
         * Gets this viewport's type.  This is assigned by the ViewportManager
         * when the viewport is created.
         * @return The type of viewport.
         */
        ViewportManager::ViewportType &getType() const {
            return this->viewPortType;
        }

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
            return this->useAutoInteractionMode;
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
        void setAutoSceneUpdate(bool on) {
            this->autoSceneUpdate = on;
        }

        /**
         * Gets the auto scene update property.
         * @return
         */
        bool getAutoSceneUpdate() const {
            return this->autoSceneUpdate;
        }

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
        void setEnableKeyBindings(bool enable) {
            this->enableKeyBindings = enable;
        }

        /**
         * Gets whether or not key bindings (hot keys) are enabled for this
         * viewport.
         * @return True if enabled.
         */
        bool getEnableKeyBindings() const {
            return this->enableKeyBindings;
        }

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
        void releaseMouseCursor(const QPoint &mousePosition = QPoint(-1,-1));

        /**
         * Sets the scene to be rendered by this viewport.
         * @param scene The new scene to be rendered.
         */
        void setScene(dtCore::Scene *scene);

        /**
         * Gets the scene currently being rendered by this viewport.
         * @return A pointer to the scene.
         */
        dtCore::Scene *getScene() {
            return this->scene.get();
        }

        /**
         * Sets the overlay object for this viewport.
         * @param overlay The new overlay.
         * @note Of the overlay parameter is NULL, the overlay is cleared from this
         *  viewport.
         */
        void setOverlay(ViewportOverlay *overlay);

        /**
         * Gets the current overlay assigned to this viewport.
         * @return The overlay assigned to this viewport.
         */
        ViewportOverlay *getOverlay() {
            return this->overlay.get();
        }

        /**
         * Sets the viewport's camera.  The camera determines the point of view
         * from which the current scene is rendered.
         * @param cam The new camera to use.
         */
        void setCamera(Camera *cam) {
            this->camera = cam;
        }

        /**
         * Gets this viewport's camera.
         * @return
         */
        Camera *getCamera() {
            return this->camera.get();
        }

        /**
         * Sets the mouse sensitivity for this viewport.  All mouse input into
         * this viewport is them scaled by the sensitivity factor.  A higher value
         * implies that mouse movement will have less impact on the viewport.
         * The default is 10.
         * @param value
         */
        void setMouseSensitivity(float value) {
            this->mouseSensitivity = value;
        }

        /**
         * Gets this viewport's mouse sensitivity.
         * @return
         */
        float getMouseSensitivity() const {
            return this->mouseSensitivity;
        }

        /**
         * Sets the current render style of this viewport.
         * @param style The new style.
         * @param refreshView If this is true, the viewport will be told to
         * redraw itself.  Note, by default this is true, however, if this
         * method is called before the viewport is initialized an exception
         * will be thrown.
         */
        void setRenderStyle(const RenderStyle &style, bool refreshView = true);

        /**
         * Gets the current render style of this viewport.
         * @return
         */
        const RenderStyle &getRenderStyle() const {
            return *(this->renderStyle);
        }

        /**
         * Gets the interaction mode currently in use by this viewport.
         * @return The current interaction mode.
         */
        const InteractionMode &getInteractionMode() const {
            return *(this->interactionMode);
        }

        /**
         * Returns whether or not the viewport has been initialized.
         * @return True if the viewport has been initialized.
         */
        bool isInitialized() const {
            return this->initialized;
        }

        /**
         * Tells the viewport to repaint itself.
         */
        void refresh();

        /**
         * Sets the background color of this viewport.
         * The default color is a darkish blue.
         * @param color The new color of the viewport background.
         */
        void setClearColor(const osg::Vec4 &color);

    public slots:
        ///Sets the current interaction mode to CAMERA.
        void setCameraMode();

        ///Sets the current interation mode to SELECT_ACTOR.
        void setActorSelectMode();

        ///Sets the current interaction mode to TRANSLATE_ACTOR.
        void setActorTranslateMode();

        ///Sets the current interaction mode to ROTATE_ACTOR.
        void setActorRotateMode();

        ///Moves the camera such that the actor is clearly visible.
        void onGotoActor(proxyRefPtr proxy);

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
        Viewport(ViewportManager::ViewportType &type, const std::string &name,
            QWidget *parent = NULL, QGLWidget *shareWith = NULL);

        /**
         * Destroys this viewport.
         */
        virtual ~Viewport();

        /**
         * Called by the Qt windowing system when the viewport is to be initialized.
         */
        virtual void initializeGL();

        /**
         * Called by the Qt windowing system when the viewport is resized by the user.
         */
        virtual void resizeGL(int width, int height);

        /**
         * Called when the viewport needs to redraw itself.
         */
        virtual void paintGL();

        /**
         * Renders the scene as is viewed from the viewport's currently assigned
         * camera.
         */
        virtual void renderFrame();

        /**
         * Returns the underlying scene view that is attached to this viewport.
         * @return
         */
        osgUtil::SceneView *getSceneView() {
            return this->sceneView.get();
        }

        /**
         * Returns the state set for this viewport.  This determines how the scene is
         * rendered.
         * @return The global state set for this viewport.
         */
        osg::StateSet *getGlobalStateSet() {
            return this->globalStateSet.get();
        }

        /**
         * Tells this viewport to listen to global UI events.
         */
        virtual void connectInteractionModeSlots();

        /**
         * Tells the viewport to stop listening to global UI events.
         */
        virtual void disconnectInteractionModeSlots();

        /**
         * Makes sure the current interaction mode is in sync with the active action
         * object in EditorActions.
         */
        void syncWithModeActions();

        /**
         * Updates a property on the current actor selection.
         * @param propName The name of the property to update.
         * @note
         *  This method sends an actorPropertyChange event for each actor proxy
         *  selected.  This method is used in the ortho and perspective viewports
         *  to update the current selection after it has been translated, rotated, etc.
         * @see EditorEvents::emitActorPropertyChanged
         */
        void updateActorSelectionProperty(const std::string &propName);

        /**
         * Saves the original version of the actor rotation or translation values.
         * This is used so that undo/redo can track the old value of a property.
         * @param propName The name of the property to update.
         * @note
         *  It clears the list of any previous old values.
         */
        void saveSelectedActorOrigValues(const std::string &propName);

        /**
         * Spins through the actor proxies currently in the map and if they
         * are using billboards to represent themselves, orient the billboards
         * to be inline with the camera.
         * @note The billboard behavior of OpenSceneGraph was not suitable to
         *  to the needs of the actor proxy billboards, therefore, a different
         *  implementation is required.
         */
        void updateActorProxyBillboards();

        /**
         * Camera attached to this viewport.
         * @note
         *  This is not created by this viewport.  It must be created by
         *  classes extending the base viewport.
         */
        osg::ref_ptr<Camera> camera;

        /**
         * Tracks whether we are currently in a batch change transaction
         */
        bool inChangeTransaction;

    private:
        ///Sets up the initial render state of this viewport.
        void setupInitialRenderState();

        ///Allow the viewport manager to have access to the viewport so it can create it.
        friend class ViewportManager;

    private:
        float mouseSensitivity;
        std::string name;
        ViewportManager::ViewportType &viewPortType;
        const RenderStyle *renderStyle;
        const InteractionMode *interactionMode;

        bool useAutoInteractionMode;
        bool autoSceneUpdate;
        bool initialized;
        bool enableKeyBindings;

        QCursor oldMouseCursor;
        QPoint oldMouseLocation;
        bool cacheMouseLocation;

        // holds the original values of translation and/or rotation.  This should
        // be set in BeginEdit and cleared in EndEdit
        std::map<std::string,std::vector<std::string> > selectedActorOrigValues;

        osg::ref_ptr<ViewportOverlay> overlay;
        osg::ref_ptr<dtCore::Scene> scene;
        osg::ref_ptr<osg::FrameStamp> frameStamp;
        osg::ref_ptr<osgUtil::SceneView> sceneView;
        osg::ref_ptr<osg::StateSet> globalStateSet;
        osg::ref_ptr<osg::ClearNode> clearNode;

        /**
         * The root node group holds to branches.  The first branch is the scene
         * currently assigned to this viewport.  The other branch is reserved for
         * overlays and other objects that are editor specific and are not
         * a part of the actual scene.
         */
        osg::ref_ptr<osg::Group> rootNodeGroup;
    };

}

#endif
