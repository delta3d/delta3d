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
#include <QAction>
#include <QIcon>
#include <QToolButton>
#include <QLabel>
#include <QMenu>
#include <QContextMenuEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include "dtEditQt/viewportcontainer.h"
#include "dtEditQt/viewport.h"
#include "dtEditQt/viewportmanager.h"
#include "dtEditQt/orthoviewport.h"
#include "dtEditQt/uiresources.h"

namespace dtEditQt
{
    ///////////////////////////////////////////////////////////////////////////////
    ViewportContainer::ViewportContainer(Viewport *vp, QWidget *parent) : QWidget(parent)
    {
        this->layout = new QVBoxLayout(this);
        this->layout->setMargin(0);
        this->layout->setSpacing(0);
        this->cameraMovementMenu = NULL;
        this->cameraSpeedGroup = NULL;
        createActions();
        createToolBar();
        createContextMenu();

        if (vp != NULL)
            setViewport(vp);
        else
            this->viewPort = NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportContainer::setViewport(Viewport *viewPort)
    {
        this->viewPort = viewPort;
        this->viewportTitle->setText(tr(viewPort->getName().c_str()));
        this->viewPort->setParent(this);
        this->layout->addWidget(this->viewPort);
        connect(this->viewPort,SIGNAL(renderStyleChanged()),
                this,SLOT(onViewportRenderStyleChanged()));

        if (this->viewPort->getType() == ViewportManager::ViewportType::PERSPECTIVE)
            addCameraControlWidget();
        else {
            if (this->cameraMovementMenu != NULL) {
                delete this->cameraMovementMenu;
                this->cameraMovementMenu = NULL;
            }
            if (this->cameraSpeedGroup) {
                delete this->cameraSpeedGroup;
                this->cameraSpeedGroup = NULL;
            }
        }

        //Manually call the slot the first time the viewport is set so the state
        //of the actions are set properly.
        onViewportRenderStyleChanged();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportContainer::contextMenuEvent(QContextMenuEvent *e)
    {
        //Only allow the right-click menu to be invoked when right-clicking the
        //toolbar.
        if (this->toolBar->underMouse()) {
            this->contextMenu->exec(e->globalPos());
        }
        else {
            e->ignore();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportContainer::createActions()
    {
        //First create our action group so the buttons will be mutually
        //exclusive. (They are all toggle buttons and only one can be
        //active at any given time).
        this->renderStyleActionGroup = new QActionGroup(this);
        this->setWireFrameAction = new QAction(QIcon(UIResources::ICON_VIEWMODE_WIREFRAME.c_str()),
                                               tr("&Wireframe Mode"),this);
        this->setWireFrameAction->setActionGroup(this->renderStyleActionGroup);
        this->setWireFrameAction->setCheckable(true);
        connect(this->setWireFrameAction,SIGNAL(triggered()),
                this,SLOT(setWireFrameView()));

        this->setTexturesOnlyAction = new QAction(QIcon(UIResources::ICON_VIEWMODE_TEXTURES.c_str()),
                                                          tr("&Texture Only Mode"),this);
        this->setTexturesOnlyAction->setActionGroup(this->renderStyleActionGroup);
        this->setTexturesOnlyAction->setCheckable(true);
        connect(this->setTexturesOnlyAction,SIGNAL(triggered()),
                this,SLOT(setTexturesOnlyView()));

        this->setLightingOnlyAction = new QAction(QIcon(UIResources::ICON_VIEWMODE_LIGHTING.c_str()),
                                                          tr("&Lighting Only Mode"),this);
        this->setLightingOnlyAction->setActionGroup(this->renderStyleActionGroup);
        this->setLightingOnlyAction->setCheckable(true);
        connect(this->setLightingOnlyAction,SIGNAL(triggered()),
                this,SLOT(setLightingOnlyView()));

        this->setTexturesAndLightingAction =
                new QAction(QIcon(UIResources::ICON_VIEWMODE_TEXTURES_AND_LIGHTING.c_str()),
                            tr("&Texture and Lighting Mode"),this);
        this->setTexturesAndLightingAction->setActionGroup(this->renderStyleActionGroup);
        this->setTexturesAndLightingAction->setCheckable(true);
        connect(this->setTexturesAndLightingAction,SIGNAL(triggered()),
                        this,SLOT(setTexturesAndLightingView()));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportContainer::createToolBar()
    {
        QToolButton *button = NULL;

        //Create our "toolbar" widget.
        this->toolBar = new QFrame(this);
        this->toolBar->setFrameStyle(QFrame::Box | QFrame::Raised);
        this->toolBar->setFixedHeight(25);

        QBoxLayout *layout = new QHBoxLayout(this->toolBar);
        this->buttonLayout = new QHBoxLayout();

        layout->setAlignment(Qt::AlignLeft);
        layout->setSpacing(0);
        layout->setMargin(0);

        this->buttonLayout->setAlignment(Qt::AlignLeft);
        this->buttonLayout->setSpacing(0);
        this->buttonLayout->setMargin(1);

        //Put a label which holds the name of the viewport.
        this->viewportTitle = new QLabel(this->toolBar);
        this->viewportTitle->setMargin(3);
        this->viewportTitle->setAlignment(Qt::AlignLeft);
        QFont labelFont = this->viewportTitle->font();
        labelFont.setBold(true);
        this->viewportTitle->setFont(labelFont);

        layout->addWidget(this->viewportTitle);
        layout->addStretch(1);
        layout->addLayout(buttonLayout);

        button = new QToolButton(this->toolBar);
        button->setDefaultAction(this->setWireFrameAction);
        button->setAutoRaise(true);
        button->setFocusPolicy(Qt::NoFocus);
        this->buttonLayout->addWidget(button);

        button = new QToolButton(this->toolBar);
        button->setDefaultAction(this->setLightingOnlyAction);
        button->setAutoRaise(true);
        button->setFocusPolicy(Qt::NoFocus);
        this->buttonLayout->addWidget(button);

        button = new QToolButton(this->toolBar);
        button->setDefaultAction(this->setTexturesOnlyAction);
        button->setAutoRaise(true);
        button->setFocusPolicy(Qt::NoFocus);
        this->buttonLayout->addWidget(button);

        button = new QToolButton(this->toolBar);
        button->setDefaultAction(this->setTexturesAndLightingAction);
        button->setAutoRaise(true);
        button->setFocusPolicy(Qt::NoFocus);
        this->buttonLayout->addWidget(button);

        this->layout->addWidget(this->toolBar);
    }

    void ViewportContainer::createContextMenu()
    {
        this->contextMenu = new QMenu(this);

        QMenu *styles = new QMenu(tr("Render Styles"),this->contextMenu);
        styles->addAction(this->setWireFrameAction);
        styles->addAction(this->setTexturesOnlyAction);
        styles->addAction(this->setLightingOnlyAction);
        styles->addAction(this->setTexturesAndLightingAction);
        this->contextMenu->addMenu(styles);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportContainer::addCameraControlWidget()
    {
        if (this->cameraSpeedGroup != NULL)
            return;

        this->cameraSpeedGroup = new QActionGroup(this);

        this->cameraSpeedSlowest = new QAction(tr("&Slowest"),this->cameraSpeedGroup);
        this->cameraSpeedSlowest->setCheckable(true);
        connect(this->cameraSpeedSlowest,SIGNAL(triggered()),this,SLOT(setCameraSpeedSlowest()));

        this->cameraSpeedSlow = new QAction(tr("S&low"),this->cameraSpeedGroup);
        this->cameraSpeedSlow->setCheckable(true);
        connect(this->cameraSpeedSlow,SIGNAL(triggered()),this,SLOT(setCameraSpeedSlow()));

        this->cameraSpeedNormal = new QAction(tr("&Normal"),this->cameraSpeedGroup);
        this->cameraSpeedNormal->setCheckable(true);
        connect(this->cameraSpeedNormal,SIGNAL(triggered()),this,SLOT(setCameraSpeedNormal()));

        this->cameraSpeedFast = new QAction(tr("&Fast"),this->cameraSpeedGroup);
        this->cameraSpeedFast->setCheckable(true);
        connect(this->cameraSpeedFast,SIGNAL(triggered()),this,SLOT(setCameraSpeedFast()));

        this->cameraSpeedFastest = new QAction(tr("F&astest"),this->cameraSpeedGroup);
        this->cameraSpeedFastest->setCheckable(true);
        connect(this->cameraSpeedFastest,SIGNAL(triggered()),this,SLOT(setCameraSpeedFastest()));

        this->cameraMovementMenu = new QMenu(tr("Camera Speed"),this->contextMenu);
        this->cameraMovementMenu->addAction(this->cameraSpeedSlowest);
        this->cameraMovementMenu->addAction(this->cameraSpeedSlow);
        this->cameraMovementMenu->addAction(this->cameraSpeedNormal);
        this->cameraMovementMenu->addAction(this->cameraSpeedFast);
        this->cameraMovementMenu->addAction(this->cameraSpeedFastest);
        this->contextMenu->addMenu(this->cameraMovementMenu);

        setCameraSpeedNormal();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportContainer::setCameraSpeedSlowest()
    {
        if (this->viewPort != NULL) {
            this->viewPort->setMouseSensitivity(100.0f);
            this->cameraSpeedSlowest->setChecked(true);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportContainer::setCameraSpeedSlow()
    {
        if (this->viewPort != NULL) {
            this->viewPort->setMouseSensitivity(50.0f);
            this->cameraSpeedSlow->setChecked(true);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportContainer::setCameraSpeedNormal()
    {
        if (this->viewPort != NULL) {
            this->viewPort->setMouseSensitivity(10.0f);
            this->cameraSpeedNormal->setChecked(true);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportContainer::setCameraSpeedFast()
    {
        if (this->viewPort != NULL) {
            this->viewPort->setMouseSensitivity(3.0f);
            this->cameraSpeedFast->setChecked(true);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportContainer::setCameraSpeedFastest()
    {
        if (this->viewPort != NULL) {
            this->viewPort->setMouseSensitivity(0.2f);
            this->cameraSpeedFastest->setChecked(true);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportContainer::onViewportRenderStyleChanged()
    {
        const Viewport::RenderStyle &currStyle = this->viewPort->getRenderStyle();

        if (currStyle == Viewport::RenderStyle::WIREFRAME) {
            this->setWireFrameAction->setChecked(true);
        }
        else if (currStyle == Viewport::RenderStyle::TEXTURED) {
            this->setTexturesOnlyAction->setChecked(true);
        }
        else if (currStyle == Viewport::RenderStyle::LIT) {
            this->setLightingOnlyAction->setChecked(true);
        }
        else if (currStyle == Viewport::RenderStyle::LIT_AND_TEXTURED) {
            this->setTexturesAndLightingAction->setChecked(true);
        }
    }

}
