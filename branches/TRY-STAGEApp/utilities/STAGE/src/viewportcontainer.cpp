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
 */
#include <prefix/dtstageprefix-src.h>
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtGui/QToolButton>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QFrame>
#include <QtGui/QLineEdit>
#include <QtGui/QDoubleValidator>
#include <dtEditQt/viewportcontainer.h>
#include <dtEditQt/viewport.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/orthoviewport.h>
#include <dtEditQt/uiresources.h>

namespace dtEditQt
{
   ///////////////////////////////////////////////////////////////////////////////
   ViewportContainer::ViewportContainer(Viewport* vp, QWidget* parent)
      : QWidget(parent)
      , mLayout(new QVBoxLayout(this))
      , mCameraMovementMenu(NULL)
      , mCameraSpeedGroup(NULL)
   {
      mLayout->setMargin(0);
      mLayout->setSpacing(0);

      createActions();
      createToolBar();
      createContextMenu();

      if (vp != NULL)
      {
         setViewport(vp);
      }
      else
      {
         mViewPort = NULL;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportContainer::setViewport(Viewport* viewPort)
   {
      mViewPort = viewPort;
      mViewportTitle->setText(tr(viewPort->getName().c_str()));
      mViewPort->setParent(this);
      mLayout->addWidget(mViewPort->GetQGLWidget());
      connect(mViewPort,SIGNAL(renderStyleChanged()),
              this, SLOT(onViewportRenderStyleChanged()));

      if (mViewPort->getType() == ViewportManager::ViewportType::PERSPECTIVE)
      {
         addCameraControlWidget();
      }
      else
      {
         if (mCameraMovementMenu != NULL)
         {
            delete mCameraMovementMenu;
            mCameraMovementMenu = NULL;
         }
         if (mCameraSpeedGroup)
         {
            delete mCameraSpeedGroup;
            mCameraSpeedGroup = NULL;
         }
      }

      // Manually call the slot the first time the viewport is set so the state
      // of the actions are set properly.
      onViewportRenderStyleChanged();

      // listen for when the camera moves
      connect(viewPort->getCamera(), SIGNAL(PositionMoved(double, double, double)),
              this, SLOT(OnCameraMoved(double, double, double)));

      // update the widgets manually to get them initialized
      OnCameraMoved(viewPort->getCamera()->getPosition().x(),
                    viewPort->getCamera()->getPosition().y(),
                    viewPort->getCamera()->getPosition().z());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportContainer::contextMenuEvent(QContextMenuEvent* e)
   {
      // Only allow the right-click menu to be invoked when right-clicking the
      // toolbar.
      if (mToolBar->underMouse())
      {
         mContextMenu->exec(e->globalPos());
      }
      else
      {
         e->ignore();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportContainer::createActions()
   {
      // First create our action group so the buttons will be mutually
      // exclusive. (They are all toggle buttons and only one can be
      // active at any given time).
      mRenderStyleActionGroup = new QActionGroup(this);
      mSetWireFrameAction = new QAction(QIcon(UIResources::ICON_VIEWMODE_WIREFRAME.c_str()),
                                       tr("&Wireframe Mode"), this);
      mSetWireFrameAction->setActionGroup(mRenderStyleActionGroup);
      mSetWireFrameAction->setCheckable(true);
      connect(mSetWireFrameAction,SIGNAL(triggered()),
              this, SLOT(setWireFrameView()));

      mSetTexturesOnlyAction = new QAction(QIcon(UIResources::ICON_VIEWMODE_TEXTURES.c_str()),
                                          tr("&Texture Only Mode"), this);
      mSetTexturesOnlyAction->setActionGroup(mRenderStyleActionGroup);
      mSetTexturesOnlyAction->setCheckable(true);
      connect(mSetTexturesOnlyAction, SIGNAL(triggered()),
              this, SLOT(setTexturesOnlyView()));

      mSetLightingOnlyAction = new QAction(QIcon(UIResources::ICON_VIEWMODE_LIGHTING.c_str()),
                                          tr("&Lighting Only Mode"), this);
      mSetLightingOnlyAction->setActionGroup(mRenderStyleActionGroup);
      mSetLightingOnlyAction->setCheckable(true);
      connect(mSetLightingOnlyAction, SIGNAL(triggered()),
              this, SLOT(setLightingOnlyView()));

      mSetTexturesAndLightingAction =
         new QAction(QIcon(UIResources::ICON_VIEWMODE_TEXTURES_AND_LIGHTING.c_str()),
            tr("&Texture and Lighting Mode"), this);
      mSetTexturesAndLightingAction->setActionGroup(mRenderStyleActionGroup);
      mSetTexturesAndLightingAction->setCheckable(true);
      connect(mSetTexturesAndLightingAction, SIGNAL(triggered()),
              this, SLOT(setTexturesAndLightingView()));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportContainer::createToolBar()
   {
      QToolButton* button = NULL;

      // Create our "toolbar" widget.
      mToolBar = new QFrame(this);
      mToolBar->setFrameStyle(QFrame::Box | QFrame::Raised);
      mToolBar->setFixedHeight(25);

      QBoxLayout* layout = new QHBoxLayout(mToolBar);
      mButtonLayout = new QHBoxLayout();

      layout->setAlignment(Qt::AlignLeft);
      layout->setSpacing(0);
      layout->setMargin(0);

      mButtonLayout->setAlignment(Qt::AlignLeft);
      mButtonLayout->setSpacing(0);
      mButtonLayout->setMargin(1);

      // Put a label which holds the name of the viewport.
      mViewportTitle = new QLabel(mToolBar);
      mViewportTitle->setMargin(3);
      mViewportTitle->setAlignment(Qt::AlignLeft);
      QFont labelFont = mViewportTitle->font();
      labelFont.setBold(true);
      mViewportTitle->setFont(labelFont);
      layout->addWidget(mViewportTitle);

      SetupPositionWidgets(layout);

      layout->addStretch(1);
      layout->addLayout(mButtonLayout);

      button = new QToolButton(mToolBar);
      button->setDefaultAction(mSetWireFrameAction);
      button->setAutoRaise(true);
      button->setFocusPolicy(Qt::NoFocus);
      mButtonLayout->addWidget(button);

      button = new QToolButton(mToolBar);
      button->setDefaultAction(mSetLightingOnlyAction);
      button->setAutoRaise(true);
      button->setFocusPolicy(Qt::NoFocus);
      mButtonLayout->addWidget(button);

      button = new QToolButton(mToolBar);
      button->setDefaultAction(mSetTexturesOnlyAction);
      button->setAutoRaise(true);
      button->setFocusPolicy(Qt::NoFocus);
      mButtonLayout->addWidget(button);

      button = new QToolButton(mToolBar);
      button->setDefaultAction(mSetTexturesAndLightingAction);
      button->setAutoRaise(true);
      button->setFocusPolicy(Qt::NoFocus);
      mButtonLayout->addWidget(button);

      mLayout->addWidget(mToolBar);
   }

   void ViewportContainer::createContextMenu()
   {
      mContextMenu = new QMenu(this);

      QMenu* styles = new QMenu(tr("Render Styles"), mContextMenu);
      styles->addAction(mSetWireFrameAction);
      styles->addAction(mSetTexturesOnlyAction);
      styles->addAction(mSetLightingOnlyAction);
      styles->addAction(mSetTexturesAndLightingAction);
      mContextMenu->addMenu(styles);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportContainer::addCameraControlWidget()
   {
      if (mCameraSpeedGroup != NULL)
      {
         return;
      }

      mCameraSpeedGroup = new QActionGroup(this);

      mCameraSpeedSlowest = new QAction(tr("&Slowest"), mCameraSpeedGroup);
      mCameraSpeedSlowest->setCheckable(true);
      connect(mCameraSpeedSlowest, SIGNAL(triggered()), this, SLOT(setCameraSpeedSlowest()));

      mCameraSpeedSlow = new QAction(tr("S&low"), mCameraSpeedGroup);
      mCameraSpeedSlow->setCheckable(true);
      connect(mCameraSpeedSlow,SIGNAL(triggered()), this, SLOT(setCameraSpeedSlow()));

      mCameraSpeedNormal = new QAction(tr("&Normal"), mCameraSpeedGroup);
      mCameraSpeedNormal->setCheckable(true);
      connect(mCameraSpeedNormal,SIGNAL(triggered()), this, SLOT(setCameraSpeedNormal()));

      mCameraSpeedFast = new QAction(tr("&Fast"), mCameraSpeedGroup);
      mCameraSpeedFast->setCheckable(true);
      connect(mCameraSpeedFast,SIGNAL(triggered()), this, SLOT(setCameraSpeedFast()));

      mCameraSpeedFastest = new QAction(tr("F&astest"), mCameraSpeedGroup);
      mCameraSpeedFastest->setCheckable(true);
      connect(mCameraSpeedFastest,SIGNAL(triggered()), this, SLOT(setCameraSpeedFastest()));

      mCameraMovementMenu = new QMenu(tr("Camera Speed"), mContextMenu);
      mCameraMovementMenu->addAction(mCameraSpeedSlowest);
      mCameraMovementMenu->addAction(mCameraSpeedSlow);
      mCameraMovementMenu->addAction(mCameraSpeedNormal);
      mCameraMovementMenu->addAction(mCameraSpeedFast);
      mCameraMovementMenu->addAction(mCameraSpeedFastest);
      mContextMenu->addMenu(mCameraMovementMenu);

      setCameraSpeedNormal();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportContainer::setCameraSpeedSlowest()
   {
      if (mViewPort != NULL)
      {
         mViewPort->setMouseSensitivity(100.0f);
         mCameraSpeedSlowest->setChecked(true);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportContainer::setCameraSpeedSlow()
   {
      if (mViewPort != NULL)
      {
         mViewPort->setMouseSensitivity(50.0f);
         mCameraSpeedSlow->setChecked(true);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportContainer::setCameraSpeedNormal()
   {
      if (mViewPort != NULL)
      {
         mViewPort->setMouseSensitivity(10.0f);
         mCameraSpeedNormal->setChecked(true);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportContainer::setCameraSpeedFast()
   {
      if (mViewPort != NULL)
      {
         mViewPort->setMouseSensitivity(3.0f);
         mCameraSpeedFast->setChecked(true);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportContainer::setCameraSpeedFastest()
   {
      if (mViewPort != NULL)
      {
         mViewPort->setMouseSensitivity(0.2f);
         mCameraSpeedFastest->setChecked(true);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportContainer::onViewportRenderStyleChanged()
   {
      const Viewport::RenderStyle& currStyle = mViewPort->getRenderStyle();

      if (currStyle == Viewport::RenderStyle::WIREFRAME)
      {
         mSetWireFrameAction->setChecked(true);
      }
      else if (currStyle == Viewport::RenderStyle::TEXTURED)
      {
         mSetTexturesOnlyAction->setChecked(true);
      }
      else if (currStyle == Viewport::RenderStyle::LIT)
      {
         mSetLightingOnlyAction->setChecked(true);
      }
      else if (currStyle == Viewport::RenderStyle::LIT_AND_TEXTURED)
      {
         mSetTexturesAndLightingAction->setChecked(true);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ViewportContainer::OnCameraMoved(double x, double y, double z)
   {
      mPositionEditWidgets.at(0)->setText(QString::number(x, 'f', 2));
      mPositionEditWidgets.at(1)->setText(QString::number(y, 'f', 2));
      mPositionEditWidgets.at(2)->setText(QString::number(z, 'f', 2));
   }

   //////////////////////////////////////////////////////////////////////////
   void ViewportContainer::OnNewPositionEntered()
   {
      mViewPort->onGotoPosition(mPositionEditWidgets.at(0)->text().toDouble(),
                                     mPositionEditWidgets.at(1)->text().toDouble(),
                                     mPositionEditWidgets.at(2)->text().toDouble());
   }

   //////////////////////////////////////////////////////////////////////////
   void ViewportContainer::SetupPositionWidgets(QBoxLayout* layout)
   {
      const double minValue = -99999.99;
      const double maxValue = 99999.99;

      QLineEdit* editX = new QLineEdit(this);
      editX->setToolTip(tr("X"));
      editX->setValidator(new QDoubleValidator(minValue, maxValue, 2, editX));
      editX->setMinimumWidth(25);
      mPositionEditWidgets.push_back(editX);

      QLineEdit* editY = new QLineEdit(this);
      editY->setToolTip(tr("Y"));
      editY->setValidator(new QDoubleValidator(minValue, maxValue, 2, editY));
      editY->setMinimumWidth(25);
      mPositionEditWidgets.push_back(editY);

      QLineEdit* editZ = new QLineEdit(this);
      editZ->setToolTip(tr("Z"));
      editZ->setValidator(new QDoubleValidator(minValue, maxValue, 2, editZ));
      editZ->setMinimumWidth(25);
      mPositionEditWidgets.push_back(editZ);

      layout->addWidget(editX);
      layout->addWidget(editY);
      layout->addWidget(editZ);

      connect(editX, SIGNAL(editingFinished()), this, SLOT(OnNewPositionEntered()));
      connect(editY, SIGNAL(editingFinished()), this, SLOT(OnNewPositionEntered()));
      connect(editZ, SIGNAL(editingFinished()), this, SLOT(OnNewPositionEntered()));
   }

} // namespace dtEditQt
