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

#ifndef DELTA_RESOURCE_TREE_WIDGET
#define DELTA_RESOURCE_TREE_WIDGET

#include <QtCore/QList>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QIcon>

#include <dtDAL/project.h>
#include <osg/ref_ptr>

namespace dtEditQt{

    class ResourceTreeNode;

    /**
    * Class ResourceTree
    * @note Created this sub class to add much needed functionality to the QTreeWidget class
    */
    class ResourceTree : public QTreeWidget
    {

    public:
        ResourceTree(QWidget *parent = 0){}
        virtual ~ResourceTree(){}

    };

    /**
    * Class ResourceTreeWidget
    * @note This class is a subclass of QTreeWidgetItem. It is a very simple class
    * that holds properties associated with resources.
    */
    class ResourceTreeWidget : public QTreeWidgetItem
    {
    public:
        /**
        * Constructor
        */
        // root node
        ResourceTreeWidget(ResourceTree *parent);

        // category node
        ResourceTreeWidget(ResourceTreeWidget *parent = 0);

        // resource node
        ResourceTreeWidget(ResourceTreeWidget *parent, const dtDAL::ResourceDescriptor resource);

        ResourceTreeWidget(ResourceTreeWidget *parent, ResourceTreeWidget *item);

        /**
        * Destructor
        */
        virtual ~ResourceTreeWidget();

        dtDAL::ResourceDescriptor &getResourceDescriptor();

        /**
        * Recursive function to construct the tree
        * @param An iterator of a tree that contains ResourceTreeNode
        */
        void recursivelyCreateResourceTree(const dtUtil::tree<dtDAL::ResourceTreeNode>::const_iterator &iter, QIcon *resourceIcon = 0);

        /**
        * This returns a boolean if the current resource is in fact a resource
        * @return boolean
        */
        bool isResource(){return this->resource;}

        /**
        * setCategoryName
        * @param QString of the current category NodeText
        */
        void setCategoryName(const QString newCategoryName){this->categoryName = newCategoryName;}

        /**
        * getCategoryName
        * @return QString category name captured from the node
        */
        QString getCategoryName(){return this->categoryName;}

        /**
        * setCategoryFullName
        * @param QString of the full category name including the path
        */
        void setCategoryFullName(const QString categoryFullName){this->categoryFullName = categoryFullName;}

        /**
        * getCategoryFullName
        * @return QString of the full category name including the path
        */
        QString getCategoryFullName(){return this->categoryFullName;}

        /**
        * setIfResource
        * @param boolean to set if this is a resource
        */
        void setIfResource(const bool isResource){resource = isResource;}

        /**
        * setResourceDescriptor
        * @param ResourceDescriptor
        */
        void setResourceDescriptor(dtDAL::ResourceDescriptor &myDescriptor){myResource = myDescriptor;}

    private:

        void setIndex(int index){this->itemIndex = index;}
        int getIndex(){return itemIndex;}

        // This holds the type of the currently selected resource
        // only valid resources hold a resouce type - meaning
        // we exclude categories
        dtDAL::ResourceDescriptor myResource;
        bool resource;
        QString categoryFullName;
        QString categoryName;
        int itemIndex;
        ResourceTreeWidget *category;
    };
}
#endif
