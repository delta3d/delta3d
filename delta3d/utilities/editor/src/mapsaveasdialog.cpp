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

#include "dtEditQt/mapsaveasdialog.h"

#include <QGroupBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QValidator>
#include <QPushButton>
#include <QCloseEvent>
#include <QMessageBox>
#include <QGroupBox>
#include <QLabel>

#include "dtDAL/project.h"
#include "dtDAL/map.h"
#include "dtDAL/exception.h"

namespace dtEditQt
{
    MapSaveAsDialog::MapSaveAsDialog(QWidget *parent) : QDialog(parent)
    {
        setWindowTitle(tr("Save Map As"));

        QGroupBox *groupBox = new QGroupBox("Map",this);
        QGridLayout *gridLayout = new QGridLayout(groupBox);
        QLabel *label;

        //Create the properties fields..
        label = new QLabel(tr("Name:"),groupBox);
        label->setAlignment(Qt::AlignRight);
        nameEdit = new QLineEdit(groupBox);
        gridLayout->addWidget(label,0,0);
        gridLayout->addWidget(nameEdit,0,1);

        label = new QLabel(tr("FileName:"),groupBox);
        label->setAlignment(Qt::AlignRight);
        fileEdit = new QLineEdit(groupBox);
        fileEdit->setEnabled(false);
        //fileEdit->setValidator(new QValidator(fileEdit));
        gridLayout->addWidget(label,1,0);
        gridLayout->addWidget(fileEdit,1,1);

        label = new QLabel(tr("Description:"),groupBox);
        label->setAlignment(Qt::AlignRight);
        descEdit = new QTextEdit(groupBox);
        gridLayout->addWidget(label,2,0);
        gridLayout->addWidget(descEdit,2,1);

        //Create the buttons...
        okButton = new QPushButton(tr("OK"),this);
        QPushButton *cancelButton = new QPushButton(tr("Cancel"),this);
        QHBoxLayout *buttonLayout = new QHBoxLayout;

        okButton->setEnabled(false);
        buttonLayout->addStretch(1);
        buttonLayout->addWidget(okButton);
        buttonLayout->addWidget(cancelButton);
        buttonLayout->addStretch(1);

        connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
        connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(groupBox);
        mainLayout->addLayout(buttonLayout);

        connect(nameEdit, SIGNAL(textChanged(const QString&)), this, SLOT(edited(const QString&)));
    }

    ///////////////////////// SLOTS ///////////////////////////////
    void MapSaveAsDialog::edited(const QString &newText)
    {
        QString text = newText;

        text.replace('-', '_');
        text.replace(' ', '_');

        fileEdit->setText(text);

        //Enable the ok button now that we have text.
        !text.isEmpty() ? okButton->setEnabled(true) : okButton->setEnabled(false);
    }

    std::string MapSaveAsDialog::getMapName()
    {
        return nameEdit->text().toStdString();
    }

    std::string MapSaveAsDialog::getMapFileName()
    {
        return fileEdit->text().toStdString();
    }

    std::string MapSaveAsDialog::getMapDescription()
    {
        return descEdit->toPlainText().toStdString();
    }
}
