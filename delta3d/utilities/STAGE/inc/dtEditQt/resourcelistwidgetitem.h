/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2009
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
 * This software was developed by the U. S. Government
 *
 * Michael Day
 */

#ifndef STAGE_RESOURCE_LIST_WIDGET_ITEM_H__
#define STAGE_RESOURCE_LIST_WIDGET_ITEM_H__

#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QIcon>

#include <dtDAL/project.h>

namespace dtEditQt
{
   /**
   * Class ResourceDragListWidget
   * @note Created this sub class to handle list widget's with drag/drop.
   */
   class ResourceDragListWidget : public QListWidget
   {  
   public:
      ResourceDragListWidget(QWidget* parent = 0);
      virtual ~ResourceDragListWidget() {}

      /**
      * Sets the name of the resource.
      *
      * @param[in]  resourceName  The name of the resource.
      */
      void setResourceName(const std::string& resourceName);

   protected:
      void dragEnterEvent(QDragEnterEvent *event);
      void dragMoveEvent(QDragMoveEvent *event);
      void dropEvent(QDropEvent *event);
      void startDrag(Qt::DropActions supportedActions);

      std::string mResourceName;
   };

   /**
    * Class ResourceListWidgetItem
    * @note This class is a subclass of QListWidgetItem. It is a very simple class
    * that holds properties associated with resources.
    */
   class ResourceListWidgetItem : public QListWidgetItem
   {
   public:
      /**
       * Constructor
       */

      // resource node
      //ResourceTreeWidget(ResourceTreeWidget* parent, const dtDAL::ResourceDescriptor resource);
      ResourceListWidgetItem(const dtDAL::ResourceDescriptor resource,
                             const QIcon& icon, const QString& text = "",
                             QListWidget* parent = 0,
                             QListWidgetItem::ItemType type = QListWidgetItem::Type);

      /**
       * Destructor
       */
      virtual ~ResourceListWidgetItem();

      dtDAL::ResourceDescriptor& getResourceDescriptor();

      /**
       * This returns a boolean if the current resource is in fact a resource
       * @return boolean
       */
      bool isResource() { return mIsResource; }

      /**
       * setCategoryFullName
       * @param QString of the full category name including the path
       */
      void setCategoryFullName(const QString categoryFullName) { mCategoryFullName = categoryFullName; }

      /**
       * getCategoryFullName
       * @return QString of the full category name including the path
       */
      QString getCategoryFullName() { return mCategoryFullName; }

      /**
       * setIfResource
       * @param boolean to set if this is a resource
       */
      void setIsResource(const bool isResource) { mIsResource = isResource; }

      /**
       * setResourceDescriptor
       * @param ResourceDescriptor
       */
      void setResourceDescriptor(dtDAL::ResourceDescriptor& descriptor) { mResource = descriptor; }

   private:
      void setIndex(int index) { mItemIndex = index; }
      int getIndex() { return mItemIndex; }

      // This holds the type of the currently selected resource
      // only valid resources hold a resouce type - meaning
      // we exclude categories
      dtDAL::ResourceDescriptor mResource;
      bool                      mIsResource;
      QString                   mCategoryFullName;
      QString                   mCategoryName;
      int                       mItemIndex;
   };

} // namespace dtEditQt

#endif //STAGE_RESOURCE_LIST_WIDGET_ITEM_H__
