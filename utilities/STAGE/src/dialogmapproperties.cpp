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
#include <prefix/stageprefix.h>
#include <dtEditQt/dialogmapproperties.h>
#include <dtEditQt/editorevents.h>

#include <QtGui/QGroupBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QTextEdit>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>
#include <QtGui/QMessageBox>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DialogMapProperties::DialogMapProperties(QWidget* parent)
      : QDialog(parent)
   {
      QLabel*      label;
      QGridLayout* gridLayout;

      setWindowTitle(tr("Map Properties"));

      // Create the map section...
      QGroupBox* mapGroup = new QGroupBox(tr("Map"), this);
      gridLayout = new QGridLayout(mapGroup);

      label = new QLabel(tr("Name:"), mapGroup);
      label->setAlignment(Qt::AlignRight);
      mName = new QLineEdit(mapGroup);
      gridLayout->addWidget(label, 0, 0);
      gridLayout->addWidget(mName, 0, 1);

      label = new QLabel(tr("Description:"), mapGroup);
      label->setAlignment(Qt::AlignRight);
      mDescription = new QLineEdit(mapGroup);
      gridLayout->addWidget(label, 1, 0);
      gridLayout->addWidget(mDescription, 1, 1);

      // Create the properties section...
      QGroupBox* propsGroup = new QGroupBox(tr("Properties"),this);
      gridLayout = new QGridLayout(propsGroup);

      label = new QLabel(tr("Author:"), propsGroup);
      label->setAlignment(Qt::AlignRight);
      mAuthor = new QLineEdit(propsGroup);
      gridLayout->addWidget(label, 0, 0);
      gridLayout->addWidget(mAuthor, 0, 1);

      label = new QLabel(tr("Copyright:"), propsGroup);
      label->setAlignment(Qt::AlignRight);
      mCopyright = new QLineEdit(propsGroup);
      gridLayout->addWidget(label, 1, 0);
      gridLayout->addWidget(mCopyright, 1, 1);

      label = new QLabel(tr("Comments:"),propsGroup);
      mComments = new QTextEdit(propsGroup);
      gridLayout->addWidget(label, 2, 0, 1, 2);
      gridLayout->addWidget(mComments, 3, 0, 1, 2);

      // Create the ok and cancel buttons...
      QPushButton* okButton     = new QPushButton(tr("OK"),     this);
      QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);
      QHBoxLayout* buttonLayout = new QHBoxLayout;

      okButton->setDefault(true);
      buttonLayout->addStretch(1);
      buttonLayout->addWidget(okButton);
      buttonLayout->addWidget(cancelButton);
      buttonLayout->addStretch(1);
      connect(okButton,     SIGNAL(clicked()), this, SLOT(onOk()));
      connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

      // Add all our components to the main layout.
      QVBoxLayout* mainLayout = new QVBoxLayout(this);
      mainLayout->addWidget(mapGroup);
      mainLayout->addWidget(propsGroup);
      mainLayout->addLayout(buttonLayout);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DialogMapProperties::onOk()
   {
      // Maps must at least have a valid name so check it before closing the
      // dialog.
      if (mName->text().isEmpty())
      {
         QMessageBox::critical(this,tr("Error"),
            tr("Maps must have a valid name."), tr("OK"));
      }
      else
      {
         EditorEvents::GetInstance().emitMapPropertyChanged();
         accept();
      }
   }

} // namespace dtEditQt
