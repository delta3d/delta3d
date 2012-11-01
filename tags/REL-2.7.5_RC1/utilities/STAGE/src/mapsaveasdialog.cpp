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

   MapSaveAsDialog::MapSaveAsDialog(const std::string& mapName, const std::string& categoryName, const std::string& description, QWidget* parent)
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
      nameEdit->setText(mapName.c_str());
      gridLayout->addWidget(label,    0, 0);
      gridLayout->addWidget(nameEdit, 0, 1);

      label = new QLabel(tr("Category:"),groupBox);
      label->setAlignment(Qt::AlignRight);
      categoryEdit = new QLineEdit(groupBox);
      categoryEdit->setText(categoryName.c_str());
      gridLayout->addWidget(label,        1, 0);
      gridLayout->addWidget(categoryEdit, 1, 1);

      label = new QLabel(tr("FileName:"),groupBox);
      label->setAlignment(Qt::AlignRight);
      fileEdit = new QLineEdit(groupBox);
      fileEdit->setEnabled(false);
      //fileEdit->setValidator(new QValidator(fileEdit));
      gridLayout->addWidget(label,    2, 0);
      gridLayout->addWidget(fileEdit, 2, 1);

      label = new QLabel(tr("Description:"),groupBox);
      label->setAlignment(Qt::AlignRight);
      descEdit = new QTextEdit(groupBox);
      descEdit->setText(description.c_str());
      gridLayout->addWidget(label,    3, 0);
      gridLayout->addWidget(descEdit, 3, 1);

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

      updateFileName();

      // Enable the ok button now that we have text.
      !mapName.empty() ? okButton->setEnabled(true) : okButton->setEnabled(false);

      QVBoxLayout* mainLayout = new QVBoxLayout(this);
      mainLayout->addWidget(groupBox);
      mainLayout->addLayout(buttonLayout);

      connect(nameEdit,     SIGNAL(textChanged(const QString&)), this, SLOT(nameEdited(const QString&)));
      connect(categoryEdit, SIGNAL(textChanged(const QString&)), this, SLOT(categoryEdited(const QString&)));
   }

   ///////////////////////// SLOTS ///////////////////////////////
   void MapSaveAsDialog::nameEdited(const QString& newText)
   {
      updateFileName();

      // Enable the ok button now that we have text.
      !newText.isEmpty() ? okButton->setEnabled(true) : okButton->setEnabled(false);
   }

   void MapSaveAsDialog::categoryEdited(const QString& newText)
   {
      updateFileName();
   }

   void MapSaveAsDialog::updateFileName()
   {
      QString nameText = nameEdit->text();
      nameText.replace('-', '_');
      nameText.replace(' ', '_');

      QString categoryText = categoryEdit->text();
      categoryText.replace('-', '_');
      categoryText.replace(' ', '_');
      categoryText.replace('.', '_');

      if (categoryText.isEmpty())
      {
         fileEdit->setText(nameText);
      }
      else
      {
         fileEdit->setText(categoryText + dtUtil::FileUtils::PATH_SEPARATOR + nameText);
      }
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
