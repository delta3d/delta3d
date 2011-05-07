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

#include <dtDirectorQt/nodetabs.h>

#include <dtDirector/directorgraph.h>
#include <dtDirector/nodemanager.h>

#include <dtDirectorQt/nodescene.h>
#include <dtDirectorQt/directoreditor.h>

#include <QtGui/QGraphicsView>


namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   NodeTabs::NodeTabs(QWidget* parent)
      : QToolBox(parent)
      , mpEditor(NULL)
      , mpGraph(NULL)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NodeTabs::SetEditor(DirectorEditor* editor)
   {
      mpEditor = editor;
      mpGraph = new DirectorGraph(editor->GetDirector());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NodeTabs::RefreshNodes(NodeType::NodeTypeEnum nodeType)
   {
      // Clear out any previous items and re-add translation item
      for (int toolIndex = count() - 1; toolIndex >= 0; --toolIndex)
      {
         QWidget* item = widget(toolIndex);
         removeItem(toolIndex);
         delete item;
      }

      // In the case of showing Macro nodes, make sure we show our
      // standard Macro along with custom editor macros.
      if (nodeType == NodeType::MACRO_NODE)
      {
         // Make sure the category tab exists.
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
            connect(scene, SIGNAL(CreateNode(const QString&, const QString&)),
               this, SIGNAL(CreateNode(const QString&, const QString&)));
            view->setScene(scene);

            insertItem(0, view, "Base");
            setCurrentIndex(0);
         }

         if (scene)
         {
            // Add our default empty macro.
            scene->CreateMacro("");
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
                  connect(scene, SIGNAL(CreateNode(const QString&, const QString&)),
                     this, SIGNAL(CreateNode(const QString&, const QString&)));
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
               }
            }
         }
      }

      layout()->setSpacing(0);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NodeTabs::AddCustomEditor(const std::string& editor)
   {
      // Make sure the category tab exists.
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
         connect(scene, SIGNAL(CreateNode(const QString&, const QString&)),
            this, SIGNAL(CreateNode(const QString&, const QString&)));
         view->setScene(scene);

         insertItem(0, view, "Base");
         setCurrentIndex(0);
      }

      if (scene)
      {
         scene->CreateMacro(editor);
      }

      layout()->setSpacing(0);
   }

} // namespace dtDirector

//////////////////////////////////////////////////////////////////////////
