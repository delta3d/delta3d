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
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>
#include <QtGui/QDrag>
#include <QtGui/QDragMoveEvent>

#include "dtEditQt/resourcetreewidget.h"
#include <dtUtil/log.h>
#include "dtEditQt/uiresources.h"

namespace dtEditQt
{

   ////////////////////////////////////////////////////////////////////////////////
   // RESOURCE DRAG TREE
   ////////////////////////////////////////////////////////////////////////////////

   ResourceDragTree::ResourceDragTree(QWidget* parent)
   : ResourceTree(parent)
   {
      setDragEnabled(true);
      //setAcceptDrops(true);
      setDropIndicatorShown(true);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ResourceDragTree::setResourceName(const std::string& resourceName)
   {
      mResourceName = resourceName;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ResourceDragTree::dragEnterEvent(QDragEnterEvent *event)
   {
      if (event->mimeData()->hasFormat(mResourceName.c_str()))
      {
         event->accept();
      }
      else
      {
         event->ignore();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ResourceDragTree::dragMoveEvent(QDragMoveEvent *event)
   {
      if (event->mimeData()->hasFormat(mResourceName.c_str()))
      {
         event->setDropAction(Qt::MoveAction);
         event->accept();
      }
      else
      {
         event->ignore();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ResourceDragTree::dropEvent(QDropEvent *event)
   {
      if (event->mimeData()->hasFormat(mResourceName.c_str()))
      {
         event->setDropAction(Qt::MoveAction);
         event->accept();
      }
      else
      {
         event->ignore();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ResourceDragTree::startDrag(Qt::DropActions /*supportedActions*/)
   {
      ResourceTreeWidget *item = dynamic_cast<ResourceTreeWidget*>(currentItem());

      // Only resource items can be dragged.
      if (!item || !item->isResource())
      {
         return;
      }

      QByteArray itemData;
      QDataStream dataStream(&itemData, QIODevice::WriteOnly);
      QIcon icon = item->icon(0);
      QPixmap pixmap = icon.pixmap(16);
      dtCore::ResourceDescriptor resource = item->getResourceDescriptor();
      QString resourceIdentity = resource.GetResourceIdentifier().c_str();

      dataStream << resourceIdentity;

      QMimeData *mimeData = new QMimeData;
      mimeData->setData(mResourceName.c_str(), itemData);

      QDrag *drag = new QDrag(this);
      drag->setMimeData(mimeData);
      drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
      drag->setPixmap(pixmap);

      if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
      {
      }
   }


   ////////////////////////////////////////////////////////////////////////////////
   // RESOURCE TREE WIDGET
   ///////////////////////////////////////////////////////////////////////////////

   ResourceTreeWidget::ResourceTreeWidget(ResourceTree* parent)
      : QTreeWidgetItem(parent)
   {
      //LOG_INFO("Initializing ResourceTree - as a root node (QTreeWidget parent)");
   }
   /////////////////////////////////////////////////////////////////////////////////
   ResourceTreeWidget::ResourceTreeWidget(ResourceTreeWidget* parent)
      : QTreeWidgetItem(parent)
   {
      LOG_DEBUG("Initializing ResourceTreeWidget - as a root node (QTreeWidget parent)");
      // null object - category
   }
   ///////////////////////////////////////////////////////////////////////////////
   ResourceTreeWidget::ResourceTreeWidget(ResourceTreeWidget* parent, dtCore::ResourceDescriptor newResource)
      : QTreeWidgetItem(parent)
   {
      LOG_DEBUG("Initializing ResourceTreeWidget - Resource node");

      mIsResource = false;
      // store the resource assocated with this treeitem
      mResource = newResource;
   }

   ResourceTreeWidget::ResourceTreeWidget(ResourceTreeWidget* parent, ResourceTreeWidget* item)
      : QTreeWidgetItem(parent, item)
   {

   }

   ///////////////////////////////////////////////////////////////////////////////
   ResourceTreeWidget::~ResourceTreeWidget() {}

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::ResourceDescriptor &ResourceTreeWidget::getResourceDescriptor()
   {
      return mResource;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceTreeWidget::recursivelyCreateResourceTree(const dtUtil::tree<dtCore::ResourceTreeNode>::const_iterator& iter, const QIcon& resourceIcon)
   {
      QIcon icon;
      icon.addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER_OPEN.c_str()), QIcon::Normal, QIcon::On);
      icon.addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER.c_str()), QIcon::Normal, QIcon::Off);

      for (dtUtil::tree<dtCore::ResourceTreeNode>::const_iterator i = iter.tree_ref().in();
         i != iter.tree_ref().end();
         ++i)
      {
         if (i->isCategory())
         {
            mCategory = new ResourceTreeWidget(this);
            mCategory->setText(0, QString(i->getNodeText().c_str()));
            mCategory->setIsResource(false);
            // The full path is required so we can delete the category
            mCategory->setCategoryFullName(QString(i->getFullCategory().c_str()));
            mCategory->setCategoryName(QString(i->getNodeText().c_str()));
            // iterate down the branch until we are finished with this node
            mCategory->recursivelyCreateResourceTree(i, resourceIcon);
            mCategory->setIcon(0, icon);
         }
         else
         {
            // pass in the resource
            ResourceTreeWidget* innerNode = new ResourceTreeWidget(this,i->getResource());
            innerNode->setText(0,QString(i->getNodeText().c_str()));
            innerNode->setIsResource(true);
            innerNode->setIcon(0,resourceIcon);
         }
      }
   }
   ///////////////////////////////////////////////////////////////////////////////

} // namespace dtEditQt
