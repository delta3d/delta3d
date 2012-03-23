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
#include <dtDirectorQt/valueitem.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/editorscene.h>
#include <dtDirectorQt/linkitem.h>
#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undopropertyevent.h>
#include <dtDirectorQt/undocreateevent.h>
#include <dtDirectorQt/editorview.h>

#include <dtDirector/valuenode.h>

#include <QtGui/QGraphicsScene>
#include <QtGui/QInputDialog>
#include <QtGui/QMenu>

#include <dtCore/datatype.h>

#include <dtUtil/mathdefines.h>

#include <osg/Vec2>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   ValueItem::ValueItem(Node* node, bool readOnly, bool imported, QGraphicsItem* parent, EditorScene* scene)
       : NodeItem(node, readOnly, imported, parent, scene)
       , mValueText(NULL)
       , mValueLink(NULL)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueItem::Draw()
   {
      NodeItem::Draw();

      if (!mNode.valid())
      {
         return;
      }

      mLoading = true;

      ValueNode *valueNode = mNode->AsValueNode();
      if (valueNode)
      {
         SetTitle(GetNodeTitle());
         if (mNode->ValuesExposed()) SetupValues();
         SetValueText(valueNode->GetValueLabel());

         DrawTitle();
         DrawValues();

         DrawPolygonTop();
         DrawPolygonRightFlat();
         if (mNode->ValuesExposed()) DrawPolygonBottomFlat();
         else                        DrawPolygonBottomRound();
         DrawPolygonLeftFlat();

         int size = mNodeWidth;
         if (size < mNodeHeight) size = mNodeHeight;

         // add height of bottom-round
         mNodeHeight += mNodeWidth / 2;

         if (!mValueLink)
         {
            mValueLink = new ValueNodeLinkItem(this, this, mScene);
         }

         if (mValueLink)
         {
            QPolygonF poly;
            poly << QPointF(-LINK_SIZE/2, 0.0f) << QPointF(LINK_SIZE/2, 0.0f) <<
               QPointF(LINK_SIZE/2, -LINK_LENGTH + LINK_SIZE/2) <<
               QPointF(0, -LINK_LENGTH) <<
               QPointF(-LINK_SIZE/2, -LINK_LENGTH + LINK_SIZE/2);

            mValueLink->setPolygon(poly);
            mValueLink->setPos(mNodeWidth / 2, 0.0f);

            mValueLink->SetPropertyType(valueNode->GetPropertyType().GetTypeId());
            mValueLink->setPen(Qt::NoPen);
            mValueLink->setBrush(GetNodeColor());
         }

         setPolygon(mPolygon);

         osg::Vec2 pos = mNode->GetPosition();
         setPos(pos.x(), pos.y());

         SetComment(mNode->GetComment());

         SetDefaultPen();
         SetBackgroundGradient();

         DrawGlow();
      }

      mLoading = false;
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueItem::SetTitle(std::string text)
   {
      if (!mTitle)
      {
         mTitle = new GraphicsTextItem(this, scene());
         mTitle->setTextWidth(MIN_NODE_WIDTH);

         if (mIsReadOnly)
         {
            QFont font = mTitle->font();
            font = QFont(font.family(), font.pointSize(), font.weight(), false);
            mTitle->setFont(font);
            mTitle->setDefaultTextColor(Qt::darkGray);
         }

#if(QT_VERSION >= 0x00040500)
         mTitle->setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);
#endif
      }

      mTitle->setHtml((std::string("<center>") + text + "</center>").c_str());

      // Create the title background.
      QRectF bounds = mTitle->boundingRect();
      mTitleHeight = bounds.height();

      if (mNodeHeight < int(mTitleHeight)) mNodeHeight = int(mTitleHeight);

      // Clamp the bounds to our min and max.
      if (bounds.width() > MAX_NODE_WIDTH - 2) bounds.setWidth(MAX_NODE_WIDTH - 2);
      if (bounds.width() < MIN_NODE_WIDTH - 2) bounds.setWidth(MIN_NODE_WIDTH - 2);

      // Resize the width of the node if it is not wide enough already.
      if (bounds.width() + 2 > mNodeWidth)
      {
         mNodeWidth = int(bounds.width());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ValueItem::DrawTitle()
   {
      if (mTitle)
      {
         mTitle->setTextWidth(mNodeWidth);
      }

      NodeItem::DrawTitle();
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueItem::DrawComment()
   {
      float scale = 1.0f;
      if (mScene)
      {
         scale = 1.0f / mScene->GetView()->GetZoomScale();
      }
      scale = scale * 1.5;

      mComment->setScale(scale);
      mComment->setTextWidth(dtUtil::Max(100, mNodeWidth));

      // Create the title background.
      QRectF nodeBounds = boundingRect();

      mComment->setPos(nodeBounds.x(), nodeBounds.height());
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueItem::SetValueText(const std::string& text)
   {
      if (!mValueText)
      {
         mValueText = new GraphicsTextItem(this, scene());
         mValueText->setTextWidth(MIN_NODE_WIDTH);

         if (mIsReadOnly)
         {
            QFont font = mValueText->font();
            font = QFont(font.family(), font.pointSize(), font.weight(), false);
            mValueText->setFont(font);
            mValueText->setDefaultTextColor(Qt::darkGray);
         }

#if(QT_VERSION >= 0x00040500)
         mValueText->setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);
#endif
      }

      mValueText->setHtml((std::string("<center>") + text + "</center>").c_str());
      mValueText->setTextWidth(mNodeWidth);

      if (text.empty()) return;

      // Create the title background.
      QRectF bounds = mValueText->boundingRect();

      float y = mNodeHeight - bounds.height();
      if (y < mTitleHeight)
      {
         mNodeHeight += int(mTitleHeight - y);
         y = mTitleHeight;
      }

      mValueHeight += bounds.height() + 11;

      mValueText->setPos(0, y + 11);
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueItem::ConnectLinks(bool fullConnect)
   {
      NodeItem::ConnectLinks(fullConnect);

      if (fullConnect && mNode.valid())
      {
         ValueNode* valueNode = mNode->AsValueNode();
         if (valueNode)
         {
            int count = (int)valueNode->GetLinks().size();
            for (int index = 0; index < count; index++)
            {
               ValueLink* link = valueNode->GetLinks()[index];
               if (!link) continue;

               NodeItem* item = mScene->GetNodeItem(link->GetOwner()->GetID());
               if (!item) continue;

               int valueCount = (int)item->GetValues().size();
               for (int valueIndex = 0; valueIndex < valueCount; valueIndex++)
               {
                  ValueData& value = item->GetValues()[valueIndex];
                  if (value.link)
                  {
                     int linkCount = (int)value.link->GetLinks().size();
                     value.ResizeLinks(linkCount, mScene);
                     for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
                     {
                        if (value.link->GetLinks()[linkIndex] == mNode.get())
                        {
                           NodeItem::ConnectLinks(value, this, linkIndex);
                           break;
                        }
                     }
                  }
               }
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueItem::OnGotoReference()
   {
      dtCore::ActorProperty* prop = mNode->GetProperty("Reference");
      if (prop)
      {
         // Find the referenced value node.
         std::string name = prop->ToString();

         // Search for the referenced node anywhere in the current graph
         // or any of its parents.
         Node* refNode = NULL;
         DirectorGraph* graph = mNode->GetGraph();
         while (graph)
         {
            refNode = graph->GetValueNode(name, false, true);
            if (refNode) break;

            graph = graph->GetParent();
         }

         if (refNode)
         {
            // Center the view on the referenced node.
            EditorScene* scene = mScene;
            DirectorGraph* newGraph = NULL;

            if (refNode->GetGraph() == refNode->GetGraph()->GetDirector()->GetGraphRoot())
            {
               newGraph = scene->GetEditor()->GetDirector()->GetGraphRoot();
            }
            else
            {
               ID graphID = refNode->GetGraph()->GetID();
               graphID.index = -1;
               newGraph = scene->GetEditor()->GetDirector()->GetGraph(graphID, false);
            }

            if (newGraph)
            {
               scene->SetGraph(newGraph);
               NodeItem* item = scene->GetNodeItem(refNode->GetID(), true);
               if (item)
               {
                  scene->clearSelection();
                  item->setSelected(true);
                  scene->CenterSelection();
               }
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueItem::OnCreateReference()
   {
      EditorScene* scene = mScene;
      std::string name = mNode->GetName();

      UndoManager* undoManager = scene->GetEditor()->GetUndoManager();
      bool multipleEvents = false;

      // If the value is already a reference, make sure we reference
      // the actual value instead of the reference.
      const NodeType& type = mNode->GetType();
      if (type.GetFullName() == "Core.Reference")
      {
         Node* node = mNode.get();

         // Keep looping until we find an actual value, or no value.
         while (node)
         {
            dtCore::ActorProperty* prop = node->GetProperty("Reference");
            if (prop) name = prop->ToString();

            node = scene->GetEditor()->GetDirector()->GetValueNode(name);
            if (!node || node->GetType() != type)
            {
               break;
            }
         }
      }
      // If the value to be referenced is not named yet, prompt the user
      else if (name.empty())
      {
         QString valueName = QInputDialog::getText(NULL, "Name Value", "Please enter a unique name for the value to be referenced:");
         if (!valueName.isEmpty())
         {
            undoManager->BeginMultipleEvents("Creation of Reference Value Node.");
            multipleEvents = true;

            dtCore::RefPtr<UndoPropertyEvent> event = new UndoPropertyEvent(undoManager->GetEditor(), mNode->GetID(), "Name", "", valueName.toStdString());
            undoManager->AddEvent(event);

            name = valueName.toStdString();
            mNode->SetName(name);
         }
         else
         {
            return;
         }
      }

      osg::Vec2 position = mNode->GetPosition();

      Node* node = scene->CreateNode("Reference", "Core", position.x() + 20.0f, position.y() + 20.0f);
      if (node)
      {
         dtCore::ActorProperty* prop = node->GetProperty("Reference");
         if (prop) prop->FromString(name);

         scene->Refresh();

         // Create an undo event for this creation event.
         if (undoManager)
         {
            dtCore::RefPtr<UndoCreateEvent> event = new UndoCreateEvent(undoManager->GetEditor(), node->GetID(), scene->GetGraph()->GetID());
            event->SetDescription("Creation of Reference Value Node.");
            undoManager->AddEvent(event);
         }
      }

      if (multipleEvents)
      {
         undoManager->EndMultipleEvents();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
   {
      setSelected(true);
      QMenu menu;
      bool hasDefault = mScene->GetEditor()->OnContextValueNode(mNode.get(), menu);

      if (mNode->GetType().GetFullName() == "Core.Reference")
      {
         QAction* gotoRefAction = menu.addAction("Go to Referenced Value");
         connect(gotoRefAction, SIGNAL(triggered()), this, SLOT(OnGotoReference()));
         if (!hasDefault)
         {
            menu.setDefaultAction(gotoRefAction);
            hasDefault = true;
         }
      }
      else if (mNode->GetType().GetFullName() == "Core.Value Link")
      {
         menu.addAction(mScene->GetEditor()->GetParentAction());
         if (!hasDefault)
         {
            menu.setDefaultAction(mScene->GetEditor()->GetParentAction());
            hasDefault = true;
         }
      }

      QAction* refAction = menu.addAction("Create Reference");
      connect(refAction, SIGNAL(triggered()), this, SLOT(OnCreateReference()));
      if (!hasDefault) menu.setDefaultAction(refAction);

      menu.addSeparator();
      menu.addAction(mScene->GetMacroSelectionAction());
      menu.addSeparator();
      menu.addAction(mScene->GetEditor()->GetCutAction());
      menu.addAction(mScene->GetEditor()->GetCopyAction());
      menu.addSeparator();
      menu.addAction(mScene->GetEditor()->GetDeleteAction());
      menu.exec(event->screenPos());
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
   {
      NodeItem::mouseDoubleClickEvent(event);

      if (event->button() == Qt::LeftButton)
      {
         // Check the double click event handler first.
         if (!mScene->GetEditor()->OnDoubleClickValueNode(mNode.get()))
         {
            // If this value is a reference node, jump to its' referenced value.
            if (mNode->GetType().GetFullName() == "Core.Reference")
            {
               OnGotoReference();
            }
            // If this value is a value link node, jump to the parent graph.
            else if (mNode->GetType().GetFullName() == "Core.Value Link")
            {
               mScene->GetEditor()->on_action_Step_Out_Of_Graph_triggered();
            }
            // Anything else creates a new reference.
            else
            {
               OnCreateReference();
            }
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
