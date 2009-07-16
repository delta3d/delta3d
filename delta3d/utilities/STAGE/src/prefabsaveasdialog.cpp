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
 * Jeffrey P. Houde
 */

#include <dtEditQt/prefabsaveasdialog.h>
#include <prefix/dtstageprefix-src.h>

#include <QtGui/QFileDialog>
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
#include <QtGui/QToolButton>

#include <dtCore/globals.h>
#include <dtDAL/project.h>
#include <dtDAL/map.h>
#include <dtDAL/exceptionenum.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/editordata.h>

namespace dtEditQt
{
   ///////////////////////////////////////////////////////////////////////////////
   PrefabSaveDialog::PrefabSaveDialog(QWidget* parent)
      : QDialog(parent)
   {
      setWindowTitle(tr("Save Prefab"));

      QGroupBox*   groupBox = new QGroupBox("Prefab",this);
      QGridLayout* gridLayout = new QGridLayout(groupBox);
      QLabel*      label;

      // Create the properties fields..
      label = new QLabel(tr("Name:"),groupBox);
      label->setAlignment(Qt::AlignRight);
      nameEdit = new QLineEdit(groupBox);
      gridLayout->addWidget(label,    0, 0);
      gridLayout->addWidget(nameEdit, 0, 1);      

      label = new QLabel(tr("Category:"), groupBox);
      label->setAlignment(Qt::AlignRight);
      mCategoryEdit = new QLineEdit("General", groupBox);
      gridLayout->addWidget(label, 1, 0);
      gridLayout->addWidget(mCategoryEdit, 1, 1);

      label = new QLabel(tr("Icon:\n(Click to change)"), groupBox);
      label->setAlignment(Qt::AlignRight);

      mIconFilePath = dtDAL::Project::GetInstance().GetContext();

      //make sure Prefabs folder exists
      if (! dtUtil::FileUtils::GetInstance().FileExists(mIconFilePath + "/Prefabs"))
      {
         dtUtil::FileUtils::GetInstance().MakeDirectory(mIconFilePath + "/Prefabs");         
      }
      //make sure icons folder exists (not nested on purpose -- Prefabs folder may exist
      //when Prefabs/icons does not)
      if (! dtUtil::FileUtils::GetInstance().FileExists(mIconFilePath + "/Prefabs/icons"))
      {
         dtUtil::FileUtils::GetInstance().MakeDirectory(mIconFilePath + "/Prefabs/icons");
      }

      mIconFilePath += "/Prefabs/icons/Icon_NoIcon64.png";

      //put our "empty icon" icon in the prefabs icon folder if it is missing
      if (! dtUtil::FileUtils::GetInstance().FileExists(mIconFilePath))
      {
         std::string templateIconFile = dtCore::GetDeltaRootPath() +
                                    "/utilities/STAGE/icons/Icon_NoIcon64.png";

         try
         {         
            dtUtil::FileUtils::GetInstance().FileCopy(templateIconFile,
                                                      mIconFilePath, false);
         }
         catch (dtUtil::Exception e)
         {
            //don't care if the no_icon icon doesn't make it, so give no message
         }
      }      
      
      mIcon = new QIcon(mIconFilePath.c_str());      
      mIconButton = new QToolButton(groupBox);      
      mIconButton->setIconSize(QSize(64,64));
      mIconButton->setIcon(*mIcon);      
      gridLayout->addWidget(label, 2, 0);
      gridLayout->addWidget(mIconButton, 2, 1);
      
/*
      label = new QLabel(tr("FileName:"),groupBox);
      label->setAlignment(Qt::AlignRight);
      fileEdit = new QLineEdit(groupBox);
      fileEdit->setEnabled(false);
      //fileEdit->setValidator(new QValidator(fileEdit));
      gridLayout->addWidget(label,    3, 0);
      gridLayout->addWidget(fileEdit, 3, 1);
*/

      label = new QLabel(tr("Description:"),groupBox);
      label->setAlignment(Qt::AlignRight);
      descEdit = new QTextEdit(groupBox);
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
      connect(mIconButton,  SIGNAL(clicked()), this, SLOT(IconChanged()));

      QVBoxLayout* mainLayout = new QVBoxLayout(this);
      mainLayout->addWidget(groupBox);
      mainLayout->addLayout(buttonLayout);

      connect(nameEdit, SIGNAL(textChanged(const QString&)), this, SLOT(edited(const QString&)));
   }

   ///////////////////////// SLOTS ///////////////////////////////
   void PrefabSaveDialog::edited(const QString& newText)
   {
      QString text = newText;      

      // Enable the ok button if we have text.
      !text.isEmpty() ? okButton->setEnabled(true) : okButton->setEnabled(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabSaveDialog::IconChanged()
   {
      std::string pathToIcon = mIconFilePath.substr(0, mIconFilePath.find_last_of("\\/"));

      mIconFilePath = QFileDialog::getOpenFileName(this, tr("Choose a new icon"), pathToIcon.c_str()).toStdString();

      if(mIconFilePath.empty())
         return;
      
      if(mIcon != NULL)
      {
         delete mIcon;         
      }

      if(! EnsureIconFolderExists())
      {
         return;
      }            

      QString prefabIconsDir(std::string(dtEditQt::EditorData::GetInstance().getCurrentProjectContext() + 
                                "/" + dtEditQt::EditorActions::PREFAB_DIRECTORY + "/icons").c_str());
      //get rid of backslashes ... need the two strings to be EXACTLY the same
      prefabIconsDir.replace(QRegExp("\\\\"), QString("/"));

      //Need to ensure icon is in the project in the prefabs folder --
      //if not, copy it there:
      QString iconPathOnly = mIconFilePath.substr(0, mIconFilePath.find_last_of("\\/")).c_str();
      //get rid of backslashes ... need the two strings to be EXACTLY the same
      iconPathOnly.replace(QRegExp("\\\\"), QString("/"));

      if(iconPathOnly != prefabIconsDir)
      {
         std::string iconFileNameOnly = mIconFilePath.substr(mIconFilePath.find_last_of("\\/"));
         std::string iconNewPath = prefabIconsDir.toStdString() + iconFileNameOnly;

         dtUtil::FileUtils::GetInstance().FileCopy(mIconFilePath, iconNewPath, true);
         
         mIconFilePath = iconNewPath;
      }
      
      mIcon = new QIcon(mIconFilePath.c_str());
      mIconButton->setIcon(*mIcon);
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string PrefabSaveDialog::getPrefabCategory()
   {
      return mCategoryEdit->text().toStdString();
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string PrefabSaveDialog::getPrefabName()
   {
      return nameEdit->text().toStdString();
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string PrefabSaveDialog::getPrefabFileName()
   {
      //file name has to be massaged -- some characters are not allowed in filenames,
      //and they are: \/:*?"<>| (and we are not going to allow spaces, either)
      
      QString prefabFileName = nameEdit->text();
      prefabFileName.replace(QRegExp("[\\\\/:\\*\\?\"<>| ]"), QString("_"));
      
      return prefabFileName.toStdString();
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string PrefabSaveDialog::getPrefabDescription()
   {
      return descEdit->toPlainText().toStdString();
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string PrefabSaveDialog::GetPrefabIconFileName()
   {
      //Only want to return the file name, NOT the path... Icons will always
      //live in the prefabs/icons folder of the project's context folder.
      return mIconFilePath.substr(mIconFilePath.find_last_of("\\/") + 1);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabSaveDialog::setPrefabCategory(const std::string cat)
   {
      mCategoryEdit->setText(cat.c_str());
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool PrefabSaveDialog::EnsureIconFolderExists()
   {      
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.PushDirectory(dtDAL::Project::GetInstance().GetContext());

      std::string prefabIconsDir = EditorActions::PREFAB_DIRECTORY + "/icons";

      // If the prefab directory does not exist, create it first.
      if (!fileUtils.DirExists(EditorActions::PREFAB_DIRECTORY))
      {
         try 
         {
            fileUtils.MakeDirectory(EditorActions::PREFAB_DIRECTORY);
         }
         catch (const dtUtil::Exception& e)
         {
            LOG_ERROR(e.What());

            QMessageBox::critical((QWidget *) dtEditQt::EditorData::GetInstance().getMainWindow(),
               tr("Error"), QString(e.What().c_str()), tr("OK"));
            return false;
         }
      }

      // Now make sure the prefab icons directory exists
      if (!fileUtils.DirExists(prefabIconsDir))
      {
         try 
         {
            fileUtils.MakeDirectory(prefabIconsDir);
         }
         catch (const dtUtil::Exception& e)
         {
            LOG_ERROR(e.What());

            QMessageBox::critical((QWidget *)EditorData::GetInstance().getMainWindow(),
               tr("Error"), QString(e.What().c_str()), tr("OK"));
            return false;
         }
      }

      return true;
   }

} // namespace dtEditQt
