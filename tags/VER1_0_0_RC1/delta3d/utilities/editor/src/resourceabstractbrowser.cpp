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
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QList>
#include <QMenu>
#include <QAction>
#include <QTreeWidgetItem>
#include <QContextMenuEvent>
#include <QPixmap>
#include <QIcon>

#include "dtEditQt/resourceabstractbrowser.h"
#include "dtDAL/resourcedescriptor.h"
#include "dtEditQt/resourcetreewidget.h"
#include "dtEditQt/uiresources.h"

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    ResourceAbstractBrowser::ResourceAbstractBrowser(dtDAL::DataType *type,QWidget *parent)
        : QWidget(parent)
    {
        tree = new ResourceTree(parent);
        tree->setColumnCount(1);
        tree->header()->hide();
        tree->setFocusPolicy(Qt::StrongFocus);

        this->rootName = QString(type->GetDisplayName().c_str());
        this->resourceType = type;
        this->parent = parent;

        // create an event filter so we can grab tree key events
        tree->installEventFilter(this);

        // setup right mouse click context menu
        createActions();
        createContextMenu();

        // Connect the projectChanged signal so we can populate the tree
        // at the appropriate time
        connect(&EditorEvents::getInstance(),SIGNAL(projectChanged()),
        this,SLOT(onProjectChanged()));

        connect(tree,SIGNAL(doubleClicked(const QModelIndex)),this,SLOT(doubleClickEvent()));
    }
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceAbstractBrowser::buildResourceTree(dtDAL::DataType &type, QWidget *parent, QIcon *resourceIcon)
    {
        // make sure we have a valid type before we build the tree
        if(!type.GetDisplayName().empty())
        {
            // grab an instance of our project
            dtDAL::Project& project = dtDAL::Project::GetInstance();

            // refresh the project
            project.Refresh();
            project.GetResourcesOfType(type,iterTree);

            QIcon *icon = new QIcon();
            icon->addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER_OPEN.c_str()),QIcon::Normal,QIcon::On);
            icon->addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER.c_str()),QIcon::Normal,QIcon::Off);

            // construct our tree
            root = new ResourceTreeWidget(tree);
            root->setText(0,rootName);
            root->recursivelyCreateResourceTree(iterTree,resourceIcon);
            root->setIfResource(false);
            root->setIcon(0,*icon);
 
            selection = new ResourceTreeWidget();
            selection->setCategoryName(QString(type.GetName().c_str()));

            // connect tree signals
            connect(tree, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    ResourceTreeWidget* ResourceAbstractBrowser::currentSelection()
    {
        selection = NULL;

        if (tree != NULL) 
        {
            QList <QTreeWidgetItem *> list = tree->selectedItems();

            if (!list.isEmpty()) 
            {
                selection = dynamic_cast<ResourceTreeWidget*>(list[0]);
            }
            if(selection != NULL)
            {
                if(selection->isResource())
                {
                    setEditorDataDescriptor(selection->getResourceDescriptor());
                }
                else
                {
                    resetEditorDataDescriptor();
                }
            }
        }
        return selection;
    }
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceAbstractBrowser::removeTreeNode()
    {
        int index;
        //dtDAL::Project& project = dtDAL::Project::GetInstance();

        if(selection!=NULL)
        {
            QTreeWidgetItem *parent = selection->parent();

            if (parent != NULL) 
            {
                index = parent->indexOfChild(selection);
                parent->takeChild(index);
                tree->setItemExpanded(parent, true);
            } 
            else 
            {
                index = tree->indexOfTopLevelItem(selection);
                tree->takeTopLevelItem(index);
                tree->setItemExpanded(root, true);
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceAbstractBrowser::addTreeNode(QString &nodeText, dtDAL::ResourceDescriptor descriptor,
        bool resource)
    {
        if(!nodeText.isEmpty())
        {
            ResourceTreeWidget *child = new ResourceTreeWidget(selection);
            child->setText(0,nodeText);
            child->setIfResource(resource);
            child->setResourceDescriptor(descriptor);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceAbstractBrowser::refreshResourceTree()
    {
        // Set the editor data to NULL
        resetEditorDataDescriptor();

        // clear the tree
        tree->clear();

        // rebuild the tree
        buildResourceTree(*resourceType, getCurrentParent(), &resourceIcon);

        // change the selection to the root to be nice
        tree->setCurrentItem(tree->topLevelItem(0));
        selectionChanged();
    }
    ///////////////////////////////////////////////////////////////////////////////
    bool ResourceAbstractBrowser::eventFilter(QObject *obj, QEvent *e)
    {
        if (obj == tree) 
        {
            //For some reason, KeyPress is getting defined by something...
            //Without this undef, it will not compile under Linux..
            //It would be great if someone could figure out exactly what's
            //going on.
            #undef KeyPress
            if (e->type() == QEvent::KeyPress) 
            {
                QKeyEvent *keyEvent = (QKeyEvent *)e;
                switch(keyEvent->key())
                {
                case Qt::Key_Return :
                    //invokeSelection
                    // enterKeySelected();
                    break;
                case Qt::Key_Enter:
                    //invokeSelection
                    // enterKeySelected();
                    break;
                default:
                    return tree->eventFilter(obj,e);
                }
            }
            else
            {
                // pass the event on to the parent class
                return tree->eventFilter(obj, e);
            }
        }
        return false;
    }
    ///////////////////////////////////////////////////////////////////////////////
    // Context Menu
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceAbstractBrowser::contextMenuEvent(QContextMenuEvent *e)
    {
        if (tree->underMouse()) 
        {
            contextMenu->exec(e->globalPos());
        }
        else 
        {
            e->ignore();
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceAbstractBrowser::createContextMenu()
    {
        contextMenu = new QMenu(this);
        contextMenu->addAction(setRefreshAction);
        contextMenu->addAction(setDeleteAction);
        contextMenu->addAction(setCategoryAction);
        contextMenu->addAction(setImportAction);
    }
    ///////////////////////////////////////////////////////////////////////////////
    // Actions
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceAbstractBrowser::createActions()
    {
        setRefreshAction = new QAction(tr("&Refresh"),getCurrentParent());
        setRefreshAction->setCheckable(false);
        connect(setRefreshAction,SIGNAL(triggered()),this,SLOT(refreshSelected()));

        setDeleteAction  = new QAction(tr("&Delete"),getCurrentParent());
        setDeleteAction->setCheckable(false);
        connect(setDeleteAction,SIGNAL(triggered()),this,SLOT(deleteSelected()));
        setDeleteAction->setEnabled(false);

        setCategoryAction = new QAction(tr("&Create Category"),getCurrentParent());
        setCategoryAction->setCheckable(false);
        connect(setCategoryAction, SIGNAL(triggered()),this,SLOT(createCategory()));
        setCategoryAction->setEnabled(false);

        setImportAction = new QAction(tr("&Import Resource"),getCurrentParent());
        setImportAction->setCheckable(false);
        connect(setImportAction, SIGNAL(triggered()),this,SLOT(importSelected()));
        setImportAction->setEnabled(false);
    }
    ///////////////////////////////////////////////////////////////////////////////
    QGroupBox* ResourceAbstractBrowser::standardButtons(const QString &groupBoxName)
    {
        // surrounds our tree and buttons
        QGroupBox *group = new QGroupBox(tr("Tools"));

        // contains 2 cells, one for the tree and the other for the buttons
        QGridLayout *grid = new QGridLayout(group);

        // contains our  buttons
        QHBoxLayout *hbox = new QHBoxLayout();

        importBtn = new QPushButton(""/*"Import"*/,group);
        importBtn->setToolTip("Import - add a new resource to the project");
        importBtn->setIcon(QPixmap(UIResources::LARGE_ICON_EDIT_IMPORT.c_str()));
        connect(importBtn,SIGNAL(clicked()),this,SLOT(importSelected()));

        deleteBtn = new QPushButton(""/*"Delete"*/,group);
        deleteBtn->setIcon(QPixmap(UIResources::LARGE_ICON_EDIT_DELETE.c_str()));
        deleteBtn->setToolTip("Delete - remove the current resource or category from the project");
        connect(deleteBtn,SIGNAL(clicked()),this,SLOT(deleteSelected()));
        deleteBtn->setDisabled(true);

        refreshBtn = new QPushButton(""/*"Refresh"*/,group);
        refreshBtn->setToolTip("Refresh - update the categories and resources directly from your hard drive");
        refreshBtn->setIcon(QPixmap(UIResources::LARGE_ICON_EDIT_REFRESH.c_str()));
        connect(refreshBtn,SIGNAL(clicked()),this,SLOT(refreshSelected()));

        categoryBtn = new QPushButton(""/*Category"*/,group);
        categoryBtn->setToolTip("Create Category - create a new category folder in the current category");
        categoryBtn->setIcon(QPixmap(UIResources::LARGE_ICON_EDIT_CATEGORY.c_str()));
        connect(categoryBtn,SIGNAL(clicked()),this,SLOT(createCategory()));

        hbox->addStretch(1);
        hbox->addWidget(importBtn,0);
        hbox->addWidget(deleteBtn,0);
        hbox->addWidget(refreshBtn,0);
        hbox->addWidget(categoryBtn,0);
        hbox->addStretch(1);

        grid->addLayout(hbox,0,0);

        return group;
    }
    ///////////////////////////////////////////////////////////////////////////////
    // Slots
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceAbstractBrowser::onProjectChanged()
    {
        // grab an instance of our project
        dtDAL::Project& project = dtDAL::Project::GetInstance();

        if (!project.IsContextValid())
            return;

        refreshResourceTree();
    }
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceAbstractBrowser::selectionChanged()
    {
        ResourceTreeWidget *selection = currentSelection();

        if(selection !=NULL)
        {
            // checks if we have the root of the browser tree
            if(tree->indexOfTopLevelItem(selection)==0)
            {
                deleteBtn->setDisabled(true);
                setDeleteAction->setEnabled(false);
            }
            else
            {
                deleteBtn->setDisabled(false);
                setDeleteAction->setEnabled(true);
            }

            // handle turning on and off buttons and context menu items
            if(selection->isResource())
            {
                importBtn->setDisabled(true);
                categoryBtn->setDisabled(true);
                setCategoryAction->setEnabled(false);
                setImportAction->setEnabled(false);
            }
            else
            {
                importBtn->setDisabled(false);
                categoryBtn->setDisabled(false);
                setCategoryAction->setEnabled(true);
                setImportAction->setEnabled(true);
            }
        }
        else
        {
            importBtn->setDisabled(true);
            categoryBtn->setDisabled(true);
            setImportAction->setEnabled(false);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceAbstractBrowser::deleteSelected()
    {
        ResourceTreeWidget *selection = currentSelection();
        dtDAL::Project& project = dtDAL::Project::GetInstance();

        QString categoryPath;

        if(selection)
        {
	    //int index;
            bool resource;
            QString message;
            QString category;
            //QTreeWidgetItem *parent = selection->parent();

            resource = selection->isResource();

            if(resource)
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
            QMessageBox *mb = new QMessageBox("Browser",
                message,
                QMessageBox::Question,
                QMessageBox::Yes | QMessageBox::Default,
                QMessageBox::No | QMessageBox::Escape,
                QMessageBox::NoButton);

            mb->setButtonText(QMessageBox::Yes, "Yes");
            mb->setButtonText(QMessageBox::No, "No");

            switch(mb->exec()) 
            {
                case QMessageBox::Yes :
                    // Delete the resource
                    if(resource)
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
                            *resourceType,true);
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
        tree->setCurrentItem(tree->topLevelItem(0));
    }
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceAbstractBrowser::refreshSelected()
    {
        //ResourceTreeWidget *selection = currentSelection();
        ResourceAbstractBrowser::refreshResourceTree();
    }
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceAbstractBrowser::importSelected()
    {
        ResourceTreeWidget *selection = currentSelection();
        //QTreeWidgetItem *parent = selection->parent();
        QString categoryPath;
        QString resourcePath;
        QString resourceName;
        
        // Setup the import dialog default parameters for this browser
        importDialog = new ResourceImportDialog(this,*resourceType);

        // Grab the filter from the resourceTypeHandler
        if(!fileDialogDir.isEmpty())
        {
            importDialog->setLastDirectory(fileDialogDir);
        }

        importDialog->setCategory(selection->getCategoryFullName());
        importDialog->updateData();

        try
        {
            importDialog->exec();
        }
        catch(const dtDAL::Exception& e)
        {
            return;
        }

        // This is the current full category path
        categoryPath = QString(selection->getCategoryFullName());

        // Importing resources
        if(!importDialog->getResourceFileList().isEmpty())
        {
            QList<QString> files = importDialog->getResourceFileList();
            QList<dtDAL::ResourceDescriptor> descList = importDialog->getDescriptorList();

            std::string categoryRoot = resourceType->GetName();
            QString root = QString(categoryRoot.c_str());

            for(int i=0;i<files.size();++i)
            { 
                
                resourcePath = "";
                resourceName = "";

                if(files.size()==1)
                {
                    resourceName = importDialog->getResourceName();
                }
                else
                {
                    resourceName = files.at(i);
                }
                if(resourceName!="")
                {
                    ResourceTreeWidget *resource = new ResourceTreeWidget(selection);
                    resource->setIfResource(true);
                    resource->setText(0,resourceName);
                    resource->setCategoryName(descList.at(i).GetDisplayName().c_str());
                    resource->setCategoryFullName(descList.at(i).GetResourceIdentifier().c_str());
                    resource->setIcon(0,resourceIcon);
                    resourcePath = QString(descList.at(i).GetResourceIdentifier().c_str());
                    
                    // create our own resource descriptor
                    dtDAL::ResourceDescriptor *myResource = new dtDAL::ResourceDescriptor(resourceName.toStdString(),resourcePath.toStdString());
                    resource->setResourceDescriptor(*myResource);
                    
                    // change the selection
                    tree->setCurrentItem(resource);
                }
            }
        }
        // Set the editor data to NULL
        resetEditorDataDescriptor();
        selectionChanged();
        // Save the last directory of the dialog
        fileDialogDir = importDialog->getLastDirectory();
    }
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceAbstractBrowser::resetEditorDataDescriptor()
    {
        if(*resourceType == dtDAL::DataType::SOUND)
        {
            EditorData::getInstance().setCurrentSoundResource(dtDAL::ResourceDescriptor());
        }
        else if(*resourceType == dtDAL::DataType::STATIC_MESH)
        {
            EditorData::getInstance().setCurrentMeshResource(dtDAL::ResourceDescriptor());
        }
        else if(*resourceType == dtDAL::DataType::TEXTURE)
        {
            EditorData::getInstance().setCurrentTextureResource(dtDAL::ResourceDescriptor());
        }
        else if(*resourceType == dtDAL::DataType::PARTICLE_SYSTEM)
        {
            EditorData::getInstance().setCurrentParticleResource(dtDAL::ResourceDescriptor());
        }
        else if(*resourceType == dtDAL::DataType::CHARACTER)
        {
            EditorData::getInstance().setCurrentCharacterResource(dtDAL::ResourceDescriptor());
        }
        else if(*resourceType == dtDAL::DataType::TERRAIN)
        {
            EditorData::getInstance().setCurrentTerrainResource(dtDAL::ResourceDescriptor());
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceAbstractBrowser::setEditorDataDescriptor(dtDAL::ResourceDescriptor &descriptor)
    {
        if(*resourceType == dtDAL::DataType::SOUND)
        {
            EditorData::getInstance().setCurrentSoundResource(descriptor);
        }
        else if(*resourceType == dtDAL::DataType::STATIC_MESH)
        {
            EditorData::getInstance().setCurrentMeshResource(descriptor);
        }
        else if(*resourceType == dtDAL::DataType::TEXTURE)
        {
            EditorData::getInstance().setCurrentTextureResource(descriptor);
        }
        else if(*resourceType == dtDAL::DataType::PARTICLE_SYSTEM)
        {
            EditorData::getInstance().setCurrentParticleResource(descriptor);
        }
        else if(*resourceType == dtDAL::DataType::CHARACTER)
        {
            EditorData::getInstance().setCurrentCharacterResource(descriptor);
        }
        else if(*resourceType == dtDAL::DataType::TERRAIN)
        {
            EditorData::getInstance().setCurrentTerrainResource(descriptor);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceAbstractBrowser::doubleClickEvent(){}
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceAbstractBrowser::deleteItemEvent(){}
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceAbstractBrowser::createCategory()
    {
        ResourceTreeWidget *selection = currentSelection();
        //QTreeWidgetItem *parent = selection->parent();

        QString categoryPath;
        QString newCategory;

        // This is the current full category path
        categoryPath = QString(selection->getCategoryFullName());

        // setup the category dialog with default properties
        AddCategoryDialog *catDialog = new AddCategoryDialog(this);
        catDialog->setType(*resourceType);
        catDialog->setCategoryPath(categoryPath);
        catDialog->setCreate(true);
        catDialog->exec();

        newCategory = catDialog->getCategory();

        if(!newCategory.isEmpty())
        {
            ResourceTreeWidget *child = new ResourceTreeWidget(selection);
            child->setIfResource(false);
            child->setText(0,newCategory);
            child->setCategoryFullName(catDialog->getCategoryPath());
            child->setCategoryName(newCategory);
            child->setIcon(0,QPixmap(UIResources::ICON_TINY_FOLDER.c_str()));
        }
        // Set the editor data to NULL
        resetEditorDataDescriptor();
    }
}
