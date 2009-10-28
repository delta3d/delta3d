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
 */

#include "dtEditQt/resourcelistwidgetitem.h"

#include <prefix/dtstageprefix-src.h>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>
#include <QtGui/QDrag>
#include <QtGui/QDragMoveEvent>

#include <dtUtil/log.h>
#include "dtEditQt/uiresources.h"

namespace dtEditQt
{
   ////////////////////////////////////////////////////////////////////////////////
   // RESOURCE DRAG TREE
   ////////////////////////////////////////////////////////////////////////////////

   ResourceDragListWidget::ResourceDragListWidget(QWidget* parent)
      : QListWidget(parent)
   {
      setDragEnabled(true);
      //setAcceptDrops(true);
      setDropIndicatorShown(true);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ResourceDragListWidget::setResourceName(const std::string& resourceName)
   {
      mResourceName = resourceName;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ResourceDragListWidget::dragEnterEvent(QDragEnterEvent *event)
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
   void ResourceDragListWidget::dragMoveEvent(QDragMoveEvent *event)
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
   void ResourceDragListWidget::dropEvent(QDropEvent *event)
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
   void ResourceDragListWidget::startDrag(Qt::DropActions /*supportedActions*/)
   {
      ResourceListWidgetItem *item = dynamic_cast<ResourceListWidgetItem*>(currentItem());

      // Only resource items can be dragged.
      if (!item || !item->isResource())
      {
         return;
      }

      QByteArray itemData;
      QDataStream dataStream(&itemData, QIODevice::WriteOnly);
      QIcon icon = item->icon();
      QPixmap pixmap = icon.pixmap(16);
      dtDAL::ResourceDescriptor resource = item->getResourceDescriptor();
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
   // RESOURCE LIST WIDGET ITEM
   ///////////////////////////////////////////////////////////////////////////////

   ///////////////////////////////////////////////////////////////////////////////
   ResourceListWidgetItem::ResourceListWidgetItem(const dtDAL::ResourceDescriptor resource,
      const QIcon& icon, const QString& text /* = "" */, QListWidget* parent /* = 0 */,
      QListWidgetItem::ItemType type /* = QListWidgetItem::Type */)
         : QListWidgetItem(icon, text, parent, type)
         , mResource(resource)
   {
      if (resource.GetResourceIdentifier() == "")
      {
         mIsResource = false;
      }
      else
      {
         mIsResource = true;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   ResourceListWidgetItem::~ResourceListWidgetItem() {}

   ///////////////////////////////////////////////////////////////////////////////
   dtDAL::ResourceDescriptor& ResourceListWidgetItem::getResourceDescriptor()
   {
      return mResource;
   }

} // namespace dtEditQt
