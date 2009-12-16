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

#include <dtDirectorQt/macroitem.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/editorscene.h>
#include <dtDirectorQt/linkitem.h>
#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undopropertyevent.h>

#include <dtDirector/director.h>

#include <QtGui/QGraphicsScene>
#include <QtGui/QMenu>

#include <osg/Vec2>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   MacroItem::MacroItem(DirectorGraph* graph, QGraphicsItem* parent, EditorScene* scene)
       : NodeItem(NULL, parent, scene)
       , mGraph(graph)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void MacroItem::Draw()
   {
      NodeItem::Draw();

      mLoading = true;

      if (mGraph)
      {
         SetTitle(mGraph->mName);
         DrawInputs();
         SetupValues();
         DrawOutputs();

         // Now draw the node.
         DrawTitle();
         DrawValues();

         DrawPolygonTop();
         if (!mOutputs.empty()) DrawPolygonRightFlat();
         else                   DrawPolygonRightRound();
         DrawPolygonBottomFlat();
         if (!mInputs.empty()) DrawPolygonLeftFlat();
         else                  DrawPolygonLeftRound();

         DrawDividers();

         int size = mNodeWidth;
         if (size < mNodeHeight) size = mNodeHeight;

         QRadialGradient radialGradient(mNodeWidth/2, mNodeHeight/2, size, mNodeWidth/2, mNodeHeight/2);
         if (mGraph->GetEnabled())
         {
            setPen(QPen(Qt::darkGray, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

            QColor color = Qt::darkGreen;
            color.setAlphaF(0.80f);
            radialGradient.setColorAt(0.0, color);

            color = Qt::green;
            color.setAlphaF(0.80f);
            radialGradient.setColorAt(1.0, color);
         }
         else
         {
            setPen(QPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

            QColor color = Qt::darkGreen;
            color.setAlphaF(0.25f);
            radialGradient.setColorAt(0.0, color);

            color = Qt::green;
            color.setAlphaF(0.25f);
            radialGradient.setColorAt(1.0, color);
         }

         setBrush(radialGradient);
         setPolygon(mPolygon);

         osg::Vec2 pos = mGraph->GetPosition();
         setPos(pos.x(), pos.y());

         SetComment(mGraph->GetComment());

         if (mGraph->GetComment().empty())
         {
            setToolTip("A Macro.  Double click this node to step in and view this macro's contents.");
         }
         else
         {
            setToolTip(QString("A Macro.  Double click this node to step in and view this macro's contents.") +
               QString("\nComment - ") + QString(mGraph->GetComment().c_str()));
         }
      }

      mLoading = false;
   }

   //////////////////////////////////////////////////////////////////////////
   void MacroItem::FindLinks()
   {
      // First clear all current links.
      NodeItem::FindLinks();

      if (!mGraph.valid()) return;

      std::vector<dtCore::RefPtr<EventNode> > inputs = mGraph->GetInputNodes();
      int count = (int)inputs.size();
      for (int index = 0; index < count; index++)
      {
         if (inputs[index]->GetEnabled())
         {
            mInputs.push_back(InputData());
            InputData& data = mInputs.back();

            data.link = &inputs[index]->GetInputLinks()[0];

            data.linkGraphic = new InputLinkItem(this, (int)mInputs.size()-1, this, mScene);
            data.linkName = new GraphicsTextItem(data.linkGraphic, mScene);
            data.linkName->setAcceptHoverEvents(false);
         }
      }

      std::vector<dtCore::RefPtr<ActionNode> > outputs = mGraph->GetOutputNodes();
      count = (int)outputs.size();
      for (int index = 0; index < count; index++)
      {
         if (outputs[index]->GetEnabled())
         {
            mOutputs.push_back(OutputData());
            OutputData& data = mOutputs.back();

            data.link = &outputs[index]->GetOutputLinks()[0];

            data.linkGraphic = new OutputLinkItem(this, (int)mOutputs.size()-1, this, mScene);
            data.linkName = new GraphicsTextItem(data.linkGraphic, mScene);
            data.linkName->setAcceptHoverEvents(false);
         }
      }

      std::vector<dtCore::RefPtr<ValueNode> > values = mGraph->GetExternalValueNodes();
      count = (int)values.size();
      for (int index = 0; index < count; index++)
      {
         if (values[index]->GetEnabled())
         {
            mValues.push_back(ValueData());
            ValueData& data = mValues.back();

            data.link = &values[index]->GetValueLinks()[0];

            data.linkGraphic = new ValueLinkItem(this, (int)mValues.size()-1, this, mScene);
            data.linkName = new GraphicsTextItem(data.linkGraphic, mScene);
            data.linkName->setAcceptHoverEvents(false);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId MacroItem::GetID()
   {
      if (mGraph.valid())
      {
         return mGraph->GetID();
      }

      return NodeItem::GetID();
   }

   //////////////////////////////////////////////////////////////////////////
   bool MacroItem::HasID(const dtCore::UniqueId& id)
   {
      int count = (int)mInputs.size();
      for (int index = 0; index < count; index++)
      {
         InputData& data = mInputs[index];

         if (data.link->GetOwner()->GetID() == id) return true;
      }

      count = (int)mOutputs.size();
      for (int index = 0; index < count; index++)
      {
         OutputData& data = mOutputs[index];

         if (data.link->GetOwner()->GetID() == id) return true;
      }

      count = (int)mValues.size();
      for (int index = 0; index < count; index++)
      {
         ValueData& data = mValues[index];

         if (data.link->GetOwner()->GetID() == id) return true;
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void MacroItem::BeginMoveEvent()
   {
      dtDAL::ActorProperty* prop = mGraph->GetProperty("Position");
      if (prop) mOldPosition = prop->ToString();
   }

   //////////////////////////////////////////////////////////////////////////
   void MacroItem::EndMoveEvent()
   {
      dtDAL::ActorProperty* prop = mGraph->GetProperty("Position");
      if (prop)
      {
         std::string value = prop->ToString();

         // Ignore the property if the node did not move.
         if (value == mOldPosition) return;

         // Notify the undo manager of the property changes.
         dtCore::RefPtr<UndoPropertyEvent> event = new UndoPropertyEvent(mScene->GetEditor(), mGraph->GetID(), prop->GetName(), mOldPosition, value);
         mScene->GetEditor()->GetUndoManager()->AddEvent(event.get());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void MacroItem::OpenMacro()
   {
      if (!mScene) return;

      // Open the subgraph.
      mScene->GetEditor()->OpenGraph(mGraph);
   }

   //////////////////////////////////////////////////////////////////////////
   void MacroItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
   {
      QMenu menu;
      QAction* stepInAction = menu.addAction("Step Inside Macro");
      connect(stepInAction, SIGNAL(triggered()), this, SLOT(OpenMacro()));
      menu.addSeparator();
      menu.addAction(mScene->GetMacroSelectionAction());
      menu.addSeparator();
      menu.addAction(mScene->GetEditor()->GetCutAction());
      menu.addAction(mScene->GetEditor()->GetCopyAction());
      menu.addSeparator();
      menu.addAction(mScene->GetEditor()->GetShowLinkAction());
      menu.addAction(mScene->GetEditor()->GetHideLinkAction());
      menu.addSeparator();
      menu.addAction(mScene->GetEditor()->GetDeleteAction());
      menu.exec(event->screenPos());
   }

   //////////////////////////////////////////////////////////////////////////
   QVariant MacroItem::itemChange(GraphicsItemChange change, const QVariant &value)
   {
      if (!mGraph.valid()) return value;

      if (change == QGraphicsItem::ItemPositionHasChanged)
      {
         QPointF newPos = value.toPointF();
         mGraph->mPosition.set(newPos.x(), newPos.y());

         if (!mLoading)
         {
            ConnectLinks(true);
         }
      }
      else if (change == QGraphicsItem::ItemSelectedHasChanged)
      {
         if (isSelected())
         {
            setZValue(zValue() + 100.0f);
            mScene->AddSelected(mGraph.get());
         }
         else
         {
            setZValue(zValue() - 100.0f);
            mScene->RemoveSelected(mGraph.get());
         }
      }

      return value;
   }

   //////////////////////////////////////////////////////////////////////////
   void MacroItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
   {
      NodeItem::mouseDoubleClickEvent(event);
      OpenMacro();
   }
}

//////////////////////////////////////////////////////////////////////////
