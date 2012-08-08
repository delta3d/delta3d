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
#include <dtDirectorQt/macroitem.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/editorscene.h>
#include <dtDirectorQt/linkitem.h>
#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undopropertyevent.h>
#include <dtDirectorQt/undolinkvisibilityevent.h>
#include <dtDirectorQt/customeditortool.h>
#include <dtDirectorQt/editornotifier.h>

#include <dtDirector/director.h>

#include <QtGui/QGraphicsScene>
#include <QtGui/QMenu>
#include <QtGui/QGraphicsColorizeEffect>

#include <osg/Vec2>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   MacroItem::MacroItem(DirectorGraph* graph, bool readOnly, bool imported, QGraphicsItem* parent, EditorScene* scene)
       : NodeItem(NULL, readOnly, imported, parent, scene)
       , mGraph(graph)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void MacroItem::Draw()
   {
      NodeItem::Draw();

      mLoading = true;

      if (mGraph.valid())
      {
         SetTitle(GetNodeTitle());
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
         SetBackgroundGradient();

         DrawGlow();
      }

      mLoading = false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MacroItem::DrawGlow()
   {
      if (!mScene)
      {
         return;
      }

      float maxGlow = 0.0f;
      EditorNotifier* notifier = mScene->GetEditor()->GetNotifier();
      if (notifier == NULL)
      {
         return;
      }

      maxGlow = GetMaxGlowForGraph(notifier, mGraph.get());

      // If the graph is imported, find all imported graphs and get their glow values.
      if (mGraph->IsImported())
      {
         std::vector<DirectorGraph*> importedGraphs = mGraph->GetImportedGraphs();
         int count = (int)importedGraphs.size();
         for (int index = 0; index < count; ++index)
         {
            DirectorGraph* graph = importedGraphs[index];
            if (graph)
            {
               float glow = GetMaxGlowForGraph(notifier, graph);
               if (glow > maxGlow)
               {
                  maxGlow = glow;
               }
            }
         }
      }

      if (maxGlow > 0.5f)
      {
         SetHighlight(1.0f);
      }
      else
      {
         SetHighlight(maxGlow * 2.0f);
      }

      std::vector<dtCore::RefPtr<EventNode> > inputs = mGraph->GetInputNodes();
      int count = (int)inputs.size();
      int inputIndex = 0;
      for (int index = 0; index < count; index++)
      {
         EditorNotifier::GlowData* glowData =
            notifier->GetGlowData(inputs[inputIndex]);
         if (glowData && glowData->input > -1)
         {
            InputData& data = mInputs[index];
            data.DrawGlow(glowData->inputGlow);
         }

         inputIndex++;
      }

      std::vector<dtCore::RefPtr<ActionNode> > outputs = mGraph->GetOutputNodes();
      count = (int)outputs.size();
      int outputIndex = 0;
      for (int index = 0; index < count; index++)
      {
         EditorNotifier::GlowData* glowData =
            notifier->GetGlowData(outputs[index]);
         if (glowData && !glowData->outputGlows.empty())
         {
            OutputData& data = mOutputs[outputIndex];
            data.DrawGlow(glowData->outputGlows[0]);
         }

         outputIndex++;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   float MacroItem::GetMaxGlowForGraph(EditorNotifier* notifier, DirectorGraph* graph)
   {
      float maxGlow = 0.0f;

      if (notifier && graph)
      {
         std::vector<Node*> nodes;
         graph->GetAllNodes(nodes);

         int count = (int)nodes.size();
         for (int index = 0; index < count; ++index)
         {
            Node* node = nodes[index];
            if (node)
            {
               EditorNotifier::GlowData* glowData =
                  notifier->GetGlowData(node);

               if (glowData && glowData->glow > maxGlow)
               {
                  maxGlow = glowData->glow;
               }
            }
         }
      }

      return maxGlow;
   }

   //////////////////////////////////////////////////////////////////////////
   std::string MacroItem::GetNodeTitle()
   {
      if (!mGraph)
      {
         return "";
      }

      std::string title = "<i>";
      if (!mGraph->GetEditor().empty())
      {
         title += mGraph->GetEditor() + " ";
      }
      title += "Macro</i>";
      if (mHasHiddenLinks) title += "*";

      std::string name  = mGraph->GetName();
      if (!name.empty())
      {
         title += "<br><b>"+ name +"</b>";
      }

      return title;
   }

   //////////////////////////////////////////////////////////////////////////
   void MacroItem::FindLinks()
   {
      // First clear all current links.
      NodeItem::FindLinks();

      if (!mGraph.valid()) return;

      ID id = mGraph->GetID();
      id.index = -1;

      // First show links from each imported script.
      std::vector<DirectorGraph*> importedGraphs = mGraph->GetImportedGraphs();
      int count = (int)importedGraphs.size();
      for (int index = 0; index < count; ++index)
      {
         DirectorGraph* importedGraph = importedGraphs[index];
         if (importedGraph)
         {
            FindLinks(importedGraph);
         }
      }

      FindLinks(mGraph.get());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MacroItem::FindLinks(DirectorGraph* graph)
   {
      if (!graph)
      {
         return;
      }

      std::vector<dtCore::RefPtr<EventNode> > inputs = graph->GetInputNodes();
      int count = (int)inputs.size();
      for (int index = 0; index < count; index++)
      {
         mInputs.push_back(InputData());
         InputData& data = mInputs.back();

         data.node = this;
         data.link = &inputs[index]->GetInputLinks()[0];
         if (!data.link->GetVisible()) mHasHiddenLinks = true;

         data.linkGraphic = new InputLinkItem(this, (int)mInputs.size()-1, this, mScene, data.link->GetComment());
         data.linkName = new GraphicsTextItem(data.linkGraphic, mScene);
         data.linkName->setAcceptHoverEvents(false);
         if (mIsReadOnly)
         {
            QFont font = data.linkName->font();
            font = QFont(font.family(), font.pointSize(), font.weight(), false);
            data.linkName->setFont(font);
            data.linkName->setDefaultTextColor(Qt::darkGray);
         }
      }

      std::vector<dtCore::RefPtr<ActionNode> > outputs = graph->GetOutputNodes();
      count = (int)outputs.size();
      for (int index = 0; index < count; index++)
      {
         mOutputs.push_back(OutputData());
         OutputData& data = mOutputs.back();

         data.node = this;
         data.link = &outputs[index]->GetOutputLinks()[0];
         if (!data.link->GetVisible()) mHasHiddenLinks = true;

         data.linkGraphic = new OutputLinkItem(this, (int)mOutputs.size()-1, this, mScene, data.link->GetComment());
         data.linkName = new GraphicsTextItem(data.linkGraphic, mScene);
         data.linkName->setAcceptHoverEvents(false);
         if (mIsReadOnly)
         {
            QFont font = data.linkName->font();
            font = QFont(font.family(), font.pointSize(), font.weight(), false);
            data.linkName->setFont(font);
            data.linkName->setDefaultTextColor(Qt::darkGray);
         }
      }

      std::vector<dtCore::RefPtr<ValueNode> > values = graph->GetExternalValueNodes();
      count = (int)values.size();
      for (int index = 0; index < count; index++)
      {
         if (values[index]->GetValueLinks()[0].GetExposed())
         {
            mValues.push_back(ValueData());
            ValueData& data = mValues.back();

            data.node = this;
            data.link = &values[index]->GetValueLinks()[0];
            if (!data.link->GetVisible()) mHasHiddenLinks = true;

            data.linkGraphic = new ValueLinkItem(this, (int)mValues.size()-1, this, mScene, data.link->GetComment());
            data.linkName = new GraphicsTextItem(data.linkGraphic, mScene);
            data.linkName->setAcceptHoverEvents(false);
            if (mIsReadOnly)
            {
               QFont font = data.linkName->font();
               font = QFont(font.family(), font.pointSize(), font.weight(), false);
               data.linkName->setFont(font);
               data.linkName->setDefaultTextColor(Qt::darkGray);
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Vec2 MacroItem::GetPosition()
   {
      return mGraph->GetPosition();
   }

   //////////////////////////////////////////////////////////////////////////
   dtDirector::ID MacroItem::GetID()
   {
      if (mGraph.valid())
      {
         return mGraph->GetID();
      }

      return NodeItem::GetID();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool MacroItem::HasNode(Node* node)
   {
      if (mGraph.valid())
      {
         std::vector<Node*> nodes;
         mGraph->GetAllNodes(nodes, false);

         int count = (int)nodes.size();
         for (int index = 0; index < count; index++)
         {
            Node* testNode = nodes[index];

            if (testNode == node) return true;
         }
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool MacroItem::HasID(const dtDirector::ID& id)
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

      if (mGraph.valid())
      {
         osg::Vec4 rgba = mGraph->GetColor();
         color.setRgbF(rgba.r(), rgba.g(), rgba.b(), rgba.a());

         if (mIsReadOnly)
         {
            color = color.light(150);
         }
      }

      return color;
   }

   //////////////////////////////////////////////////////////////////////////
   void MacroItem::BeginMoveEvent()
   {
      dtCore::ActorProperty* prop = mGraph->GetProperty("Position");
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
            const std::vector<ValueNode*>& nodes = data.link->GetLinks();
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
      dtCore::ActorProperty* prop = mGraph->GetProperty("Position");
      if (prop)
      {
         std::string value = prop->ToString();

         // Ignore the property if the node did not move.
         if (value == mOldPosition) return;

         // Notify the undo manager of the property changes.
         dtCore::RefPtr<UndoPropertyEvent> event = new UndoPropertyEvent(mScene->GetEditor(), mGraph->GetID(), prop->GetName(), mOldPosition, value);
         if (mGraph->GetEditor().empty())
         {
            event->SetDescription("Movement of Macro Node \'" + mGraph->GetName() + "\'.");
         }
         else
         {
            event->SetDescription("Movement of \'" + mGraph->GetEditor() +
               "\' Macro Node \'" + mGraph->GetName() + "\'.");
         }
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
            const std::vector<ValueNode*>& nodes = data.link->GetLinks();
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
      mScene->GetEditor()->OpenGraph(mGraph.get());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MacroItem::OpenCustomTool()
   {
      if (!mScene || !mGraph) return;

      if (!mGraph->GetEditor().empty())
      {
         CustomEditorTool* tool = DirectorEditor::GetRegisteredTool(mGraph->GetEditor());
         if (tool)
         {
            tool->Open(mScene->GetEditor(), mGraph.get());
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MacroItem::ExposeLink(QAction* action)
   {
      if (!action || !mGraph.valid())
      {
         return;
      }

      std::vector<dtCore::RefPtr<ValueNode> > values = mGraph->GetExternalValueNodes();
      int count = (int)values.size();
      for (int index = 0; index < count; ++index)
      {
         ValueNode* valueNode = values[index];
         if (valueNode && !valueNode->GetValueLinks()[0].GetExposed() &&
            valueNode->GetValueLinks()[0].GetName() == action->text().toStdString())
         {
            ValueLink& link = valueNode->GetValueLinks()[0];

            link.SetExposed(true);
            link.SetVisible(true);

            dtCore::RefPtr<UndoLinkVisibilityEvent> event = new UndoLinkVisibilityEvent(mScene->GetEditor(), GetID(), 2, link.GetName(), true, true);
            event->SetDescription("Exposing of value link \'" + link.GetName() + "\' for Macro Node \'" + mGraph->GetName() + "\'.");
            mScene->GetEditor()->GetUndoManager()->AddEvent(event);

            mScene->Refresh();
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
         CustomEditorTool* tool = DirectorEditor::GetRegisteredTool(mGraph->GetEditor());
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
      menu.addAction(mScene->GetGroupSelectionAction());
      menu.addSeparator();
      menu.addAction(mScene->GetEditor()->GetCutAction());
      menu.addAction(mScene->GetEditor()->GetCopyAction());
      menu.addSeparator();
      menu.addMenu(GetLinkVisibilityMenu());
      menu.addSeparator();
      menu.addMenu(GetExposeLinkMenu());
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

      if (event->button() == Qt::LeftButton)
      {
         // If this graph contains a custom editor, add an option to open it with that editor.
         if (!mGraph->GetEditor().empty())
         {
            CustomEditorTool* tool = DirectorEditor::GetRegisteredTool(mGraph->GetEditor());
            if (tool)
            {
               tool->Open(mScene->GetEditor(), mGraph.get());
               return;
            }
         }

         OpenMacro();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   QMenu* MacroItem::GetExposeLinkMenu()
   {
      if (!mGraph.valid())
      {
         return NULL;
      }

      QMenu* menu = new QMenu("Expose Value");
      menu->setDisabled(true);
      connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(ExposeLink(QAction*)));

      std::vector<dtCore::RefPtr<ValueNode> > values = mGraph->GetExternalValueNodes();
      int count = (int)values.size();
      for (int index = 0; index < count; ++index)
      {
         ValueNode* valueNode = values[index];
         if (valueNode &&
            !valueNode->GetValueLinks()[0].GetExposed())
         {
            menu->addAction(valueNode->GetValueLinks()[0].GetName().c_str());
            menu->setEnabled(true);
         }
      }

      return menu;
   }
}

//////////////////////////////////////////////////////////////////////////
