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
#include <dtDirectorQt/undodeleteevent.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/graphtabs.h>
#include <dtDirectorQt/editorview.h>
#include <dtDirectorQt/editorscene.h>

#include <dtDirector/director.h>
#include <dtDirector/directorgraph.h>
#include <dtDirector/nodemanager.h>

#include <QtGui/QGraphicsRectItem>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   UndoDeleteEvent::UndoDeleteEvent(DirectorEditor* editor,
                                    const dtDirector::ID& nodeID,
                                    const dtDirector::ID& parentID)
      : UndoEvent(editor)
      , mParentID(parentID)
      , mNodeID(nodeID)
      , mName("")
      , mCategory("")
   {
      dtCore::PropertyContainer* container = NULL;

      // Memorize all of the current nodes properties, in case we need to
      // undo this event and restore the node.
      Node* node = mEditor->GetDirector()->GetNode(nodeID);
      if (node)
      {
         container = dynamic_cast<dtCore::PropertyContainer*>(node);
         mName = node->GetType().GetName();
         mCategory = node->GetType().GetCategory();

         // Save the link data.
         int count = (int)node->GetInputLinks().size();
         for (int index = 0; index < count; index++)
         {
            SaveLink(node->GetInputLinks()[index]);
         }
         count = (int)node->GetOutputLinks().size();
         for (int index = 0; index < count; index++)
         {
            SaveLink(node->GetOutputLinks()[index]);
         }
         count = (int)node->GetValueLinks().size();
         for (int index = 0; index < count; index++)
         {
            SaveLink(node->GetValueLinks()[index]);
         }

         ValueNode* valueNode = node->AsValueNode();
         if (valueNode)
         {
            count = (int)valueNode->GetLinks().size();
            for (int index = 0; index < count; index++)
            {
               SaveLink(valueNode);
            }
         }
      }

      DirectorGraph* graph = NULL;
      if (!container)
      {
         graph = mEditor->GetDirector()->GetGraph(nodeID);
         container = dynamic_cast<dtCore::PropertyContainer*>(graph);
      }

      // Save the properties of this container.
      if (container)
      {
         std::vector<dtCore::ActorProperty*> propList;
         container->GetPropertyList(propList);

         int count = (int)propList.size();
         for (int index = 0; index < count; index++)
         {
            dtCore::ActorProperty* prop = propList[index];
            if (prop && !prop->IsReadOnly())
            {
               PropData data;
               data.mPropName = prop->GetName();
               data.mValue = prop->ToString();
               mProperties.push_back(data);
            }
         }

         // If the container is another graph, make sure we create
         // delete events for all of the sub nodes.
         if (graph)
         {
            count = (int)graph->GetEventNodes().size();
            for (int index = 0; index < count; index++)
            {
               CreateSubEventForNode(graph->GetEventNodes()[index]);
            }

            count = (int)graph->GetActionNodes().size();
            for (int index = 0; index < count; index++)
            {
               CreateSubEventForNode(graph->GetActionNodes()[index]);
            }

            count = (int)graph->GetValueNodes().size();
            for (int index = 0; index < count; index++)
            {
               CreateSubEventForNode(graph->GetValueNodes()[index]);
            }

            count = (int)graph->GetSubGraphs().size();
            for (int index = 0; index < count; index++)
            {
               CreateSubEventForGraph(graph->GetSubGraphs()[index]);
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   UndoDeleteEvent::~UndoDeleteEvent()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoDeleteEvent::Undo()
   {
      Undo(true);
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoDeleteEvent::Undo(bool isParent)
   {
      DirectorGraph* parent = mEditor->GetDirector()->GetGraph(mParentID);

      dtCore::RefPtr<dtCore::PropertyContainer> container = NULL;
      dtCore::RefPtr<Node> node = NULL;

      // An empty name means the node is a graph.
      if (mName.empty())
      {
         DirectorGraph* graph = new DirectorGraph(mEditor->GetDirector());
         container = graph;

         if (graph && parent)
         {
            // Initialize the graph.
            graph->BuildPropertyMap();

            // Add the graph back to its parent.
            parent->AddGraph(graph);

            // Restore the graphs ID.
            graph->SetID(mNodeID);
         }
      }
      else
      {
         // Create the node.
         node = NodeManager::GetInstance().CreateNode(mName, mCategory, parent);
         container = node.get();

         if (node.valid() && parent)
         {
            // Restore the nodes ID.
            node->SetID(mNodeID);
         }
      }

      // Now restore all the properties.
      if (container)
      {
         int count = (int)mProperties.size();
         for (int index = 0; index < count; index++)
         {
            PropData& data = mProperties[index];
            dtCore::ActorProperty* prop = container->GetProperty(data.mPropName);
            if (prop) prop->FromString(data.mValue);
         }
      }

      // Restore all links.
      RestoreLinks();

      if (node.valid())
      {
         node->OnFinishedLoading();
         if (parent->GetDirector() && parent->GetDirector()->HasStarted())
         {
            node->OnStart();
         }
      }

      // Now execute all sub events.
      int count = (int)mSubEvents.size();
      for (int index = 0; index < count; index++)
      {
         dtCore::RefPtr<UndoDeleteEvent> event = mSubEvents[index];
         if (event.valid())
         {
            event->Undo(false);
         }
      }

      // Save the refreshing for the parent event only.
      if (isParent)
      {
         mEditor->RefreshGraph(parent);
         mEditor->Refresh();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoDeleteEvent::Redo()
   {
      Redo(true);
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoDeleteEvent::Redo(bool isParent)
   {
      // Now execute all sub events.
      int count = (int)mSubEvents.size();
      for (int index = 0; index < count; index++)
      {
         dtCore::RefPtr<UndoDeleteEvent> event = mSubEvents[index];
         if (event.valid())
         {
            event->Redo(false);
         }
      }

      // Remove the node from all UI's
      count = mEditor->GetGraphTabs()->count();
      for (int index = 0; index < count; index++)
      {
         EditorView* view = dynamic_cast<EditorView*>(mEditor->GetGraphTabs()->widget(index));
         if (view && view->GetScene() && view->GetScene()->GetGraph())
         {
            // If the current graph or any of its parents are being deleted,
            // change the current graph to the parent of that parent.
            DirectorGraph* graph = view->GetScene()->GetGraph();
            while (graph)
            {
               if (graph->GetID() == mNodeID)
               {
                  view->GetScene()->SetGraph(graph->GetParent());
                  break;
               }

               graph = graph->GetParent();
            }

            // We need to find the node item that belongs to the scene.
            NodeItem* nodeItem = view->GetScene()->GetNodeItem(mNodeID, true);
            if (nodeItem) view->GetScene()->DeleteNode(nodeItem);
         }
      }

      // Delete the node.
      if (!mEditor->GetDirector()->DeleteGraph(mNodeID))
      {
         mEditor->GetDirector()->DeleteNode(mNodeID);
      }

      // Save the refresh for the parent event only.
      if (isParent)
      {
         DirectorGraph* parent = mEditor->GetDirector()->GetGraph(mParentID);

         // Refresh the editor.
         mEditor->RefreshGraph(parent);
         mEditor->Refresh();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoDeleteEvent::SaveLink(InputLink& link)
   {
      LinkData mainData;
      mainData.mSourceID = link.GetOwner()->GetID();
      mainData.mDestID.clear();
      mainData.mSource = link.GetName();
      mainData.mDest = "";
      mainData.mVisible = link.GetVisible();
      mInputs.push_back(mainData);

      int count = link.GetLinks().size();
      for (int index = 0; index < count; index++)
      {
         OutputLink* destLink = link.GetLinks()[index];
         if (destLink)
         {
            LinkData data;
            data.mSourceID = mainData.mSourceID;
            data.mDestID = destLink->GetOwner()->GetID();

            data.mVisible = mainData.mVisible;

            data.mSource = mainData.mSource;
            data.mDest = destLink->GetName();
            mInputs.push_back(data);
         }
      }

   }

   //////////////////////////////////////////////////////////////////////////
   void UndoDeleteEvent::SaveLink(OutputLink& link)
   {
      LinkData mainData;
      mainData.mSourceID = link.GetOwner()->GetID();
      mainData.mDestID.clear();
      mainData.mSource = link.GetName();
      mainData.mDest = "";
      mainData.mVisible = link.GetVisible();
      mOutputs.push_back(mainData);

      int count = link.GetLinks().size();
      for (int index = 0; index < count; index++)
      {
         InputLink* destLink = link.GetLinks()[index];
         if (destLink)
         {
            LinkData data;
            data.mSourceID = mainData.mSourceID;
            data.mDestID = destLink->GetOwner()->GetID();

            data.mSource = mainData.mSource;
            data.mDest = destLink->GetName();

            data.mVisible = mainData.mVisible;
            mOutputs.push_back(data);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoDeleteEvent::SaveLink(ValueLink& link)
   {
      LinkData mainData;
      mainData.mSourceID = link.GetOwner()->GetID();
      mainData.mDestID.clear();
      mainData.mSource = link.GetName();
      mainData.mDest = "";
      mainData.mVisible = link.GetVisible();
      mValues.push_back(mainData);

      int count = link.GetLinks().size();
      for (int index = 0; index < count; index++)
      {
         ValueNode* destNode = link.GetLinks()[index];
         if (destNode)
         {
            LinkData data;
            data.mSourceID = mainData.mSourceID;
            data.mDestID = destNode->GetID();

            data.mSource = mainData.mSource;
            data.mDest = "";

            data.mVisible = mainData.mVisible;
            mValues.push_back(data);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoDeleteEvent::SaveLink(ValueNode* node)
   {
      if (!node) return;

      int count = (int)node->GetLinks().size();
      for (int index = 0; index < count; index++)
      {
         ValueLink* link = node->GetLinks()[index];
         if (link)
         {
            LinkData data;
            data.mSourceID = link->GetOwner()->GetID();
            data.mDestID = node->GetID();

            data.mSource = link->GetName();
            data.mDest = "";

            data.mVisible = true;
            mValues.push_back(data);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoDeleteEvent::RestoreLinks()
   {
      // Inputs.
      int count = (int)mInputs.size();
      for (int index = 0; index < count; index++)
      {
         LinkData& data = mInputs[index];
         Node* sourceNode = mEditor->GetDirector()->GetNode(data.mSourceID, true);
         Node* destNode = mEditor->GetDirector()->GetNode(data.mDestID, true);

         if (sourceNode)
         {
            InputLink* sourceLink = sourceNode->GetInputLink(data.mSource);
            if (sourceLink) sourceLink->SetVisible(data.mVisible);

            if (destNode)
            {
               OutputLink* destLink = destNode->GetOutputLink(data.mDest);

               if (sourceLink && destLink)
               {
                  sourceLink->Connect(destLink);
               }
            }
         }
      }

      // Outputs.
      count = (int)mOutputs.size();
      for (int index = 0; index < count; index++)
      {
         LinkData& data = mOutputs[index];
         Node* sourceNode = mEditor->GetDirector()->GetNode(data.mSourceID, true);
         Node* destNode = mEditor->GetDirector()->GetNode(data.mDestID, true);

         if (sourceNode)
         {
            OutputLink* sourceLink = sourceNode->GetOutputLink(data.mSource);
            if (sourceLink) sourceLink->SetVisible(data.mVisible);

            if (destNode)
            {
               InputLink* destLink = destNode->GetInputLink(data.mDest);

               if (sourceLink && destLink)
               {
                  sourceLink->Connect(destLink);
               }
            }
         }
      }

      // Values.
      count = (int)mValues.size();
      for (int index = 0; index < count; index++)
      {
         LinkData& data = mValues[index];
         Node* sourceNode = mEditor->GetDirector()->GetNode(data.mSourceID, true);
         Node* destNode = mEditor->GetDirector()->GetNode(data.mDestID, true);

         if (sourceNode)
         {
            ValueLink* sourceLink = sourceNode->GetValueLink(data.mSource);
            if (sourceLink)
            {
               sourceLink->SetVisible(data.mVisible);

               if (destNode)
               {
                  sourceLink->Connect(destNode->AsValueNode());
               }
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoDeleteEvent::CreateSubEventForNode(Node* node)
   {
      if (!node) return;

      dtCore::RefPtr<UndoDeleteEvent> event = new UndoDeleteEvent(mEditor, node->GetID(), node->GetGraph()->GetID());
      mSubEvents.push_back(event);
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoDeleteEvent::CreateSubEventForGraph(DirectorGraph* graph)
   {
      if (!graph) return;

      dtCore::RefPtr<UndoDeleteEvent> event = new UndoDeleteEvent(mEditor, graph->GetID(), graph->GetParent()->GetID());
      mSubEvents.push_back(event);
   }
}

//////////////////////////////////////////////////////////////////////////
