/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Simulation, Training, and Game Editor (STAGE)
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 * David A. Guthrie
 */
#include <prefix/dtstageprefix-src.h>

#include <dtEditQt/gameeventdialog.h>

#include <QtGui/QLineEdit>
#include <QtGui/QCloseEvent>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>

#include <QtGui/QGroupBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QMessageBox>

#include <dtDAL/gameevent.h>

namespace dtEditQt
{
   /// Constructor
   GameEventDialog::GameEventDialog(QWidget *parent, dtDAL::GameEvent& event, bool isNew): QDialog(parent)
   {
      if (isNew)
         setWindowTitle(tr("New Game Event"));
      else
         setWindowTitle(tr("Edit Game Event"));
         
      mGameEvent = &event;   
      
      QGroupBox *groupBox = new QGroupBox(tr("Game Event"), this);
      QGridLayout *gridLayout = new QGridLayout(groupBox);
      QLabel *label;

      label = new QLabel(tr("Name:"),this);
      label->setAlignment(Qt::AlignRight);
      mNameEdit = new QLineEdit(this);
      gridLayout->addWidget(label,0,0);
      gridLayout->addWidget(mNameEdit,0,1);

      label = new QLabel(tr("Description:"),this);
      label->setAlignment(Qt::AlignRight);
      mDescEdit = new QTextEdit(this);
      gridLayout->addWidget(label,1,0);
      gridLayout->addWidget(mDescEdit,1,1);
      
      if (!isNew)
      {
         mNameEdit->insert(event.GetName().c_str());
         mDescEdit->document()->setPlainText(event.GetDescription().c_str());
      }

      //Create the buttons...
      mOKButton = new QPushButton(tr("OK"),this);
      QPushButton *cancelButton = new QPushButton(tr("Cancel"),this);
      QHBoxLayout *buttonLayout = new QHBoxLayout;

      mOKButton->setEnabled(false);
      buttonLayout->addStretch(1);
      buttonLayout->addWidget(mOKButton);
      buttonLayout->addWidget(cancelButton);
      buttonLayout->addStretch(1);

      connect(mOKButton, SIGNAL(clicked()), this, SLOT(ApplyChanges()));
      connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

      QVBoxLayout *mainLayout = new QVBoxLayout(this);
      mainLayout->addWidget(groupBox);
      mainLayout->addLayout(buttonLayout);

      setLayout(mainLayout);
      
      connect(mNameEdit, SIGNAL(textChanged(const QString&)), this, SLOT(Edited(const QString&)));

      //Make sure the button is enabled/disabled properly.
      Edited(mNameEdit->text());
   }

   /// slot for receiving the text changing signal
   void GameEventDialog::Edited(const QString &newText)
   {
      //Enable the ok button now that we have text.
      mOKButton->setEnabled(!newText.isEmpty());
   }

   /// slot for applying the changes made
   void GameEventDialog::ApplyChanges()
   {
      if(mNameEdit->text().isEmpty()) 
      {
         QMessageBox::critical(this, tr("Error"),
             tr("A Game Event must have a valid name."),tr("OK"));
      }
      else
      {
         mGameEvent->SetName(mNameEdit->text().toStdString());
         mGameEvent->SetDescription(mDescEdit->toPlainText().toStdString());
         accept();
      }
   }
   
}
