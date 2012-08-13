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
#include <dtDirectorQt/nodetabs.h>
#include <dtDirectorQt/nodeitem.h>

#include <dtDirector/directorgraph.h>
#include <dtDirector/nodemanager.h>

#include <dtDirectorQt/nodescene.h>
#include <dtDirectorQt/directoreditor.h>

#include <QtGui/QGraphicsView>
#include <QtGui/QLayout>


namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   NodeTabs::NodeTabs(QWidget* parent)
      : QToolBox(parent)
      , mIsEmpty(true)
      , mpEditor(NULL)
      , mpGraph(NULL)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool NodeTabs::IsEmpty() const
   {
      return mIsEmpty;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NodeTabs::SetEditor(DirectorEditor* editor, dtDirector::Director* nodeSceneDirector)
   {
      mpEditor = editor;

      if (nodeSceneDirector)
      {
         mpDirector = nodeSceneDirector;
         mpGraph = new DirectorGraph(nodeSceneDirector);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NodeTabs::RefreshNodes(NodeType::NodeTypeEnum nodeType)
   {
      mIsEmpty = true;

      // Clear out any previous items and re-add translation item
      for (int toolIndex = count() - 1; toolIndex >= 0; --toolIndex)
      {
         QWidget* item = widget(toolIndex);
         removeItem(toolIndex);
         delete item;
      }

      if (!mpEditor->GetDirector())
      {
         return;
      }

      // In the case of showing Macro nodes, make sure we show our
      // standard Macro along with custom editor macros.
      if (nodeType == NodeType::MACRO_NODE)
      {
         // Make sure the base category exists.
         NodeScene* scene = NULL;
         int tabCount = count();
         for (int tabIndex = 0; tabIndex < tabCount; ++tabIndex)
         {
            if (itemText(tabIndex).toStdString() == "Base")
            {
               QGraphicsView* view = dynamic_cast<QGraphicsView*>(widget(tabIndex));
               if (view)
               {
                  scene = dynamic_cast<NodeScene*>(view->scene());
               }
               break;
            }
         }

         if (!scene)
         {
            scene = new NodeScene(mpEditor, mpGraph);
            QGraphicsView* view = new QGraphicsView(scene);
            connect(scene, SIGNAL(CreateNode(const QString&, const QString&, const QString&)),
               this, SIGNAL(CreateNode(const QString&, const QString&, const QString&)));
            view->setScene(scene);

            insertItem(0, view, "Base");
            setCurrentIndex(0);
         }

         if (scene)
         {
            // Add our default empty macro.
            scene->CreateMacro("");

            std::vector<std::string> tools = mpEditor->GetRegisteredToolList(mpEditor->GetDirector());
            int count = (int)tools.size();
            for (int index = 0; index < count; ++index)
            {
               const std::string& tool = tools[index];
               scene->CreateMacro(tool);
               mIsEmpty = false;
            }
         }
      }

      std::vector<const NodeType*> nodes;
      NodeManager::GetInstance().GetNodeTypes(nodes);

      int nodeCount = (int)nodes.size();
      for (int index = 0; index < nodeCount; index++)
      {
         const NodeType* node = nodes[index];

         if (node)
         {
            // Make sure the node we found is a type valid for this script.
            NodePluginRegistry* reg = NodeManager::GetInstance().GetRegistryForType(*node);
            if (!reg || !mpEditor->GetDirector()->HasLibrary(reg->GetName()))
            {
               continue;
            }

            if (node->GetNodeType() == nodeType)
            {
               // Make sure the category tab exists.
               NodeScene* scene = NULL;
               int tabCount = count();
               for (int tabIndex = 0; tabIndex < tabCount; ++tabIndex)
               {
                  if (itemText(tabIndex).toStdString() == node->GetFolder())
                  {
                     QGraphicsView* view = dynamic_cast<QGraphicsView*>(widget(tabIndex));
                     if (view)
                     {
                        scene = dynamic_cast<NodeScene*>(view->scene());
                     }
                     break;
                  }
               }

               if (!scene)
               {
                  scene = new NodeScene(mpEditor, mpGraph);
                  QGraphicsView* view = new QGraphicsView(scene);
                  connect(scene, SIGNAL(CreateNode(const QString&, const QString&, const QString&)),
                     this, SIGNAL(CreateNode(const QString&, const QString&, const QString&)));
                  view->setScene(scene);

                  if (node->GetFolder() == "Base")
                  {
                     insertItem(0, view, node->GetFolder().c_str());
                     setCurrentIndex(0);
                  }
                  else
                  {
                     addItem(view, node->GetFolder().c_str());
                  }
               }

               if (scene)
               {
                  scene->CreateNode(nodeType, node->GetName(), node->GetCategory());
                  mIsEmpty = false;
               }
            }
         }
      }

      NodeScene* scene = NULL;
      int tabCount = count();
      for (int tabIndex = 0; tabIndex < tabCount; ++tabIndex)
      {
         QGraphicsView* view = dynamic_cast<QGraphicsView*>(widget(tabIndex));
         if (view)
         {
            scene = dynamic_cast<NodeScene*>(view->scene());
            if (scene)
            {
               scene->CenterNodes(view);
            }
         }
      }

      // If no nodes exists, we need to at least have one scene for it to
      // render properly.
      // Make sure the base category exists.
      if (tabCount == 0)
      {
         scene = new NodeScene(mpEditor, mpGraph);
         QGraphicsView* view = new QGraphicsView(scene);
         connect(scene, SIGNAL(CreateNode(const QString&, const QString&, const QString&)),
            this, SIGNAL(CreateNode(const QString&, const QString&, const QString&)));
         view->setScene(scene);

         insertItem(0, view, "Base");
         setCurrentIndex(0);
      }

      layout()->setSpacing(0);
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeTabs::SearchNodes(const QString& searchText, DirectorGraph* graph)
   {
      NodeScene* scene = NULL;
      QGraphicsView* view = dynamic_cast<QGraphicsView*>(widget(0));
      if (!view)
      {
         QWidget* item = widget(0);
         if (item)
         {
            removeItem(0);
            delete item;
         }

         scene = new NodeScene(mpEditor, mpGraph);
         view = new QGraphicsView(scene);
         connect(scene, SIGNAL(CreateNode(const QString&, const QString&, const QString&)),
            this, SIGNAL(CreateNode(const QString&, const QString&, const QString&)));
         view->setScene(scene);

         insertItem(0, view, "Search");
         setCurrentIndex(0);
      }

      if (view)
      {
         scene = dynamic_cast<NodeScene*>(view->scene());
      }

      if (!scene)
      {
         return;
      }

      scene->Clear();

      if (searchText.isEmpty())
      {
         return;
      }

      std::vector<const NodeType*> nodes;
      NodeManager::GetInstance().GetNodeTypes(nodes);

      int nodeCount = (int)nodes.size();
      for (int index = 0; index < nodeCount; index++)
      {
         const NodeType* node = nodes[index];

         if (node)
         {
            // Make sure the node we found is a type valid for this script.
            NodePluginRegistry* reg = NodeManager::GetInstance().GetRegistryForType(*node);
            if (!reg || !mpEditor->GetDirector()->HasLibrary(reg->GetName()))
            {
               continue;
            }

            QString nodeTypeName = node->GetFullName().c_str();
            if (nodeTypeName.contains(searchText, Qt::CaseInsensitive))
            {
               scene->CreateNode(node->GetNodeType(), node->GetName(), node->GetCategory());
               mIsEmpty = false;
            }
         }
      }

      // Also search for reference values.
      std::map<std::string, dtDirector::ValueNode*> refMap;
      DirectorGraph* searchGraph = graph;
      while (searchGraph)
      {
         std::vector<dtCore::RefPtr<ValueNode> >& values = searchGraph->GetValueNodes();
         int count = (int)values.size();
         for (int index = 0; index < count; ++index)
         {
            ValueNode* node = values[index];
            QString valueName = node->GetName().c_str();
            if (node && !valueName.isEmpty() && valueName.contains(searchText, Qt::CaseInsensitive))
            {
               std::map<std::string, dtDirector::ValueNode*>::iterator iter = refMap.find(node->GetName());
               if (iter == refMap.end())
               {
                  refMap[node->GetName()] = node;
               }
            }
         }

         searchGraph = searchGraph->GetParent();
      }

      std::map<std::string, dtDirector::ValueNode*>::iterator iter;
      for (iter = refMap.begin(); iter != refMap.end(); ++iter)
      {
         std::string refName = iter->first;
         ValueNode* node = iter->second;
         NodeItem* refItem = scene->CreateNode(dtDirector::NodeType::VALUE_NODE, node->GetType().GetName(), node->GetType().GetCategory());
         if (refItem && refItem->GetNode())
         {
            refItem->setData(Qt::UserRole + 0, QString::fromStdString("Reference"));
            refItem->setData(Qt::UserRole + 1, QString::fromStdString("Core"));
            refItem->setData(Qt::UserRole + 2, QString::fromStdString(refName));
            refItem->GetNode()->SetName(refName);
         }
      }

      scene->CenterNodes(view);

      layout()->setSpacing(0);
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeTabs::SearchReferenceNodes(DirectorGraph* graph)
   {
      NodeScene* scene = NULL;
      QGraphicsView* view = dynamic_cast<QGraphicsView*>(widget(0));
      if (!view)
      {
         QWidget* item = widget(0);
         if (item)
         {
            removeItem(0);
            delete item;
         }

         scene = new NodeScene(mpEditor, mpGraph);
         view = new QGraphicsView(scene);
         connect(scene, SIGNAL(CreateNode(const QString&, const QString&, const QString&)),
            this, SIGNAL(CreateNode(const QString&, const QString&, const QString&)));
         view->setScene(scene);

         insertItem(0, view, "Search");
         setCurrentIndex(0);
      }

      if (view)
      {
         scene = dynamic_cast<NodeScene*>(view->scene());
      }

      if (!scene)
      {
         return;
      }

      scene->Clear();

      std::map<std::string, dtDirector::ValueNode*> refMap;
      DirectorGraph* searchGraph = graph;
      while (searchGraph)
      {
         std::vector<dtCore::RefPtr<ValueNode> >& values = searchGraph->GetValueNodes();
         int count = (int)values.size();
         for (int index = 0; index < count; ++index)
         {
            ValueNode* node = values[index];
            if (node && !node->GetName().empty())
            {
               std::map<std::string, dtDirector::ValueNode*>::iterator iter = refMap.find(node->GetName());
               if (iter == refMap.end())
               {
                  refMap[node->GetName()] = node;
               }
            }
         }

         searchGraph = searchGraph->GetParent();
      }

      std::map<std::string, dtDirector::ValueNode*>::iterator iter;
      for (iter = refMap.begin(); iter != refMap.end(); ++iter)
      {
         std::string refName = iter->first;
         ValueNode* node = iter->second;
         NodeItem* refItem = scene->CreateNode(dtDirector::NodeType::VALUE_NODE, node->GetType().GetName(), node->GetType().GetCategory());
         if (refItem && refItem->GetNode())
         {
            refItem->setData(Qt::UserRole + 0, QString::fromStdString("Reference"));
            refItem->setData(Qt::UserRole + 1, QString::fromStdString("Core"));
            refItem->setData(Qt::UserRole + 2, QString::fromStdString(refName));
            refItem->GetNode()->SetName(refName);
         }
      }

      scene->CenterNodes(view);

      layout()->setSpacing(0);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NodeTabs::AddCustomEditor(const std::string& editor)
   {
      if (mpEditor==NULL || mpGraph==NULL)
      {
         return;
      }
      
      // Make sure the category tab exists.
      NodeScene* scene = NULL;
      QGraphicsView* view = NULL;

      int tabCount = count();
      for (int tabIndex = 0; tabIndex < tabCount; ++tabIndex)
      {
         if (itemText(tabIndex).toStdString() == "Base")
         {
            view = dynamic_cast<QGraphicsView*>(widget(tabIndex));
            if (view)
            {
               scene = dynamic_cast<NodeScene*>(view->scene());
            }
            break;
         }
      }

      if (!scene)
      {
         scene = new NodeScene(mpEditor, mpGraph);
         view = new QGraphicsView(scene);
         connect(scene, SIGNAL(CreateNode(const QString&, const QString&, const QString&)),
            this, SIGNAL(CreateNode(const QString&, const QString&, const QString&)));
         view->setScene(scene);

         insertItem(0, view, "Base");
         setCurrentIndex(0);
      }

      if (scene)
      {
         scene->CreateMacro(editor);
         mIsEmpty = false;
      }

      scene->CenterNodes(view);

      layout()->setSpacing(0);
   }

} // namespace dtDirector

//////////////////////////////////////////////////////////////////////////
