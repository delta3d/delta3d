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
* William E. Johnson II
*/
#include <prefix/dtstageprefix-src.h>
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

#include <dtEditQt/preferencesdialog.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editoractions.h>

namespace dtEditQt
{
    PreferencesDialog::PreferencesDialog(QWidget *parent) : 
       QDialog(parent), 
       mProjectCheck(NULL), 
       mMapCheck(NULL), 
       mRigidCamCheck(NULL), 
       mColor(NULL), 
       mSaveMins(NULL)
    {
        QVBoxLayout *vLay   = new QVBoxLayout(this);
        QGroupBox   *group  = new QGroupBox(tr("Preferences"));
        QGridLayout *grid   = new QGridLayout(group);
        QHBoxLayout *hLay   = new QHBoxLayout;
        QPushButton *ok     = new QPushButton(tr("OK"));
        QPushButton *cancel = new QPushButton(tr("Cancel"));
        QLabel      *label  = new QLabel(tr("Load Last Project on Startup"));
        mColor              = new QPushButton;
        mProjectCheck       = new QCheckBox;
        mMapCheck           = new QCheckBox;
        mRigidCamCheck      = new QCheckBox;
        mSaveMins           = new QSpinBox;

        mProjectCheck->setToolTip(tr("Enables the loading of the most recently loaded project on startup."));
        mMapCheck->setToolTip(tr("Enables the loading of most recently used map on startup. Note that a map cannot be loaded without a project."));
        mRigidCamCheck->setToolTip(tr("Enables the rigid body movement of the camera in a scene."));
        mColor->setToolTip(tr("Selects the color that will be used to highlight selected actors."));
        mSaveMins->setToolTip(tr("Selects the number of minutes in between autosaves. Setting the number of minutes to zero will disable autosave."));

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
        grid->addWidget(label, 3, 0);
        grid->addWidget(mSaveMins,  3, 2);

        label = new QLabel(tr("Selection Color"));
        label->setToolTip(tr("Selects the color that will be used to highlight selected actors."));
        grid->addWidget(label, 4, 0);
        grid->addWidget(mColor, 4, 2);

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
        connect(mSaveMins,      SIGNAL(valueChanged(int)), this, SLOT(onAutosaveDelayChange(int)));
        connect(ok,             SIGNAL(clicked()),         this, SLOT(onOk()));
        connect(cancel,         SIGNAL(clicked()),         this, SLOT(reject()));
        connect(mColor,         SIGNAL(clicked()),         this, SLOT(onColorSelect()));

        setWindowTitle(tr("Preference Editor"));
        
        // Set the existing values
        mSaveMins->setValue((EditorActions::GetInstance().saveMilliSeconds / 1000) / 60);
        
        setNewPalette();

        bool loadProject = EditorData::GetInstance().getLoadLastProject();
        bool loadMap     = EditorData::GetInstance().getLoadLastMap();
        bool rigidCamera = EditorData::GetInstance().getRigidCamera();

        mProjectCheck->setChecked(loadProject);
        
        mMapCheck->setChecked((loadProject && loadMap));
        
        mRigidCamCheck->setChecked(rigidCamera);
        
        setModal(true);
        //resize(200, 300);
        setFixedSize(325, 300);
    }

    void PreferencesDialog::onLastProjectCheckBox(int state)
    {
       bool isChecked = state == Qt::Checked;
       if(!isChecked)
       {
          //mMapCheck->setEnabled(false);
          mMapCheck->setChecked(false);
       }
    }

    void PreferencesDialog::onLastMapCheckBox(int state)
    {
       bool isChecked = state == Qt::Checked;
       if(isChecked)
       {
          //mProjectCheck->setEnabled(true);
          mProjectCheck->setChecked(true);
       }
    }

    void PreferencesDialog::onColorSelect()
    {
        QColor selectedColor = QColorDialog::getColor(EditorData::GetInstance().getSelectionColor(), this);
        EditorData::GetInstance().setSelectionColor(selectedColor);
        
        setNewPalette();
    }

    void PreferencesDialog::onOk()
    {
        EditorData::GetInstance().setLoadLastProject(mProjectCheck->isChecked());
        EditorData::GetInstance().setLoadLastMap(mMapCheck->isChecked());
        EditorData::GetInstance().setRigidCamera(mRigidCamCheck->isChecked());
        // No point in resetting the interval if it didn't change
        int milliSecs = mSaveMins->value() * 60 * 1000;
        if(EditorActions::GetInstance().saveMilliSeconds != milliSecs)
        {
            EditorActions::GetInstance().saveMilliSeconds = milliSecs;
            EditorActions::GetInstance().getTimer()->setInterval(milliSecs);
        }
        accept();
    }

    void PreferencesDialog::setNewPalette()
    {
        QColor selectedColor = EditorData::GetInstance().getSelectionColor();
        QPalette *palette = new QPalette;
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
}
