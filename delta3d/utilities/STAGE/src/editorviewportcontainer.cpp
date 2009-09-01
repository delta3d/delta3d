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
 * Jeffrey P. Houde
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
#include <QtGui/QCheckBox>
#include <dtEditQt/editorviewportcontainer.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/uiresources.h>

namespace dtEditQt
{
   ///////////////////////////////////////////////////////////////////////////////
   EditorViewportContainer::EditorViewportContainer(QWidget* child, QWidget* parent)
      : QWidget(parent)
      , mLayout(new QVBoxLayout(this))
   {
      mLayout->setMargin(0);
      mLayout->setSpacing(0);

      createToolBar();

      setChild(child);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewportContainer::updateSnaps()
   {
      setSnapTranslation();
      setSnapRotation();
      setSnapScale();
      setSnapEnabled(0);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewportContainer::setChild(QWidget* child)
   {
      if (child)
      {
         //child->setParent(this); //TODO  is this required?
         mLayout->addWidget(child);
         mLayout->addWidget(mToolBar);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewportContainer::setSnapTranslation()
   {
      ViewportManager::GetInstance().emitSetSnapTranslation(mEditTrans->text().toFloat());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewportContainer::setSnapRotation()
   {
      ViewportManager::GetInstance().emitSetSnapRotation(mEditAngle->text().toFloat());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewportContainer::setSnapScale()
   {
      ViewportManager::GetInstance().emitSetSnapScale(mEditScale->text().toFloat());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewportContainer::setSnapEnabled(int state)
   {
      ViewportManager::GetInstance().emitSetSnapEnabled(mCheckBoxTrans->isChecked(), mCheckBoxAngle->isChecked(), mCheckBoxScale->isChecked());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewportContainer::onSetSnapEnabled(bool translation, bool rotation, bool scale)
   {
      mCheckBoxTrans->setChecked(translation);
      mCheckBoxAngle->setChecked(rotation);
      mCheckBoxScale->setChecked(scale);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewportContainer::onSetSnapTranslation(float value)
   {
      mEditTrans->setText(QString::number(value));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewportContainer::onSetSnapRotation(float value)
   {
      mEditAngle->setText(QString::number(value));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewportContainer::onSetSnapScale(float value)
   {
      mEditScale->setText(QString::number(value));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewportContainer::createToolBar()
   {
      // Create our "toolbar" widget.
      mToolBar = new QFrame(this);
      mToolBar->setFrameStyle(QFrame::Box | QFrame::Raised);
      mToolBar->setFixedHeight(25);

      QBoxLayout* layout = new QHBoxLayout(mToolBar);

      layout->setAlignment(Qt::AlignLeft);
      layout->setSpacing(5);
      layout->setMargin(0);
      layout->addSpacing(5);

      SetupSnapWidgets(layout);
      layout->addSpacing(5);
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorViewportContainer::SetupSnapWidgets(QBoxLayout* layout)
   {
      QLabel* label = NULL;

      mCheckBoxTrans = new QCheckBox(this);
      mCheckBoxTrans->setCheckable(true);
      mCheckBoxTrans->setToolTip(tr("Enables the translation snap tool."));
      mCheckBoxTrans->setMaximumWidth(15);
      layout->addWidget(mCheckBoxTrans);

      label = new QLabel(mToolBar);
      label->setText(tr("Snap Translation"));
      label->setMargin(3);
      label->setAlignment(Qt::AlignLeft);
      layout->addWidget(label);

      mEditTrans = new QLineEdit(this);
      mEditTrans->setToolTip(tr("Sets the translation snap increment for the editor gizmos."));
      mEditTrans->setValidator(new QDoubleValidator(0.01, 99999.99, 2, mEditTrans));
      mEditTrans->setMinimumWidth(35);
      mEditTrans->setText(QString::number(10, 'f', 2));
      layout->addWidget(mEditTrans);

      mCheckBoxAngle = new QCheckBox(this);
      mCheckBoxAngle->setCheckable(true);
      mCheckBoxAngle->setToolTip(tr("Enables the translation snap tool."));
      mCheckBoxAngle->setMaximumWidth(15);
      layout->addWidget(mCheckBoxAngle);

      label = new QLabel(mToolBar);
      label->setText(tr("Snap Angle"));
      label->setMargin(3);
      label->setAlignment(Qt::AlignLeft);
      layout->addWidget(label);

      mEditAngle = new QLineEdit(this);
      mEditAngle->setToolTip(tr("Sets the angular snap increment for the editor gizmos."));
      mEditAngle->setValidator(new QDoubleValidator(1, 360, 2, mEditAngle));
      mEditAngle->setMinimumWidth(35);
      mEditAngle->setText(QString::number(45, 'f', 2));
      layout->addWidget(mEditAngle);

      mCheckBoxScale = new QCheckBox(this);
      mCheckBoxScale->setCheckable(true);
      mCheckBoxScale->setToolTip(tr("Enables the translation snap tool."));
      mCheckBoxScale->setMaximumWidth(15);
      layout->addWidget(mCheckBoxScale);

      label = new QLabel(mToolBar);
      label->setText(tr("Snap Scale"));
      label->setMargin(3);
      label->setAlignment(Qt::AlignLeft);
      layout->addWidget(label);

      mEditScale = new QLineEdit(this);
      mEditScale->setToolTip(tr("Sets the scalar snap increment for the editor gizmos."));
      mEditScale->setValidator(new QDoubleValidator(0.01, 99999.99, 2, mEditScale));
      mEditScale->setMinimumWidth(35);
      mEditScale->setText(QString::number(1, 'f', 2));
      layout->addWidget(mEditScale);

      connect(mEditTrans, SIGNAL(editingFinished()), this, SLOT(setSnapTranslation()));
      connect(mEditAngle, SIGNAL(editingFinished()), this, SLOT(setSnapRotation()));
      connect(mEditScale, SIGNAL(editingFinished()), this, SLOT(setSnapScale()));

      connect(mCheckBoxTrans, SIGNAL(stateChanged(int)), this, SLOT(setSnapEnabled(int)));
      connect(mCheckBoxAngle, SIGNAL(stateChanged(int)), this, SLOT(setSnapEnabled(int)));
      connect(mCheckBoxScale, SIGNAL(stateChanged(int)), this, SLOT(setSnapEnabled(int)));

      connect(&ViewportManager::GetInstance(), SIGNAL(setSnapEnabled(bool, bool, bool)), this, SLOT(onSetSnapEnabled(bool, bool, bool)));
      connect(&ViewportManager::GetInstance(), SIGNAL(setSnapTranslation(float)), this, SLOT(onSetSnapTranslation(float)));
      connect(&ViewportManager::GetInstance(), SIGNAL(setSnapRotation(float)), this, SLOT(onSetSnapRotation(float)));
      connect(&ViewportManager::GetInstance(), SIGNAL(setSnapScale(float)), this, SLOT(onSetSnapScale(float)));
   }

} // namespace dtEditQt
