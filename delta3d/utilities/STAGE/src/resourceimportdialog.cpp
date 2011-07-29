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
 * Teague Coonan
 */
#include <prefix/stageprefix.h>
#include <QtGui/QFileDialog>
#include <QtCore/QFileInfo>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include <QtGui/QGridLayout>
#include <QtGui/QBoxLayout>
#include <QtGui/QComboBox>

#include <map>

#include "dtEditQt/resourceimportdialog.h"
#include <dtCore/datatype.h>
#include <dtCore/project.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   ResourceImportDialog::ResourceImportDialog(QWidget* parent, dtCore::DataType& dataType)
      : QDialog(parent)
   {
      setWindowTitle(tr("Import Resources"));
      setModal(true);

      mResourceType = &dataType;
      //dtUtil::Log &mLogger = dtUtil::Log::GetInstance();

      // we have to call this here for correct file pathing
      importDialog();
   }

   ///////////////////////////////////////////////////////////////////////////////
   ResourceImportDialog::~ResourceImportDialog(){}

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceImportDialog::importDialog()
   {
      // create main grid
      QVBoxLayout* vbox = new QVBoxLayout(this);

      // create grid layout for top grid cell
      QGroupBox*   group   = new QGroupBox();
      QGridLayout* topGrid = new QGridLayout(group);

      QLabel* nameLabel       = new QLabel("Name: ",      group);
      QLabel* catLabel        = new QLabel("Category: ",  group);
      QLabel* fileLabel       = new QLabel("File Path: ", group);
      QLabel* typeLabel       = new QLabel("Type: ",      group);

      mNameEdit                = new QLineEdit(group);
      mCatEdit                 = new QLineEdit(group);
      mFileEdit                = new QLineEdit(group);
      mTypeEdit                = new QComboBox(group);

      // first column
      topGrid->addWidget(nameLabel, 0, 0);
      topGrid->addWidget(catLabel,  1, 0);
      topGrid->addWidget(fileLabel, 2, 0);
      topGrid->addWidget(typeLabel, 3, 0);

      // second column
      topGrid->addWidget(mNameEdit, 0, 1);
      topGrid->addWidget(mCatEdit,  1, 1);
      topGrid->addWidget(mFileEdit, 2, 1);
      topGrid->addWidget(mTypeEdit, 3, 1);

      // third column
      mFileBtn = new QPushButton("...", this);
      topGrid->addWidget(mFileBtn, 2, 2);

      mNameEdit->setMinimumWidth(200);
      mCatEdit->setMinimumWidth(200);
      mFileEdit->setMinimumWidth(200);
      mTypeEdit->setMinimumWidth(200);

      // create grid layout for bottom grid area
      //QGridLayout* bottomGrid = new QGridLayout();
      QHBoxLayout* hbox = new QHBoxLayout();

      mImportBtn  = new QPushButton("Import", this);
      QPushButton* cancelBtn  = new QPushButton("Cancel", this);

      hbox->addStretch(1);
      hbox->addWidget(mImportBtn, 0, Qt::AlignCenter);
      hbox->addWidget(cancelBtn,  0, Qt::AlignCenter);
      hbox->addStretch(1);

      // add the layouts
      vbox->addWidget(group);
      vbox->addLayout(hbox);
      mCatEdit->insert(getCategory());

      mHandler.clear();
      dtCore::Project& project = dtCore::Project::GetInstance();
      project.GetHandlersForDataType(*mResourceType, mHandler);

      // populate the type edit drop down
      if (mHandler.size() > 1)
      {
         // this list will be our drop down selection items
         QStringList filterList;
         filterList.clear();

         // enable our type list
         mTypeEdit->setDisabled(false);
         std::map<std::string, std::string> description;
         description.clear();
         for (unsigned i = 0; i < mHandler.size(); ++i)
         {
            filterList.append(mHandler.at(i)->GetTypeHandlerDescription().c_str());
         }
         mTypeEdit->addItems(filterList);
      }
      else
      {
         // if we only have 1 item in our list then the drop down
         // should be disabled.
         mTypeEdit->addItem(QString(mResourceType->GetName().c_str()));
         mTypeEdit->setDisabled(true);
      }

      mImportBtn->setDisabled(true);
      mFileEdit->setDisabled(true);
      mNameEdit->setDisabled(true);
      mCatEdit->setDisabled(true);

      connect(mImportBtn, SIGNAL(clicked()), this, SLOT(addResource()));
      connect(cancelBtn,  SIGNAL(clicked()), this, SLOT(closeImportDialog()));
      connect(mFileBtn,   SIGNAL(clicked()), this, SLOT(fileDialog()));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceImportDialog::updateData()
   {
      mCatEdit->insert(getCategory());
   }

   ///////////////////////////////////////////////////////////////////////////////
   // slots:
   ///////////////////////////////////////////////////////////////////////////////
   void ResourceImportDialog::closeImportDialog()
   {
      mFileList.clear();
      close();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceImportDialog::fileDialog()
   {
      QStringList list;
      QString context;
      //bool isTerrain = false;

      dtCore::Project& project = dtCore::Project::GetInstance();
      context = QString(project.GetContext().c_str());

      mNameEdit->clear();
      mFileEdit->clear();

      mFileList.clear();
      mFilterList.clear();

      // check if we have a project context
      if (context.isEmpty())
      {
         return;
      }

      // if we got this far then create a file dialog
      QFileDialog* fd = new QFileDialog(this);
      fd->setFileMode(QFileDialog::ExistingFiles);

      // handle the file filters here
      mHandler.clear();
      project.GetHandlersForDataType(*mResourceType, mHandler);

      std::map<std::string, std::string> filters;
      filters.clear();

      for (unsigned i = 0 ; i < mHandler.size(); ++i)
      {
         // this is required to find the selected terrain group
         bool found = false;

         std::map<std::string, std::string>::iterator iter;

         // spin through the filters for the selected combobox
         if (*mResourceType == dtCore::DataType::TERRAIN)
         {
            // if we find our filters than the handler will be updated with the correct file filters
            // otherwise the loop will end safely when i reaches the handler size
            while (found != true && i < mHandler.size())
            {
               if (mTypeEdit->currentText() == mHandler.at(i)->GetTypeHandlerDescription().c_str())
               {
                  found = true;
                  filters = mHandler.at(i)->GetFileFilters();
               }
               else
               {
                  ++i;
               }
            }
         }
         else
         {
            filters = mHandler.at(i)->GetFileFilters();
         }

         for (iter = filters.begin(); iter != filters.end(); ++iter)
         {
            QString filter;
            filter = "";

            filter += iter->second.c_str();
            if (iter->first.find_first_of('.') == std::string::npos)
            {
               filter += "(*.";
            }
            else
            {
               filter += "(";
            }

            filter += iter->first.c_str();
            filter += ")";
            mFilterList.append(filter);
         }

         if (found == true)
         {
            break;
         }
      }

      fd->setFilters(mFilterList);
      fd->setReadOnly(true);

      // put the user in the last known directory if it exists
      if (getLastDirectory().isEmpty())
      {
         fd->setDirectory(context);
      }
      else
      {
         fd->setDirectory(getLastDirectory());
      }
      // create the file dialog
      if (fd->exec())
      {
         list = fd->selectedFiles();

         if (list.size() > 0)
         {
            // handles multiple files
            // find the context where the file(s) were selected
            mFile = "";
            mFile = list.at(0);

            if (!mFile.isEmpty())
            {
               // insert the directory context
               QFileInfo fi(mFile);
               QString actualPath = fi.path();
               actualPath = actualPath.replace("/",QString(dtUtil::FileUtils::PATH_SEPARATOR));
               actualPath = actualPath.replace("\\",QString(dtUtil::FileUtils::PATH_SEPARATOR));
               mFileEdit->insert(actualPath);

               // add the filenames to the name field
               for (int i = 0; i < list.size(); ++i)
               {
                  mFile = list.at(i);
                  QFileInfo fi(mFile);
                  QString names = fi.fileName();

                  if (list.size() == 1)
                  {
                     if (mNameEdit->text().isEmpty())
                     {
                        mNameEdit->insert(names);
                        mNameEdit->setDisabled(false);
                     }
                  }
                  else
                  {
                     mNameEdit->insert(names + ";");
                  }
                  mFileList.append(names);
               }
            }
         }

         // enable the import button
         mImportBtn->setDisabled(false);

         if (list.size() > 1)
         {
            mNameEdit->setDisabled(true);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceImportDialog::addResource()
   {
      QString resourceName;
      QString fullPath;
      QString suffix;

      dtCore::Project& project = dtCore::Project::GetInstance();
      //dtUtil::FileUtils& fileUtil = dtUtil::FileUtils::GetInstance();

      if (!mFileList.isEmpty())
      {
         for (int i = 0;i < mFileList.size(); ++i)
         {
            // Grab the full file name
            resourceName = mFileList.at(i);

            // Create the full path to the file
            fullPath = mFileEdit->text() + "/" + resourceName;
            fullPath = fullPath.replace("/",  QString(dtUtil::FileUtils::PATH_SEPARATOR));
            fullPath = fullPath.replace("\\", QString(dtUtil::FileUtils::PATH_SEPARATOR));

            QFileInfo fi(fullPath);

            // check this in case our file was renamed
            if (mFileList.size() == 1)
            {
               suffix = fi.suffix();
               resourceName = mNameEdit->text();
               QFileInfo fi(resourceName);
               resourceName = fi.baseName();
               setResourceName(resourceName + "." + suffix);
            }
            else
            {
               resourceName = fi.baseName();
            }

            try
            {
               mDescriptorList.append(project.AddResource(resourceName.toStdString(),
                  fullPath.toStdString(), mCatEdit->text().toStdString(), *getType()));

               setCategoryPath(mCatEdit->text());
               QFileInfo pathOfDir(fullPath);
               fullPath = pathOfDir.absolutePath();
               setLastDirectory(fullPath);

            }
            catch(const dtUtil::Exception& e)
            {
               mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, e.What().c_str());
               throw e;
            }
         }
         close();
      }
   }

} // namespace dtEditQt
