/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2008 MOVES Institute
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
*/

#include "ShaderTree.h"

#include <QtGui/QAction>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>

////////////////////////////////////////////////////////////////////////////////
ShaderTree::ShaderTree(QWidget* parent)
   : QTreeWidget(parent)
{
   CreateContextActions();
   CreateContextMenus();
}

////////////////////////////////////////////////////////////////////////////////
ShaderTree::~ShaderTree()
{
}

////////////////////////////////////////////////////////////////////////////////
void ShaderTree::SetShaderSourceEnabled(bool vertexEnabled,
                                        bool geometryEnabled,
                                        bool fragmentEnabled)
{
   mOpenVertexSource->setEnabled(vertexEnabled);
   mOpenGeometrySource->setEnabled(geometryEnabled);
   mOpenFragmentSource->setEnabled(fragmentEnabled);
}

////////////////////////////////////////////////////////////////////////////////
void ShaderTree::CreateContextActions()
{
   // Definition Actions
   mEditShaderDef = new QAction(tr("&Edit"), this);
   mEditShaderDef->setCheckable(false);
   mEditShaderDef->setStatusTip(tr("Edits the Shader Definition File (Not implemented yet)"));
   mEditShaderDef->setEnabled(false);
   connect(mEditShaderDef, SIGNAL(triggered()), parent(), SLOT(OnEditShaderDef()));

   mRemoveShaderDef = new QAction(tr("&Remove"), this);
   mRemoveShaderDef->setCheckable(false);
   mRemoveShaderDef->setStatusTip(tr("Removes the Shader Definition from the list"));
   connect(mRemoveShaderDef, SIGNAL(triggered()), parent(), SLOT(OnRemoveShaderDef()));

   // Program Actions
   mOpenVertexSource = new QAction(tr("&Open Vertex Source"), this);
   mOpenVertexSource->setCheckable(false);
   mOpenVertexSource->setStatusTip(tr("Open Current Vertex Shader"));
   connect(mOpenVertexSource, SIGNAL(triggered()), parent(), SLOT(OnOpenCurrentVertexShaderSources()));

   mOpenGeometrySource = new QAction(tr("&Open Geometry Source"), this);
   mOpenGeometrySource->setCheckable(false);
   mOpenGeometrySource->setStatusTip(tr("Open Current Geometry Shader"));
   connect(mOpenGeometrySource, SIGNAL(triggered()), parent(), SLOT(OnOpenCurrentGeometryShaderSources()));

   mOpenFragmentSource = new QAction(tr("&Open Fragment Source"), this);
   mOpenFragmentSource->setCheckable(false);
   mOpenFragmentSource->setStatusTip(tr("Open Current Fragment Shader"));
   connect(mOpenFragmentSource, SIGNAL(triggered()), parent(), SLOT(OnOpenCurrentFragmentShaderSources()));
}

////////////////////////////////////////////////////////////////////////////////
void ShaderTree::CreateContextMenus()
{
   // Definition Context
   mDefinitionContext = new QMenu(this);
   mDefinitionContext->addAction(mEditShaderDef);
   mDefinitionContext->addSeparator();
   mDefinitionContext->addAction(mRemoveShaderDef);

   // Program Context
   mProgramContext = new QMenu(this);
   mProgramContext->addAction(mOpenVertexSource);
   mProgramContext->addAction(mOpenFragmentSource);
}

////////////////////////////////////////////////////////////////////////////////
void ShaderTree::contextMenuEvent(QContextMenuEvent *contextEvent)
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

   // Iterate through each definition to see if they were selected.
   for (int itemIndex = 0; itemIndex < topLevelItemCount(); ++itemIndex)
   {
      QTreeWidgetItem* fileItem = topLevelItem(itemIndex);

      if (fileItem == clickedItem)
      {
         // Show Shader Definition Context menu.
         mDefinitionContext->exec(contextEvent->globalPos());
         return;
      }

      // Iterate through each program to see if they were selected.
      for (int groupIndex = 0; groupIndex < fileItem->childCount(); groupIndex++)
      {
         QTreeWidgetItem* groupItem = fileItem->child(groupIndex);

         for (int programIndex = 0; programIndex < groupItem->childCount(); programIndex++)
         {
            QTreeWidgetItem* programItem = groupItem->child(programIndex);

            if (programItem == clickedItem)
            {
               mProgramContext->exec(contextEvent->globalPos());
               return;
            }
         }
      }
   }

   contextEvent->ignore();
}

