/*
* Delta3D Open Source Game and Simulation Engine 
* Simulation, Training, and Game Editor (STAGE)
* Copyright (C) 2005, BMH Associates, Inc.
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
* @author Teague Coonan
*/

#ifndef DELTA_RESOURCE_IMPORT_DIALOG
#define DELTA_RESOURCE_IMPORT_DIALOG

#include <QtCore/QStringList>
#include <QtGui/QPushButton>
#include <QtCore/QString>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtCore/QList>

#include <osg/Referenced>
#include <osg/ref_ptr>
#include <vector>

#include <dtDAL/datatype.h>
#include <dtDAL/project.h>
#include <dtDAL/resourcedescriptor.h>
#include <dtDAL/resourcehelper.h>
#include <dtUtil/fileutils.h>

class QMainWindow;
class QVBoxWidget;
class QComboBox;
class QScrollView;

namespace dtEditQt{

    /**
    * @class ResourceImportDialog
    * @brief This is a generic class that handles the importation of resources
    *        through a selection dialog and file browsing dialog.
    */
    class ResourceImportDialog : public QDialog
    {
        Q_OBJECT
    public:
        /**
        * ConstrucTOR
        */
        ResourceImportDialog(QWidget *parent = 0, dtDAL::DataType &dataType = dtDAL::DataType::UNKNOWN);

        /**
        * DestrucTOR
        */
        virtual ~ResourceImportDialog();

        /**
        * Import dialog creates the dialog layout and widgets
        */
        void importDialog();

        /**
        * getType grabs the set dataType. This will be useful for terrain selection
        * @return a resource DataType
        */
        dtDAL::DataType *getType(){return resourceType;}

        /**
        * setCategory stores the category name
        * @param QString of the currently selected category from the resource tree
        */
        void setCategory(const QString &myCategory){this->category = myCategory;}

        /**
        * getCategory
        * @return QString category selected from the resource tree
        */
        QString getCategory(){return this->category;}
        
        /**
        * setCategoryPath
        * @param QString of the path to the category currently selected
        */
        void setCategoryPath(const QString &myCategoryPath){this->categoryPath = myCategoryPath;}
        
        /**
        * setLastDirectory
        * @param QString of the last directory the user imported from.
        */
        void setLastDirectory(const QString &lastDir){this->lastDirectory = lastDir;}

        /**
        * getLastDirectory
        * @return QString of the last directory imported from.
        */
        QString getLastDirectory(){return this->lastDirectory;}
        
        /**
        * getCategoryPath
        * @return QString path to the category
        */
        QString getCategoryPath(){return this->categoryPath;}

        /**
        * setResourceName
        * @param QString of the name of the resource
        */
        void setResourceName(const QString &myResource){this->resource = myResource;}

        /**
        * getResourceName
        * @return QString of the resource name
        */
        QString getResourceName(){return this->resource;}

        /**
        * getResourceFileList
        * @return QList of the files imported
        */
        QList<QString> getResourceFileList(){ return fileList;}

        QList<dtDAL::ResourceDescriptor> getDescriptorList(){return descriptorList;}
        /**
        * @brief updateData is a simple method that populates the qlineedit fields after the object
        * has been created.
        */
        void updateData();

        /**
        * setDescriptor
        * @param const ResourceDescriptor
        */
        void setDescriptor(const dtDAL::ResourceDescriptor &descriptor){this->descriptor = descriptor;}

        /**
        * getDescriptor
        * @param grabs the resourceDescriptor assigned to the new resource
        */
        const dtDAL::ResourceDescriptor getDescriptor(){return this->descriptor;}

        /**
        * resourceCreated
        * @return boolean if a resource has been created
        */
        bool resourceCreated(){return created;}

    private slots:
        /**
        * Slot: Assign a selection to the object
        */
        void fileDialog();

        /**
        * Slot: Add selected resource
        */
        void addResource();

        /**
        * Slot: Close window
        */
        void closeImportDialog();

    private:

        QLineEdit *nameEdit;
        QLineEdit *catEdit;
        QLineEdit *fileEdit;
        QComboBox *typeEdit;

        QList<QString> fileList;
        QList<dtDAL::ResourceDescriptor> descriptorList;
        QPushButton *importBtn;
        QPushButton *fileBtn;

        QString name;
        QString resource;
        QString category;
        QString categoryPath;
        QString file;
        QString type;
        QStringList filterList;
        QString fileExt;
        QString lastDirectory;
        dtDAL::ResourceTreeNode *resourceTreeNode;
        dtDAL::DataType *resourceType;
        dtDAL::ResourceDescriptor descriptor;
        dtUtil::Log *mLogger;

        // filter vector to fill for file types
        std::vector<osg::ref_ptr<const dtDAL::ResourceTypeHandler> >handler;

        bool created;
    };

    /**
    * @class AddCategoryDialog
    * @brief This will pop up a seperate dialog for entering a category
    */
    class AddCategoryDialog : public QDialog
    {
        Q_OBJECT

    public:
        /**
        * Constructor
        */
        AddCategoryDialog(QWidget *parent)
        {
            setWindowTitle(tr("Create New Category"));
            setModal(true);
            create = false;

            // required for correct file path
            categoryDialog();

            categoryEdit->setFocus();
        }
        /**
        * Destructor
        */
        virtual ~AddCategoryDialog(){}

        /**
        * Create the category dialog
        */
        void categoryDialog()
        {
            // PushButtons
            createBtn  = new QPushButton("Create", this);
            QPushButton *cancelBtn  = new QPushButton("Cancel", this);

            // Create main grid
            QVBoxLayout *vbox = new QVBoxLayout(this);

            // Create grid layout
            QGroupBox   *group   = new QGroupBox();
            QGridLayout *mainGrid = new QGridLayout(group);

            // Category edit box
            categoryEdit = new QLineEdit(group);
            categoryEdit->setMinimumWidth(200);

            // Labels
            QLabel *categoryLabel   = new QLabel("Category: ",group);

            mainGrid->addWidget(categoryLabel,0,0);
            mainGrid->addWidget(categoryEdit,0,1);

            QHBoxLayout *hbox = new QHBoxLayout();
            hbox->addStretch(1);
            hbox->addWidget(createBtn,0,Qt::AlignCenter);
            hbox->addWidget(cancelBtn,0,Qt::AlignCenter);
            hbox->addStretch(1);

            // add the layouts
            vbox->addWidget(group);
            vbox->addLayout(hbox);

            connect(createBtn, SIGNAL(clicked()), this, SLOT(createCategory()));
            connect(cancelBtn, SIGNAL(clicked()), this, SLOT(close()));
        }

        /**
        * Set the category from the edit field
        * @param takes a QString
        */
        void setCategory(const QString &myCategory){newCategory = myCategory;}

        /**
        * Set the category path
        * @param takes a QString
        */
        void setCategoryPath(const QString &myCategoryPath){currPath = myCategoryPath;}

        /**
        * SetType sets the current resource type
        * @param DataType
        */
        void setType(dtDAL::DataType &myResourceType){resourceType=&myResourceType;}

        /**
        * getType grabs the set dataType. This will be useful for terrain selection
        * @return a resource DataType
        */
        dtDAL::DataType *getType(){return resourceType;}

        /**
        * Get the category Path
        * @return QString
        */
        QString getCategoryPath(){return this->currPath;}

        /**
        * This retrieves the new category supplied by the user
        * @return QString of the new category entered by the user
        */
        QString getCategory(){return this->newCategory;}

        /**
        * This sets whether the category should be created
        * @param boolean
        */
        void setCreate(bool canCreate){create = canCreate;}

        /**
        * getCreate()
        * @return boolean whether or not this dialog can create a category
        */
        bool getCreate(){return this->create;}

    private slots:
        /**
        * Creates a new category supplied by the user
        */
        void createCategory()
        {
            // grab an instance to our project
            dtDAL::Project& project = dtDAL::Project::GetInstance();
            //dtUtil::FileUtils& futil = dtUtil::FileUtils::GetInstance();

            // full path to our category
            QString fullCategory;
            fullCategory = getCategoryPath();

            // Add the users entered text
            if(!fullCategory.isEmpty())
            {
                fullCategory = fullCategory+":"+categoryEdit->text();
            }
            else
            {
                fullCategory = categoryEdit->text();
            }

            setCategory(categoryEdit->text());
            setCategoryPath(fullCategory);

            if(create)
            {
                if(!fullCategory.isEmpty())
                {
                    project.CreateResourceCategory(fullCategory.toStdString(),*getType());
                }
            }
            close();
        }

    private:
        QPushButton *createBtn;
        QLineEdit   *categoryEdit;
        QString newCategory;
        QString currPath;
        dtDAL::DataType *resourceType;
        bool create;
    };

}
#endif
