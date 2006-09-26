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
* Matthew W. Campbell
*/
#ifndef DELTA_PERSPECTIVE_VIEWPORT
#define DELTA_PERSPECTIVE_VIEWPORT

#include "dtEditQt/viewport.h"

namespace dtEditQt
{

    /**
     * This class provides a 3D first person style view into the scene.  Its camera
     * mode allows the user to fly around a scene and view it from any angle.
     */
    class PerspectiveViewport : public Viewport
    {
        Q_OBJECT
    public:

        /**
         * Enumerates the specific types of interactions a perspective viewport
         * supports.  These extend the interactions of the base viewport.  For example,
         * when the overall mode is camera mode, the perspective viewport supports
         * more specific behavior.
         */
        class InteractionModeExt : public dtUtil::Enumeration
        {
            DECLARE_ENUM(InteractionModeExt);
        public:
            static const InteractionModeExt CAMERA_TRANSLATE;
            static const InteractionModeExt CAMERA_NAVIGATE;
            static const InteractionModeExt CAMERA_LOOK;
            static const InteractionModeExt ACTOR_AXIS_X;
            static const InteractionModeExt ACTOR_AXIS_Y;
            static const InteractionModeExt ACTOR_AXIS_Z;
            static const InteractionModeExt NOTHING;

        private:
            InteractionModeExt(const std::string &name) : dtUtil::Enumeration(name)
            {
                AddInstance(this);
            }
        };

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
        void moveCamera(float dx, float dy);

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
        void setMoveActorWithCamera(bool value) {
            this->attachActorToCamera = value;
        }

        /**
         * Gets whether or not an actor should be linked to the camera when
         * in translate mode.
         * @return True if enabled.
         */
        bool getMoveActorWithCamera() const {
            return this->attachActorToCamera;
        }

    public slots:

        /**
         * Puts the perspective viewport options in sync with the editor preferences.
         */
        void onEditorPreferencesChanged();

    protected:
        /**
         * Constructs the perspective viewport.
         * @param name
         * @param parent
         * @param shareWith
         */
        PerspectiveViewport(const std::string &name, QWidget *parent = NULL,
                            QGLWidget *shareWith = NULL);

        /**
         * Destroys the viewport.
         */
        virtual ~PerspectiveViewport() { }

        /**
         * Initializes the viewport.  This just sets the render style to be
         * textured and un-lit.
         */
        void initializeGL();

        /**
         * Sets the perspective projection viewing parameters of this viewport's
         * camera.
         * @param width The new width of the viewport.
         * @param height The new height of the viewport.
         */
        void resizeGL(int width, int height);

        /**
         * Called when the user presses a mouse button in the viewport.  Based on
         * the combination of buttons pressed, the viewport's current mode will
         * be set.
         * @param e
         * @see ModeType
         */
        void mousePressEvent(QMouseEvent *e);

        /**
         * Called when the user releases a mouse button in the viewport.  Based on
         * the buttons released, the viewport's current mode is updated
         * accordingly.
         * @param e
         */
        void mouseReleaseEvent(QMouseEvent *e);

        /**
         * Called when the user moves the mouse while pressing any combination of
         * mouse buttons.  Based on the current mode, the camera is updated.
         * @param dx the adjusted change in x that the mouse moved.
         * @param dy the adjusted change in y that the mouse moved.
         */
        virtual void onMouseMoveEvent(QMouseEvent *e, float dx, float dy);

        /**
         * Called from the mousePressEvent handler.  This sets the viewport state
         * to properly respond to mouse movement events when in camera mode.
         * @param e
         */
        void beginCameraMode(QMouseEvent *e);

        /**
         * Called from the mouseReleaseEvent handler.  This restores the state of
         * the viewport as it was before camera mode was entered.
         * @param e
         */
        void endCameraMode(QMouseEvent *e);

        /**
         * Called from the mousePressEvent handler.  Depending on what modifier
         * key is pressed, this puts the viewport state into a mode that enables
         * actor manipulation.
         * @param e
         */
        void beginActorMode(QMouseEvent *e);

        /**
         * Called from the mouseReleaseEvent handler.  This restores the state of
         * the viewport as it was before actor mode was entered.
         * @param e
         */
        void endActorMode(QMouseEvent *e);

        /**
         * Takes the current selection of actors and attaches them to the camera.
         * Therefore, as the camera moves, the actors move along with it.
         */
        void attachCurrentSelectionToCamera();

        /**
         * This method is called during mouse movement events if the viewport is
         * currently in the manipulation mode that translates the current actor
         * selection.  This method then goes through the current actor selection
         * and translates each one based on delta mouse movements.
         * @param e The mouse move event.
         * @param dx The change along the mouse x axis.
         * @param dy The change along the mouse y axis.
         */
        void translateCurrentSelection(QMouseEvent *e, float dx, float dy);

        /**
         * This method is called during mouse movement events if the viewport is
         * currently in the manipulation mode that rotates the current actor
         * selection.  This method then goes through the current actor selection
         * and rotates each one based on delta mouse movements.
         * @note
         *  If there is only one actor selected, the rotation is about its local center.
         *  However, if there are multiple actors selected, the rotation is about the
         *  center point of the selection.
         * @param e The mouse move event.
         * @param dx The change along the mouse x axis.
         * @param dy The change along the mouse y axis.
         */
        void rotateCurrentSelection(QMouseEvent *e, float dx, float dy);

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

    private:
        const InteractionModeExt *currentMode;
        bool attachActorToCamera;

        ///Allow the ViewportManager access to it can create perspective viewports.
        friend class ViewportManager;
    };
}

#endif //DELTA_PERSPECTIVE_VIEWPORT
