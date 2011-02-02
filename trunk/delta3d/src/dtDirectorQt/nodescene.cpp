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
 * Author: Eric R. Heine
 */

#include <dtDirectorQt/nodescene.h>

#include <dtDirector/directorgraph.h>
#include <dtDirector/nodemanager.h>
#include <dtDirectorQt/actionitem.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/groupitem.h>
#include <dtDirectorQt/macroitem.h>
#include <dtDirectorQt/scriptitem.h>
#include <dtDirectorQt/valueitem.h>

#include <QtGui/QDrag>
#include <QtCore/QMimeData>

////////////////////////////////////////////////////////////////////////////////
static const float NODE_BUFFER = 25.0f;

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   NodeScene::NodeScene(DirectorEditor* parent)
      : QGraphicsScene(parent)
      , mpEditor(parent)
      , mpDraggedItem(NULL)
   {
      setBackgroundBrush(Qt::lightGray);

      mpGraph = new DirectorGraph(parent->GetDirector());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NodeScene::RefreshNodes(NodeType::NodeTypeEnum nodeType)
   {
      // Clear out any previous items and re-add translation item
      clear();
      mpTranslationItem = new QGraphicsRectItem(NULL, this);

      std::vector<const NodeType*> nodes;
      NodeManager::GetInstance().GetNodeTypes(nodes);

      int count = (int)nodes.size();
      float nodeY = 0.0f;
      float maxWidth = 0.0f;
      for (int index = 0; index < count; index++)
      {
         const NodeType* node = nodes[index];

         if (node)
         {
            // Make sure the node we found is a type valid for this script.
            NodePluginRegistry* reg = NodeManager::GetInstance().GetRegistryForType(*node);
            if (!reg || (reg->GetName() != "dtDirectorNodes" && !mpEditor->GetDirector()->HasLibrary(reg->GetName())))
            {
               continue;
            }

            // Find the primary folder to map the node to.
            std::map<std::string, QMenu*>* folderMap = NULL;

            if (node->GetNodeType() == nodeType)
            {
               CreateNode(nodeType, node->GetName(), node->GetCategory(), NODE_BUFFER, nodeY, maxWidth);
               nodeY += NODE_BUFFER;
            }
         }
      }
      QRectF sceneBounds = sceneRect();
      sceneBounds.setHeight(nodeY + NODE_BUFFER);
      sceneBounds.setWidth(maxWidth + NODE_BUFFER);
      setSceneRect(sceneBounds);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NodeScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NodeScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent)
   {
      QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
      QPointF scenePos = mouseEvent->scenePos() - mpTranslationItem->scenePos();
      NodeItem* selectedItem = GetNodeItemAtPos(scenePos);
      if (selectedItem != NULL)
      {
         QString name = selectedItem->QGraphicsItem::data(Qt::UserRole).toString();
         QString category = selectedItem->QGraphicsItem::data(Qt::UserRole + 1).toString();
         emit CreateNode(name, category);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NodeScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
   {
      QPointF scenePos = mouseEvent->scenePos() - mpTranslationItem->scenePos();
      mpDraggedItem = GetNodeItemAtPos(scenePos);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NodeScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
   {
      if (mpDraggedItem != NULL)
      {
         QDrag* drag = new QDrag(mouseEvent->widget());
         QMimeData* mime = new QMimeData;
         drag->setMimeData(mime);
         QVariant name = mpDraggedItem->QGraphicsItem::data(Qt::UserRole);
         QVariant category = mpDraggedItem->QGraphicsItem::data(Qt::UserRole + 1);
         mime->setData("Name", name.toByteArray());
         mime->setData("Category", category.toByteArray());
         drag->exec();
         mpDraggedItem = NULL;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NodeScene::CreateNode(NodeType::NodeTypeEnum nodeType, const std::string& name,
      const std::string& category, float x, float& y, float& maxWidth)
   {
      dtCore::RefPtr<Node> node = NodeManager::GetInstance().CreateNode(name, category, mpGraph);
      if (node.valid())
      {
         NodeItem* item = NULL;
         node->SetPosition(osg::Vec2(x, y));
         switch (nodeType)
         {
            case NodeType::VALUE_NODE:
            {
               item = new ValueItem(node, mpTranslationItem, NULL);
               break;
            }
            case NodeType::MACRO_NODE:
            {
               if (node->GetType().GetFullName() == "Core.Reference Script")
               {
                  item = new ScriptItem(node, mpTranslationItem, NULL);
               }
               else
               {
                  item = new MacroItem(NULL, mpTranslationItem, NULL);
               }
               break;
            }
            case NodeType::MISC_NODE:
            {
               if (node->GetType().GetFullName() == "Core.Group Box")
               {
                  item = new GroupItem(node, mpTranslationItem, NULL, false);
                  break;
               }
            }
            default:
            {
               item = new ActionItem(node, mpTranslationItem, NULL);
               break;
            }
         }

         if (item != NULL)
         {
            item->setFlag(QGraphicsItem::ItemIsMovable, false);
            item->setFlag(QGraphicsItem::ItemIsSelectable, false);
            item->setAcceptedMouseButtons(Qt::NoButton);
            item->setData(Qt::UserRole, QString::fromStdString(name));
            item->setData(Qt::UserRole + 1, QString::fromStdString(category));

#if(QT_VERSION >= 0x00040600)
            item->setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
#endif
            item->Draw();
            y += item->boundingRect().height();
            if (item->boundingRect().width() > maxWidth)
            {
               maxWidth = item->boundingRect().width();
            }
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   NodeItem* NodeScene::GetNodeItemAtPos(const QPointF& pos)
   {
      QList<QGraphicsItem*> nodes = items(pos);
      int count = (int)nodes.size();
      for (int index = 0; index < count; index++)
      {
         NodeItem* nodeItem = dynamic_cast<NodeItem*>(nodes[index]);
         if (nodeItem != NULL)
         {
            return nodeItem;
         }
      }

      return NULL;
   }

} // namespace dtDirector

//////////////////////////////////////////////////////////////////////////
