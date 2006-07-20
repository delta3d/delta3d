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
#ifndef DELTA_VIEWPORT_CONTAINER
#define DELTA_VIEWPORT_CONTAINER

#include <QtGui/QWidget>
#include "dtEditQt/viewport.h"

class QAction;
class QActionGroup;
class QLabel;
class QMenu;
class QContextMenuEvent;
class QBoxLayout;
class QFrame;

namespace dtEditQt
{
    /**
     * The viewport is a higher level container widget used to house viewport widgets and
     * control their behavior.  The container has a toolbar in addition to the viewport
     * which contains actions that determine for example, how the viewport displays its
     * contents.
     * @note
     *  The viewports themselves have methods to control their behavior, therefore, this
     *  class acts as a UI interface to that functionality.
     */
    class ViewportContainer : public QWidget
    {
        Q_OBJECT

    public:

        /**
         * Constructs the viewport container.  Note, there is no viewport currently assigned
         * to it.
         * @param parent The parent widget to assign to this container.
         * @return
         * @see setViewport
         */
        ViewportContainer(Viewport *vp = NULL, QWidget *parent = NULL);

        /**
         * Assigns a viewport to this container.
         * @param viewPort The viewport to put inside this container.
         */
        void setViewport(Viewport *viewPort);

    public slots:

        /**
         * Tells the current viewport object to render its contents in wireframe mode.
         * If the current viewport is invalid, this method does nothing.
         * @see Viewport::RenderStyle
         */
        void setWireFrameView() {
            if (this->viewPort != NULL)
                this->viewPort->setRenderStyle(Viewport::RenderStyle::WIREFRAME);
        }

        /**
         * Tells the current viewport object to render its contents in textured mode.
         * If the current viewport is invalid, this method does nothing.
         * @see Viewport::RenderStyle
         */
        void setTexturesOnlyView() {
            if (this->viewPort != NULL)
                this->viewPort->setRenderStyle(Viewport::RenderStyle::TEXTURED);
        }

        /**
         * Tells the current viewport object to render its contents with lighting
         * enabled. If the current viewport is invalid, this method does nothing.
         * @see Viewport::RenderStyle
         */
        void setLightingOnlyView() {
            if (this->viewPort != NULL)
                this->viewPort->setRenderStyle(Viewport::RenderStyle::LIT);
        }

        /**
         * Tells the current viewport object to render its contents in textured mode
         * as well as with lighting enabled.  If the current viewport is invalid,
         * this method does nothing.
         * @see Viewport::RenderStyle
         */
        void setTexturesAndLightingView() {
            if (this->viewPort != NULL)
                this->viewPort->setRenderStyle(Viewport::RenderStyle::LIT_AND_TEXTURED);
        }

        /**
         * This method is called when the render style of the viewport owned by this
         * container has changed.
         */
        void onViewportRenderStyleChanged();

        ///Sets the camera speed to the slowest setting.
        void setCameraSpeedSlowest();

        ///Sets the camera speed to a slow setting.
        void setCameraSpeedSlow();

        ///Sets the camera speed to the default.
        void setCameraSpeedNormal();

        ///Sets the camera speed to a fast setting.
        void setCameraSpeedFast();

        ///Sets the camera speed to the fastest setting.
        void setCameraSpeedFastest();

    protected:
        ///Creates the toolbar action objects.
        void createActions();

        ///Adds the action objects to this containers toolbar.
        void createToolBar();

        ///Creates a right-click menu for the toolbar of this container.
        void createContextMenu();

        /**
         * Overridden to ensure the right-click menu only appears when right clicking
         * on the toolbar and not on the viewport widget.
         */
        void contextMenuEvent(QContextMenuEvent *e);

        /**
         * Creates a widget for controlling the camera speed on this viewport.
         * @note Only gets called if the viewport in this container is a perspective viewport.
         */
        void addCameraControlWidget();

    private:
        Viewport *viewPort;

        QBoxLayout *layout;
        QBoxLayout *buttonLayout;
        QLabel *viewportTitle;
        QMenu *contextMenu;
        QMenu *cameraMovementMenu;
        QFrame *toolBar;

        QActionGroup *renderStyleActionGroup;
        QActionGroup *cameraSpeedGroup;

        //Action objects.
        QAction *setWireFrameAction;
        QAction *setTexturesOnlyAction;
        QAction *setLightingOnlyAction;
        QAction *setTexturesAndLightingAction;

        QAction *cameraSpeedSlowest;
        QAction *cameraSpeedSlow;
        QAction *cameraSpeedNormal;
        QAction *cameraSpeedFast;
        QAction *cameraSpeedFastest;
    };
}

#endif
