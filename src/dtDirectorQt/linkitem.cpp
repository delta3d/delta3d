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
#include <dtDirectorQt/linkitem.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/editorscene.h>
#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undolinkevent.h>
#include <dtDirectorQt/undolinkvisibilityevent.h>
#include <dtDirectorQt/undocreateevent.h>
#include <dtDirectorQt/undolinkevent.h>
#include <dtDirectorQt/undopropertyevent.h>
#include <dtDirectorQt/editorview.h>
#include <dtDirectorQt/graphtabs.h>

#include <dtDirector/outputlink.h>
#include <dtDirector/nodemanager.h>

#include <dtCore/datatype.h>

#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>


namespace dtDirector
{
   const unsigned int LinkItem::LINE_WIDTH = 2;

   LinkItem::LinkItem(NodeItem* nodeItem, int linkIndex, QGraphicsItem* parent, EditorScene* scene, const std::string& comment): QGraphicsPolygonItem(parent, scene)
      , mDrawing(NULL)
      , mHighlight(NULL)
      , mScene(scene)
      , mLinkIndex(linkIndex)
      , mAltModifier(false)
      , mAlwaysHighlight(false)
      , mNodeItem(nodeItem)
   {
      mHighlightPen  = QPen(Qt::yellow, LINE_WIDTH, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
      mLinkGraphicPen = Qt::NoPen;

      if (nodeItem && nodeItem->flags() & QGraphicsItem::ItemIsSelectable)
      {
         setAcceptHoverEvents(true);

         QWidget::setToolTip(comment.c_str());
         QGraphicsPolygonItem::setToolTip(comment.c_str());
      }
      else
      {
         setAcceptHoverEvents(false);
         setAcceptedMouseButtons(Qt::NoButton);
      }

      if ((mScene) && mScene->GetEditor())
      {
         connect(this, SIGNAL(LinkConnected()), mScene->GetEditor(), SLOT(OnPlayClickSound()));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LinkItem::SetPenColor(const QColor& color)
   {
      mLinkGraphicPen = QPen(color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
   }

   //////////////////////////////////////////////////////////////////////////
   void LinkItem::SetHighlight(bool enable)
   {
      LinkItem* linkGraphic = GetLinkGraphic();
      if( !linkGraphic )
      {
         return;
      }

      if( enable || mAlwaysHighlight )
      {
         //mLinkGraphicPen = Qt::NoPen;//linkGraphic->pen();
         linkGraphic->setPen(mHighlightPen);
      }
      else
      {
         QPen drawPen = mLinkGraphicPen;
         if (mNodeItem->IsImported() || mNodeItem->IsReadOnly())
         {
            QColor color = drawPen.color();
            color.setAlphaF(color.alphaF() * 0.5f);
            drawPen.setColor(color.light(150));
         }

         linkGraphic->setPen(drawPen);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void LinkItem::SetAlwaysHighlight(bool enabled)
   {
      mAlwaysHighlight = enabled;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LinkItem::SetHighlightAlpha(float alpha)
   {
      QColor baseColor = mLinkGraphicPen.color();
      QColor newColor = Qt::yellow;

      newColor.setRedF((newColor.redF() * alpha) + (baseColor.redF() * (1.0f - alpha)));
      newColor.setGreenF((newColor.greenF() * alpha) + (baseColor.greenF() * (1.0f - alpha)));
      newColor.setBlueF((newColor.blueF() * alpha) + (baseColor.blueF() * (1.0f - alpha)));

      mHighlightPen.setColor(newColor);
   }

   //////////////////////////////////////////////////////////////////////////
   void LinkItem::SetHighlightConnector(bool enable, QGraphicsPathItem* connector, bool isReadOnly, bool isRemoved)
   {
      if (enable)
      {
         connector->setPen(mHighlightPen);
      }
      else
      {
         QPen drawPen = mLinkGraphicPen;
         if (isRemoved)
         {
            drawPen.setWidth(1);
            QVector<qreal> dashes;
            dashes << 2 << 6;
            drawPen.setDashPattern(dashes);
         }
         else if (isReadOnly)
         {
            drawPen.setWidth(1);
         }

         connector->setPen(drawPen);
      }
      connector->setZValue(connector->zValue() + (enable ? 10 : -10));
   }

   //////////////////////////////////////////////////////////////////////////
   void LinkItem::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
   {
      if (!mouseEvent) return;

      if (mouseEvent->button() == Qt::XButton1 ||
         mouseEvent->button() == Qt::XButton2)
      {
         return;
      }

      if (mouseEvent->modifiers() == Qt::AltModifier)
      {
         mAltModifier = true;
         return;
      }

      mAltModifier = false;

      // Begin drawing a link.
      if (mHighlight) delete mHighlight;

      mHighlight = new QGraphicsPathItem(NULL, mScene);
      mHighlight->setZValue(40.0f);
      mHighlight->setPen(QPen(Qt::yellow, LINE_WIDTH + 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

      mDrawing = new QGraphicsPathItem(mHighlight, mScene);
      mDrawing->setPen(QPen(Qt::black, LINE_WIDTH, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   InputLinkItem::InputLinkItem(NodeItem* nodeItem, int linkIndex, QGraphicsItem* parent, EditorScene* scene, const std::string& comment)
      : LinkItem(nodeItem, linkIndex, parent, scene, comment)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   LinkItem* InputLinkItem::GetLinkGraphic()
   {
      return this;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InputLinkItem::DrawGlow(float glow)
   {
      if (glow > 0.5f)
      {
         SetAlwaysHighlight(true);
         SetHighlightAlpha(1.0f);
      }
      else if (glow > 0.0f)
      {
         SetAlwaysHighlight(true);
         SetHighlightAlpha(glow * 2.0f);
      }
      else if (glow == 0.0f)
      {
         SetAlwaysHighlight(false);
         SetHighlightAlpha(1.0f);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void InputLinkItem::Disconnect(OutputLink* output)
   {
      InputLink* input = mNodeItem->GetInputs()[mLinkIndex].link;
      if (!input) return;

      std::string undoDescription = "Disconnection of input link(s) for ";
      if (mNodeItem->GetNode())
      {
         undoDescription += "Node \'" + mNodeItem->GetNode()->GetTypeName() + "\'.";
      }
      else if (mNodeItem->GetMacro())
      {
         undoDescription += "Macro Node \'" + mNodeItem->GetMacro()->GetName() + "\'.";
      }

      if (!output)
      {
         if (input->GetLinks().size())
         {
            mScene->GetEditor()->GetUndoManager()->BeginMultipleEvents(undoDescription);

            while (!input->GetLinks().empty())
            {
               output = input->GetLinks()[0];
               Disconnect(output);
            }

            mScene->GetEditor()->GetUndoManager()->EndMultipleEvents();
         }
      }
      else
      {
         if (input->Disconnect(output))
         {
            dtCore::RefPtr<UndoLinkEvent> event = new UndoLinkEvent(
               mScene->GetEditor(),
               UndoLinkEvent::INPUT_LINK,
               input->GetOwner()->GetID(),
               output->GetOwner()->GetID(),
               input->GetName(),
               output->GetName(),
               false);
            event->SetDescription(undoDescription);
            mScene->GetEditor()->GetUndoManager()->AddEvent(event);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void InputLinkItem::Disconnect(QAction* action)
   {
      if (action->text() == "Disconnect All")
      {
         Disconnect();
         mScene->Refresh();
      }
      else
      {
         InputLink* input = mNodeItem->GetInputs()[mLinkIndex].link;
         if (!input) return;
         int count = (int)input->GetLinks().size();
         for (int index = 0; index < count; index++)
         {
            if (input->GetLinks()[index]->GetName() == action->statusTip().toStdString())
            {
               Disconnect(input->GetLinks()[index]);
               mScene->Refresh();
               break;
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InputLinkItem::HideLink()
   {
      InputLink* input = mNodeItem->GetInputs()[mLinkIndex].link;
      if (!input) return;

      // Hide this link.
      input->SetVisible(false);

      std::string undoDescription = "Hiding of input link \'" + input->GetName() + "\' for ";
      if (mNodeItem->GetNode())
      {
         undoDescription += "Node \'" + mNodeItem->GetNode()->GetTypeName() + "\'.";
      }
      else if (mNodeItem->GetMacro())
      {
         if (!mNodeItem->GetMacro()->GetEditor().empty())
         {
            undoDescription += "\'" + mNodeItem->GetMacro()->GetEditor() + "\' ";
         }
         undoDescription += "Macro Node \'" + mNodeItem->GetMacro()->GetName() + "\'.";
      }

      mScene->GetEditor()->GetUndoManager()->BeginMultipleEvents(undoDescription);

      dtCore::RefPtr<UndoLinkVisibilityEvent> event = new UndoLinkVisibilityEvent(mScene->GetEditor(), mNodeItem->GetID(), 0, input->GetName(), false);
      mScene->GetEditor()->GetUndoManager()->AddEvent(event);

      // Disconnect all links.
      Disconnect();

      mScene->GetEditor()->GetUndoManager()->EndMultipleEvents();

      mScene->Refresh();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InputLinkItem::ShowLink()
   {
      InputLink* input = mNodeItem->GetInputs()[mLinkIndex].link;
      if (!input) return;

      // Show this link.
      input->SetVisible(true);

      std::string undoDescription = "Showing of input link \'" + input->GetName() + "\' for ";
      if (mNodeItem->GetNode())
      {
         undoDescription += "Node \'" + mNodeItem->GetNode()->GetTypeName() + "\'.";
      }
      else if (mNodeItem->GetMacro())
      {
         if (!mNodeItem->GetMacro()->GetEditor().empty())
         {
            undoDescription += "\'" + mNodeItem->GetMacro()->GetEditor() + "\' ";
         }
         undoDescription += "Macro Node \'" + mNodeItem->GetMacro()->GetName() + "\'.";
      }

      dtCore::RefPtr<UndoLinkVisibilityEvent> event = new UndoLinkVisibilityEvent(mScene->GetEditor(), mNodeItem->GetID(), 0, input->GetName(), true);
      event->SetDescription(undoDescription);
      mScene->GetEditor()->GetUndoManager()->AddEvent(event);

      mScene->Refresh();
   }

   //////////////////////////////////////////////////////////////////////////
   void InputLinkItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
   {
      SetHighlight(true);

      // Highlight all connected output links.
      if (mLinkIndex >= 0 && mLinkIndex < (int)mNodeItem->GetInputs().size())
      {
         InputData& data = mNodeItem->GetInputs()[mLinkIndex];
         if (data.link)
         {
            int linkCount = (int)data.link->GetLinks().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               OutputLink* output = data.link->GetLinks()[linkIndex];
               if (!output) continue;

               NodeItem* item = mScene->GetNodeItem(output->GetOwner()->GetID());
               if (!item) continue;

               int outputCount = (int)item->GetOutputs().size();
               for (int outputIndex = 0; outputIndex < outputCount; outputIndex++)
               {
                  if (item->GetOutputs()[outputIndex].link == output)
                  {
                     item->GetOutputs()[outputIndex].linkGraphic->SetHighlight(true, data.link);
                     break;
                  }
               }
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void InputLinkItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
   {
      SetHighlight(false);

      // Remove the highlight on all connected output links.
      if (mLinkIndex >= 0 && mLinkIndex < (int)mNodeItem->GetInputs().size())
      {
         InputData& data = mNodeItem->GetInputs()[mLinkIndex];
         if (data.link)
         {
            int linkCount = (int)data.link->GetLinks().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               OutputLink* output = data.link->GetLinks()[linkIndex];
               if (!output) continue;

               NodeItem* item = mScene->GetNodeItem(output->GetOwner()->GetID());
               if (!item) continue;

               int outputCount = (int)item->GetOutputs().size();
               for (int outputIndex = 0; outputIndex < outputCount; outputIndex++)
               {
                  if (item->GetOutputs()[outputIndex].link == output)
                  {
                     item->GetOutputs()[outputIndex].linkGraphic->SetHighlight(false, data.link);
                     break;
                  }
               }
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void InputLinkItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
   {
      if (mHighlight)
      {
         delete mHighlight;
         mHighlight = NULL;
         mDrawing = NULL;
      }

      if (!mouseEvent) return;

      QPointF mousePos = mouseEvent->scenePos();

      // Find and highlight any output links being hovered over.
      QList<QGraphicsItem*> hoverList = mScene->items(mousePos.x(), mousePos.y(), 1, 1);
      if (!hoverList.empty())
      {
         int count = (int)hoverList.size();
         for (int index = 0; index < count; index++)
         {
            // If we're holding alt, only delete all links if we are still hovering
            // the same input.
            if (mAltModifier)
            {
               if (hoverList[index] == this)
               {
                  Disconnect();
                  mScene->Refresh();
                  break;
               }
            }
            else
            {
               OutputLinkItem* item = dynamic_cast<OutputLinkItem*>(hoverList[index]);
               if (item)
               {
                  InputLink* input = mNodeItem->GetInputs()[mLinkIndex].link;
                  OutputLink* output = item->mNodeItem->GetOutputs()[item->mLinkIndex].link;

                  // Create a new connection between these two links.
                  if (input->Connect(output))
                  {
                     std::string undoDescription = "Connection of link between ";
                     if (mNodeItem->GetNode())
                     {
                        undoDescription += "Node \'" + mNodeItem->GetNode()->GetTypeName() + "\' and ";
                     }
                     else if (mNodeItem->GetMacro())
                     {
                        undoDescription += "Macro Node \'" + mNodeItem->GetMacro()->GetName() + "\' and ";
                     }

                     if (item->mNodeItem->GetNode())
                     {
                        undoDescription += "Node \'" + item->mNodeItem->GetNode()->GetTypeName() + "\'.";
                     }
                     else if (item->mNodeItem->GetMacro())
                     {
                        undoDescription += "Macro Node \'" + item->mNodeItem->GetMacro()->GetName() + "\'.";
                     }

                     dtCore::RefPtr<UndoLinkEvent> event = new UndoLinkEvent(
                        mScene->GetEditor(),
                        UndoLinkEvent::INPUT_LINK,
                        input->GetOwner()->GetID(),
                        output->GetOwner()->GetID(),
                        input->GetName(),
                        output->GetName(),
                        true);
                     event->SetDescription(undoDescription);
                     mScene->GetEditor()->GetUndoManager()->AddEvent(event);
                  }
                  mNodeItem->ConnectLinks(true);
                  emit LinkConnected();
                  break;
               }
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void InputLinkItem::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
   {
      if (!mouseEvent || !mHighlight || !mDrawing) return;

      // Update the drawn spline.
      QPainterPath path;
      if (mLinkIndex >= 0 && mLinkIndex < (int)mNodeItem->GetInputs().size())
      {
         InputData& data = mNodeItem->GetInputs()[mLinkIndex];
         if (data.linkGraphic)
         {
            QPointF start(data.linkGraphic->scenePos());
            QPointF end(mouseEvent->scenePos());
            //float height = mNodeItem->scenePos().y();

            // Modify the positions based on the translation of the background item.
            //QPointF offset = mScene->GetTranslationItem()->scenePos();
            //start += offset;
            //end += offset;
            //height += offset.y();

            start.setX(start.x() + LINK_SIZE/2);
            start.setY(start.y() + LINK_SIZE/2);

            NodeItem* targetNode = NULL;

            // Find and highlight any output links being hovered over.
            QList<QGraphicsItem*> hoverList = mScene->items(mouseEvent->scenePos().x(), mouseEvent->scenePos().y(), 1, 1);
            if (!hoverList.empty())
            {
               int count = (int)hoverList.size();
               for (int index = 0; index < count; index++)
               {
                  OutputLinkItem* item = dynamic_cast<OutputLinkItem*>(hoverList[index]);
                  if (item)
                  {
                     // Snap the end position to the output link.
                     end = QPointF(item->scenePos())/* + offset*/;
                     end.setX(end.x() + LINK_LENGTH);
                     end.setY(end.y() + LINK_SIZE/2);

                     targetNode = item->GetNodeItem();
                     break;
                  }
               }
            }

            QPainterPath path = mNodeItem->CreateConnectionH(end, start, mNodeItem, targetNode, false, true);
            mHighlight->setPath(path);
            mDrawing->setPath(path);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void InputLinkItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
   {
      InputLink* input = mNodeItem->GetInputs()[mLinkIndex].link;
      if (input)
      {
         QMenu menu;
         QAction* hideAction = menu.addAction("Hide Link");
         connect(hideAction, SIGNAL(triggered()), this, SLOT(HideLink()));
         menu.addSeparator();

         QMenu* dcMenu = menu.addMenu("Disconnect");
         dcMenu->addAction("Disconnect All");
         dcMenu->addSeparator();

         int count = (int)input->GetLinks().size();
         dcMenu->setEnabled(count > 0);
         for (int index = 0; index < count; index++)
         {
            QString nodeName = input->GetLinks()[index]->GetOwner()->GetTypeName().c_str();
            QString linkName = input->GetLinks()[index]->GetName().c_str();
            QAction* dcAction = dcMenu->addAction(QString("Disconnect from \'") +
               linkName + "\' on node \'" + nodeName + "\'");
            dcAction->setStatusTip(linkName);
         }

         connect(dcMenu, SIGNAL(triggered(QAction*)), this, SLOT(Disconnect(QAction*)));

         menu.exec(event->screenPos());
         return;
      }

      QGraphicsPolygonItem::contextMenuEvent(event);
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   OutputLinkItem::OutputLinkItem(NodeItem* nodeItem, int linkIndex, QGraphicsItem* parent, EditorScene* scene, const std::string& comment)
      : LinkItem(nodeItem, linkIndex, parent, scene, comment)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   LinkItem* OutputLinkItem::GetLinkGraphic()
   {
      return this;
      //if( !mNodeItem || mLinkIndex > (int)mNodeItem->GetOutputs().size()-1 )
      //{
      //   return NULL;
      //}
      //return mNodeItem->GetOutputs()[mLinkIndex].linkGraphic;
   }

   //////////////////////////////////////////////////////////////////////////
   void OutputLinkItem::DrawGlow(float glow)
   {
      if (glow > 0.5f)
      {
         SetAlwaysHighlight(true);
         SetHighlightAlpha(1.0f);
      }
      else if (glow > 0.0f)
      {
         SetAlwaysHighlight(true);
         SetHighlightAlpha(glow * 2.0f);
      }
      else if (glow == 0.0f)
      {
         SetAlwaysHighlight(false);
         SetHighlightAlpha(1.0f);
      }
      else
      {
         return;
      }

      SetHighlight(mAlwaysHighlight);
      if (mLinkIndex >= 0 && mLinkIndex < (int)mNodeItem->GetOutputs().size())
      {
         OutputData& data = mNodeItem->GetOutputs()[mLinkIndex];
         if (data.link)
         {
            int linkCount = (int)data.link->GetLinks().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               InputLink* link = data.link->GetLinks()[linkIndex];
               if (!link) continue;

               NodeItem* item = mScene->GetNodeItem(link->GetOwner()->GetID());
               if (!item) continue;

               int inputCount = (int)item->GetInputs().size();
               for (int inputIndex = 0; inputIndex < inputCount; inputIndex++)
               {
                  if (item->GetInputs()[inputIndex].link == link)
                  {
                     item->GetInputs()[inputIndex].linkGraphic->SetAlwaysHighlight(mAlwaysHighlight);
                     item->GetInputs()[inputIndex].linkGraphic->SetHighlight(false);
                     break;
                  }
               }
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void OutputLinkItem::SetHighlight(bool enable, InputLink* inputLink)
   {
      LinkItem::SetHighlight(enable);

      // Highlight all connection splines.
      if (mLinkIndex >= 0 && mLinkIndex < (int)mNodeItem->GetOutputs().size())
      {
         OutputData& data = mNodeItem->GetOutputs()[mLinkIndex];

         if (!inputLink)
         {
            unsigned int i = 0;
            for (i = 0; i < data.link->GetLinks().size(); i++)
            {
               InputLink* link = data.link->GetLinks()[i];
               QGraphicsPathItem* connector = (i < data.linkConnectors.size()) ? data.linkConnectors[i] : NULL;
               if (connector && link)
               {
                  bool readOnly = data.link->GetOwner()->IsOutputLinkImported(data.link->GetName(), link->GetOwner()->GetID(), link->GetName());
                  SetHighlightConnector(mAlwaysHighlight || enable, connector, readOnly);
               }
            }
            for (;i < (unsigned int)data.linkConnectors.size(); i++)
            {
               QGraphicsPathItem* connector = (i < data.linkConnectors.size()) ? data.linkConnectors[i] : NULL;
               if (connector)
               {
                  SetHighlightConnector(false, connector, true, true);
               }
            }
         }
         else
         {
            unsigned int i = 0;
            for (; i < data.link->GetLinks().size(); i++)
            {
               InputLink* link = data.link->GetLinks()[i];
               if (link != inputLink)
               {
                  continue;
               }

               QGraphicsPathItem* connector = (i < data.linkConnectors.size()) ? data.linkConnectors[i] : NULL;
               if (connector)
               {
                  bool readOnly = data.link->GetOwner()->IsOutputLinkImported(data.link->GetName(), link->GetOwner()->GetID(), link->GetName());
                  SetHighlightConnector(mAlwaysHighlight || enable, connector, readOnly);
               }
            }
            for (;i < (unsigned int)data.linkConnectors.size(); i++)
            {
               QGraphicsPathItem* connector = (i < data.linkConnectors.size()) ? data.linkConnectors[i] : NULL;
               if (connector)
               {
                  SetHighlightConnector(false, connector, true, true);
               }
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void OutputLinkItem::Disconnect(InputLink* input)
   {
      OutputLink* output = mNodeItem->GetOutputs()[mLinkIndex].link;
      if (!output) return;

      std::string undoDescription = "Disconnection of output link(s) for ";
      if (mNodeItem->GetNode())
      {
         undoDescription += "Node \'" + mNodeItem->GetNode()->GetTypeName() + "\'.";
      }
      else if (mNodeItem->GetMacro())
      {
         undoDescription += "Macro Node \'" + mNodeItem->GetMacro()->GetName() + "\'.";
      }

      if (!input)
      {
         if (output->GetLinks().size())
         {
            mScene->GetEditor()->GetUndoManager()->BeginMultipleEvents(undoDescription);

            while (!output->GetLinks().empty())
            {
               input = output->GetLinks()[0];
               Disconnect(input);
            }

            mScene->GetEditor()->GetUndoManager()->EndMultipleEvents();
         }
      }
      else
      {
         if (input->Disconnect(output))
         {
            dtCore::RefPtr<UndoLinkEvent> event = new UndoLinkEvent(
               mScene->GetEditor(),
               UndoLinkEvent::INPUT_LINK,
               input->GetOwner()->GetID(),
               output->GetOwner()->GetID(),
               input->GetName(),
               output->GetName(),
               false);
            event->SetDescription(undoDescription);
            mScene->GetEditor()->GetUndoManager()->AddEvent(event);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void OutputLinkItem::Disconnect(QAction* action)
   {
      if (action->text() == "Disconnect All")
      {
         Disconnect();
         mScene->Refresh();
      }
      else
      {
         OutputLink* output = mNodeItem->GetOutputs()[mLinkIndex].link;
         if (!output) return;
         int count = (int)output->GetLinks().size();
         for (int index = 0; index < count; index++)
         {
            if (output->GetLinks()[index]->GetName() == action->statusTip().toStdString())
            {
               Disconnect(output->GetLinks()[index]);
               mScene->Refresh();
               break;
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OutputLinkItem::HideLink()
   {
      OutputLink* output = mNodeItem->GetOutputs()[mLinkIndex].link;
      if (!output) return;

      // Hide this link.
      output->SetVisible(false);

      std::string undoDescription = "Hiding of output link \'" + output->GetName() + "\' for ";
      if (mNodeItem->GetNode())
      {
         undoDescription += "Node \'" + mNodeItem->GetNode()->GetTypeName() + "\'.";
      }
      else if (mNodeItem->GetMacro())
      {
         if (!mNodeItem->GetMacro()->GetEditor().empty())
         {
            undoDescription += "\'" + mNodeItem->GetMacro()->GetEditor() + "\' ";
         }
         undoDescription += "Macro Node \'" + mNodeItem->GetMacro()->GetName() + "\'.";
      }

      mScene->GetEditor()->GetUndoManager()->BeginMultipleEvents(undoDescription);

      dtCore::RefPtr<UndoLinkVisibilityEvent> event = new UndoLinkVisibilityEvent(mScene->GetEditor(), mNodeItem->GetID(), 1, output->GetName(), false);
      mScene->GetEditor()->GetUndoManager()->AddEvent(event);

      // Disconnect all links.
      Disconnect();

      mScene->GetEditor()->GetUndoManager()->EndMultipleEvents();

      mScene->Refresh();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OutputLinkItem::ShowLink()
   {
      OutputLink* output = mNodeItem->GetOutputs()[mLinkIndex].link;
      if (!output) return;

      // Show this link.
      output->SetVisible(true);

      std::string undoDescription = "Showing of output link \'" + output->GetName() + "\' for ";
      if (mNodeItem->GetNode())
      {
         undoDescription += "Node \'" + mNodeItem->GetNode()->GetTypeName() + "\'.";
      }
      else if (mNodeItem->GetMacro())
      {
         if (!mNodeItem->GetMacro()->GetEditor().empty())
         {
            undoDescription += "\'" + mNodeItem->GetMacro()->GetEditor() + "\' ";
         }
         undoDescription += "Macro Node \'" + mNodeItem->GetMacro()->GetName() + "\'.";
      }

      dtCore::RefPtr<UndoLinkVisibilityEvent> event = new UndoLinkVisibilityEvent(mScene->GetEditor(), mNodeItem->GetID(), 1, output->GetName(), true);
      event->SetDescription(undoDescription);
      mScene->GetEditor()->GetUndoManager()->AddEvent(event);

      mScene->Refresh();
   }

   //////////////////////////////////////////////////////////////////////////
   void OutputLinkItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
   {
      SetHighlight(true);

      // Highlight all connected input links.
      if (mLinkIndex >= 0 && mLinkIndex < (int)mNodeItem->GetOutputs().size())
      {
         OutputData& data = mNodeItem->GetOutputs()[mLinkIndex];
         if (data.link)
         {
            int linkCount = (int)data.link->GetLinks().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               InputLink* link = data.link->GetLinks()[linkIndex];
               if (!link) continue;

               NodeItem* item = mScene->GetNodeItem(link->GetOwner()->GetID());
               if (!item) continue;

               int inputCount = (int)item->GetInputs().size();
               for (int inputIndex = 0; inputIndex < inputCount; inputIndex++)
               {
                  if (item->GetInputs()[inputIndex].link == link)
                  {
                     item->GetInputs()[inputIndex].linkGraphic->SetHighlight(true);
                     break;
                  }
               }
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void OutputLinkItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
   {
      SetHighlight(false);

      // Remove the highlight on all connected input links.
      if (mLinkIndex >= 0 && mLinkIndex < (int)mNodeItem->GetOutputs().size())
      {
         OutputData& data = mNodeItem->GetOutputs()[mLinkIndex];
         if (data.link)
         {
            int linkCount = (int)data.link->GetLinks().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               InputLink* link = data.link->GetLinks()[linkIndex];
               if (!link) continue;

               NodeItem* item = mScene->GetNodeItem(link->GetOwner()->GetID());
               if (!item) continue;

               int inputCount = (int)item->GetInputs().size();
               for (int inputIndex = 0; inputIndex < inputCount; inputIndex++)
               {
                  if (item->GetInputs()[inputIndex].link == link)
                  {
                     item->GetInputs()[inputIndex].linkGraphic->SetHighlight(false);
                     break;
                  }
               }
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void OutputLinkItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
   {
      if (mHighlight)
      {
         delete mHighlight;
         mHighlight = NULL;
         mDrawing = NULL;
      }

      if (!mouseEvent) return;

      QPointF mousePos = mouseEvent->scenePos();

      // Find and highlight any output links being hovered over.
      QList<QGraphicsItem*> hoverList = mScene->items(mousePos.x(), mousePos.y(), 1, 1);
      if (!hoverList.empty())
      {
         int count = (int)hoverList.size();
         for (int index = 0; index < count; index++)
         {
            // If we are holding alt, we need to make sure we are still hovering
            // over the same item before we clear all links.
            if (mAltModifier)
            {
               if (hoverList[index] == this)
               {
                  Disconnect();
                  mScene->Refresh();
                  break;
               }
            }
            else
            {
               InputLinkItem* item = dynamic_cast<InputLinkItem*>(hoverList[index]);
               if (item)
               {
                  InputLink* input = item->mNodeItem->GetInputs()[item->mLinkIndex].link;
                  OutputLink* output = mNodeItem->GetOutputs()[mLinkIndex].link;

                  // Create a new connection between these two links.
                  if (input->Connect(output))
                  {
                     std::string undoDescription = "Connection of link between ";
                     if (mNodeItem->GetNode())
                     {
                        undoDescription += "Node \'" + mNodeItem->GetNode()->GetTypeName() + "\' and ";
                     }
                     else if (mNodeItem->GetMacro())
                     {
                        undoDescription += "Macro Node \'" + mNodeItem->GetMacro()->GetName() + "\' and ";
                     }

                     if (item->mNodeItem->GetNode())
                     {
                        undoDescription += "Node \'" + item->mNodeItem->GetNode()->GetTypeName() + "\'.";
                     }
                     else if (item->mNodeItem->GetMacro())
                     {
                        undoDescription += "Macro Node \'" + item->mNodeItem->GetMacro()->GetName() + "\'.";
                     }

                     dtCore::RefPtr<UndoLinkEvent> event = new UndoLinkEvent(
                        mScene->GetEditor(),
                        UndoLinkEvent::INPUT_LINK,
                        input->GetOwner()->GetID(),
                        output->GetOwner()->GetID(),
                        input->GetName(),
                        output->GetName(),
                        true);
                     event->SetDescription(undoDescription);
                     mScene->GetEditor()->GetUndoManager()->AddEvent(event);
                  }
                  mNodeItem->ConnectLinks(true);
                  emit LinkConnected();
                  break;
               }
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void OutputLinkItem::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
   {
      if (!mouseEvent || !mHighlight || !mDrawing) return;

      // Update the drawn spline.
      QPainterPath path;
      if (mLinkIndex >= 0 && mLinkIndex < (int)mNodeItem->GetOutputs().size())
      {
         OutputData& data = mNodeItem->GetOutputs()[mLinkIndex];
         if (data.linkGraphic)
         {
            QPointF start(data.linkGraphic->scenePos());
            QPointF end(mouseEvent->scenePos());
            //float height = mNodeItem->scenePos().y();

            // Modify the positions based on the translation of the background item.
            //QPointF offset = mScene->GetTranslationItem()->scenePos();
            //start += offset;
            //end += offset;
            //height += offset.y();

            start.setX(start.x() + LINK_LENGTH);
            start.setY(start.y() + LINK_SIZE/2);

            NodeItem* targetNode = NULL;

            // Find and highlight any output links being hovered over.
            QList<QGraphicsItem*> hoverList = mScene->items(mouseEvent->scenePos().x(), mouseEvent->scenePos().y(), 1, 1);
            if (!hoverList.empty())
            {
               int count = (int)hoverList.size();
               for (int index = 0; index < count; index++)
               {
                  InputLinkItem* item = dynamic_cast<InputLinkItem*>(hoverList[index]);
                  if (item)
                  {
                     // Snap the end position to the output link.
                     end = QPointF(item->scenePos())/* + offset*/;
                     end.setX(end.x() + LINK_SIZE/2);
                     end.setY(end.y() + LINK_SIZE/2);

                     targetNode = item->GetNodeItem();
                     break;
                  }
               }
            }

            QPainterPath path = mNodeItem->CreateConnectionH(start, end, mNodeItem, targetNode, true, true);
            mHighlight->setPath(path);
            mDrawing->setPath(path);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void OutputLinkItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
   {
      OutputLink* output = mNodeItem->GetOutputs()[mLinkIndex].link;
      if (output)
      {
         QMenu menu;
         QAction* hideAction = menu.addAction("Hide Link");
         connect(hideAction, SIGNAL(triggered()), this, SLOT(HideLink()));
         menu.addSeparator();

         QMenu* dcMenu = menu.addMenu("Disconnect");
         dcMenu->addAction("Disconnect All");
         dcMenu->addSeparator();

         int count = (int)output->GetLinks().size();
         dcMenu->setEnabled(count > 0);
         for (int index = 0; index < count; index++)
         {
            QString nodeName = output->GetLinks()[index]->GetOwner()->GetTypeName().c_str();
            QString linkName = output->GetLinks()[index]->GetName().c_str();
            QAction* dcAction = dcMenu->addAction(QString("Disconnect from \'") +
               linkName + "\' on node \'" + nodeName + "\'");
            dcAction->setStatusTip(linkName);
         }

         connect(dcMenu, SIGNAL(triggered(QAction*)), this, SLOT(Disconnect(QAction*)));

         menu.exec(event->screenPos());
         return;
      }

      QGraphicsPolygonItem::contextMenuEvent(event);
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   ValueLinkItem::ValueLinkItem(NodeItem* nodeItem, int linkIndex, QGraphicsItem* parent, EditorScene* scene, const std::string& comment)
      : LinkItem(nodeItem, linkIndex, parent, scene, comment)
      , mType(dtCore::DataType::UNKNOWN_ID)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   LinkItem* ValueLinkItem::GetLinkGraphic()
   {
      return this;
      //if( !mNodeItem || mLinkIndex > (int)mNodeItem->GetValues().size()-1 )
      //{
      //   return NULL;
      //}
      //return mNodeItem->GetValues()[mLinkIndex].linkGraphic;
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueLinkItem::SetHighlight(bool enable, Node* valueNode)
   {
      LinkItem::SetHighlight(enable);

      // Highlight the connection splines.
      if (mLinkIndex >= 0 && mLinkIndex < (int)mNodeItem->GetValues().size())
      {
         ValueData& data = mNodeItem->GetValues()[mLinkIndex];
         if (!valueNode)
         {
            int count = (int)data.link->GetLinks().size();
            int index = 0;
            for (; index < count; index++)
            {
               ValueNode* link = data.link->GetLinks()[index];
               QGraphicsPathItem* connector = (index < (int)data.linkConnectors.size()) ? data.linkConnectors[index] : NULL;
               if (connector && link)
               {
                  bool readOnly = data.link->GetOwner()->IsValueLinkImported(data.link->GetName(), link->GetID());
                  SetHighlightConnector(enable, connector, readOnly);
               }
            }
            for (;index < (int)data.linkConnectors.size(); index++)
            {
               QGraphicsPathItem* connector = (index < (int)data.linkConnectors.size()) ? data.linkConnectors[index] : NULL;
               if (connector)
               {
                  SetHighlightConnector(false, connector, true, true);
               }
            }
         }
         else
         {
            int count = (int)data.link->GetLinks().size();
            int index = 0;
            for (; index < count; index++)
            {
               ValueNode* link = data.link->GetLinks()[index];
               if (link == valueNode)
               {
                  if (index < (int)data.linkConnectors.size())
                  {
                     QGraphicsPathItem* connector = data.linkConnectors[index];
                     if (connector)
                     {
                        bool readOnly = data.link->GetOwner()->IsValueLinkImported(data.link->GetName(), link->GetID());
                        SetHighlightConnector(enable, connector, readOnly);
                     }
                  }
               }
            }
            for (; index < (int)data.linkConnectors.size(); ++index)
            {
               QGraphicsPathItem* connector = (index < (int)data.linkConnectors.size()) ? data.linkConnectors[index] : NULL;
               if (connector)
               {
                  SetHighlightConnector(false, connector, true, true);
               }
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueLinkItem::Disconnect(ValueNode* output)
   {
      ValueLink* input = mNodeItem->GetValues()[mLinkIndex].link;
      if (!input) return;

      std::string undoDescription = "Disconnection of value link(s) for ";
      if (mNodeItem->GetNode())
      {
         undoDescription += "Node \'" + mNodeItem->GetNode()->GetTypeName() + "\'.";
      }
      else if (mNodeItem->GetMacro())
      {
         undoDescription += "Macro Node \'" + mNodeItem->GetMacro()->GetName() + "\'.";
      }

      if (!output)
      {
         if (input->GetLinks().size())
         {
            mScene->GetEditor()->GetUndoManager()->BeginMultipleEvents(undoDescription);

            while (!input->GetLinks().empty())
            {
               ValueNode* output = input->GetLinks()[0];
               Disconnect(output);
            }

            mScene->GetEditor()->GetUndoManager()->EndMultipleEvents();
         }
      }
      else
      {
         if (input->Disconnect(output))
         {
            dtCore::RefPtr<UndoLinkEvent> event = new UndoLinkEvent(
               mScene->GetEditor(),
               UndoLinkEvent::VALUE_LINK,
               input->GetOwner()->GetID(),
               output->GetID(),
               input->GetName(),
               output->GetName(),
               false);
            event->SetDescription(undoDescription);
            mScene->GetEditor()->GetUndoManager()->AddEvent(event);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueLinkItem::Disconnect(QAction* action)
   {
      if (action->text() == "Disconnect All")
      {
         Disconnect();
         mScene->Refresh();
      }
      else
      {
         ValueLink* input = mNodeItem->GetValues()[mLinkIndex].link;
         if (!input) return;
         int count = (int)input->GetLinks().size();
         for (int index = 0; index < count; index++)
         {
            if (input->GetLinks()[index]->GetName() == action->statusTip().toStdString())
            {
               Disconnect(input->GetLinks()[index]);
               mScene->Refresh();
               break;
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ValueLinkItem::HideLink()
   {
      ValueLink* input = mNodeItem->GetValues()[mLinkIndex].link;
      if (!input) return;

      // Hide this link.
      input->SetVisible(false);

      std::string undoDescription = "Hiding of value link \'" + input->GetName() + "\' for ";
      if (mNodeItem->GetNode())
      {
         undoDescription += "Node \'" + mNodeItem->GetNode()->GetTypeName() + "\'.";
      }
      else if (mNodeItem->GetMacro())
      {
         if (!mNodeItem->GetMacro()->GetEditor().empty())
         {
            undoDescription += "\'" + mNodeItem->GetMacro()->GetEditor() + "\' ";
         }
         undoDescription += "Macro Node \'" + mNodeItem->GetMacro()->GetName() + "\'.";
      }

      mScene->GetEditor()->GetUndoManager()->BeginMultipleEvents(undoDescription);

      dtCore::RefPtr<UndoLinkVisibilityEvent> event = new UndoLinkVisibilityEvent(mScene->GetEditor(), mNodeItem->GetID(), 2, input->GetName(), false);
      mScene->GetEditor()->GetUndoManager()->AddEvent(event);

      // Disconnect all links.
      Disconnect();

      mScene->GetEditor()->GetUndoManager()->EndMultipleEvents();

      mScene->Refresh();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ValueLinkItem::ShowLink()
   {
      ValueLink* input = mNodeItem->GetValues()[mLinkIndex].link;
      if (!input) return;

      // Show this link.
      input->SetVisible(true);

      std::string undoDescription = "Showing of value link \'" + input->GetName() + "\' for ";
      if (mNodeItem->GetNode())
      {
         undoDescription += "Node \'" + mNodeItem->GetNode()->GetTypeName() + "\'.";
      }
      else if (mNodeItem->GetMacro())
      {
         if (!mNodeItem->GetMacro()->GetEditor().empty())
         {
            undoDescription += "\'" + mNodeItem->GetMacro()->GetEditor() + "\' ";
         }
         undoDescription += "Macro Node \'" + mNodeItem->GetMacro()->GetName() + "\'.";
      }

      dtCore::RefPtr<UndoLinkVisibilityEvent> event = new UndoLinkVisibilityEvent(mScene->GetEditor(), mNodeItem->GetID(), 2, input->GetName(), true);
      event->SetDescription(undoDescription);
      mScene->GetEditor()->GetUndoManager()->AddEvent(event);

      mScene->Refresh();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ValueLinkItem::RemoveLink()
   {
      ValueLink* input = mNodeItem->GetValues()[mLinkIndex].link;
      if (!input) return;

      // Remove this link.
      input->SetExposed(false);

      std::string undoDescription = "Removal of value link \'" + input->GetName() + "\' for ";
      if (mNodeItem->GetNode())
      {
         undoDescription += "Node \'" + mNodeItem->GetNode()->GetTypeName() + "\'.";
      }
      else if (mNodeItem->GetMacro())
      {
         undoDescription += "Macro Node \'" + mNodeItem->GetMacro()->GetName() + "\'.";
      }

      mScene->GetEditor()->GetUndoManager()->BeginMultipleEvents(undoDescription);

      dtCore::RefPtr<UndoLinkVisibilityEvent> event = new UndoLinkVisibilityEvent(mScene->GetEditor(), mNodeItem->GetID(), 2, input->GetName(), false, true);
      mScene->GetEditor()->GetUndoManager()->AddEvent(event);

      // Disconnect all links.
      Disconnect();

      mScene->GetEditor()->GetUndoManager()->EndMultipleEvents();
      mScene->Refresh();
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueLinkItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
   {
      SetHighlight(true);

      // Highlight all connected value nodes.
      if (mLinkIndex >= 0 && mLinkIndex < (int)mNodeItem->GetValues().size())
      {
         ValueData& data = mNodeItem->GetValues()[mLinkIndex];
         if (data.link)
         {
            int linkCount = (int)data.link->GetLinks().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               ValueNode* valueNode = data.link->GetLinks()[linkIndex];
               if (!valueNode) continue;

               ValueItem* item = dynamic_cast<ValueItem*>(mScene->GetNodeItem(valueNode->GetID()));
               if (!item) continue;

               if (item->GetValueLink())
               {
                  item->GetValueLink()->SetHighlight(true);
               }
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueLinkItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
   {
      SetHighlight(false);

      // Remove the highlight from all connected value nodes.
      if (mLinkIndex >= 0 && mLinkIndex < (int)mNodeItem->GetValues().size())
      {
         ValueData& data = mNodeItem->GetValues()[mLinkIndex];
         if (data.link)
         {
            int linkCount = (int)data.link->GetLinks().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               ValueNode* valueNode = data.link->GetLinks()[linkIndex];
               if (!valueNode) continue;

               ValueItem* item = dynamic_cast<ValueItem*>(mScene->GetNodeItem(valueNode->GetID()));
               if (!item) continue;

               if (item->GetValueLink())
               {
                  item->GetValueLink()->SetHighlight(false);
               }
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ValueNode* ValueLinkItem::CreateValueNode(QPointF newPos, bool refreshViews)
   {
      ValueNode* newNode = NULL;

      // Determine the type of node to create.
      ValueLink* input = mNodeItem->GetValues()[mLinkIndex].link;
      if (input)
      {
         DirectorGraph* graph = NULL;
         if (mNodeItem->GetNode())
         {
            graph = mNodeItem->GetNode()->GetGraph();
         }
         else if (mNodeItem->GetMacro())
         {
            graph = mNodeItem->GetMacro()->GetParent();
         }

         dtCore::DataType* dataType = &input->GetPropertyType();
         if (*dataType == dtCore::DataType::UNKNOWN)
         {
            if (input->GetDefaultProperty())
            {
               dataType = &input->GetDefaultProperty()->GetPropertyType();
            }
            else
            {
               // As a fallback, unknown data types will be considered strings
               // as string form is more or less universal to all data types
               // (eg ToString/FromString methods found on all properties).
               dataType = &dtCore::DataType::STRING;
            }
         }

         const dtDirector::NodeType* type = NodeManager::GetInstance().FindNodeType(*dataType);
         if (type)
         {
            dtCore::RefPtr<Node> node =
               NodeManager::GetInstance().CreateNode(*type, graph);
            if (node.valid())
            {
               node->OnFinishedLoading();
               if (graph->GetDirector() && graph->GetDirector()->HasStarted())
               {
                  node->OnStart();
               }

               // Get a position directly under the link.
               node->SetPosition(osg::Vec2(newPos.x(), newPos.y()));

               newNode = node->AsValueNode();
               DirectorEditor* editor = mScene->GetEditor();

               editor->GetUndoManager()->BeginMultipleEvents("Creation of Value Node \'" + type->GetName() + "\'.");

               if (!input->AllowMultiple())
               {
                  Disconnect();
               }

               dtCore::RefPtr<UndoCreateEvent> event = new UndoCreateEvent(editor, node->GetID(), graph->GetID());
               editor->GetUndoManager()->AddEvent(event);

               // Create a new connection between this link and our new value node.
               if (input->Connect(newNode))
               {
                  dtCore::RefPtr<UndoLinkEvent> event = new UndoLinkEvent(
                     editor,
                     UndoLinkEvent::VALUE_LINK,
                     input->GetOwner()->GetID(),
                     newNode->GetID(),
                     input->GetName(),
                     newNode->GetName(),
                     true);
                  editor->GetUndoManager()->AddEvent(event);
                  emit LinkConnected();
               }
               // If for any reason we could not make the connection,
               // undo our current operation and bail out.
               else
               {
                  std::string error = "Failed to create Value Node \'";
                  error += newNode->GetTypeName();
                  error += "\' for Value Link \'";
                  error += input->GetName();
                  error += "\' because the Node could not connect to the Link.";
                  LOG_ERROR(error);
                  editor->GetUndoManager()->UndoCurrentMultipleEvent();
                  return NULL;
               }

               // Now set the default value to the current value of the link.
               if (input->GetDefaultProperty())
               {
                  std::string value = input->GetDefaultProperty()->ToString();

                  std::string oldValue = newNode->GetString();
                  newNode->SetString(value);
                  newNode->SetString(value, "Initial Value");

                  dtCore::RefPtr<UndoPropertyEvent> event = new UndoPropertyEvent(
                     editor,
                     newNode->GetID(),
                     "Value",
                     oldValue,
                     value);
                  editor->GetUndoManager()->AddEvent(event);

                  event = new UndoPropertyEvent(
                     editor,
                     newNode->GetID(),
                     "Initial Value",
                     oldValue,
                     value);
                  editor->GetUndoManager()->AddEvent(event);
               }

               editor->GetUndoManager()->EndMultipleEvents();

               if (refreshViews)
               {
                  // Now refresh the all editors that view the same graph.
                  int count = editor->GetGraphTabs()->count();
                  for (int index = 0; index < count; index++)
                  {
                     EditorView* view = dynamic_cast<EditorView*>(editor->GetGraphTabs()->widget(index));
                     if (view && view->GetScene())
                     {
                        if (view->GetScene()->GetGraph() == graph)
                        {
                           // First remember the position of the translation node.
                           QPointF trans = view->GetScene()->GetTranslationItem()->pos();
                           view->GetScene()->SetGraph(graph);
                           view->GetScene()->GetTranslationItem()->setPos(trans);
                        }
                     }
                  }
                  editor->Refresh();
               }
            }
            else
            {
               // If for any reason our node was not created, undo and bail out.
               std::string error = "Failed to create Value Node of type \'";
               error += type->GetName();
               error += "\'.";
               LOG_ERROR(error);
            }
         }
      }

      return newNode;
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueLinkItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
   {
      if (mHighlight)
      {
         delete mHighlight;
         mHighlight = NULL;
         mDrawing = NULL;
      }

      if (!mouseEvent) return;

      QPointF mousePos = mouseEvent->scenePos();

      ValueLink* input = mNodeItem->GetValues()[mLinkIndex].link;

      // Find and highlight any output links being hovered over.
      QList<QGraphicsItem*> hoverList = mScene->items(mousePos.x(), mousePos.y(), 1, 1);
      if (!hoverList.empty())
      {
         int count = (int)hoverList.size();
         for (int index = 0; index < count; index++)
         {
            if (mAltModifier)
            {
               if (hoverList[index] == this)
               {
                  Disconnect();
                  mScene->Refresh();
                  break;
               }
            }
            else
            {
               // Check if the user is attempting to directly connect two value links together.
               ValueLinkItem* otherLink = dynamic_cast<ValueLinkItem*>(hoverList[index]);
               if (otherLink && otherLink != this)
               {
                  ValueLink* output = otherLink->mNodeItem->GetValues()[otherLink->mLinkIndex].link;
                  //Node* otherNode = otherLink->mNodeItem->GetNode();

                  std::string undoDescription = "Connection of value link between ";
                  if (mNodeItem->GetNode())
                  {
                     undoDescription += "Node \'" + mNodeItem->GetNode()->GetTypeName() + "\' and ";
                  }
                  else if (mNodeItem->GetMacro())
                  {
                     undoDescription += "Macro Node \'" + mNodeItem->GetMacro()->GetName() + "\' and ";
                  }

                  if (otherLink->mNodeItem->GetNode())
                  {
                     undoDescription += "Node \'" + otherLink->mNodeItem->GetNode()->GetTypeName() + "\'.";
                  }
                  else if (otherLink->mNodeItem->GetMacro())
                  {
                     undoDescription += "Macro Node \'" + otherLink->mNodeItem->GetMacro()->GetName() + "\'.";
                  }

                  DirectorEditor* editor = mScene->GetEditor();

                  // We are going to attempt multiple connections, so group these
                  // events into a single undo.
                  editor->GetUndoManager()->BeginMultipleEvents(undoDescription);

                  // If our link does not allow multiple connections, we should start
                  // by disconnecting whatever we currently have.
                  if (!output->AllowMultiple())
                  {
                     otherLink->Disconnect();
                  }

                  // Find a position that is between both links.
                  QPointF offset = mScene->GetTranslationItem()->scenePos();
                  QPointF linkPos1 = mNodeItem->GetValues()[mLinkIndex].linkGraphic->scenePos() - offset;
                  QPointF linkPos2 = otherLink->GetLinkGraphic()->scenePos() - offset;
                  QPointF newPos;
                  newPos.setX((linkPos1.x() + linkPos2.x()) * 0.5f - (MIN_NODE_WIDTH * 0.5f));
                  if (linkPos1.y() > linkPos2.y())
                  {
                     newPos.setY(linkPos1.y() + 50.0f);
                  }
                  else
                  {
                     newPos.setY(linkPos2.y() + 50.0f);
                  }

                  // Create a new value node that can be used to connect the two
                  // links together.
                  ValueNode* newNode = CreateValueNode(newPos, false);
                  if (!newNode)
                  {
                     editor->GetUndoManager()->UndoCurrentMultipleEvent();
                     break;
                  }

                  // Create a new connection between our new value node
                  // and the target link.
                  if (output->Connect(newNode))
                  {
                     dtCore::RefPtr<UndoLinkEvent> event = new UndoLinkEvent(
                        editor,
                        UndoLinkEvent::VALUE_LINK,
                        output->GetOwner()->GetID(),
                        newNode->GetID(),
                        output->GetName(),
                        newNode->GetName(),
                        true);
                     editor->GetUndoManager()->AddEvent(event);
                     emit LinkConnected();
                  }
                  // If for any reason we could not make the connection,
                  // undo our current operation and bail out.
                  else
                  {
                     std::string error = "Failed to create Value Node \'";
                     error += newNode->GetTypeName();
                     error += "\' for Value Link \'";
                     error += output->GetName();
                     error += "\' because the Node could not connect to the Link.";
                     LOG_ERROR(error);
                     editor->GetUndoManager()->UndoCurrentMultipleEvent();
                     break;
                  }

                  editor->GetUndoManager()->EndMultipleEvents();

                  // Now refresh the all editors that view the same graph.
                  DirectorGraph* graph = NULL;
                  if (mNodeItem->GetNode())
                  {
                     graph = mNodeItem->GetNode()->GetGraph();
                  }
                  else if (mNodeItem->GetMacro())
                  {
                     graph = mNodeItem->GetMacro()->GetParent();
                  }

                  int count = editor->GetGraphTabs()->count();
                  for (int index = 0; index < count; index++)
                  {
                     EditorView* view = dynamic_cast<EditorView*>(editor->GetGraphTabs()->widget(index));
                     if (view && view->GetScene())
                     {
                        if (view->GetScene()->GetGraph() == graph)
                        {
                           // First remember the position of the translation node.
                           QPointF trans = view->GetScene()->GetTranslationItem()->pos();
                           view->GetScene()->SetGraph(graph);
                           view->GetScene()->GetTranslationItem()->setPos(trans);
                        }
                     }
                  }
                  editor->Refresh();
                  break;
               }
               else
               {
                  ValueNodeLinkItem* item = dynamic_cast<ValueNodeLinkItem*>(hoverList[index]);
                  if (item)
                  {
                     ValueNode* output = item->mNodeItem->GetNode()->AsValueNode();

                     std::string undoDescription = "Connection of value link between ";
                     if (mNodeItem->GetNode())
                     {
                        undoDescription += "Node \'" + mNodeItem->GetNode()->GetTypeName() + "\' and ";
                     }
                     else if (mNodeItem->GetMacro())
                     {
                        undoDescription += "Macro Node \'" + mNodeItem->GetMacro()->GetName() + "\' and ";
                     }

                     undoDescription += "Value Node \'" + item->mNodeItem->GetNode()->GetTypeName() + "\'.";

                     // If this link does not allow multiple connections, then
                     // make sure we disconnect the link from any values first.
                     if (!input->AllowMultiple())
                     {
                        mScene->GetEditor()->GetUndoManager()->BeginMultipleEvents(undoDescription);
                        Disconnect();
                     }

                     // Create a new connection between these two links.
                     if (input->Connect(output))
                     {
                        dtCore::RefPtr<UndoLinkEvent> event = new UndoLinkEvent(
                           mScene->GetEditor(),
                           UndoLinkEvent::VALUE_LINK,
                           input->GetOwner()->GetID(),
                           output->GetID(),
                           input->GetName(),
                           output->GetName(),
                           true);
                        event->SetDescription(undoDescription);
                        mScene->GetEditor()->GetUndoManager()->AddEvent(event);
                        emit LinkConnected();
                     }

                     if (!input->AllowMultiple())
                     {
                        mScene->GetEditor()->GetUndoManager()->EndMultipleEvents();
                     }

                     // Refresh the entire scene to make sure all nodes and links are
                     // colored properly.
                     mScene->Refresh();
                     break;
                  }
               }
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ValueLinkItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
   {
      if (!mouseEvent) return;

      //QPointF mousePos = mouseEvent->scenePos();

      //ValueLink* link = mNodeItem->GetValues()[mLinkIndex].link;

      // Find a position that is between both links.
      QPointF offset = mScene->GetTranslationItem()->scenePos();
      QPointF linkPos = scenePos() - offset + QPointF(MIN_NODE_WIDTH * -0.5f, 50.0f);

      // Create a new value node that can be used to connect the two
      // links together.
      CreateValueNode(linkPos, true);
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueLinkItem::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
   {
      if (!mouseEvent || !mHighlight || !mDrawing) return;

      // Update the drawn spline.
      QPainterPath path;
      if (mLinkIndex >= 0 && mLinkIndex < (int)mNodeItem->GetValues().size())
      {
         ValueData& data = mNodeItem->GetValues()[mLinkIndex];
         if (data.linkGraphic)
         {
            QPointF start(data.linkGraphic->scenePos());
            QPointF end(mouseEvent->scenePos());

            // Modify the positions based on the translation of the background item.
            QPointF offset = mScene->GetTranslationItem()->scenePos();
            start += offset;
            end += offset;

            if (data.link->IsOutLink())
            {
               start.setY(start.y() + LINK_LENGTH);
            }
            else
            {
               start.setY(start.y() + LINK_LENGTH - LINK_SIZE / 2);
            }

            bool connectingTwoValueLinks = false;

            // Find and highlight any output links being hovered over.
            QList<QGraphicsItem*> hoverList = mScene->items(mouseEvent->scenePos().x(), mouseEvent->scenePos().y(), 1, 1);
            if (!hoverList.empty())
            {
               int count = (int)hoverList.size();
               for (int index = 0; index < count; index++)
               {
                  ValueNodeLinkItem* item = dynamic_cast<ValueNodeLinkItem*>(hoverList[index]);
                  if (item)
                  {
                     // Snap the end position to the output link.
                     end = QPointF(item->scenePos()) + offset;
                     end.setY(end.y() - LINK_LENGTH);
                     break;
                  }
                  else
                  {
                     ValueLinkItem* valueLink = dynamic_cast<ValueLinkItem*>(hoverList[index]);
                     if (valueLink && valueLink  != this)
                     {
                        // Snap the end position to the other value link.
                        connectingTwoValueLinks = true;
                        end = QPointF(valueLink->scenePos()) + offset;

                        if (valueLink->GetNodeItem()->GetValues()[valueLink->mLinkIndex].link->IsOutLink())
                        {
                           end.setY(end.y() + LINK_LENGTH);
                        }
                        else
                        {
                           end.setY(end.y() + LINK_LENGTH - LINK_SIZE / 2);
                        }
                        break;
                     }
                  }
               }
            }

            QPainterPath path = mNodeItem->CreateConnectionV(start, end, true, connectingTwoValueLinks);
            mHighlight->setPath(path);
            mDrawing->setPath(path);
         }
      }
   }


   //////////////////////////////////////////////////////////////////////////
   void ValueLinkItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
   {
      ValueLink* input = mNodeItem->GetValues()[mLinkIndex].link;
      if (input)
      {
         QMenu menu;
         QAction* hideAction = menu.addAction("Hide Link");
         QAction* removeAction = menu.addAction("Remove Link");
         connect(hideAction, SIGNAL(triggered()), this, SLOT(HideLink()));
         connect(removeAction, SIGNAL(triggered()), this, SLOT(RemoveLink()));
         menu.addSeparator();

         QMenu* dcMenu = menu.addMenu("Disconnect");
         dcMenu->addAction("Disconnect All");
         dcMenu->addSeparator();

         int count = (int)input->GetLinks().size();
         dcMenu->setEnabled(count > 0);
         for (int index = 0; index < count; index++)
         {
            QString linkName = input->GetLinks()[index]->GetTypeName().c_str();
            QAction* dcAction = dcMenu->addAction(QString("Disconnect from \'") +
               linkName + "\'");
            dcAction->setStatusTip(linkName);
         }

         connect(dcMenu, SIGNAL(triggered(QAction*)), this, SLOT(Disconnect(QAction*)));

         menu.exec(event->screenPos());
         return;
      }

      QGraphicsPolygonItem::contextMenuEvent(event);
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   ValueNodeLinkItem::ValueNodeLinkItem(ValueItem* valueItem, QGraphicsItem* parent, EditorScene* scene)
      : LinkItem(valueItem, 0, parent, scene, "")
      , mType(dtCore::DataType::UNKNOWN_ID)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueNodeLinkItem::Disconnect(ValueLink* input)
   {
      ValueNode* output = mNodeItem->GetNode()->AsValueNode();
      if (!output) return;

      std::string undoDescription = "Disconnection of value link(s) for Value Node \'" +
         mNodeItem->GetNode()->GetName() + "\'.";

      if (!input)
      {
         if (output->GetLinks().size())
         {
            mScene->GetEditor()->GetUndoManager()->BeginMultipleEvents(undoDescription);

            int count = 0;
            while (count < (int)output->GetLinks().size())
            {
               ValueLink* input = output->GetLinks()[count];
               if (input)
               {
                  // Ignore all connected reference value nodes.
                  if (input->GetOwner()->AsValueNode() &&
                      input->GetOwner()->GetType().GetFullName() == "Core.Reference")
                  {
                     count++;
                     continue;
                  }
                  Disconnect(input);
               }
            }

            mScene->GetEditor()->GetUndoManager()->EndMultipleEvents();
         }
      }
      else
      {
         if (input->Disconnect(output))
         {
            dtCore::RefPtr<UndoLinkEvent> event = new UndoLinkEvent(
               mScene->GetEditor(),
               UndoLinkEvent::VALUE_LINK,
               input->GetOwner()->GetID(),
               output->GetID(),
               input->GetName(),
               output->GetName(),
               false);
            event->SetDescription(undoDescription);
            mScene->GetEditor()->GetUndoManager()->AddEvent(event);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueNodeLinkItem::Disconnect(QAction* action)
   {
      if (action->text() == "Disconnect All")
      {
         Disconnect();
         mScene->Refresh();
      }
      else
      {
         ValueNode* output = mNodeItem->GetNode()->AsValueNode();
         if (!output) return;
         int count = (int)output->GetLinks().size();
         for (int index = 0; index < count; index++)
         {
            if (output->GetLinks()[index]->GetName() == action->statusTip().toStdString())
            {
               Disconnect(output->GetLinks()[index]);
               mScene->Refresh();
               break;
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueNodeLinkItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
   {
      SetHighlight(true);

      // Highlight all connected value links.
      if (mNodeItem && mNodeItem->GetNode())
      {
         ValueNode* valueNode = mNodeItem->GetNode()->AsValueNode();
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
                     for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
                     {
                        if (value.link->GetLinks()[linkIndex] == mNodeItem->GetNode())
                        {
                           value.linkGraphic->SetHighlight(true, mNodeItem->GetNode());
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
   void ValueNodeLinkItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
   {
      SetHighlight(false);

      // Remove the highlight to all connected value links.
      if (mNodeItem && mNodeItem->GetNode())
      {
         ValueNode* valueNode = mNodeItem->GetNode()->AsValueNode();
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
                     for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
                     {
                        if (value.link->GetLinks()[linkIndex] == mNodeItem->GetNode())
                        {
                           value.linkGraphic->SetHighlight(false, mNodeItem->GetNode());
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
   LinkItem* ValueNodeLinkItem::GetLinkGraphic()
   {
      return static_cast<ValueItem*>(mNodeItem)->GetValueLink();
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueNodeLinkItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
   {
      if (mHighlight)
      {
         delete mHighlight;
         mHighlight = NULL;
         mDrawing = NULL;
      }

      if (!mouseEvent) return;

      QPointF mousePos = mouseEvent->scenePos();

      // Find and highlight any output links being hovered over.
      QList<QGraphicsItem*> hoverList = mScene->items(mousePos.x(), mousePos.y(), 1, 1);
      if (!hoverList.empty())
      {
         int count = (int)hoverList.size();
         for (int index = 0; index < count; index++)
         {
            if (mAltModifier)
            {
               if (hoverList[index] == this)
               {
                  Disconnect();
                  mScene->Refresh();
                  break;
               }
            }
            else
            {
               ValueLinkItem* item = dynamic_cast<ValueLinkItem*>(hoverList[index]);
               if (item)
               {
                  ValueLink* input = item->mNodeItem->GetValues()[item->mLinkIndex].link;
                  ValueNode* output = mNodeItem->GetNode()->AsValueNode();

                  std::string undoDescription = "Connection of value link between Value Node \'" +
                     mNodeItem->GetNode()->GetTypeName() + "\' and ";

                  if (item->mNodeItem->GetNode())
                  {
                     undoDescription += "Node \'" + item->mNodeItem->GetNode()->GetTypeName() + "\'.";
                  }
                  else if (item->mNodeItem->GetMacro())
                  {
                     undoDescription += "Macro Node \'" + item->mNodeItem->GetMacro()->GetName() + "\'.";
                  }

                  // If this link does not allow multiple connections, then
                  // make sure we disconnect the link from any values first.
                  if (!input->AllowMultiple())
                  {
                     mScene->GetEditor()->GetUndoManager()->BeginMultipleEvents(undoDescription);
                     item->mNodeItem->GetValues()[item->mLinkIndex].linkGraphic->Disconnect();
                  }

                  // Create a new connection between these two links.
                  if (input->Connect(output))
                  {
                     dtCore::RefPtr<UndoLinkEvent> event = new UndoLinkEvent(
                        mScene->GetEditor(),
                        UndoLinkEvent::VALUE_LINK,
                        input->GetOwner()->GetID(),
                        output->GetID(),
                        input->GetName(),
                        output->GetName(),
                        true);
                     event->SetDescription(undoDescription);
                     mScene->GetEditor()->GetUndoManager()->AddEvent(event);
                     emit LinkConnected();
                  }

                  if (!input->AllowMultiple())
                  {
                     mScene->GetEditor()->GetUndoManager()->EndMultipleEvents();
                  }

                  // Refresh the entire scene to make sure all nodes and links are
                  // colored properly.
                  mScene->Refresh();
                  break;
               }
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueNodeLinkItem::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
   {
      if (!mouseEvent || !mHighlight || !mDrawing) return;

      // Update the drawn spline.
      QPointF start(mNodeItem->scenePos());
      QPointF end(mouseEvent->scenePos());

      // Modify the positions based on the translation of the background item.
      QPointF offset = mScene->GetTranslationItem()->scenePos();
      start += offset;
      end += offset;

      start.setX(start.x() + static_cast<ValueItem*>(mNodeItem)->mNodeWidth/2);
      start.setY(start.y() - LINK_LENGTH);

      // Find and highlight any value links being hovered over.
      QList<QGraphicsItem*> hoverList = mScene->items(mouseEvent->scenePos().x(), mouseEvent->scenePos().y(), 1, 1);
      if (!hoverList.empty())
      {
         int count = (int)hoverList.size();
         for (int index = 0; index < count; index++)
         {
            ValueLinkItem* item = dynamic_cast<ValueLinkItem*>(hoverList[index]);
            if (item)
            {
               // Snap the end position to the output link.
               end = QPointF(item->scenePos()) + offset;

               ValueData& data = item->mNodeItem->GetValues()[item->mLinkIndex];

               if (data.link->IsOutLink())
               {
                  end.setY(end.y() + LINK_LENGTH);
               }
               else
               {
                  end.setY(end.y() + LINK_LENGTH - LINK_SIZE/2);
               }
               break;
            }
         }
      }

      QPainterPath path = mNodeItem->CreateConnectionV(end, start);
      mHighlight->setPath(path);
      mDrawing->setPath(path);
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueNodeLinkItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
   {
      ValueNode* output = mNodeItem->GetNode()->AsValueNode();
      if (output)
      {
         QMenu menu;
         QMenu* dcMenu = menu.addMenu("Disconnect");
         dcMenu->addAction("Disconnect All");
         dcMenu->addSeparator();

         int finalCount = 0;
         int count = (int)output->GetLinks().size();
         for (int index = 0; index < count; index++)
         {
            // Ignore all connected reference value nodes.
            if (output->GetLinks()[index]->GetOwner()->AsValueNode() &&
                output->GetLinks()[index]->GetOwner()->GetType().GetFullName() == "Core.Reference")
            {
               continue;
            }

            QString nodeName = output->GetLinks()[index]->GetOwner()->GetTypeName().c_str();
            QString linkName = output->GetLinks()[index]->GetName().c_str();
            QAction* dcAction = dcMenu->addAction(QString("Disconnect from \'") +
               linkName + "\' on node \'" + nodeName + "\'");
            dcAction->setStatusTip(linkName);
            finalCount++;
         }
         dcMenu->setEnabled(finalCount > 0);

         connect(dcMenu, SIGNAL(triggered(QAction*)), this, SLOT(Disconnect(QAction*)));

         menu.exec(event->screenPos());
         return;
      }

      QGraphicsPolygonItem::contextMenuEvent(event);
   }
}

//////////////////////////////////////////////////////////////////////////
