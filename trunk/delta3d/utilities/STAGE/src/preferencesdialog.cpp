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
 * William E. Johnson II, Curtiss Murphy
 */
#include <prefix/stageprefix.h>
#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QCheckBox>
#include <QtGui/QSpinBox>
#include <QtGui/QColorDialog>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtCore/QTimer>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>
#include <QtGui/QColor>
#include <QtGui/QPalette>
#include <QtGui/QGroupBox>
#include <QtGui/QLineEdit>

#include <dtEditQt/preferencesdialog.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editoractions.h>

namespace dtEditQt
{

   //////////////////////////////////////////////////////////////////
   PreferencesDialog::PreferencesDialog(QWidget* parent)
      : QDialog(parent)
      , mProjectCheck(NULL)
      , mMapCheck(NULL)
      , mRigidCamCheck(NULL)
      , mColor(NULL)
      , mSaveMins(NULL)
   {
      QVBoxLayout* vLay   = new QVBoxLayout(this);
      QGroupBox*   group  = new QGroupBox(tr("Preferences"));
      QGridLayout* grid   = new QGridLayout(group);
      QHBoxLayout* hLay   = new QHBoxLayout;
      QPushButton* ok     = new QPushButton(tr("OK"));
      QPushButton* cancel = new QPushButton(tr("Cancel"));
      QLabel*      label  = new QLabel(tr("Load Last Project on Startup"));
      mColor              = new QPushButton;
      mProjectCheck       = new QCheckBox;
      mMapCheck           = new QCheckBox;
      mRigidCamCheck      = new QCheckBox;
      mSaveMins           = new QSpinBox;

      mActorOffsetDistance = new QLineEdit;
      QDoubleValidator* validator = new QDoubleValidator(mActorOffsetDistance);
      validator->setDecimals(4);
      mActorOffsetDistance->setValidator(validator);


      mProjectCheck->setToolTip(tr("Enables the loading of the most recently loaded project on startup."));
      mMapCheck->setToolTip(tr("Enables the loading of most recently used map on startup. Note that a map cannot be loaded without a project."));
      mRigidCamCheck->setToolTip(tr("Enables the rigid body movement of the camera in a scene."));
      mColor->setToolTip(tr("Selects the color that will be used to highlight selected actors."));
      mSaveMins->setToolTip(tr("Selects the number of minutes in between autosaves. Setting the number of minutes to zero will disable autosave."));
      mActorOffsetDistance->setToolTip(tr("How far away new or duplicated objects should be. Should be small (1.0) for indoor maps, large for big objects, outdoors (10.0)."));

      vLay->addWidget(group);

      label->setToolTip(tr("Enables the loading of the last project on startup."));
      grid->addWidget(label, 0, 0);
      grid->addWidget(mProjectCheck, 0, 2);

      label = new QLabel(tr("Load Last Map on Startup"));
      label->setToolTip(tr("Enables the loading of the last map on startup. Note that a map cannot be loaded without a project."));
      grid->addWidget(label, 1, 0);
      grid->addWidget(mMapCheck, 1, 2);

      label = new QLabel(tr("Rigid Body Camera"));
      label->setToolTip(tr("Enables the rigid body movement of a camera in a scene."));
      grid->addWidget(label, 2, 0);
      grid->addWidget(mRigidCamCheck, 2, 2);


      label = new QLabel(tr("Autosave Delay"));
      label->setToolTip(tr("Selects the number of minutes in between autosaves. Setting the number of minutes to zero will disable autosave."));
      grid->addWidget(label, 4, 0);
      grid->addWidget(mSaveMins,  4, 2);

      label = new QLabel(tr("Selection Color"));
      label->setToolTip(tr("Selects the color that will be used to highlight selected actors."));
      grid->addWidget(label, 5, 0);
      grid->addWidget(mColor, 5, 2);

      label = new QLabel(tr("Actor Creation Offset"));
      label->setToolTip(tr("How far away new or duplicated objects should be. Should be small (1.0) for indoor maps, large for big objects, outdoors (10.0)."));
      grid->addWidget(label, 6, 0);
      grid->addWidget(mActorOffsetDistance, 6, 2);

      mSaveMins->setMinimum(0);
      mSaveMins->setMaximum(60);
      mSaveMins->setSuffix(tr(" min(s)"));

      hLay->setParent(vLay);
      hLay->addStretch(1);
      hLay->addWidget(ok);
      hLay->addWidget(cancel);
      hLay->addStretch(1);

      vLay->addLayout(hLay);

      connect(mProjectCheck,  SIGNAL(stateChanged(int)), this, SLOT(onLastProjectCheckBox(int)));
      connect(mMapCheck,      SIGNAL(stateChanged(int)), this, SLOT(onLastMapCheckBox(int)));
      connect(mColor,         SIGNAL(clicked()),         this, SLOT(onColorSelect()));
      connect(ok,             SIGNAL(clicked()),         this, SLOT(onOk()));
      connect(cancel,         SIGNAL(clicked()),         this, SLOT(reject()));

      setWindowTitle(tr("Preference Editor"));

      // Set the existing values
      mSaveMins->setValue((EditorActions::GetInstance().mSaveMilliSeconds / 1000) / 60);

      setNewPalette();

      bool loadProject = EditorData::GetInstance().getLoadLastProject();
      bool loadMap     = EditorData::GetInstance().getLoadLastMap();
      bool rigidCamera = EditorData::GetInstance().getRigidCamera();

      mProjectCheck->setChecked(loadProject);
      mMapCheck->setChecked((loadProject && loadMap));
      mRigidCamCheck->setChecked(rigidCamera);

      float actorOffsetDistance = EditorData::GetInstance().GetActorCreationOffset();
      mActorOffsetDistance->setText(QString::number(actorOffsetDistance, 'f', 5));

      setModal(true);
      //resize(200, 300);
      setFixedSize(325, 300);
   }

   //////////////////////////////////////////////////////////////////
   void PreferencesDialog::onLastProjectCheckBox(int state)
   {
      bool isChecked = state == Qt::Checked;
      if (!isChecked)
      {
         //mMapCheck->setEnabled(false);
         mMapCheck->setChecked(false);
      }
   }

   //////////////////////////////////////////////////////////////////
   void PreferencesDialog::onLastMapCheckBox(int state)
   {
      bool isChecked = state == Qt::Checked;
      if (isChecked)
      {
         //mProjectCheck->setEnabled(true);
         mProjectCheck->setChecked(true);
      }
   }

   //////////////////////////////////////////////////////////////////
   void PreferencesDialog::onColorSelect()
   {
      QColor selectedColor = QColorDialog::getColor(EditorData::GetInstance().getSelectionColor(), this);
      EditorData::GetInstance().setSelectionColor(selectedColor);

      setNewPalette();
   }

   //////////////////////////////////////////////////////////////////
   void PreferencesDialog::onOk()
   {
      EditorData::GetInstance().setLoadLastProject(mProjectCheck->isChecked());
      EditorData::GetInstance().setLoadLastMap(mMapCheck->isChecked());
      EditorData::GetInstance().setRigidCamera(mRigidCamCheck->isChecked());

      // No point in resetting the interval if it didn't change
      int milliSecs = mSaveMins->value() * 60 * 1000;
      if (EditorActions::GetInstance().mSaveMilliSeconds != milliSecs)
      {
         EditorActions::GetInstance().mSaveMilliSeconds = milliSecs;
         EditorActions::GetInstance().getTimer()->setInterval(milliSecs);
      }

      // Creation offset.
      bool success = false;
      float result = mActorOffsetDistance->text().toFloat(&success);
      if (success)
      {
         EditorData::GetInstance().SetActorCreationOffset(result);
      }

      accept();
   }

   //////////////////////////////////////////////////////////////////
   void PreferencesDialog::setNewPalette()
   {
      QColor selectedColor = EditorData::GetInstance().getSelectionColor();
      QPalette* palette = new QPalette;
      palette->setColor(QPalette::Disabled, QPalette::Background, selectedColor);
      palette->setColor(QPalette::Active,   QPalette::Background, selectedColor);
      palette->setColor(QPalette::Inactive, QPalette::Background, selectedColor);
      palette->setColor(QPalette::Normal,   QPalette::Background, selectedColor);
      mColor->setPalette(*palette);

      int r, g, b;
      selectedColor.getRgb(&r, &g, &b);
      mColor->setText(tr("R=")  + QString::number(r) +
         tr(" G=") + QString::number(g) +
         tr(" B=") + QString::number(b));
   }

} // namespace dtEditQt
