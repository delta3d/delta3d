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
#include <QtGui/QFormLayout>
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
      QFormLayout* form   = new QFormLayout(group);
      QHBoxLayout* hLay   = new QHBoxLayout;
      QPushButton* ok     = new QPushButton(tr("OK"));
      QPushButton* cancel = new QPushButton(tr("Cancel"));
      mColor              = new QPushButton;
      mProjectCheck       = new QCheckBox;
      mMapCheck           = new QCheckBox;
      mRigidCamCheck      = new QCheckBox;
      mSaveMins           = new QSpinBox;

      mActorOffsetDistance = new QLineEdit;
      QDoubleValidator* validator = new QDoubleValidator(mActorOffsetDistance);
      validator->setDecimals(4);
      mActorOffsetDistance->setValidator(validator);

      mNumRecentProjects = new QSpinBox;
      mNumRecentProjects->setMinimum(1);
      mNumRecentProjects->setMaximum(25);

      mProjectCheck->setToolTip(tr("Enables the loading of the most recently loaded project on startup."));
      mMapCheck->setToolTip(tr("Enables the loading of most recently used map on startup. Note that a map cannot be loaded without a project."));
      mRigidCamCheck->setToolTip(tr("Enables the rigid body movement of the camera in a scene."));
      mColor->setToolTip(tr("Selects the color that will be used to highlight selected actors."));
      mSaveMins->setToolTip(tr("Selects the number of minutes in between autosaves. Setting the number of minutes to zero will disable autosave."));
      mActorOffsetDistance->setToolTip(tr("How far away new or duplicated objects should be. Should be small (1.0) for indoor maps, large for big objects, outdoors (10.0)."));
      mNumRecentProjects->setToolTip(tr("The number of recent projects that will be listed in the File->Recent Projects menu."));

      vLay->addWidget(group);

      form->addRow(tr("Load Last &Project on Startup:"), mProjectCheck);
      form->addRow(tr("Load Last &Map on Startup"), mMapCheck);
      form->addRow(tr("&Rigid Body Camera"), mRigidCamCheck);
      form->addRow(tr("&Autosave Delay"), mSaveMins);
      form->addRow(tr("Selection &Color"), mColor);
      form->addRow(tr("Actor Creation &Offset"), mActorOffsetDistance);
      form->addRow(tr("&Number of Recent Projects"), mNumRecentProjects);

      mSaveMins->setMinimum(0);
      mSaveMins->setMaximum(60);
      mSaveMins->setSuffix(tr(" min(s)"));

      hLay->addStretch(1);
      hLay->addWidget(ok);
      hLay->addWidget(cancel);
      hLay->addStretch(1);

      QWidget* buttonWidget = new QWidget;
      buttonWidget->setLayout(hLay);

      vLay->addWidget(buttonWidget);

      connect(mProjectCheck,  SIGNAL(stateChanged(int)), this, SLOT(onLastProjectCheckBox(int)));
      connect(mMapCheck,      SIGNAL(stateChanged(int)), this, SLOT(onLastMapCheckBox(int)));
      connect(mColor,         SIGNAL(clicked()),         this, SLOT(onColorSelect()));
      connect(ok,             SIGNAL(clicked()),         this, SLOT(onOk()));
      connect(cancel,         SIGNAL(clicked()),         this, SLOT(reject()));

      setWindowTitle(tr("Preference Editor"));
      EditorActions& ea = EditorActions::GetInstance();
      // Set the existing values
      mSaveMins->setValue((ea.mSaveMilliSeconds / 1000) / 60);

      setNewPalette();

      EditorData& edData = EditorData::GetInstance();
      bool loadProject = edData.getLoadLastProject();
      bool loadMap     = edData.getLoadLastMap();
      bool rigidCamera = edData.getRigidCamera();

      mProjectCheck->setChecked(loadProject);
      mMapCheck->setChecked((loadProject && loadMap));
      mRigidCamCheck->setChecked(rigidCamera);

      float actorOffsetDistance = edData.GetActorCreationOffset();
      mActorOffsetDistance->setText(QString::number(actorOffsetDistance, 'f', 5));

      mNumRecentProjects->setValue(edData.GetNumRecentProjects());

      setModal(true);
      setFixedSize(minimumSize());
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
      if (selectedColor.isValid())
      {
         EditorData::GetInstance().setSelectionColor(selectedColor);
      }

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

      EditorData::GetInstance().SetNumRecentProjects(mNumRecentProjects->value());

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
