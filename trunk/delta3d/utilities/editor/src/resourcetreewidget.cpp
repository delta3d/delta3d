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
#include <QString>
#include <QList>
#include <QPixmap>
#include <QIcon>

#include "dtEditQt/resourcetreewidget.h"
#include <dtUtil/log.h>
#include "dtEditQt/uiresources.h"
#include <dtDAL/tree.h>

namespace dtEditQt{

    ///////////////////////////////////////////////////////////////////////////////
    ResourceTreeWidget::ResourceTreeWidget(ResourceTree *parent)
        : QTreeWidgetItem(parent)
    {
        //LOG_INFO("Initializing ResourceTree - as a root node (QTreeWidget parent)");
    }
    /////////////////////////////////////////////////////////////////////////////////
    ResourceTreeWidget::ResourceTreeWidget(ResourceTreeWidget *parent)
        : QTreeWidgetItem(parent)
    {
        LOG_INFO("Initializing ResourceTreeWidget - as a root node (QTreeWidget parent)");
        // null object - category
    }
    ///////////////////////////////////////////////////////////////////////////////
    ResourceTreeWidget::ResourceTreeWidget(ResourceTreeWidget *parent, dtDAL::ResourceDescriptor newResource)
        : QTreeWidgetItem(parent)
    {
        LOG_INFO("Initializing ResourceTreeWidget - Resource node");
        
        resource = false;
        // store the resource assocated with this treeitem
        myResource = newResource;
    }
    ResourceTreeWidget::ResourceTreeWidget(ResourceTreeWidget *parent, ResourceTreeWidget *item)
        : QTreeWidgetItem(parent,item)
    {

    }
    ///////////////////////////////////////////////////////////////////////////////
    ResourceTreeWidget::~ResourceTreeWidget(){}
    ///////////////////////////////////////////////////////////////////////////////
    dtDAL::ResourceDescriptor &ResourceTreeWidget::getResourceDescriptor()
    {
        return this->myResource;
    }
    ///////////////////////////////////////////////////////////////////////////////
    void ResourceTreeWidget::recursivelyCreateResourceTree(const core::tree<dtDAL::ResourceTreeNode>::const_iterator& iter, QIcon *resourceIcon)
    { 
        
        QIcon *icon = new QIcon();
        icon->addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER_OPEN.c_str()),QIcon::Normal,QIcon::On);
        icon->addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER.c_str()),QIcon::Normal,QIcon::Off);

        for (core::tree<dtDAL::ResourceTreeNode>::const_iterator i = iter.tree_ref().in();
            i != iter.tree_ref().end();
            ++i)
        {
            
            if(i->isCategory())
            {   
                category = new ResourceTreeWidget(this);
                category->setText(0,QString(i->getNodeText().c_str()));
                category->setIfResource(false);
                // The full path is required so we can delete the category
                category->setCategoryFullName(QString(i->getFullCategory().c_str()));
                category->setCategoryName(QString(i->getNodeText().c_str()));
                // iterate down the branch until we are finished with this node
                category->recursivelyCreateResourceTree(i,resourceIcon);
                category->setIcon(0,*icon);
                
            }
            else
            {
                // pass in the resource
                ResourceTreeWidget *innerNode = new ResourceTreeWidget(this,i->getResource());
                innerNode->setText(0,QString(i->getNodeText().c_str()));
                innerNode->setIfResource(true);
                innerNode->setIcon(0,*resourceIcon);
            }
         }
    }
    ///////////////////////////////////////////////////////////////////////////////
}
