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
#include <prefix/dtstageprefix-src.h>
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
#include <dtDAL/datatype.h>
#include <dtDAL/project.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   ResourceImportDialog::ResourceImportDialog(QWidget* parent, dtDAL::DataType& dataType)
      : QDialog(parent)
   {
      setWindowTitle(tr("Import Resources"));
      setModal(true);

      resourceType = &dataType;
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

      nameEdit                = new QLineEdit(group);
      catEdit                 = new QLineEdit(group);
      fileEdit                = new QLineEdit(group);
      typeEdit                = new QComboBox(group);

      // first column
      topGrid->addWidget(nameLabel, 0, 0);
      topGrid->addWidget(catLabel,  1, 0);
      topGrid->addWidget(fileLabel, 2, 0);
      topGrid->addWidget(typeLabel, 3, 0);

      // second column
      topGrid->addWidget(nameEdit, 0, 1);
      topGrid->addWidget(catEdit,  1, 1);
      topGrid->addWidget(fileEdit, 2, 1);
      topGrid->addWidget(typeEdit, 3, 1);

      // third column
      fileBtn = new QPushButton("...", this);
      topGrid->addWidget(fileBtn, 2, 2);

      nameEdit->setMinimumWidth(200);
      catEdit->setMinimumWidth(200);
      fileEdit->setMinimumWidth(200);
      typeEdit->setMinimumWidth(200);

      // create grid layout for bottom grid area
      //QGridLayout* bottomGrid = new QGridLayout();
      QHBoxLayout* hbox = new QHBoxLayout();

      importBtn  = new QPushButton("Import", this);
      QPushButton* cancelBtn  = new QPushButton("Cancel", this);

      hbox->addStretch(1);
      hbox->addWidget(importBtn, 0, Qt::AlignCenter);
      hbox->addWidget(cancelBtn, 0, Qt::AlignCenter);
      hbox->addStretch(1);

      // add the layouts
      vbox->addWidget(group);
      vbox->addLayout(hbox);
      catEdit->insert(getCategory());

      handler.clear();
      dtDAL::Project& project = dtDAL::Project::GetInstance();
      project.GetHandlersForDataType(*resourceType, handler);

      // populate the type edit drop down
      if (handler.size() > 1)
      {
         // this list will be our drop down selection items
         QStringList filterList;
         filterList.clear();

         // enable our type list
         typeEdit->setDisabled(false);
         std::map<std::string, std::string> description;
         description.clear();
         for (unsigned i = 0; i < handler.size(); ++i)
         {
            filterList.append(handler.at(i)->GetTypeHandlerDescription().c_str());
         }
         typeEdit->addItems(filterList);
      }
      else
      {
         // if we only have 1 item in our list then the drop down
         // should be disabled.
         typeEdit->addItem(QString(resourceType->GetName().c_str()));
         typeEdit->setDisabled(true);
      }

      importBtn->setDisabled(true);
      fileEdit->setDisabled(true);
      nameEdit->setDisabled(true);
      catEdit->setDisabled(true);

      connect(importBtn, SIGNAL(clicked()), this, SLOT(addResource()));
      connect(cancelBtn, SIGNAL(clicked()), this, SLOT(closeImportDialog()));
      connect(fileBtn,   SIGNAL(clicked()), this, SLOT(fileDialog()));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceImportDialog::updateData()
   {
      catEdit->insert(getCategory());
   }

   ///////////////////////////////////////////////////////////////////////////////
   // slots:
   ///////////////////////////////////////////////////////////////////////////////
   void ResourceImportDialog::closeImportDialog()
   {
      fileList.clear();
      close();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceImportDialog::fileDialog()
   {
      QStringList list;
      QString context;
      //bool isTerrain = false;

      dtDAL::Project& project = dtDAL::Project::GetInstance();
      context = QString(project.GetContext().c_str());

      nameEdit->clear();
      fileEdit->clear();

      fileList.clear();
      filterList.clear();

      // check if we have a project context
      if (context.isEmpty())
      {
         return;
      }

      // if we got this far then create a file dialog
      QFileDialog* fd = new QFileDialog(this);
      fd->setFileMode(QFileDialog::ExistingFiles);

      // handle the file filters here
      handler.clear();
      project.GetHandlersForDataType(*resourceType, handler);

      std::map<std::string, std::string> filters;
      filters.clear();

      for (unsigned i = 0 ; i < handler.size(); ++i)
      {
         // this is required to find the selected terrain group
         bool found = false;

         std::map<std::string, std::string>::iterator iter;

         // spin through the filters for the selected combobox
         if (*resourceType == dtDAL::DataType::TERRAIN)
         {
            // if we find our filters than the handler will be updated with the correct file filters
            // otherwise the loop will end safely when i reaches the handler size
            while (found != true && i < handler.size())
            {
               if (typeEdit->currentText() == handler.at(i)->GetTypeHandlerDescription().c_str())
               {
                  found = true;
                  filters = handler.at(i)->GetFileFilters();
               }
               else
               {
                  ++i;
               }
            }
         }
         else
         {
            filters = handler.at(i)->GetFileFilters();
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
            filterList.append(filter);        
         }

         if (found == true)
         {
            break;
         }
      }

      fd->setFilters(filterList);
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
            file = "";
            file = list.at(0);

            if (!file.isEmpty())
            {
               // insert the directory context
               QFileInfo fi(file);
               QString actualPath = fi.path();
               actualPath = actualPath.replace("/",QString(dtUtil::FileUtils::PATH_SEPARATOR));
               actualPath = actualPath.replace("\\",QString(dtUtil::FileUtils::PATH_SEPARATOR));
               fileEdit->insert(actualPath);

               // add the filenames to the name field
               for (int i = 0; i < list.size(); ++i)
               {
                  file = list.at(i);
                  QFileInfo fi(file);
                  QString names = fi.fileName();

                  if (list.size() == 1)
                  {
                     if (nameEdit->text().isEmpty())
                     {
                        nameEdit->insert(names);
                        nameEdit->setDisabled(false);
                     }
                  }
                  else
                  {
                     nameEdit->insert(names+";");
                  }
                  fileList.append(names);
               }
            }
         }

         // enable the import button
         importBtn->setDisabled(false);

         if (list.size() > 1)
         {
            nameEdit->setDisabled(true);
         }
      }
   }
   ///////////////////////////////////////////////////////////////////////////////
   void ResourceImportDialog::addResource()
   {
      QString resourceName;
      QString fullPath;
      QString suffix;

      dtDAL::Project& project = dtDAL::Project::GetInstance();
      //dtUtil::FileUtils& fileUtil = dtUtil::FileUtils::GetInstance();

      if (!fileList.isEmpty())
      {
         for (int i = 0;i < fileList.size(); ++i)
         {
            // Grab the full file name
            resourceName = fileList.at(i);

            // Create the full path to the file
            fullPath = fileEdit->text() + "/" + resourceName;
            fullPath = fullPath.replace("/",  QString(dtUtil::FileUtils::PATH_SEPARATOR));
            fullPath = fullPath.replace("\\", QString(dtUtil::FileUtils::PATH_SEPARATOR));

            QFileInfo fi(fullPath);

            // check this in case our file was renamed
            if (fileList.size() == 1)
            {
               suffix = fi.suffix();
               resourceName = nameEdit->text();
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
               descriptorList.append(project.AddResource(resourceName.toStdString(),
                  fullPath.toStdString(), catEdit->text().toStdString(), *this->getType()));

               setCategoryPath(catEdit->text());
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
