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
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QHBoxLayout>
#include <QtCore/QList>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QAction>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>

#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/resourceabstractbrowser.h>
#include <dtCore/resourcedescriptor.h>
#include <dtEditQt/resourcetreewidget.h>
#include <dtEditQt/uiresources.h>
#include <dtEditQt/stagecamera.h>

#include <dtCore/transformable.h>
#include <dtCore/transform.h>

#include <osg/BoundingSphere>
#include <osg/Node>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   ResourceAbstractBrowser::ResourceAbstractBrowser(dtCore::DataType* type, QWidget* parent)
      : QWidget(parent)
   {
      mTree = new ResourceDragTree(parent);
      mTree->setColumnCount(1);
      mTree->header()->hide();
      mTree->setFocusPolicy(Qt::StrongFocus);

      mRootName = QString(type->GetDisplayName().c_str());
      mResourceType = type;
      mParent = parent;

      // create an event filter so we can grab tree key events
      mTree->installEventFilter(this);

      // setup right mouse click context menu
      createActions();
      createContextMenu();

      // Connect the projectChanged signal so we can populate the tree
      // at the appropriate time
      connect(&EditorEvents::GetInstance(), SIGNAL(projectChanged()),
         this, SLOT(onProjectChanged()));

      connect(mTree, SIGNAL(doubleClicked(const QModelIndex)), this, SLOT(doubleClickEvent()));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceAbstractBrowser::buildResourceTree(dtCore::DataType& type, QWidget* parent, const QIcon& resourceIcon)
   {
      // make sure we have a valid type before we build the tree
      if (!type.GetDisplayName().empty())
      {
         // grab an instance of our project
         dtCore::Project& project = dtCore::Project::GetInstance();

         project.GetResourcesOfType(type, mIterTree);

         QIcon icon;
         icon.addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER_OPEN.c_str()), QIcon::Normal, QIcon::On);
         icon.addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER.c_str()),      QIcon::Normal, QIcon::Off);

         // construct our tree
         mRoot = new ResourceTreeWidget(mTree);
         mRoot->setText(0, mRootName);
         mRoot->recursivelyCreateResourceTree(mIterTree, resourceIcon);
         mRoot->setIsResource(false);
         mRoot->setIcon(0, icon);

         mSelection = new ResourceTreeWidget();
         mSelection->setCategoryName(QString(type.GetName().c_str()));

         // connect tree signals
         connect(mTree, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
      }
   }
   ///////////////////////////////////////////////////////////////////////////////
   ResourceTreeWidget* ResourceAbstractBrowser::currentSelection()
   {
      mSelection = NULL;

      if (mTree != NULL)
      {
         QList <QTreeWidgetItem*> list = mTree->selectedItems();

         if (!list.isEmpty())
         {
            mSelection = dynamic_cast<ResourceTreeWidget*>(list[0]);
         }
         if (mSelection != NULL)
         {
            if (mSelection->isResource())
            {
               setEditorDataDescriptor(mSelection->getResourceDescriptor());
            }
            else
            {
               resetEditorDataDescriptor();
            }
         }
      }
      return mSelection;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceAbstractBrowser::removeTreeNode()
   {
      int index;
      //dtCore::Project& project = dtCore::Project::GetInstance();

      if (mSelection != NULL)
      {
         QTreeWidgetItem* parent = mSelection->parent();

         if (parent != NULL)
         {
            index = parent->indexOfChild(mSelection);
            parent->takeChild(index);
            mTree->setItemExpanded(parent, true);
         }
         else
         {
            index = mTree->indexOfTopLevelItem(mSelection);
            mTree->takeTopLevelItem(index);
            mTree->setItemExpanded(mRoot, true);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceAbstractBrowser::addTreeNode(QString& nodeText, dtCore::ResourceDescriptor descriptor,
      bool resource)
   {
      if (!nodeText.isEmpty())
      {
         ResourceTreeWidget* child = new ResourceTreeWidget(mSelection);
         child->setText(0, nodeText);
         child->setIsResource(resource);
         child->setResourceDescriptor(descriptor);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceAbstractBrowser::refreshResourceTree()
   {
      // Set the editor data to NULL
      resetEditorDataDescriptor();

      // clear the tree
      mTree->clear();

      // rebuild the tree
      buildResourceTree(*mResourceType, getCurrentParent(), mResourceIcon);

      // change the selection to the root to be nice
      mTree->setCurrentItem(mTree->topLevelItem(0));
      selectionChanged();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool ResourceAbstractBrowser::eventFilter(QObject* obj, QEvent* e)
   {
      if (obj == mTree)
      {
         // For some reason, KeyPress is getting defined by something...
         // Without this undef, it will not compile under Linux..
         // It would be great if someone could figure out exactly what's
         // going on.
#undef KeyPress
         if (e->type() == QEvent::KeyPress)
         {
            QKeyEvent* keyEvent = (QKeyEvent*)e;
            switch (keyEvent->key())
            {
            case Qt::Key_Return:
               // invokeSelection
               // enterKeySelected();
               break;
            case Qt::Key_Enter:
               // invokeSelection
               // enterKeySelected();
               break;
            default:
               return mTree->eventFilter(obj, e);
            }
         }
         else
         {
            // pass the event on to the parent class
            return mTree->eventFilter(obj, e);
         }
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   // Context Menu
   ///////////////////////////////////////////////////////////////////////////////
   void ResourceAbstractBrowser::contextMenuEvent(QContextMenuEvent* e)
   {
      if (mTree->underMouse())
      {
         mContextMenu->exec(e->globalPos());
      }
      else
      {
         e->ignore();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceAbstractBrowser::createContextMenu()
   {
      mContextMenu = new QMenu(this);
      mContextMenu->addAction(mSetRefreshAction);
      mContextMenu->addAction(mSetDeleteAction);
      mContextMenu->addAction(mSetCategoryAction);
      mContextMenu->addAction(mSetImportAction);
   }

   ///////////////////////////////////////////////////////////////////////////////
   // Actions
   ///////////////////////////////////////////////////////////////////////////////
   void ResourceAbstractBrowser::createActions()
   {
      mSetRefreshAction = new QAction(tr("&Refresh"), getCurrentParent());
      mSetRefreshAction->setCheckable(false);
      connect(mSetRefreshAction,SIGNAL(triggered()), this, SLOT(refreshSelected()));

      mSetDeleteAction  = new QAction(tr("&Delete"), getCurrentParent());
      mSetDeleteAction->setCheckable(false);
      connect(mSetDeleteAction,SIGNAL(triggered()), this, SLOT(deleteSelected()));
      mSetDeleteAction->setEnabled(false);

      mSetCategoryAction = new QAction(tr("&Create Category"), getCurrentParent());
      mSetCategoryAction->setCheckable(false);
      connect(mSetCategoryAction, SIGNAL(triggered()), this, SLOT(createCategory()));
      mSetCategoryAction->setEnabled(false);

      mSetImportAction = new QAction(tr("&Import Resource"), getCurrentParent());
      mSetImportAction->setCheckable(false);
      connect(mSetImportAction, SIGNAL(triggered()), this, SLOT(importSelected()));
      mSetImportAction->setEnabled(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   QGroupBox* ResourceAbstractBrowser::standardButtons(const QString& groupBoxName)
   {
      // surrounds our tree and buttons
      QGroupBox* group = new QGroupBox(tr("Tools"));

      // contains 2 cells, one for the tree and the other for the buttons
      QGridLayout* grid = new QGridLayout(group);

      // contains our  buttons
      QHBoxLayout* hbox = new QHBoxLayout();

      mImportBtn = new QPushButton(""/*"Import"*/, group);
      mImportBtn->setToolTip("Import - add a new resource to the project");
      mImportBtn->setIcon(QPixmap(UIResources::LARGE_ICON_EDIT_IMPORT.c_str()));
      connect(mImportBtn, SIGNAL(clicked()), this, SLOT(importSelected()));

      mDeleteBtn = new QPushButton(""/*"Delete"*/, group);
      mDeleteBtn->setIcon(QPixmap(UIResources::LARGE_ICON_EDIT_DELETE.c_str()));
      mDeleteBtn->setToolTip("Delete - remove the current resource or category from the project");
      connect(mDeleteBtn, SIGNAL(clicked()), this, SLOT(deleteSelected()));
      mDeleteBtn->setDisabled(true);

      mRefreshBtn = new QPushButton(""/*"Refresh"*/, group);
      mRefreshBtn->setToolTip("Refresh - update the categories and resources directly from your hard drive");
      mRefreshBtn->setIcon(QPixmap(UIResources::LARGE_ICON_EDIT_REFRESH.c_str()));
      connect(mRefreshBtn, SIGNAL(clicked()), this, SLOT(refreshSelected()));

      mCategoryBtn = new QPushButton(""/*Category"*/, group);
      mCategoryBtn->setToolTip("Create Category - create a new category folder in the current category");
      mCategoryBtn->setIcon(QPixmap(UIResources::LARGE_ICON_EDIT_CATEGORY.c_str()));
      connect(mCategoryBtn, SIGNAL(clicked()), this, SLOT(createCategory()));

      hbox->addStretch(1);
      hbox->addWidget(mImportBtn,   0);
      hbox->addWidget(mDeleteBtn,   0);
      hbox->addWidget(mRefreshBtn,  0);
      hbox->addWidget(mCategoryBtn, 0);
      hbox->addStretch(1);

      grid->addLayout(hbox, 0, 0);

      return group;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceAbstractBrowser::SetCameraLookAt(dtEditQt::StageCamera& camera, dtCore::Transformable& transformableToView)
   {
      // Now we need to get the bounding volume to determine the extents
      // of the new static mesh.  If the extents are within a reasonable
      // size, the camera will be placed such that the static mesh is
      // slightly in front of the camera.  If the mesh is too large,
      // the camera is placed in the center of the mesh.
      const osg::BoundingSphere& bs = transformableToView.GetOSGNode()->getBound();
      float offset = (bs.radius() < 1000.0f) ? bs.radius() : 0.0f;

      dtCore::Transform xform;
      transformableToView.GetTransform(xform);

      camera.resetRotation();
      osg::Vec3 viewDir = camera.getViewDir();

      osg::Vec3 translation;
      xform.GetTranslation(translation);
      if (offset > 0.0f)
      {
         camera.setPosition(translation + viewDir * offset * -2.0f);
      }
      else
      {
         camera.setPosition(translation + bs.center());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   // Slots
   ///////////////////////////////////////////////////////////////////////////////
   void ResourceAbstractBrowser::onProjectChanged()
   {
      // grab an instance of our project
      dtCore::Project& project = dtCore::Project::GetInstance();

      if (!project.IsContextValid())
      {
         return;
      }

      refreshResourceTree();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceAbstractBrowser::selectionChanged()
   {
      ResourceTreeWidget* mSelection = currentSelection();

      if (mSelection != NULL)
      {
         // checks if we have the root of the browser tree
         if (mTree->indexOfTopLevelItem(mSelection) == 0)
         {
            mDeleteBtn->setDisabled(true);
            mSetDeleteAction->setEnabled(false);
         }
         else
         {
            mDeleteBtn->setDisabled(false);
            mSetDeleteAction->setEnabled(true);
         }

         // handle turning on and off buttons and context menu items
         if (mSelection->isResource())
         {
            mImportBtn->setDisabled(true);
            mCategoryBtn->setDisabled(true);
            mSetCategoryAction->setEnabled(false);
            mSetImportAction->setEnabled(false);
         }
         else
         {
            mImportBtn->setDisabled(false);
            mCategoryBtn->setDisabled(false);
            mSetCategoryAction->setEnabled(true);
            mSetImportAction->setEnabled(true);
         }
      }
      else
      {
         mImportBtn->setDisabled(true);
         mCategoryBtn->setDisabled(true);
         mSetImportAction->setEnabled(false);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceAbstractBrowser::deleteSelected()
   {
      ResourceTreeWidget* selection = currentSelection();
      dtCore::Project& project = dtCore::Project::GetInstance();

      QString categoryPath;

      if (selection)
      {
         //int index;
         bool resource;
         QString message;
         QString category;
         //QTreeWidgetItem* parent = selection->parent();

         resource = selection->isResource();

         if (resource)
         {
            message = "This operation will permanently delete this resource "
               "from your computer.\n"
               "Would you like to proceed?";
         }
         else
         {
            message = "This operation will permanently delete this category "
               "and all associated resources contained within.\n"
               "Would you like to proceed?";

            // required to remove this category
            category = selection->getCategoryName();
         }

         // pop up a confirmation dialog
         QMessageBox* mb = new QMessageBox("Browser",
            message,
            QMessageBox::Question,
            QMessageBox::Yes | QMessageBox::Default,
            QMessageBox::No | QMessageBox::Escape,
            QMessageBox::NoButton);

         mb->setButtonText(QMessageBox::Yes, "Yes");
         mb->setButtonText(QMessageBox::No, "No");

         switch (mb->exec())
         {
         case QMessageBox::Yes:
            // Delete the resource
            if (resource)
            {
               // delete the resource then remove it from the tree
               project.RemoveResource(selection->getResourceDescriptor());

               // This is required to register that the current item has been
               // deleted and consequently remove or refresh a preview window
               // if one exists.
               deleteItemEvent();
            }
            else
            {
               // grab the full path to the category
               categoryPath = QString(selection->getCategoryFullName());
               project.RemoveResourceCategory(categoryPath.QString::toStdString(),
                  *mResourceType, true);
            }
            // manually remove the widget item from the tree
            removeTreeNode();

            // Set the editor data to NULL
            resetEditorDataDescriptor();
            break;
         case QMessageBox::No:
            // cancel
            break;
         }
      }

      // change the selection to the root to be nice
      mTree->setCurrentItem(mTree->topLevelItem(0));
   }
   ///////////////////////////////////////////////////////////////////////////////
   void ResourceAbstractBrowser::refreshSelected()
   {
      dtCore::Project& project = dtCore::Project::GetInstance();
      // refresh the project
      project.Refresh();
      //ResourceTreeWidget* selection = currentSelection();
      refreshResourceTree();
   }
   ///////////////////////////////////////////////////////////////////////////////
   void ResourceAbstractBrowser::importSelected()
   {
      ResourceTreeWidget* selection = currentSelection();
      //QTreeWidgetItem* parent = selection->parent();
      QString categoryPath;
      QString resourcePath;
      QString resourceName;

      // Setup the import dialog default parameters for this browser
      mImportDialog = new ResourceImportDialog(this, *mResourceType);

      // Grab the filter from the resourceTypeHandler
      if (!mFileDialogDir.isEmpty())
      {
         mImportDialog->setLastDirectory(mFileDialogDir);
      }

      mImportDialog->setCategory(selection->getCategoryFullName());
      mImportDialog->updateData();

      try
      {
         mImportDialog->exec();
      }
      catch (const dtUtil::Exception&)
      {
         return;
      }

      // This is the current full category path
      categoryPath = QString(selection->getCategoryFullName());

      // Importing resources
      if (!mImportDialog->getResourceFileList().isEmpty())
      {
         QList<QString> files = mImportDialog->getResourceFileList();
         QList<dtCore::ResourceDescriptor> descList = mImportDialog->getDescriptorList();

         std::string categoryRoot = mResourceType->GetName();
         QString root = QString(categoryRoot.c_str());

         for (int i = 0; i < files.size(); ++i)
         {

            resourcePath = "";
            resourceName = "";

            if (files.size() == 1)
            {
               resourceName = mImportDialog->getResourceName();
            }
            else
            {
               resourceName = files.at(i);
            }
            if (resourceName != "")
            {
               ResourceTreeWidget* resource = new ResourceTreeWidget(selection);
               resource->setIsResource(true);
               resource->setText(0, resourceName);
               resource->setCategoryName(descList.at(i).GetDisplayName().c_str());
               resource->setCategoryFullName(descList.at(i).GetResourceIdentifier().c_str());
               resource->setIcon(0, mResourceIcon);
               resourcePath = QString(descList.at(i).GetResourceIdentifier().c_str());

               // create our own resource descriptor
               dtCore::ResourceDescriptor* myResource = new dtCore::ResourceDescriptor(resourceName.toStdString(), resourcePath.toStdString());
               resource->setResourceDescriptor(*myResource);

               // change the selection
               mTree->setCurrentItem(resource);
            }
         }
      }
      // Set the editor data to NULL
      resetEditorDataDescriptor();
      selectionChanged();
      // Save the last directory of the dialog
      mFileDialogDir = mImportDialog->getLastDirectory();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceAbstractBrowser::resetEditorDataDescriptor()
   {
      EditorData::GetInstance().setCurrentResource(*mResourceType, dtCore::ResourceDescriptor());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceAbstractBrowser::setEditorDataDescriptor(dtCore::ResourceDescriptor& descriptor)
   {
      EditorData::GetInstance().setCurrentResource(*mResourceType, descriptor);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceAbstractBrowser::doubleClickEvent() {}

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceAbstractBrowser::deleteItemEvent() {}

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceAbstractBrowser::createCategory()
   {
      ResourceTreeWidget* selection = currentSelection();
      //QTreeWidgetItem* parent = selection->parent();

      QString categoryPath;
      QString newCategory;

      // This is the current full category path
      categoryPath = QString(selection->getCategoryFullName());

      // setup the category dialog with default properties
      AddCategoryDialog* catDialog = new AddCategoryDialog(this);
      catDialog->setType(*mResourceType);
      catDialog->setCategoryPath(categoryPath);
      catDialog->setCreate(true);
      catDialog->exec();

      newCategory = catDialog->getCategory();

      if (!newCategory.isEmpty())
      {
         ResourceTreeWidget* child = new ResourceTreeWidget(selection);
         child->setIsResource(false);
         child->setText(0, newCategory);
         child->setCategoryFullName(catDialog->getCategoryPath());
         child->setCategoryName(newCategory);
         child->setIcon(0, QPixmap(UIResources::ICON_TINY_FOLDER.c_str()));
      }
      // Set the editor data to NULL
      resetEditorDataDescriptor();
   }

} // namespace dtEditQt
