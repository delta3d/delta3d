/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* @author Teague Coonan
*/
#include <QFileDialog>
#include <QFileInfo>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QBoxLayout>
#include <QComboBox>

#include "dtEditQt/resourceimportdialog.h"
#include "dtDAL/datatype.h"
#include "dtDAL/project.h"
#include "dtDAL/fileutils.h"

namespace dtEditQt
{
    ///////////////////////////////////////////////////////////////////////////////
    ResourceImportDialog::ResourceImportDialog(QWidget *parent):QDialog(parent)
    {
        setWindowTitle(tr("Import Resources"));
        setModal(true);

        // required for correct file path
        importDialog();
    }
    ///////////////////////////////////////////////////////////////////////////////
    ResourceImportDialog::~ResourceImportDialog(){}
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceImportDialog::importDialog()
    {
        // create main grid
        QVBoxLayout *vbox = new QVBoxLayout(this);

        // create grid layout for top grid cell
        QGroupBox   *group   = new QGroupBox();
        QGridLayout *topGrid = new QGridLayout(group);

        QLabel *nameLabel       = new QLabel("Name: ", group);
        QLabel *catLabel        = new QLabel("Category: ",group);
        QLabel *fileLabel       = new QLabel("File Path: ",group);
        QLabel *typeLabel       = new QLabel("Type: ",group);

        nameEdit                = new QLineEdit(group);
        catEdit                 = new QLineEdit(group);
        fileEdit                = new QLineEdit(group);
        typeEdit                = new QComboBox(group);

        // first column
        topGrid->addWidget(nameLabel,0,0);
        topGrid->addWidget(catLabel,1,0);
        topGrid->addWidget(fileLabel,2,0);
        topGrid->addWidget(typeLabel,3,0);

        // second column
        topGrid->addWidget(nameEdit,0,1);
        topGrid->addWidget(catEdit,1,1);
        topGrid->addWidget(fileEdit,2,1);
        topGrid->addWidget(typeEdit,3,1);

        // third column
        fileBtn = new QPushButton("...",this);
        topGrid->addWidget(fileBtn,2,2);

        nameEdit->setMinimumWidth(200);
        catEdit->setMinimumWidth(200);
        fileEdit->setMinimumWidth(200);
        typeEdit->setMinimumWidth(200);

        // create grid layout for bottom grid area
        QGridLayout *bottomGrid = new QGridLayout();
        QHBoxLayout *hbox = new QHBoxLayout();

        importBtn  = new QPushButton("Import", this);
        QPushButton *cancelBtn  = new QPushButton("Cancel", this);

        hbox->addStretch(1);
        hbox->addWidget(importBtn,0,Qt::AlignCenter);
        hbox->addWidget(cancelBtn,0,Qt::AlignCenter);
        hbox->addStretch(1);

        // add the layouts
        vbox->addWidget(group);
        vbox->addLayout(hbox);
        catEdit->insert(getCategory());

        // disable fields that are selected
        importBtn->setDisabled(true);
        typeEdit->setDisabled(true);
        catEdit->setDisabled(true);
        fileEdit->setDisabled(true);
        nameEdit->setDisabled(true);

        connect(importBtn, SIGNAL(clicked()), this, SLOT(addResource()));
        connect(cancelBtn, SIGNAL(clicked()), this, SLOT(closeImportDialog()));
        connect(fileBtn,SIGNAL(clicked()), this, SLOT(fileDialog()));
    }
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceImportDialog::setType(dtDAL::DataType &myResourceType)
    {
        resourceType = &myResourceType;
    }
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceImportDialog::updateData()
    {
        catEdit->insert(getCategory());
        typeEdit->addItem(QString(resourceType->GetName().c_str()));

    }
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceImportDialog::setFilter(const QString myFilter)
    {
        filter = myFilter;
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
        QDir *fileName = new QDir();
        
        dtDAL::Project& project = dtDAL::Project::GetInstance();
        context = QString(project.GetContext().c_str());

        nameEdit->clear();
        fileEdit->clear();
        fileList.clear();

        // check if we have a filter
        if(filter.toStdString().empty())
            return;

        if(context.isEmpty())
            return;

        // if we got this far then create a file dialog
        QFileDialog *fd = new QFileDialog(this);
        fd->setFileMode(QFileDialog::ExistingFiles);
        fd->setFilter(filter);
        
        // put the user in the last known directory if it exists
        if(getLastDirectory().isEmpty())
        {
            fd->setDirectory(context);
        }
        else
        {
            fd->setDirectory(getLastDirectory());
        }
        if(fd->exec())
        {
            list = fd->selectedFiles();

            if(list.size()>0)
            {
                // handles multiple files
                // find the context where the file(s) were selected
                file = "";
                file = list.at(0);
                
                if(!file.isEmpty())
                {
                    // insert the directory context
                    QFileInfo fi(file);
                    QString actualPath = fi.path();
                    actualPath = actualPath.replace("/",QString(dtDAL::FileUtils::PATH_SEPARATOR));
                    actualPath = actualPath.replace("\\",QString(dtDAL::FileUtils::PATH_SEPARATOR));
                    fileEdit->insert(actualPath);
                    
                    // add the filenames to the name field
                    for(int i=0;i<list.size();++i)
                    {
                        file = list.at(i);
                        QFileInfo fi(file);
                        QString names = fi.fileName();
                        
                        if(list.size()==1)
                        {
                            if(nameEdit->text().isEmpty())
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

            //enable the import button
            importBtn->setDisabled(false);

            if(list.size()>1)
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
       
        if(!fileList.isEmpty())
        {
            for(int i=0;i<fileList.size();++i)
            {
                // Grab the full file name
                resourceName = fileList.at(i);
                
                // Create the full path to the file
                fullPath = fileEdit->text()+"/"+resourceName;
                fullPath = fullPath.replace("/",QString(dtDAL::FileUtils::PATH_SEPARATOR));
                fullPath = fullPath.replace("\\",QString(dtDAL::FileUtils::PATH_SEPARATOR));

                // Strip the extension before we create the display name
                // for the resource descriptor
                QFileInfo fi(fullPath);
                
                // check this in case our file was renamed
                if(fileList.size()==1)
                {
                    suffix = fi.suffix();
                    resourceName = nameEdit->text();
                    QFileInfo fi(resourceName);
                    resourceName = fi.baseName();
                    setResourceName(resourceName+"."+suffix);
                }else
                {
                    resourceName = fi.baseName();
                }
                descriptorList.append(project.AddResource(resourceName.toStdString()
                    ,fullPath.toStdString(),catEdit->text().toStdString(),*this->getType()));
                /*setDescriptor(project.AddResource(resourceName.toStdString()
                    ,fullPath.toStdString(),catEdit->text().toStdString(),*this->getType()));
                */
                setCategoryPath(catEdit->text());
                QFileInfo pathOfDir(fullPath);
                fullPath = pathOfDir.absolutePath();
                setLastDirectory(fullPath);

            }            
            close();
        }
    }

}
