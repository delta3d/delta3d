/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2005, BMH Associates, Inc. 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 2.1 of the License, or (at your option) 
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
 * @author Matthew W. Campbell
*/

#include "dtEditQt/dialoglistselection.h"

#include <QStringList>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QListWidget>

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    DialogListSelection::DialogListSelection(QWidget *parent,
        const QString &windowTitle, const QString &groupBoxName) : QDialog(parent)
    {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        
        setWindowTitle(windowTitle);
        
        //Create the group box and list view.
        if (!groupBoxName.isEmpty()) {
            QGroupBox *groupBox = new QGroupBox(groupBoxName,this);
            QVBoxLayout *layout = new QVBoxLayout(groupBox);
            this->listBox = new QListWidget(groupBox);
            layout->addWidget(this->listBox);
            mainLayout->addWidget(groupBox);
        }
        else {
            this->listBox = new QListWidget(this);
            mainLayout->addWidget(this->listBox);
        }

        connect(this->listBox,SIGNAL(itemSelectionChanged()),
            this,SLOT(onSelectionChanged()));

        //Create the ok and cancel buttons...
        this->okButton = new QPushButton(tr("OK"), this);
        this->cancelButton = new QPushButton(tr("Cancel"), this);
        QHBoxLayout *buttonLayout = new QHBoxLayout;
        
        this->cancelButton->setDefault(true);
        this->okButton->setEnabled(false);
        buttonLayout->addStretch(1);
        buttonLayout->addWidget(this->okButton);
        buttonLayout->addWidget(this->cancelButton);
        buttonLayout->addStretch(1);
        connect(this->okButton,SIGNAL(clicked()),this,SLOT(accept()));
        connect(this->cancelButton,SIGNAL(clicked()),this,SLOT(reject()));

        mainLayout->addLayout(buttonLayout);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void DialogListSelection::setListItems(const QStringList &list)
    {
        if (!list.empty()) {
            this->listBox->addItems(list);
            this->listBox->setCurrentRow(0);
            this->okButton->setEnabled(true);
            this->okButton->setDefault(true);
        }
        else {
            this->okButton->setEnabled(false);
            this->cancelButton->setDefault(true);
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    void DialogListSelection::onSelectionChanged()
    {
        QListWidgetItem *item = this->listBox->currentItem();
        if (item != NULL)
            this->currentItem = item->text();
        else
            this->currentItem = tr("");
    }

}
