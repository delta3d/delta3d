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
#include <prefix/stageprefix.h>
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
#include <dtEditQt/editoractions.h>
#include <dtEditQt/viewportcontainer.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/editorviewport.h>
#include <dtEditQt/uiresources.h>
#include <QtGui/QSlider>
#include <dtUtil/mathdefines.h>

namespace dtEditQt
{
   //Camera speed scale values
   const float MINCAMSPEED = 0.2f;
   const float MAXCAMSPEED = 5.f;

   // Clip distance limit values.
   const float MINNEARCLIPDISTANCE = 0.01f;
   const float MAXNEARCLIPDISTANCE = 1.0f;
   const float MINFARCLIPDISTANCE = 50.0f;
   const float MAXFARCLIPDISTANCE = 100000.0f;

   /////////////////////////////////////////////////////////////////////////////
   ViewportContainer::ViewportContainer(Viewport* vp, QWidget* parent)
      : QWidget(parent)
      , mLayout(new QVBoxLayout(this))
      , mCameraSpeedLabel(NULL)
      , mCameraSpeedSlider(NULL)
      , mClipDistanceLabel(NULL)
      , mClipDistanceSlider(NULL)
   {
      mLayout->setMargin(0);
      mLayout->setSpacing(0);

      createActions();
      createToolBar();

      if (vp != NULL)
      {
         setViewport(vp);

         if (vp->getType() == ViewportManager::ViewportType::PERSPECTIVE)
         {
            mButtonLayout->addWidget(CreateActorCameraAlignButton(mToolBar));
         }
      }
      else
      {
         mViewPort = NULL;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ViewportContainer::setViewport(Viewport* viewPort)
   {
      mViewPort = viewPort;
      mViewportTitle->setText(tr(viewPort->getName().c_str()));
      //mViewPort->setParent(this); //TODO redundant?
      mLayout->addWidget(mViewPort->GetQGLWidget());
      connect(mViewPort,SIGNAL(renderStyleChanged()),
              this, SLOT(onViewportRenderStyleChanged()));


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


   /////////////////////////////////////////////////////////////////////////////
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

   /////////////////////////////////////////////////////////////////////////////
   void ViewportContainer::createToolBar()
   {
      QToolButton* button = NULL;

      // Create our "toolbar" widget.
      mToolBar = new QFrame(this);
      mToolBar->setFrameStyle(QFrame::Box | QFrame::Raised);

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

      // Camera speed icon.
      mCameraSpeedLabel = new QLabel("", this);
      QPixmap pix = QPixmap(UIResources::ICON_VIEWCONTROL_CAMERA_SPEED.c_str());
      pix = pix.scaled(24, 24);
      mCameraSpeedLabel->setPixmap(pix);

      // Camera speed control.
      mCameraSpeedSlider = new QSlider(Qt::Horizontal, this);
      mCameraSpeedSlider->setToolTip(tr("Camera motion speed"));
      mCameraSpeedSlider->setRange(1, 5);
      mCameraSpeedSlider->setMinimumWidth(30);
      connect(mCameraSpeedSlider, SIGNAL(valueChanged(int)), this, SLOT(SetCameraSpeed(int)));

      // Clip distance icon.
      mClipDistanceLabel = new QLabel("", this);
      pix = QPixmap(UIResources::ICON_VIEWCONTROL_CLIP_DISTANCE.c_str());
      pix = pix.scaled(24, 24);
      mClipDistanceLabel->setPixmap(pix);

      // Clip distance control.
      mClipDistanceSlider = new QSlider(Qt::Horizontal, this);
      mClipDistanceSlider->setToolTip(tr("Camera View Distance"));
      mClipDistanceSlider->setRange(1, 8);
      mClipDistanceSlider->setMinimumWidth(30);
      connect(mClipDistanceSlider, SIGNAL(valueChanged(int)), this, SLOT(SetClipDistance(int)));

      layout->addSpacing(8);
      layout->addWidget(mCameraSpeedLabel);
      layout->addWidget(mCameraSpeedSlider);

      layout->addSpacing(8);
      layout->addWidget(mClipDistanceLabel);
      layout->addWidget(mClipDistanceSlider);

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
      mToolBar->setFixedHeight(layout->sizeHint().height());
   }

   /////////////////////////////////////////////////////////////////////////////
   void ViewportContainer::SetCameraSpeed(int value)
   {
      //if someone else called this, then we'll update the widget to match
      if (sender() != mCameraSpeedSlider)
      {
         mCameraSpeedSlider->setValue(value);
      }

      if (mViewPort != NULL)
      {
         //assume the supplied value is in the range of the widget
         const float uiMin = mCameraSpeedSlider->minimum();
         const float uiMax = mCameraSpeedSlider->maximum();

         const float speed = dtUtil::MapRangeValue(float(value), uiMin, uiMax,
                                                   MINCAMSPEED, MAXCAMSPEED);
         mViewPort->setMouseSensitivity(speed);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ViewportContainer::SetClipDistance(int value)
   {
      // If someone else called this, then we'll update the widget to match.
      if (sender() != mClipDistanceSlider)
      {
         mClipDistanceSlider->setValue(value);
      }

      if (mViewPort != NULL)
      {
         // Assume the supplied value is in the range of the widget.
         const float uiMin = mClipDistanceSlider->minimum();
         const float uiMax = mClipDistanceSlider->maximum();

         const float nearClip = dtUtil::MapRangeValue(float(value), uiMin, uiMax,
                                                      MINNEARCLIPDISTANCE, MAXNEARCLIPDISTANCE);

         const float farClip = dtUtil::MapRangeValue(float(value), uiMin, uiMax,
                                                      MINFARCLIPDISTANCE, MAXFARCLIPDISTANCE);

         mViewPort->getCamera()->setNearClipPlane(nearClip);
         mViewPort->getCamera()->setFarClipPlane(farClip);

         mViewPort->refresh();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   int ViewportContainer::GetCameraSpeed() const
   {
      return mCameraSpeedSlider->value();
   }

   /////////////////////////////////////////////////////////////////////////////
   int ViewportContainer::GetClipDistance() const
   {
      return mClipDistanceSlider->value();
   }

   /////////////////////////////////////////////////////////////////////////////
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

   /////////////////////////////////////////////////////////////////////////////
   void ViewportContainer::OnCameraMoved(double x, double y, double z)
   {
      mPositionEditWidgets.at(0)->setText(QString::number(x, 'f', 2));
      mPositionEditWidgets.at(1)->setText(QString::number(y, 'f', 2));
      mPositionEditWidgets.at(2)->setText(QString::number(z, 'f', 2));
   }

   /////////////////////////////////////////////////////////////////////////////
   void ViewportContainer::OnNewPositionEntered()
   {
      mViewPort->onGotoPosition(mPositionEditWidgets.at(0)->text().toDouble(),
                                mPositionEditWidgets.at(1)->text().toDouble(),
                                mPositionEditWidgets.at(2)->text().toDouble());
   }

   /////////////////////////////////////////////////////////////////////////////
   void ViewportContainer::SetupPositionWidgets(QBoxLayout* layout)
   {
      const double minValue = -FLT_MAX;
      const double maxValue = FLT_MAX;

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

   /////////////////////////////////////////////////////////////////////////////
   void ViewportContainer::OnMoveActorOrCameraTriggered(QAction* action)
   {
      QToolButton *widget = qobject_cast<QToolButton*>(sender());
      if (widget == NULL || action == NULL)
      {
         return;
      }

      widget->setDefaultAction(action);

      EditorViewport* editorViewport = dynamic_cast<EditorViewport*>(mViewPort);
      if (editorViewport)
      {
         editorViewport->slotMoveActorOrCamera(action);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   QAbstractButton* ViewportContainer::CreateActorCameraAlignButton(QFrame* parent) const
   {
      //A QToolButton that has two Actions added to it. These actions pop up like a menu
      QToolButton* button = new QToolButton(parent);
      button->setDefaultAction(EditorActions::GetInstance().mAlignCameraToActorAction);
      connect(button, SIGNAL(triggered(QAction*)), this, SLOT(OnMoveActorOrCameraTriggered(QAction*)));

      QMenu* menu = new QMenu("CameraActorMenu");
      menu->addAction(EditorActions::GetInstance().mAlignCameraToActorAction);
      menu->addAction(EditorActions::GetInstance().mAlignActorToCameraAction);
      button->setMenu(menu);

      return button;
   }

} // namespace dtEditQt
