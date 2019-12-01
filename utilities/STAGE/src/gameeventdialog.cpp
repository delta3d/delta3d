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
 * David A. Guthrie
 */
#include <prefix/stageprefix.h>

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

#include <dtCore/gameevent.h>

namespace dtEditQt
{

   /// Constructor
   GameEventDialog::GameEventDialog(QWidget* parent, dtCore::GameEvent& event, bool isNew)
      : QDialog(parent)
   {
      if (isNew)
      {
         setWindowTitle(tr("New Game Event"));
      }
      else
      {
         setWindowTitle(tr("Edit Game Event"));
      }

      mGameEvent = &event;

      QGroupBox*   groupBox   = new QGroupBox(tr("Game Event"), this);
      QGridLayout* gridLayout = new QGridLayout(groupBox);
      QLabel*      label;

      label = new QLabel(tr("Name:"),this);
      label->setAlignment(Qt::AlignRight);
      mNameEdit = new QLineEdit(this);
      gridLayout->addWidget(label, 0, 0);
      gridLayout->addWidget(mNameEdit, 0, 1);

      label = new QLabel(tr("Description:"),this);
      label->setAlignment(Qt::AlignRight);
      mDescEdit = new QTextEdit(this);
      gridLayout->addWidget(label, 1, 0);
      gridLayout->addWidget(mDescEdit, 1, 1);

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

      QVBoxLayout* mainLayout = new QVBoxLayout(this);
      mainLayout->addWidget(groupBox);
      mainLayout->addLayout(buttonLayout);

      setLayout(mainLayout);

      connect(mNameEdit, SIGNAL(textChanged(const QString&)), this, SLOT(Edited(const QString&)));

      //Make sure the button is enabled/disabled properly.
      Edited(mNameEdit->text());
   }

   /// slot for receiving the text changing signal
   void GameEventDialog::Edited(const QString& newText)
   {
      //Enable the ok button now that we have text.
      mOKButton->setEnabled(!newText.isEmpty());
   }

   /// slot for applying the changes made
   void GameEventDialog::ApplyChanges()
   {
      if (mNameEdit->text().isEmpty())
      {
         QMessageBox::critical(this, tr("Error"),
            tr("A Game Event must have a valid name."), tr("OK"));
      }
      else
      {
         mGameEvent->SetName(mNameEdit->text().toStdString());
         mGameEvent->SetDescription(mDescEdit->toPlainText().toStdString());
         accept();
      }
   }

} // namespace dtEditQt
