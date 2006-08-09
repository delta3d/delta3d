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
 * @author Matthew W. Campbell
*/

#include "dtEditQt/dialoglistselection.h"

#include <QtCore/QStringList>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QGroupBox>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    DialogListSelection::DialogListSelection(QWidget *parent,
        const QString &windowTitle, const QString &groupBoxName) : QDialog(parent)
    {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        
        setWindowTitle(windowTitle);
        
        //Create the group box and list view.
        if (!groupBoxName.isEmpty()) 
        {
           QGroupBox *groupBox = new QGroupBox(groupBoxName,this);
           QVBoxLayout *layout = new QVBoxLayout(groupBox);
           listBox = new QListWidget(groupBox);
           layout->addWidget(listBox);
           mainLayout->addWidget(groupBox);
        }
        else 
        {
           listBox = new QListWidget(this);
           mainLayout->addWidget(listBox);
        }

#if QT_VERSION <= 0x040100
        connect(listBox,SIGNAL(itemSelectionChanged()),
            this,SLOT(onSelectionChanged()));
#else
 	     connect(listBox, SIGNAL(itemClicked(QListWidgetItem*)), 
			   this, SLOT(onItemClicked(QListWidgetItem*)));

        connect(listBox, SIGNAL(currentRowChanged(int)), 
           this, SLOT(onCurrentRowChanged(int)));
#endif

        //Create the ok and cancel buttons...
        okButton = new QPushButton(tr("OK"), this);
        cancelButton = new QPushButton(tr("Cancel"), this);
        QHBoxLayout *buttonLayout = new QHBoxLayout;
        
        cancelButton->setDefault(true);
        okButton->setEnabled(false);
        buttonLayout->addStretch(1);
        buttonLayout->addWidget(okButton);
        buttonLayout->addWidget(cancelButton);
        buttonLayout->addStretch(1);
        
        connect(okButton,SIGNAL(clicked()),this,SLOT(accept()));
        connect(cancelButton,SIGNAL(clicked()),this,SLOT(reject()));

        mainLayout->addLayout(buttonLayout);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void DialogListSelection::setListItems(const QStringList &list)
    {
        if (!list.empty()) 
        {
           listBox->addItems(list);
           listBox->setCurrentRow(0);
           okButton->setEnabled(true);
           okButton->setDefault(true);
        }
        else 
        {
           okButton->setEnabled(false);
           cancelButton->setDefault(true);
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    void DialogListSelection::onSelectionChanged()
    {
        QListWidgetItem *item = listBox->currentItem();
        if(item != NULL)
            currentItem = item->text();
        else
            currentItem = tr("");
    }
	
	///////////////////////////////////////////////////////////////////////////////
	void DialogListSelection::onItemClicked(QListWidgetItem *i)
	{
	   currentItem = i->text();	
	}

   ///////////////////////////////////////////////////////////////////////////////
   void DialogListSelection::onCurrentRowChanged(int i)
   {
      QListWidgetItem *item = listBox->item(i);
     
      currentItem = item != NULL ? item->text() : tr("");
   }
}
