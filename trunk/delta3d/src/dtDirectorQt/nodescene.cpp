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
#include <prefix/dtdirectorqtprefix.h>
#include <dtDirectorQt/nodescene.h>

#include <dtDirector/directorgraph.h>
#include <dtDirector/nodemanager.h>
#include <dtDirectorQt/actionitem.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/groupitem.h>
#include <dtDirectorQt/macroitem.h>
#include <dtDirectorQt/scriptitem.h>
#include <dtDirectorQt/valueitem.h>
#include <dtDirector/groupnode.h>
#include <dtDirectorNodes/referencescriptaction.h>

#include <QtGui/QDrag>
#include <QtCore/QMimeData>
#include <QtGui/QPainter>
#include <QtGui/QImage>

#include <QtGui/QGraphicsView>


////////////////////////////////////////////////////////////////////////////////
static const float NODE_BUFFER = 40.0f;

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   NodeScene::NodeScene(DirectorEditor* parent, DirectorGraph* graph)
      : QGraphicsScene(parent)
      , mpEditor(parent)
      , mpGraph(graph)
      , mpDraggedItem(NULL)
      , mHeight(0.0f)
      , mWidth(0.0f)
   {
      setBackgroundBrush(Qt::lightGray);

      mpItem = new QGraphicsRectItem(NULL, this);
   }

   ///////////////////////////////////////////////////////////////////////////////
   NodeItem* NodeScene::CreateNode(NodeType::NodeTypeEnum nodeType, const std::string& name,
      const std::string& category)
   {
      dtCore::RefPtr<Node> node = NodeManager::GetInstance().CreateNode(name, category, mpGraph);
      if (node.valid())
      {
         NodeItem* item = NULL;
         node->SetPosition(osg::Vec2(NODE_BUFFER, mHeight));
         switch (nodeType)
         {
         case NodeType::MUTATOR_NODE:
         case NodeType::VALUE_NODE:
            {
               item = new ValueItem(node, false, mpItem, NULL);
               break;
            }
         case NodeType::MACRO_NODE:
            {
               if (IS_A(node.get(), ReferenceScriptAction*))
               {
                  item = new ScriptItem(node, false, mpItem, NULL);
               }
               else
               {
                  return CreateMacro("");
               }
               break;
            }
         case NodeType::MISC_NODE:
            {
               if (IS_A(node.get(), GroupNode*))
               {
                  item = new GroupItem(node, false, mpItem, NULL, true);
                  break;
               }
            }
         default:
            {
               if (name == "Value Link" && category == "Core")
               {
                  item = new ValueItem(node, false, mpItem, NULL);
                  break;
               }
               item = new ActionItem(node, false, mpItem, NULL);
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
            mHeight += item->boundingRect().height();
            if (item->boundingRect().width() > mWidth)
            {
               mWidth = item->boundingRect().width();
            }

            QRectF sceneBounds = sceneRect();
            sceneBounds.setHeight(mHeight + NODE_BUFFER);
            sceneBounds.setWidth(mWidth + NODE_BUFFER);
            setSceneRect(sceneBounds);

            mHeight += NODE_BUFFER;
         }

         return item;
      }

      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   NodeItem* NodeScene::CreateMacro(const std::string& editor)
   {
      dtCore::RefPtr<DirectorGraph> graph = mpGraph->AddGraph();
      if (graph.valid())
      {
         graph->SetEditor(editor);
         graph->SetPosition(osg::Vec2(NODE_BUFFER, mHeight));

         NodeItem* item = new MacroItem(graph, false, mpItem, NULL);
         if (item != NULL)
         {
            item->setFlag(QGraphicsItem::ItemIsMovable, false);
            item->setFlag(QGraphicsItem::ItemIsSelectable, false);
            item->setAcceptedMouseButtons(Qt::NoButton);
            item->setData(Qt::UserRole, "");
            item->setData(Qt::UserRole + 1, QString::fromStdString(editor));

#if(QT_VERSION >= 0x00040600)
            item->setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
#endif
            item->Draw();
            mHeight += item->boundingRect().height();
            if (item->boundingRect().width() > mWidth)
            {
               mWidth = item->boundingRect().width();
            }

            QRectF sceneBounds = sceneRect();
            sceneBounds.setHeight(mHeight + NODE_BUFFER);
            sceneBounds.setWidth(mWidth + NODE_BUFFER);
            setSceneRect(sceneBounds);

            mHeight += NODE_BUFFER;
         }

         return item;
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeScene::CenterNodes(QGraphicsView* view)
   {
      if (!view)
      {
         return;
      }

      QRect boundRect = view->frameRect();

      if (boundRect.width() < mWidth)
      {
         boundRect.setWidth(mWidth);
      }

      mHeight = NODE_BUFFER;
      float center = boundRect.width() * 0.5f;

      QList<QGraphicsItem*> itemList = mpItem->childItems();
      int count = itemList.count();
      for (int index = 0; index < count; ++index)
      {
         NodeItem* nodeItem = dynamic_cast<NodeItem*>(itemList[index]);
         if (nodeItem)
         {
            QPointF pos = nodeItem->scenePos();
            float width = nodeItem->GetNodeWidth();

            osg::Vec2 newPos = osg::Vec2(center - (width * 0.5f), mHeight);

            Node* node = nodeItem->GetNode();
            if (node)
            {
               node->SetPosition(newPos);
            }
            else if (nodeItem->GetMacro())
            {
               if (nodeItem->GetMacro())
               {
                  nodeItem->GetMacro()->SetPosition(newPos);
               }
            }

            nodeItem->Draw();
            mHeight += nodeItem->boundingRect().height() + NODE_BUFFER;
         }
      }

      boundRect.setHeight(mHeight + NODE_BUFFER + NODE_BUFFER);

      setSceneRect(boundRect);

      view->centerOn(center, 0);
  }

   ///////////////////////////////////////////////////////////////////////////////
   void NodeScene::Clear()
   {
      removeItem(mpItem);

      delete mpItem;

      mpItem = new QGraphicsRectItem(NULL, this);

      mHeight = 0;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NodeScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NodeScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent)
   {
      QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
      QPointF scenePos = mouseEvent->scenePos();
      NodeItem* selectedItem = GetNodeItemAtPos(scenePos);
      if (selectedItem != NULL)
      {
         QString name = selectedItem->QGraphicsItem::data(Qt::UserRole).toString();
         QString category = selectedItem->QGraphicsItem::data(Qt::UserRole + 1).toString();
         QString refName = selectedItem->QGraphicsItem::data(Qt::UserRole + 2).toString();
         emit CreateNode(name, category, refName);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NodeScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
   {
      QPointF scenePos = mouseEvent->scenePos();
      mpDraggedItem = GetNodeItemAtPos(scenePos);

      if (mpDraggedItem == NULL)
      {
         mouseEvent->ignore();
         return;
      }

      //we'll render the NodeItem into a Pixmap, so there's something to see while
      //it's being dragged.

      //size of the image to cover the item.  The width is padded to account for
      //any additional geometry on the left side.
      int imageWidth = mpDraggedItem->boundingRect().width()+mpDraggedItem->boundingRect().height();
      int imageHeight = mpDraggedItem->boundingRect().height();

      QImage image(imageWidth, imageHeight, QImage::Format_ARGB32_Premultiplied);
      image.fill(qRgba(0, 0, 0, 0));

      QPainter painter;
      painter.begin(&image);
      painter.setBrush(mpDraggedItem->brush());

      //shift to the right to account for some negative geometry
      painter.translate(mpDraggedItem->boundingRect().height()/2, 0);

      painter.drawPolygon(mpDraggedItem->polygon(), Qt::WindingFill);
      painter.end();

      QDrag* drag = new QDrag(mouseEvent->widget());
      QMimeData* mime = new QMimeData;
      drag->setMimeData(mime);
      drag->setPixmap(QPixmap::fromImage(image));

      QPoint hotspot(mouseEvent->scenePos().toPoint() - static_cast<QGraphicsItem*>(mpDraggedItem)->scenePos().toPoint());

      drag->setHotSpot(hotspot + QPoint(mpDraggedItem->boundingRect().height()/2.f, 0.f));

      QVariant name = mpDraggedItem->QGraphicsItem::data(Qt::UserRole);
      QVariant category = mpDraggedItem->QGraphicsItem::data(Qt::UserRole + 1);
      QVariant refName = mpDraggedItem->QGraphicsItem::data(Qt::UserRole + 2);

      QByteArray itemData;
      QDataStream dataStream(&itemData, QIODevice::WriteOnly);
      dataStream <<  name.toString() << category.toString() << refName.toString() << QPoint(hotspot);

      //store the name, category, and hotspot data for the upcoming drop event
      mime->setData("data", itemData);
      drag->exec();
      mpDraggedItem = NULL;
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

   //////////////////////////////////////////////////////////////////////////
   void NodeScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
   {
   }
} // namespace dtDirector

//////////////////////////////////////////////////////////////////////////
