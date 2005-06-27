/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
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
* @author Matthew W. Campbell
*/
#ifndef __OrthoViewport__h
#define __OrthoViewport__h

#include <QCursor>
#include <osg/NodeVisitor>
#include <dtUtil/enumeration.h>
#include "dtEditQt/viewport.h"

#include <iostream>

namespace osg
{
    class Billboard;
}

namespace dtEditQt
{

    /**
     * The orthographic viewport renders a 2D view of the scene.  The 2D view can be
     * along each of the 3D axis.
     * @see OrthoViewType
     */
    class OrthoViewport : public Viewport
    {
        Q_OBJECT

    public:

        /**
         * An enumeration of the different types of views into the scene an
         * orthographic viewport can render.
         */
        class OrthoViewType : public dtUtil::Enumeration {
            DECLARE_ENUM(OrthoViewType);
        public:
            /**
             * Top or birds eye view.  This renders the scene along the XY plane looking
             * down the -Z axis.
             */
            static const OrthoViewType TOP;

            /**
             * Front view.  This renders the scene along the XZ plane looking down the
             * +Y axis.
             */
            static const OrthoViewType FRONT;

            /**
             * Side view.  This renders the scene along the XY plane looking down the
             * -X axis.
             */
            static const OrthoViewType SIDE;

        private:
            OrthoViewType(const std::string &name) : dtUtil::Enumeration(name)
            {
                AddInstance(this);
            }
        };

        /**
         * Enumerates the specific types of interactions an orthographic viewport
         * supports.  These extend the interactions of the base viewport.  For example,
         * when the overall mode is camera mode, the orthographic viewport supports
         * more specific behavior.
         */
        class InteractionModeExt : public dtUtil::Enumeration
        {
            DECLARE_ENUM(InteractionModeExt);
        public:
            static const InteractionModeExt CAMERA_PAN;
            static const InteractionModeExt CAMERA_ZOOM;
            static const InteractionModeExt ACTOR_AXIS_HORIZ;
            static const InteractionModeExt ACTOR_AXIS_VERT;
            static const InteractionModeExt ACTOR_AXIS_BOTH;
            static const InteractionModeExt NOTHING;

        private:
            InteractionModeExt(const std::string &name) : dtUtil::Enumeration(name)
            {
                AddInstance(this);
            }
        };

        /**
         * Sets this orthographic viewport's current view type.
         * @param type The new view type.
         */
        void setViewType(const OrthoViewType &type, bool refreshView=true);

        /**
         * Gets the type of view currently in use by the viewport.
         * @return
         */
        const OrthoViewType &getViewType() const {
            return *(this->viewType);
        }

        /**
         * Moves the camera.
         * @par
         *  The camera's movement is based on the current camera mode.<br>
         *      CAMERA_PAN - Pans the camera along the plane the
         *          viewport is looking at.<br>
         *      CAMERA_ZOOM - Zooms the camera in and out.
         *
         * @param dx
         * @param dy
         */
        void moveCamera(float dx, float dy);

        /**
         * Renders the current scene.  Overloaded to insure billboards are oriented
         * correctly for this view.
         */
        virtual void renderFrame();

    protected:
        /**
         * Constructs the orthographic viewport.
         */
        OrthoViewport(const std::string &name, QWidget *parent = NULL,
                      QGLWidget *shareWith = NULL);

        /**
         * Destroys the viewport.
         */
        virtual ~OrthoViewport() { }

        /**
         * Initializes the viewport.  This just sets the current render style
         * to be wireframe and the view type to be OrthoViewType::TOP.
         */
        void initializeGL();

        /**
         * Sets the orthographic projection parameters of the current camera.
         * @param width The width of the viewport.
         * @param height The height of the viewport.
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
         * @param e
         */
        void mouseMoveEvent(QMouseEvent *e);

        /**
         * Called when the user moves the wheel on a mouse containing a scroll wheel.
         * This causes the scene to be zoomed in and out.
         * @param e
         */
        void wheelEvent(QWheelEvent *e);

        /**
         * Called from the mousePressEvent handler.  This sets the viewport state
         * to properly respond to mouse movement events when in camera mode.
         * @param e
         */
        void beginCameraMode(QMouseEvent *e);

        /**
         * Called from the mouseReleaseEvent handler.  This restores the state of
         * the viewport to it was before camera mode was entered.
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
         * This method is called during mouse movement events if the viewport is
         * currently in the manipulation mode that translates the current actor
         * selection.  This method then goes through the current actor selection
         * and translates each one based on delta mouse movements.
         * @note
         *  Since these viewports are orthographic, when actors are translated,
         *  they are restricted to movement on the plane the orthographic view
         *  is aligned with.
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

        void warpWorldCamera(int x, int y);

    private:
        const InteractionModeExt *currentMode;
        const OrthoViewType *viewType;
        osg::Vec3 zoomToPosition;

        //Allow the ViewportManager access to this class.
        friend class ViewportManager;
    };
}

#endif
