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
#include <dtEditQt/mapdialog.h>
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

   MapDialog::MapDialog(QWidget* parent)
      : QDialog(parent)
   {
      setWindowTitle(tr("New Map"));
      mMap = NULL;

      QGroupBox*   groupBox = new QGroupBox("Map",this);
      QGridLayout* gridLayout = new QGridLayout(groupBox);
      QLabel*      label;

      //Create the properties fields..
      label = new QLabel(tr("Name:"), groupBox);
      label->setAlignment(Qt::AlignRight);
      mNameEdit = new QLineEdit(groupBox);
      gridLayout->addWidget(label,     0, 0);
      gridLayout->addWidget(mNameEdit, 0, 1);

      label = new QLabel(tr("Category:"), groupBox);
      label->setAlignment(Qt::AlignRight);
      mCategoryEdit = new QLineEdit(groupBox);
      gridLayout->addWidget(label,         1, 0);
      gridLayout->addWidget(mCategoryEdit, 1, 1);

      label = new QLabel(tr("FileName:"), groupBox);
      label->setAlignment(Qt::AlignRight);
      mFileEdit = new QLineEdit(groupBox);
      mFileEdit->setEnabled(false);
      //fileEdit->setValidator(new QValidator(fileEdit));
      gridLayout->addWidget(label,    2, 0);
      gridLayout->addWidget(mFileEdit, 2, 1);

      label = new QLabel(tr("Description:"), groupBox);
      label->setAlignment(Qt::AlignRight);
      mDescEdit = new QTextEdit(groupBox);
      gridLayout->addWidget(label,     3, 0);
      gridLayout->addWidget(mDescEdit, 3, 1);

      // Create the buttons...
      mOkButton = new QPushButton(tr("OK"), this);
      QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);
      QHBoxLayout* buttonLayout = new QHBoxLayout;

      mOkButton->setEnabled(false);
      buttonLayout->addStretch(1);
      buttonLayout->addWidget(mOkButton);
      buttonLayout->addWidget(cancelButton);
      buttonLayout->addStretch(1);

      connect(mOkButton,    SIGNAL(clicked()), this, SLOT(applyChanges()));
      connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

      QVBoxLayout* mainLayout = new QVBoxLayout(this);
      mainLayout->addWidget(groupBox);
      mainLayout->addLayout(buttonLayout);

      connect(mNameEdit, SIGNAL(textChanged(const QString&)), this, SLOT(nameEdited(const QString&)));
      connect(mCategoryEdit, SIGNAL(textChanged(const QString&)), this, SLOT(categoryEdited(const QString&)));
   }

   ///////////////////////// SLOTS ///////////////////////////////
   void MapDialog::nameEdited(const QString& newText)
   {
      updateFileName();

      // Enable the ok button now that we have text.
      !newText.isEmpty() ? mOkButton->setEnabled(true) : mOkButton->setEnabled(false);
   }

   void MapDialog::categoryEdited(const QString& newText)
   {
      updateFileName();
   }

   void MapDialog::updateFileName()
   {
      QString nameText = mNameEdit->text();
      nameText.replace('-', '_');
      nameText.replace(' ', '_');

      QString categoryText = mCategoryEdit->text();
      categoryText.replace('-', '_');
      categoryText.replace(' ', '_');
      categoryText.replace('.', '_');

      if (categoryText.isEmpty())
      {
         mFileEdit->setText(nameText);
      }
      else
      {
         mFileEdit->setText(categoryText + dtUtil::FileUtils::PATH_SEPARATOR + nameText);
      }
   }

   void MapDialog::applyChanges()
   {
      if (mNameEdit->text().isEmpty() || mFileEdit->text().isEmpty())
      {
         QMessageBox::critical(this, tr("Map Create Error"),
            tr("A map must have a valid name and file name"),tr("OK"));
      }
      else
      {
         try
         {
            mMap = &dtCore::Project::GetInstance().CreateMap(mNameEdit->text().toStdString(),
               mFileEdit->text().toStdString());
         }
         catch(dtUtil::Exception& e)
         {
            QString error = "An error occured while creating the map. ";
            error += e.What().c_str();
            LOG_ERROR(error.toStdString());
            QMessageBox::critical(this,tr("Map Create Error"),error,tr("OK"));
            reject();
            return;
         }

         mMap->SetDescription(mDescEdit->toPlainText().toStdString());
         accept();
      }
   }

} // namespace dtEditQt
