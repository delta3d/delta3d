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
#include <vector>

#include <dtCore/datatype.h>
#include <dtCore/project.h>
#include <dtCore/resourcedescriptor.h>
#include <dtCore/resourcehelper.h>
#include <dtUtil/fileutils.h>

class QMainWindow;
class QVBoxWidget;
class QComboBox;
class QScrollView;

namespace dtEditQt
{

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
       * Constructor
       */
      ResourceImportDialog(QWidget* parent = 0, dtCore::DataType& dataType = dtCore::DataType::UNKNOWN);

      /**
       * Destructor
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
      dtCore::DataType* getType() { return mResourceType; }

      /**
       * setCategory stores the category name
       * @param QString of the currently selected category from the resource tree
       */
      void setCategory(const QString& category) { mCategory = category; }

      /**
       * getCategory
       * @return QString category selected from the resource tree
       */
      QString getCategory() { return mCategory; }

      /**
       * setCategoryPath
       * @param QString of the path to the category currently selected
       */
      void setCategoryPath(const QString& categoryPath) { mCategoryPath = categoryPath; }

      /**
       * setLastDirectory
       * @param QString of the last directory the user imported from.
       */
      void setLastDirectory(const QString& lastDir) { mLastDirectory = lastDir; }

      /**
       * getLastDirectory
       * @return QString of the last directory imported from.
       */
      QString getLastDirectory() { return mLastDirectory; }

      /**
       * getCategoryPath
       * @return QString path to the category
       */
      QString getCategoryPath() { return mCategoryPath; }

      /**
       * setResourceName
       * @param QString of the name of the resource
       */
      void setResourceName(const QString& resource) { mResource = resource; }

      /**
       * getResourceName
       * @return QString of the resource name
       */
      QString getResourceName() { return mResource; }

      /**
       * getResourceFileList
       * @return QList of the files imported
       */
      QList<QString> getResourceFileList() { return mFileList; }

      QList<dtCore::ResourceDescriptor> getDescriptorList() { return mDescriptorList; }

      /**
       * @brief updateData is a simple method that populates the qlineedit fields after the object
       * has been created.
       */
      void updateData();

      /**
       * setDescriptor
       * @param const ResourceDescriptor
       */
      void setDescriptor(const dtCore::ResourceDescriptor& descriptor) { mDescriptor = descriptor; }

      /**
       * getDescriptor
       * @param grabs the resourceDescriptor assigned to the new resource
       */
      const dtCore::ResourceDescriptor& getDescriptor() const { return mDescriptor; }

      /**
       * resourceCreated
       * @return boolean if a resource has been created
       */
      bool resourceCreated() { return mCreated; }

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

      QLineEdit* mNameEdit;
      QLineEdit* mCatEdit;
      QLineEdit* mFileEdit;
      QComboBox* mTypeEdit;

      QList<QString> mFileList;
      QList<dtCore::ResourceDescriptor> mDescriptorList;
      QPushButton* mImportBtn;
      QPushButton* mFileBtn;

      QString                   mName;
      QString                   mResource;
      QString                   mCategory;
      QString                   mCategoryPath;
      QString                   mFile;
      QString                   mType;
      QStringList               mFilterList;
      QString                   mFileExt;
      QString                   mLastDirectory;
      dtCore::ResourceTreeNode*  mResourceTreeNode;
      dtCore::DataType*          mResourceType;
      dtCore::ResourceDescriptor mDescriptor;
      dtUtil::Log*              mLogger;

      // filter vector to fill for file types
      std::vector<const dtCore::ResourceTypeHandler*> mHandler;

      bool mCreated;
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
      AddCategoryDialog(QWidget* parent)
      : QDialog(parent)
      {
         setWindowTitle(tr("Create New Category"));
         setModal(true);
         mCreate = false;

         // required for correct file path
         categoryDialog();

         mCategoryEdit->setFocus();
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
         mCreateBtn = new QPushButton("Create", this);
         QPushButton* cancelBtn = new QPushButton("Cancel", this);

         // Create main grid
         QVBoxLayout* vbox = new QVBoxLayout(this);

         // Create grid layout
         QGroupBox*   group    = new QGroupBox();
         QGridLayout* mainGrid = new QGridLayout(group);

         // Category edit box
         mCategoryEdit = new QLineEdit(group);
         mCategoryEdit->setMinimumWidth(200);

         // Labels
         QLabel* categoryLabel = new QLabel("Category: ", group);

         mainGrid->addWidget(categoryLabel, 0, 0);
         mainGrid->addWidget(mCategoryEdit, 0, 1);

         QHBoxLayout* hbox = new QHBoxLayout();
         hbox->addStretch(1);
         hbox->addWidget(mCreateBtn, 0, Qt::AlignCenter);
         hbox->addWidget(cancelBtn,  0, Qt::AlignCenter);
         hbox->addStretch(1);

         // add the layouts
         vbox->addWidget(group);
         vbox->addLayout(hbox);

         connect(mCreateBtn, SIGNAL(clicked()), this, SLOT(createCategory()));
         connect(cancelBtn,  SIGNAL(clicked()), this, SLOT(close()));
      }

      /**
       * Set the category from the edit field
       * @param takes a QString
       */
      void setCategory(const QString& category) { mNewCategory = category; }

      /**
       * Set the category path
       * @param takes a QString
       */
      void setCategoryPath(const QString& categoryPath) { mCurrPath = categoryPath; }

      /**
       * SetType sets the current resource type
       * @param DataType
       */
      void setType(dtCore::DataType& resourceType) { mResourceType = &resourceType; }

      /**
       * getType grabs the set dataType. This will be useful for terrain selection
       * @return a resource DataType
       */
      dtCore::DataType* getType() { return mResourceType; }

      /**
       * Get the category Path
       * @return QString
       */
      QString getCategoryPath() { return mCurrPath; }

      /**
       * This retrieves the new category supplied by the user
       * @return QString of the new category entered by the user
       */
      QString getCategory() { return mNewCategory; }

      /**
       * This sets whether the category should be created
       * @param boolean
       */
      void setCreate(bool canCreate) { mCreate = canCreate; }

      /**
       * getCreate()
       * @return boolean whether or not this dialog can create a category
       */
      bool getCreate() { return mCreate; }

   private slots:
      /**
       * Creates a new category supplied by the user
       */
      void createCategory()
      {
         // grab an instance to our project
         dtCore::Project& project = dtCore::Project::GetInstance();
         //dtUtil::FileUtils& futil = dtUtil::FileUtils::GetInstance();

         // full path to our category
         QString fullCategory;
         fullCategory = getCategoryPath();

         // Add the users entered text
         if (!fullCategory.isEmpty())
         {
            fullCategory = fullCategory + ":" + mCategoryEdit->text();
         }
         else
         {
            fullCategory = mCategoryEdit->text();
         }

         setCategory(mCategoryEdit->text());
         setCategoryPath(fullCategory);

         if (mCreate)
         {
            if (!fullCategory.isEmpty())
            {
               project.CreateResourceCategory(fullCategory.toStdString(), *getType());
            }
         }
         close();
      }

   private:
      QPushButton*     mCreateBtn;
      QLineEdit*       mCategoryEdit;
      QString          mNewCategory;
      QString          mCurrPath;
      dtCore::DataType* mResourceType;
      bool             mCreate;
   };

} // namespace dtEditQt

#endif // DELTA_RESOURCE_IMPORT_DIALOG
