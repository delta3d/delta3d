/* 
 * Delta3D Open Source Game and Simulation Engine Level Editor 
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

#include "dtEditQt/dialogmapproperties.h"
#include "dtEditQt/editorevents.h"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    DialogMapProperties::DialogMapProperties(QWidget *parent) : QDialog(parent)
    {
        QLabel *label;
        QGridLayout *gridLayout;

        setWindowTitle(tr("Map Properties"));
        
        //Create the map section...
        QGroupBox *mapGroup = new QGroupBox(tr("Map"),this);
        gridLayout = new QGridLayout(mapGroup);
        
        label = new QLabel(tr("Name:"),mapGroup);
        label->setAlignment(Qt::AlignRight);
        this->name = new QLineEdit(mapGroup);
        gridLayout->addWidget(label,0,0);
        gridLayout->addWidget(this->name,0,1);

        label = new QLabel(tr("Description:"),mapGroup);
        label->setAlignment(Qt::AlignRight);
        this->description = new QLineEdit(mapGroup);
        gridLayout->addWidget(label,1,0);
        gridLayout->addWidget(this->description,1,1);
        
        //Create the properties section...
        QGroupBox *propsGroup = new QGroupBox(tr("Properties"),this);
        gridLayout = new QGridLayout(propsGroup);
        
        label = new QLabel(tr("Author:"),propsGroup);
        label->setAlignment(Qt::AlignRight);
        this->author = new QLineEdit(propsGroup);
        gridLayout->addWidget(label,0,0);
        gridLayout->addWidget(this->author,0,1);

        label = new QLabel(tr("Copyright:"),propsGroup);
        label->setAlignment(Qt::AlignRight);
        this->copyright = new QLineEdit(propsGroup);
        gridLayout->addWidget(label,1,0);
        gridLayout->addWidget(this->copyright,1,1);

        label = new QLabel(tr("Comments:"),propsGroup);
        this->comments = new QTextEdit(propsGroup);
        gridLayout->addWidget(label,2,0,1,2);
        gridLayout->addWidget(this->comments,3,0,1,2);
        
        //Create the ok and cancel buttons...
        QPushButton *okButton = new QPushButton(tr("OK"), this);
        QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);
        QHBoxLayout *buttonLayout = new QHBoxLayout;
        
        okButton->setDefault(true);
        buttonLayout->addStretch(1);
        buttonLayout->addWidget(okButton);
        buttonLayout->addWidget(cancelButton);
        buttonLayout->addStretch(1);
        connect(okButton,SIGNAL(clicked()),this,SLOT(onOk()));
        connect(cancelButton,SIGNAL(clicked()),this,SLOT(reject()));

        //Add all our components to the main layout.
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(mapGroup);
        mainLayout->addWidget(propsGroup);
        mainLayout->addLayout(buttonLayout);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void DialogMapProperties::onOk()
    {
        //Maps must at least have a valid name so check it before closing the
        //dialog.
        if (this->name->text().isEmpty()) {
            QMessageBox::critical(this,tr("Error"),
                tr("Maps must have a valid name."),tr("OK"));
        }
        else {
            EditorEvents::getInstance().emitMapPropertyChanged();
            accept();
        }
    }

}
