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

#ifndef DELTA_RESOURCE_TREE_WIDGET
#define DELTA_RESOURCE_TREE_WIDGET

#include <QtCore/QList>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QIcon>

#include <dtDAL/project.h>

namespace dtEditQt
{

   class ResourceTreeNode;

   /**
    * Class ResourceTree
    * @note Created this sub class to add much needed functionality to the QTreeWidget class
    */
   class ResourceTree : public QTreeWidget
   {
   public:
      ResourceTree(QWidget* parent = 0) {}
      virtual ~ResourceTree() {}
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
      ResourceTreeWidget(ResourceTree* parent);

      // category node
      ResourceTreeWidget(ResourceTreeWidget* parent = 0);

      // resource node
      ResourceTreeWidget(ResourceTreeWidget* parent, const dtDAL::ResourceDescriptor resource);

      ResourceTreeWidget(ResourceTreeWidget* parent, ResourceTreeWidget* item);

      /**
       * Destructor
       */
      virtual ~ResourceTreeWidget();

      dtDAL::ResourceDescriptor& getResourceDescriptor();

      /**
       * Recursive function to construct the tree
       * @param An iterator of a tree that contains ResourceTreeNode
       */
      void recursivelyCreateResourceTree(const dtUtil::tree<dtDAL::ResourceTreeNode>::const_iterator& iter, const QIcon& resourceIcon);

      /**
       * This returns a boolean if the current resource is in fact a resource
       * @return boolean
       */
      bool isResource() { return this->resource; }

      /**
       * setCategoryName
       * @param QString of the current category NodeText
       */
      void setCategoryName(const QString newCategoryName) { this->categoryName = newCategoryName; }

      /**
       * getCategoryName
       * @return QString category name captured from the node
       */
      QString getCategoryName() { return this->categoryName; }

      /**
       * setCategoryFullName
       * @param QString of the full category name including the path
       */
      void setCategoryFullName(const QString categoryFullName) { this->categoryFullName = categoryFullName; }

      /**
       * getCategoryFullName
       * @return QString of the full category name including the path
       */
      QString getCategoryFullName() { return this->categoryFullName; }

      /**
       * setIfResource
       * @param boolean to set if this is a resource
       */
      void setIfResource(const bool isResource) { resource = isResource; }

      /**
       * setResourceDescriptor
       * @param ResourceDescriptor
       */
      void setResourceDescriptor(dtDAL::ResourceDescriptor& myDescriptor) { myResource = myDescriptor; }

   private:
      void setIndex(int index) { this->itemIndex = index; }
      int getIndex() { return itemIndex; }

      // This holds the type of the currently selected resource
      // only valid resources hold a resouce type - meaning
      // we exclude categories
      dtDAL::ResourceDescriptor myResource;
      bool resource;
      QString categoryFullName;
      QString categoryName;
      int itemIndex;
      ResourceTreeWidget* category;
   };

} // namespace dtEditQt

#endif // DELTA_RESOURCE_TREE_WIDGET
