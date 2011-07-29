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
 * William E. Johnson II
 */
#include <prefix/stageprefix.h>
#include <dtEditQt/mapsaveasdialog.h>

#include <QtGui/QGroupBox>
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QTextEdit>
#include <QtGui/QValidator>
#include <QtGui/QPushButton>
#include <QtGui/QCloseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>

#include <dtCore/project.h>
#include <dtCore/map.h>

namespace dtEditQt
{

   MapSaveAsDialog::MapSaveAsDialog(QWidget* parent)
      : QDialog(parent)
   {
      setWindowTitle(tr("Save Map As"));

      QGroupBox*   groupBox = new QGroupBox("Map",this);
      QGridLayout* gridLayout = new QGridLayout(groupBox);
      QLabel*      label;

      // Create the properties fields..
      label = new QLabel(tr("Name:"),groupBox);
      label->setAlignment(Qt::AlignRight);
      nameEdit = new QLineEdit(groupBox);
      gridLayout->addWidget(label,    0, 0);
      gridLayout->addWidget(nameEdit, 0, 1);

      label = new QLabel(tr("FileName:"),groupBox);
      label->setAlignment(Qt::AlignRight);
      fileEdit = new QLineEdit(groupBox);
      fileEdit->setEnabled(false);
      //fileEdit->setValidator(new QValidator(fileEdit));
      gridLayout->addWidget(label,    1, 0);
      gridLayout->addWidget(fileEdit, 1, 1);

      label = new QLabel(tr("Description:"),groupBox);
      label->setAlignment(Qt::AlignRight);
      descEdit = new QTextEdit(groupBox);
      gridLayout->addWidget(label,    2, 0);
      gridLayout->addWidget(descEdit, 2, 1);

      //Create the buttons...
      okButton = new QPushButton(tr("OK"),this);
      QPushButton* cancelButton = new QPushButton(tr("Cancel"),this);
      QHBoxLayout* buttonLayout = new QHBoxLayout;

      okButton->setEnabled(false);
      buttonLayout->addStretch(1);
      buttonLayout->addWidget(okButton);
      buttonLayout->addWidget(cancelButton);
      buttonLayout->addStretch(1);

      connect(okButton,     SIGNAL(clicked()), this, SLOT(accept()));
      connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

      QVBoxLayout* mainLayout = new QVBoxLayout(this);
      mainLayout->addWidget(groupBox);
      mainLayout->addLayout(buttonLayout);

      connect(nameEdit, SIGNAL(textChanged(const QString&)), this, SLOT(edited(const QString&)));
   }

   ///////////////////////// SLOTS ///////////////////////////////
   void MapSaveAsDialog::edited(const QString& newText)
   {
      QString text = newText;

      text.replace('-', '_');
      text.replace(' ', '_');

      fileEdit->setText(text);

      // Enable the ok button now that we have text.
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

} // namespace dtEditQt
