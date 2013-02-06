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
#include <dtDirectorQt/searchbrowser.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/editorscene.h>
#include <dtDirectorQt/propertyeditor.h>
#include <dtDirectorQt/nodeitem.h>

#include <dtDirector/node.h>

#include <dtUtil/stringutils.h>

#include <QtGui/QWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QGridLayout>
#include <QtGui/QTreeWidget>
#include <QtGui/QLineEdit>
#include <QtGui/QToolButton>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   SearchMacroItem::SearchMacroItem(DirectorGraph* graph, const QString& descText)
      : QTreeWidgetItem()
      , mGraph(graph)
   {
      if (graph)
      {
         QString text = "Graph: ";
         text += graph->GetName().c_str();
         setText(0, text);
         setText(1, descText);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   SearchItem::SearchItem(Node* node, const QString& descText)
      : QTreeWidgetItem()
      , mNode(node)
   {
      if (node)
      {
         QString text = node->GetTypeName().c_str();
         text += ": ";
         text += node->GetName().c_str();
         setText(0, text);
         setText(1, descText);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   SearchBrowser::SearchBrowser(QMainWindow* parent)
      : QDockWidget(parent)
      , mGroupBox(NULL)
      , mSearchEdit(NULL)
      , mSearchButton(NULL)
      , mNodeTree(NULL)
   {
      setWindowTitle("Search Browser");

      QWidget* mainAreaWidget = new QWidget(this);
      QGridLayout* mainAreaLayout = new QGridLayout(mainAreaWidget);
      setWidget(mainAreaWidget);

      mGroupBox = new QGroupBox("Search", mainAreaWidget);
      mGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      mainAreaLayout->addWidget(mGroupBox, 0, 0);
      QGridLayout* innerLayout = new QGridLayout(mGroupBox);

      mSearchEdit = new QLineEdit(mGroupBox);
      innerLayout->addWidget(mSearchEdit, 1, 1, 1, 1);

      mSearchButton = new QToolButton(mGroupBox);
      mSearchButton->setText("Search");
      innerLayout->addWidget(mSearchButton, 1, 2, 1, 1);

      mNodeTree = new QTreeWidget(mGroupBox);
      innerLayout->addWidget(mNodeTree, 2, 1, 1, 2);
      mNodeTree->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
      mNodeTree->setExpandsOnDoubleClick(false);
      mNodeTree->setColumnCount(2);
      QStringList headerLabels;
      headerLabels.push_back("Node");
      headerLabels.push_back("Description");
      mNodeTree->setHeaderLabels(headerLabels);
      mNodeTree->setColumnWidth(0, 250);

      connect(mSearchEdit, SIGNAL(returnPressed()), this, SLOT(OnSearch()));
      connect(mSearchButton, SIGNAL(clicked()), this, SLOT(OnSearch()));
      connect(mNodeTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
         this, SLOT(OnFocusItem(QTreeWidgetItem*, int)));
      connect(mNodeTree, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
         this, SLOT(OnFocusItem(QTreeWidgetItem*, int)));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SearchBrowser::SetDirectorEditor(DirectorEditor* editor)
   {
      mEditor = editor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SearchBrowser::FocusSearch()
   {
      mSearchEdit->setFocus();
      mSearchEdit->selectAll();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool SearchBrowser::HasSearchFocus() const
   {
      if (mSearchEdit)
      {
         return mSearchEdit->hasFocus();
      }
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void SearchBrowser::OnSearch()
   {
      if (!mEditor)
      {
         return;
      }

      mNodeTree->clear();

      QString searchText = mSearchEdit->text();

      Director* script = mEditor->GetDirector();
      if (script)
      {
         DirectorGraph* graph = script->GetGraphRoot();
         SearchGraph(graph, searchText, true, NULL);
      }

      mNodeTree->expandAll();
   }

   //////////////////////////////////////////////////////////////////////////
   void SearchBrowser::OnFocusItem(QTreeWidgetItem* item, int column)
   {
      SearchItem* searchItem = dynamic_cast<SearchItem*>(item);
      if (searchItem)
      {
         Node* node = searchItem->GetNode();
         if (node)
         {
            mEditor->FocusNode(node);
            return;
         }
      }

      SearchMacroItem* macroItem = dynamic_cast<SearchMacroItem*>(item);
      if (macroItem)
      {
         DirectorGraph* graph = macroItem->GetGraph();
         if (graph)
         {
            mEditor->FocusGraph(graph);
            return;
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool SearchBrowser::SearchGraph(DirectorGraph* graph, const QString& searchText, bool searchImportedScripts, QTreeWidgetItem* parent)
   {
      bool result = false;
      if (!graph)
      {
         return result;
      }

      // Recurse through each child graph.
      int count = (int)graph->GetSubGraphs().size();
      for (int index = 0; index < count; ++index)
      {
         DirectorGraph* subGraph = graph->GetSubGraphs()[index];

         if (subGraph)
         {
            QString descText;
            bool addMacro = TestItem(subGraph, searchText, descText);

            SearchMacroItem* item = new SearchMacroItem(subGraph, descText);
            if (item)
            {
               addMacro |= SearchGraph(subGraph, searchText, searchImportedScripts, item);

               if (addMacro)
               {
                  result = true;
                  if (parent)
                  {
                     parent->addChild(item);
                  }
                  else
                  {
                     mNodeTree->addTopLevelItem(item);
                  }
               }
            }
         }
      }

      // Add any found nodes to the list.
      std::vector<Node*> nodes;
      graph->GetAllNodes(nodes, false, searchImportedScripts);

      QString descText;

      count = (int)nodes.size();
      for (int index = 0; index < count; ++index)
      {
         Node* node = nodes[index];
         if (node)
         {
            // Do a test to find if this is a valid node.
            if (TestItem(node, searchText, descText))
            {
               QTreeWidgetItem* item = new SearchItem(node, descText);

               if (parent)
               {
                  parent->addChild(item);
               }
               else
               {
                  mNodeTree->addTopLevelItem(item);
               }

               result = true;
            }
         }
      }

      return result;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool SearchBrowser::TestItem(dtCore::PropertyContainer* container, const QString& searchText, QString& outDesc)
   {
      if (!container)
      {
         return false;
      }

      // An empty search will retrieve all nodes.
      if (searchText.isEmpty())
      {
         return true;
      }

      std::vector<dtCore::ActorProperty*> propList;
      container->GetPropertyList(propList);

      int count = (int)propList.size();
      for (int index = 0; index < count; ++index)
      {
         dtCore::ActorProperty* prop = propList[index];
         if (prop)
         {
            QString nameText = prop->GetName().c_str();
            QString dataText = prop->ToString().c_str();

            if (nameText.contains(searchText, Qt::CaseInsensitive) ||
                dataText.contains(searchText, Qt::CaseInsensitive))
            {
               outDesc = nameText;
               outDesc += " = ";
               outDesc += dataText;
               return true;
            }
         }
      }

      return false;
   }
}

//////////////////////////////////////////////////////////////////////////
