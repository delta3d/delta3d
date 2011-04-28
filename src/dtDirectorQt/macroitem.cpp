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
#include <dtDirectorQt/customeditortool.h>

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
         std::string title = "<i>";
         if (!mGraph->GetEditor().empty())
         {
            title += mGraph->GetEditor() + " ";
         }
         title += "Macro</i>";

         std::string name  = mGraph->GetName();
         if( !name.empty() )
         {
            title += "<br><b>"+ name +"</b>";
         }
         SetTitle(title);
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

         SetDefaultPen();
         SetBackgroundGradient(mNodeHeight);
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
         if (inputs[index]->IsEnabled())
         {
            mInputs.push_back(InputData());
            InputData& data = mInputs.back();

            data.link = &inputs[index]->GetInputLinks()[0];
            if (!data.link->GetVisible()) mHasHiddenLinks = true;

            data.linkGraphic = new InputLinkItem(this, (int)mInputs.size()-1, this, mScene);
            data.linkName = new GraphicsTextItem(data.linkGraphic, mScene);
            data.linkName->setAcceptHoverEvents(false);
         }
      }

      std::vector<dtCore::RefPtr<ActionNode> > outputs = mGraph->GetOutputNodes();
      count = (int)outputs.size();
      for (int index = 0; index < count; index++)
      {
         if (outputs[index]->IsEnabled())
         {
            mOutputs.push_back(OutputData());
            OutputData& data = mOutputs.back();

            data.link = &outputs[index]->GetOutputLinks()[0];
            if (!data.link->GetVisible()) mHasHiddenLinks = true;

            data.linkGraphic = new OutputLinkItem(this, (int)mOutputs.size()-1, this, mScene);
            data.linkName = new GraphicsTextItem(data.linkGraphic, mScene);
            data.linkName->setAcceptHoverEvents(false);
         }
      }

      std::vector<dtCore::RefPtr<ValueNode> > values = mGraph->GetExternalValueNodes();
      count = (int)values.size();
      for (int index = 0; index < count; index++)
      {
         if (values[index]->IsEnabled())
         {
            mValues.push_back(ValueData());
            ValueData& data = mValues.back();

            data.link = &values[index]->GetValueLinks()[0];
            if (!data.link->GetVisible()) mHasHiddenLinks = true;

            data.linkGraphic = new ValueLinkItem(this, (int)mValues.size()-1, this, mScene);
            data.linkName = new GraphicsTextItem(data.linkGraphic, mScene);
            data.linkName->setAcceptHoverEvents(false);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Vec2 MacroItem::GetPosition()
   {
      return mGraph->GetPosition();
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
   QColor MacroItem::GetNodeColor() const
   {
      QColor color;

      if (mGraph)
      {
         osg::Vec4 rgba = mGraph->GetColor();
         color.setRgbF(rgba.r(), rgba.g(), rgba.b(), rgba.a());
      }

      return color;
   }

   //////////////////////////////////////////////////////////////////////////
   void MacroItem::BeginMoveEvent()
   {
      dtDAL::ActorProperty* prop = mGraph->GetProperty("Position");
      if (prop) mOldPosition = prop->ToString();

      mChainSelecting = mScene->IsHoldingShift();
      if (mChainSelecting)
      {
         mScene->BeginBatchSelection();
         int outCount = (int)mOutputs.size();
         for (int outIndex = 0; outIndex < outCount; ++outIndex)
         {
            OutputData& data = mOutputs[outIndex];
            std::vector<InputLink*>& links = data.link->GetLinks();
            int linkCount = (int)links.size();
            for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
            {
               InputLink* link = links[linkIndex];
               if (link)
               {
                  dtDirector::NodeItem* item = mScene->GetNodeItem(link->GetOwner()->GetID());
                  if (item && !item->isSelected())
                  {
                     item->setSelected(true);
                     item->BeginMoveEvent();
                  }
               }
            }
         }
         int valCount = (int)mValues.size();
         for (int valIndex = 0; valIndex < valCount; ++valIndex)
         {
            ValueData& data = mValues[valIndex];
            std::vector<ValueNode*>& nodes = data.link->GetLinks();
            int nodeCount = (int)nodes.size();
            for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
            {
               ValueNode* node = nodes[nodeIndex];
               if (node)
               {
                  dtDirector::NodeItem* item = mScene->GetNodeItem(node->GetID());
                  if (item && !item->isSelected())
                  {
                     item->setSelected(true);
                     item->BeginMoveEvent();
                  }
               }
            }
         }
         mScene->EndBatchSelection();
      }
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

      if (mChainSelecting)
      {
         mScene->BeginBatchSelection();
         int outCount = (int)mOutputs.size();
         for (int outIndex = 0; outIndex < outCount; ++outIndex)
         {
            OutputData& data = mOutputs[outIndex];
            std::vector<InputLink*>& links = data.link->GetLinks();
            int linkCount = (int)links.size();
            for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
            {
               InputLink* link = links[linkIndex];
               if (link)
               {
                  dtDirector::NodeItem* item = mScene->GetNodeItem(link->GetOwner()->GetID());
                  if (item && item->isSelected())
                  {
                     item->setSelected(false);
                     item->EndMoveEvent();
                  }
               }
            }
         }
         int valCount = (int)mValues.size();
         for (int valIndex = 0; valIndex < valCount; ++valIndex)
         {
            ValueData& data = mValues[valIndex];
            std::vector<ValueNode*>& nodes = data.link->GetLinks();
            int nodeCount = (int)nodes.size();
            for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
            {
               ValueNode* node = nodes[nodeIndex];
               if (node)
               {
                  dtDirector::NodeItem* item = mScene->GetNodeItem(node->GetID());
                  if (item && item->isSelected())
                  {
                     item->setSelected(false);
                     item->EndMoveEvent();
                  }
               }
            }
         }
         mScene->EndBatchSelection();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void MacroItem::OpenMacro()
   {
      if (!mScene) return;

      // Open the subgraph.
      mScene->GetEditor()->OpenGraph(mGraph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MacroItem::OpenCustomTool()
   {
      if (!mScene || !mGraph) return;

      if (!mGraph->GetEditor().empty())
      {
         CustomEditorTool* tool = mScene->GetEditor()->GetRegisteredTool(mGraph->GetEditor());
         if (tool)
         {
            tool->Open(mGraph.get());
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void MacroItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
   {
      setSelected(true);
      QMenu menu;

      bool hasDefault = false;
      // If this graph contains a custom editor, add an option to open it with that editor.
      if (!mGraph->GetEditor().empty())
      {
         CustomEditorTool* tool = mScene->GetEditor()->GetRegisteredTool(mGraph->GetEditor());
         if (tool)
         {
            QAction* useCustomToolAction = menu.addAction(QString("Open with \'") + mGraph->GetEditor().c_str() + QString("\' Editor"));
            connect(useCustomToolAction, SIGNAL(triggered()), this, SLOT(OpenCustomTool()));
            menu.setDefaultAction(useCustomToolAction);
            hasDefault = true;
         }
      }

      QAction* stepInAction = menu.addAction("Step Inside Macro");
      connect(stepInAction, SIGNAL(triggered()), this, SLOT(OpenMacro()));
      if (!hasDefault) menu.setDefaultAction(stepInAction);
      menu.addSeparator();
      menu.addAction(mScene->GetMacroSelectionAction());
      if (!mScene->GetSelection().empty())
      {
         QAction* createGroupAction = menu.addAction("Create Group Around Selection");
         connect(createGroupAction, SIGNAL(triggered()), mScene, SLOT(OnCreateGroupForSelection()));
      }
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
         mGraph->SetPosition(osg::Vec2(newPos.x(), newPos.y()));

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

      // If this graph contains a custom editor, add an option to open it with that editor.
      if (!mGraph->GetEditor().empty())
      {
         CustomEditorTool* tool = mScene->GetEditor()->GetRegisteredTool(mGraph->GetEditor());
         if (tool)
         {
            OpenCustomTool();
         }
      }
      else
      {
         OpenMacro();
      }
   }
}

//////////////////////////////////////////////////////////////////////////
