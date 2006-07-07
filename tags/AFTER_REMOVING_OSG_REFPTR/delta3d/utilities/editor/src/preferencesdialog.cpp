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
* @author William E. Johnson II
*/

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

#include "dtEditQt/preferencesdialog.h"
#include "dtEditQt/editordata.h"
#include "dtEditQt/editoractions.h"

namespace dtEditQt
{
    PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent)
    {
        QVBoxLayout *vLay   = new QVBoxLayout(this);
        QGroupBox   *group  = new QGroupBox(tr("Preferences"));
        QGridLayout *grid   = new QGridLayout(group);
        QHBoxLayout *hLay   = new QHBoxLayout;
        QSpinBox    *spin   = new QSpinBox;
        QPushButton *ok     = new QPushButton(tr("OK"));
        QPushButton *cancel = new QPushButton(tr("Cancel"));
        QLabel      *label  = new QLabel(tr("Load Last Project on Startup"));
        color               = new QPushButton;
        projectCheck        = new QCheckBox;
        mapCheck            = new QCheckBox;
        rigidCamCheck       = new QCheckBox;

        projectCheck->setToolTip(tr("Enables the loading of the most recently loaded project on startup."));
        mapCheck->setToolTip(tr("Enables the loading of most recently used map on startup. Note that a map cannot be loaded without a project."));
        rigidCamCheck->setToolTip(tr("Enables the rigid body movement of the camera in a scene."));
        color->setToolTip(tr("Selects the color that will be used to highlight selected actors."));
        spin->setToolTip(tr("Selects the number of minutes in between autosaves. Setting the number of minutes to zero will disable autosave."));

        vLay->addWidget(group);

        label->setToolTip(tr("Enables the loading of recent projects on startup."));
        grid->addWidget(label, 0, 0);
        grid->addWidget(projectCheck, 0, 2);

        label = new QLabel(tr("Load Last Map on Startup"));
        label->setToolTip(tr("Enables the loading of recent maps on startup. Note that a map cannot be loaded without a project."));
        grid->addWidget(label, 1, 0);
        grid->addWidget(mapCheck, 1, 2);

        label = new QLabel(tr("Rigid Body Camera"));
        label->setToolTip(tr("Enables the rigid body movement of a camera in a scene."));
        grid->addWidget(label, 2, 0);
        grid->addWidget(rigidCamCheck, 2, 2);
        
        label = new QLabel(tr("Autosave Delay"));
        label->setToolTip(tr("Selects the number of minutes in between autosaves. Setting the number of minutes to zero will disable autosave."));
        grid->addWidget(label, 3, 0);
        grid->addWidget(spin,  3, 2);

        label = new QLabel(tr("Selection Color"));
        label->setToolTip(tr("Selects the color that will be used to highlight selected actors."));
        grid->addWidget(label, 4, 0);
        grid->addWidget(color, 4, 2);

        spin->setMinimum(0);
        spin->setMaximum(60);
        spin->setSuffix(tr(" min(s)"));

        hLay->setParent(vLay);
        hLay->addStretch(1);
        hLay->addWidget(ok);
        hLay->addWidget(cancel);
        hLay->addStretch(1);

        vLay->addLayout(hLay);

        connect(projectCheck, SIGNAL(stateChanged(int)), this, SLOT(onLastProjectCheckBox(int)));
        connect(mapCheck,     SIGNAL(stateChanged(int)), this, SLOT(onLastMapCheckBox    (int)));
        connect(rigidCamCheck,SIGNAL(stateChanged(int)), this, SLOT(onRigidBodyCameraCheckBox(int)));
        connect(spin,         SIGNAL(valueChanged(int)), this, SLOT(onAutosaveDelayChange(int)));
        connect(ok,           SIGNAL(clicked()),         this, SLOT(onOk()));
        connect(cancel,       SIGNAL(clicked()),         this, SLOT(reject()));
        connect(color,        SIGNAL(clicked()),         this, SLOT(onColorSelect()));

        setWindowTitle(tr("Preference Editor"));
        
        // Set the existing values
        saveMins = (EditorActions::getInstance().saveMilliSeconds / 1000) / 60;
        spin->setValue(saveMins);
        
        setNewPalette();

        projectCheck->setCheckState(EditorData::getInstance().getLoadLastProject() ? Qt::Checked : Qt::Unchecked);
        mapCheck->setCheckState(EditorData::getInstance().getLoadLastMap() ? Qt::Checked : Qt::Unchecked);
        rigidCamCheck->setCheckState(EditorData::getInstance().getRigidCamera() ? Qt::Checked : Qt::Unchecked);
        setModal(true);
        //resize(200, 300);
        setFixedSize(300, 300);
    }

    void PreferencesDialog::onLastProjectCheckBox(int state)
    {
        if(state == Qt::Unchecked)
        {
            loadProjects = false;
            loadMaps     = false;
            mapCheck->setEnabled(false);
            mapCheck->setCheckState(Qt::Unchecked);
        }
        else
        {
            loadProjects = true;
            mapCheck->setEnabled(true);
        }
    }

    void PreferencesDialog::onLastMapCheckBox(int state)
    {
        loadMaps = (state == Qt::Checked);
    }

    void PreferencesDialog::onRigidBodyCameraCheckBox(int state)
    {
        rigidCamera = (state == Qt::Checked);
    }

    void PreferencesDialog::onAutosaveDelayChange(int i)
    {
        saveMins = i;
    }

    void PreferencesDialog::onColorSelect()
    {
        QColor selectedColor = QColorDialog::getColor(EditorData::getInstance().getSelectionColor(), this);
        EditorData::getInstance().setSelectionColor(selectedColor);
        
        setNewPalette();
    }

    void PreferencesDialog::onOk()
    {
        EditorData::getInstance().setLoadLastProject(loadProjects);
        EditorData::getInstance().setLoadLastMap(loadMaps);
        EditorData::getInstance().setRigidCamera(rigidCamera);
        // No point in resetting the interval if it didn't change
        int milliSecs = saveMins * 60 * 1000;
        if(EditorActions::getInstance().saveMilliSeconds != milliSecs)
        {
            EditorActions::getInstance().saveMilliSeconds = milliSecs;
            EditorActions::getInstance().getTimer()->setInterval(milliSecs);
        }
        accept();
    }

    void PreferencesDialog::setNewPalette()
    {
        QColor selectedColor = EditorData::getInstance().getSelectionColor();
        QPalette *palette = new QPalette;
        palette->setColor(QPalette::Disabled, QPalette::Background, selectedColor);
        palette->setColor(QPalette::Active,   QPalette::Background, selectedColor);
        palette->setColor(QPalette::Inactive, QPalette::Background, selectedColor);
        palette->setColor(QPalette::Normal,   QPalette::Background, selectedColor);
        color->setPalette(*palette);

        int r, g, b;
        selectedColor.getRgb(&r, &g, &b);
        color->setText(tr("R=")  + QString::number(r) + 
                       tr(" G=") + QString::number(g) + 
                       tr(" B=") + QString::number(b));
    }
}
