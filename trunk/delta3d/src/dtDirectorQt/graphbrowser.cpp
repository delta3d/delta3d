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
 * Author: Jeff P. Houde
 */
#include <prefix/dtdirectorqtprefix.h>
#include <dtDirectorQt/graphbrowser.h>
#include <dtDirectorQt/directoreditor.h>

#include <QtGui/QWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QGridLayout>
#include <QtGui/QTreeWidget>

namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   GraphItem::GraphItem(DirectorGraph* graph, QTreeWidgetItem* parent)
      : QTreeWidgetItem(parent)
      , mGraph(graph)
   {
      if (graph)
      {
         setText(0, graph->GetName().c_str());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   GraphBrowser::GraphBrowser(QMainWindow* parent)
      : QDockWidget(parent)
   {
      setWindowTitle("Macro Browser");

      QWidget* mainAreaWidget = new QWidget(this);
      QGridLayout* mainAreaLayout = new QGridLayout(mainAreaWidget);
      setWidget(mainAreaWidget);

      mGroupBox = new QGroupBox("Macro Hierarchy", mainAreaWidget);
      mGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      mainAreaLayout->addWidget(mGroupBox, 0, 0);
      QGridLayout* innerLayout = new QGridLayout(mGroupBox);

      mGraphTree = new QTreeWidget(mGroupBox);
      innerLayout->addWidget(mGraphTree);
      mGraphTree->setHeaderHidden(true);
      mGraphTree->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

      connect(mGraphTree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
         this, SLOT(OnItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GraphBrowser::SetDirectorEditor(DirectorEditor* editor)
   {
      mEditor = editor;
   }

   //////////////////////////////////////////////////////////////////////////
   void GraphBrowser::BuildGraphList(DirectorGraph* selected)
   {
      if (!mEditor) return;

      Director* director = mEditor->GetDirector();
      if (!director) return;

      DirectorGraph* rootGraph = director->GetGraphRoot();
      if (!rootGraph) return;

      mGraphTree->blockSignals(true);

      mGraphTree->clear();

      GraphItem* rootItem = new GraphItem(rootGraph, NULL);
      mGraphTree->addTopLevelItem(rootItem);
      rootItem->setExpanded(true);

      AddGraphItem(rootGraph, rootItem, selected);

      if (rootGraph == selected)
      {
         mGraphTree->setCurrentItem(rootItem);
      }

      mGraphTree->blockSignals(false);
   }

   //////////////////////////////////////////////////////////////////////////
   void GraphBrowser::SelectGraph(DirectorGraph* selected)
   {
      GraphItem* rootItem = dynamic_cast<GraphItem*>(mGraphTree->topLevelItem(0));
      if (!rootItem) return;

      mGraphTree->blockSignals(true);

      // If we have already found our selected item, select it and finish.
      if (rootItem->GetGraph() == selected)
      {
         mGraphTree->setCurrentItem(rootItem);
         rootItem->setExpanded(true);
      }
      else
      {
         // Recurse through our children items for the selected graph.
         SelectGraph(rootItem, selected);
      }

      mGraphTree->blockSignals(false);
   }

   //////////////////////////////////////////////////////////////////////////
   void GraphBrowser::OnItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
   {
      GraphItem* item = dynamic_cast<GraphItem*>(current);
      if (item)
      {
         DirectorGraph* graph = item->GetGraph();
         if (graph)
         {
            mEditor->OpenGraph(graph);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool GraphBrowser::AddGraphItem(DirectorGraph* graph, GraphItem* parent, DirectorGraph* selected)
   {
      if (!graph) return false;
      if (!parent) return false;

      bool hasSelected = false;

      int count = graph->GetSubGraphs().size();
      for (int index = 0; index < count; index++)
      {
         DirectorGraph* child = graph->GetSubGraphs()[index].get();
         GraphItem* item = new GraphItem(child, parent);
         if (AddGraphItem(child, item, selected))
         {
            hasSelected = true;
         }

         // If this child is the selected child, set it as the selected item
         // and expand all parents.
         if (child == selected)
         {
            item->setSelected(true);
            item->setExpanded(true);
            hasSelected = true;
         }
      }

      // If this graph item contains the selected item, make sure
      // it is expanded.
      if (hasSelected)
      {
         parent->setExpanded(true);
      }

      return hasSelected;
   }

   //////////////////////////////////////////////////////////////////////////
   bool GraphBrowser::SelectGraph(QTreeWidgetItem* parent, DirectorGraph* selected)
   {
      if (!parent) return false;
      if (!selected) return false;

      int count = parent->childCount();
      for (int index = 0; index < count; index++)
      {
         GraphItem* item = dynamic_cast<GraphItem*>(parent->child(index));
         if (item)
         {
            if (item->GetGraph() == selected)
            {
               mGraphTree->setCurrentItem(item);
               item->setExpanded(true);
               return true;
            }

            // If any children of this item contains the selected, then expand this item.
            if (SelectGraph(item, selected))
            {
               item->setExpanded(true);
               return true;
            }
         }
      }

      return false;
   }
}

//////////////////////////////////////////////////////////////////////////
