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
*/

#include "GeometryTree.h"
#include "ObjectViewerData.h"

#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>


////////////////////////////////////////////////////////////////////////////////
GeometryTree::GeometryTree(QWidget* parent)
   : QTreeWidget(parent)
{
   CreateContextActions();
   CreateContextMenus();
}

////////////////////////////////////////////////////////////////////////////////
GeometryTree::~GeometryTree()
{
}

////////////////////////////////////////////////////////////////////////////////
void GeometryTree::CreateContextActions()
{
   // Definition Actions
   mSaveAs = new QAction(tr("Save as..."), this);
   mSaveAs->setCheckable(false);
   mSaveAs->setStatusTip(tr("Save the object under another name."));
   connect(mSaveAs, SIGNAL(triggered()), parent(), SLOT(OnSaveAs()));
}

////////////////////////////////////////////////////////////////////////////////
void GeometryTree::CreateContextMenus()
{
   // Definition Context
   mObjectContext = new QMenu(this);
   mObjectContext->addAction(mSaveAs);
}

////////////////////////////////////////////////////////////////////////////////
void GeometryTree::contextMenuEvent(QContextMenuEvent* contextEvent)
{
   QTreeWidgetItem* clickedItem = this->itemAt(contextEvent->pos());
   if (!clickedItem)
   {
      contextEvent->ignore();
      return;
   }

   // De-select all items.
   QList<QTreeWidgetItem*> itemList = selectedItems();
   for (int selectedIndex = 0; selectedIndex < itemList.size(); selectedIndex++)
   {
      QTreeWidgetItem* treeItem = itemList.at(selectedIndex);
      treeItem->setSelected(false);
   }

   // Select our clicked item.
   clickedItem->setSelected(true);

   // Find the object category.
   for (int itemIndex = 0; itemIndex < topLevelItemCount(); ++itemIndex)
   {
      QTreeWidgetItem* categoryItem = topLevelItem(itemIndex);

      if (categoryItem->text(0) == STATIC_MESH_LABEL)
      {
         // Find the selected object.
         for (int objectIndex = 0; objectIndex < categoryItem->childCount(); objectIndex++)
         {
            QTreeWidgetItem* objectItem = categoryItem->child(objectIndex);

            if (objectItem == clickedItem)
            {
               mObjectContext->exec(contextEvent->globalPos());
               return;
            }
         }
      }
   }

   contextEvent->ignore();
}
