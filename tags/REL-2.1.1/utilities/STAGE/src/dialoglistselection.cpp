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
#include <prefix/dtstageprefix-src.h>
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

        connect(listBox, SIGNAL(itemDoubleClicked(QListWidgetItem*)), 
           this, SLOT(onItemDoubleClicked(QListWidgetItem*)));

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
   void DialogListSelection::onItemDoubleClicked(QListWidgetItem *i)
   {
      currentItem = i->text();
      this->done(QDialog::Accepted);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DialogListSelection::onCurrentRowChanged(int i)
   {
      QListWidgetItem *item = listBox->item(i);
     
      currentItem = item != NULL ? item->text() : tr("");
   }
}
