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

#include <dtDirector/directorgraph.h>
#include <dtDirector/director.h>
#include <dtDirector/colors.h>

#include <dtCore/actorproperty.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/stringactorproperty.h>
#include <dtCore/vectoractorproperties.h>
#include <dtCore/colorrgbaactorproperty.h>


namespace dtDirector
{
   dtCore::RefPtr<dtCore::ObjectType> DirectorGraph::TYPE(new dtCore::ObjectType("DirectorGraph","dtDirector"));
   const dtCore::ObjectType& DirectorGraph::GetObjectType() const { return *TYPE; }

   //////////////////////////////////////////////////////////////////////////
   DirectorGraph::DirectorGraph(Director* director)
      : mDirector(director)
      , mParent(NULL)
      , mComment("")
      , mEditor("")
      , mIsImported(false)
      , mIsReadOnly(false)
   {
      SetColorRGB(Colors::GREEN);

      if (director)
      {
         director->MasterListAddGraph(this);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   DirectorGraph::~DirectorGraph()
   {
      int count = (int)mSubGraphs.size();
      for (int index = 0; index < count; ++index)
      {
         DirectorGraph* subGraph = mSubGraphs[index];
         if (subGraph)
         {
            subGraph->SetParent(NULL);
         }
      }
      mSubGraphs.clear();

      if (mDirector)
      {
         count = (int)mEventNodes.size();
         for (int index = 0; index < count; ++index)
         {
            mDirector->MasterListRemoveNode(mEventNodes[index]);
         }
      }
      mEventNodes.clear();

      if (mDirector)
      {
         count = (int)mActionNodes.size();
         for (int index = 0; index < count; ++index)
         {
            mDirector->MasterListRemoveNode(mActionNodes[index]);
         }
         mActionNodes.clear();
      }

      if (mDirector)
      {
         count = (int)mValueNodes.size();
         for (int index = 0; index < count; ++index)
         {
            mDirector->MasterListRemoveNode(mValueNodes[index]);
         }
         mValueNodes.clear();
      }

      if (mDirector)
      {
         mDirector->MasterListRemoveGraph(this);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<DirectorGraph> DirectorGraph::Clone(Director* parent)
   {
      DirectorGraph* newGraph = new DirectorGraph(parent);
      if (!newGraph)
      {
         return NULL;
      }

      newGraph->BuildPropertyMap(true);

      newGraph->SetID(mID);
      newGraph->SetImported(IsImported());
      newGraph->SetReadOnly(IsReadOnly());
      newGraph->CopyPropertiesFrom(*this);

      parent->SetGraphRoot(newGraph);
      InternalClone(newGraph);

      return newGraph;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<DirectorGraph> DirectorGraph::Clone(DirectorGraph* parent)
   {
      DirectorGraph* newGraph = new DirectorGraph(parent->GetDirector());
      if (!newGraph)
      {
         return NULL;
      }

      newGraph->BuildPropertyMap(false);

      newGraph->SetID(mID);
      newGraph->SetImported(IsImported());
      newGraph->SetReadOnly(IsReadOnly());
      newGraph->CopyPropertiesFrom(*this);

      parent->AddGraph(newGraph);
      InternalClone(newGraph);

      return newGraph;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorGraph::InternalClone(DirectorGraph* newGraph)
   {
      if (!newGraph)
      {
         return;
      }

      Director* script = newGraph->GetDirector();

      std::vector<dtCore::RefPtr<Node> > allNodes;

      // Clone Event Nodes.
      std::vector<dtCore::RefPtr<EventNode> > eventNodes = GetEventNodes();
      int count = (int)eventNodes.size();
      for (int index = 0; index < count; ++index)
      {
         Node* node = eventNodes[index];
         if (node)
         {
            node->Clone(newGraph);
            allNodes.push_back(node);
         }
      }

      // Clone Action Nodes.
      std::vector<dtCore::RefPtr<ActionNode> > actionNodes = GetActionNodes();
      count = (int)actionNodes.size();
      for (int index = 0; index < count; ++index)
      {
         Node* node = actionNodes[index];
         if (node)
         {
            node->Clone(newGraph);
            allNodes.push_back(node);
         }
      }

      // Clone Value Nodes.
      std::vector<dtCore::RefPtr<ValueNode> > valueNodes = GetValueNodes();
      count = (int)valueNodes.size();
      for (int index = 0; index < count; ++index)
      {
         Node* node = valueNodes[index];
         if (node)
         {
            node->Clone(newGraph);
            allNodes.push_back(node);
         }
      }

      // Clone sub-Graphs.
      std::vector<dtCore::RefPtr<DirectorGraph> > subGraphs = GetSubGraphs();
      count = (int)subGraphs.size();
      for (int index = 0; index < count; ++index)
      {
         DirectorGraph* graph = subGraphs[index];
         if (graph)
         {
            graph->Clone(newGraph);
         }
      }

      // Connect links together again.
      count = (int)allNodes.size();
      for (int index = 0; index < count; ++index)
      {
         // Get the original node.
         Node* node = allNodes[index];
         if (node)
         {
            // Find the matching cloned node.
            Node* newNode = script->GetNode(node->GetID(), true);
            if (newNode)
            {
               // Iterate through each input link on the original node.
               int linkCount = (int)node->GetInputLinks().size();
               for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
               {
                  InputLink& link = node->GetInputLinks()[linkIndex];
                  InputLink& newLink = newNode->GetInputLinks()[linkIndex];

                  // Iterate through each connection for this link.
                  int targetCount = (int)link.GetLinks().size();
                  for (int targetIndex = 0; targetIndex < targetCount; ++targetIndex)
                  {
                     OutputLink* targetLink = link.GetLinks()[targetIndex];
                     if (targetLink)
                     {
                        // Find the matching cloned target node.
                        Node* newTargetNode = script->GetNode(targetLink->GetOwner()->GetID(), true);
                        if (newTargetNode)
                        {
                           // Make the connection.
                           newLink.Connect(newTargetNode->GetOutputLink(targetLink->GetName()));
                        }
                     }
                  }
               }

               // Iterate through each output link on the original node.
               linkCount = (int)node->GetOutputLinks().size();
               for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
               {
                  OutputLink& link = node->GetOutputLinks()[linkIndex];
                  OutputLink& newLink = newNode->GetOutputLinks()[linkIndex];

                  // Iterate through each connection for this link.
                  int targetCount = (int)link.GetLinks().size();
                  for (int targetIndex = 0; targetIndex < targetCount; ++targetIndex)
                  {
                     InputLink* targetLink = link.GetLinks()[targetIndex];
                     if (targetLink)
                     {
                        // Find the matching cloned target node.
                        Node* newTargetNode = script->GetNode(targetLink->GetOwner()->GetID(), true);
                        if (newTargetNode)
                        {
                           // Make the connection.
                           newLink.Connect(newTargetNode->GetInputLink(targetLink->GetName()));
                        }
                     }
                  }
               }

               // Iterate through each value link on the original node.
               linkCount = (int)node->GetValueLinks().size();
               for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
               {
                  ValueLink& link = node->GetValueLinks()[linkIndex];
                  ValueLink& newLink = newNode->GetValueLinks()[linkIndex];

                  // Iterate through each connection for this link.
                  int targetCount = (int)link.GetLinks().size();
                  for (int targetIndex = 0; targetIndex < targetCount; ++targetIndex)
                  {
                     ValueNode* targetNode = link.GetLinks()[targetIndex];
                     if (targetNode)
                     {
                        // Find the matching cloned target node.
                        Node* newTargetNode = script->GetNode(targetNode->GetID(), true);
                        if (newTargetNode && newTargetNode->AsValueNode())
                        {
                           // Make the connection.
                           newLink.Connect(newTargetNode->AsValueNode());
                        }
                     }
                  }
               }

               newNode->OnFinishedLoading();
            }
         }
      }

      count = (int)subGraphs.size();
      for (int index = 0; index < count; ++index)
      {
         DirectorGraph* graph = subGraphs[index];
         if (graph)
         {
            // Iterate through all the output link nodes within the graph.
            std::vector<dtCore::RefPtr<ActionNode> > outputNodes = graph->GetOutputNodes();
            int linkCount = (int)outputNodes.size();
            for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
            {
               Node* linkNode = outputNodes[linkIndex];

               if (linkNode)
               {
                  Node* newLinkNode = script->GetNode(linkNode->GetID(), true);
                  if (newLinkNode == NULL)
                  {
                     LOG_ALWAYS("Could not find link node in new graph when cloning script.");
                     continue;
                  }

                  // Iterate through the connections for this output node.
                  int targetCount = (int)linkNode->GetOutputLinks()[0].GetLinks().size();
                  for (int targetIndex = 0; targetIndex < targetCount; ++targetIndex)
                  {
                     InputLink* targetLink = linkNode->GetOutputLinks()[0].GetLinks()[targetIndex];
                     if (targetLink)
                     {
                        // Find the matching cloned target node.
                        Node* newTargetNode = script->GetNode(targetLink->GetOwner()->GetID(), true);
                        if (newTargetNode)
                        {
                           // Make the connection.
                           newLinkNode->GetOutputLinks()[0].Connect(newTargetNode->GetInputLink(targetLink->GetName()));
                        }
                     }
                  }
               }
            }

            // Iterate through all the value link nodes within the graph.
            std::vector<dtCore::RefPtr<ValueNode> > valueNodes = graph->GetExternalValueNodes();
            linkCount = (int)valueNodes.size();
            for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
            {
               Node* linkNode = valueNodes[linkIndex];

               if (linkNode)
               {
                  Node* newLinkNode = script->GetNode(linkNode->GetID(), true);

                  // Iterate through the connections for this value node.
                  int targetCount = (int)linkNode->GetValueLinks()[0].GetLinks().size();
                  for (int targetIndex = 0; targetIndex < targetCount; ++targetIndex)
                  {
                     ValueNode* targetNode = linkNode->GetValueLinks()[0].GetLinks()[targetIndex];
                     if (targetNode)
                     {
                        // Find the matching cloned target node.
                        Node* newTargetNode = script->GetNode(targetNode->GetID(), true);
                        if (newTargetNode && newTargetNode->AsValueNode())
                        {
                           // Make the connection.
                           newLinkNode->GetValueLinks()[0].Connect(newTargetNode->AsValueNode());
                        }
                     }
                  }
               }
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DirectorGraph::ShouldPropertySave(const dtCore::ActorProperty& prop) const
   {
      return !IsReadOnly() && !IsImported();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorGraph::BuildPropertyMap(bool isParent)
   {
      AddProperty(new dtCore::StringActorProperty(
         "Comment", "Comment",
         dtCore::StringActorProperty::SetFuncType(this, &DirectorGraph::SetComment),
         dtCore::StringActorProperty::GetFuncType(this, &DirectorGraph::GetComment),
         "Comment"));

      AddProperty(new dtCore::StringActorProperty(
         "Name", "Name",
         dtCore::StringActorProperty::SetFuncType(this, &DirectorGraph::SetName),
         dtCore::StringActorProperty::GetFuncType(this, &DirectorGraph::GetName),
         "The Name of the Director graph."));

      AddProperty(new dtCore::StringActorProperty(
         "Custom Editor", "Custom Editor",
         dtCore::StringActorProperty::SetFuncType(this, &DirectorGraph::SetEditor),
         dtCore::StringActorProperty::GetFuncType(this, &DirectorGraph::GetEditor),
         "The custom editor for use with this Director graph."));

      // Only sub graphs have a position.
      if (!isParent)
      {
         AddProperty(new dtCore::Vec2ActorProperty(
            "Position", "Position",
            dtCore::Vec2ActorProperty::SetFuncType(this, &DirectorGraph::SetPosition),
            dtCore::Vec2ActorProperty::GetFuncType(this, &DirectorGraph::GetPosition),
            "The Position of the Director graph in its parent.", "UI"));

         AddProperty(new dtCore::ColorRgbaActorProperty(
            "Color", "Color",
            dtCore::ColorRgbaActorProperty::SetFuncType(this, &DirectorGraph::SetColor),
            dtCore::ColorRgbaActorProperty::GetFuncType(this, &DirectorGraph::GetColor),
            "The UI color of the Node."));
     }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DirectorGraph::SetID(const ID& id)
   {
      bool result = true;
      if (id.index != mID.index)
      {
         result = SetIDIndex(id.index);
      }

      SetID(id.id);
      return result;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DirectorGraph::SetIDIndex(int index)
   {
      mDirector->MasterListRemoveGraph(this);

      return mDirector->MasterListAddGraph(this, index);
   }

   //////////////////////////////////////////////////////////////////////////
   DirectorGraph* DirectorGraph::GetGraph(const ID& id)
   {
      if (mDirector)
      {
         return mDirector->GetGraph(id);
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   Node* DirectorGraph::GetNode(const ID& id)
   {
      if (mDirector)
      {
         Node* node = mDirector->GetNode(id);

         // If this node exists in the script, make sure it is within this graph.
         if (node)
         {
            DirectorGraph* parent = node->GetGraph();
            while (parent)
            {
               if (parent == this)
               {
                  return node;
               }

               parent = parent->GetParent();
            }
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   const Node* DirectorGraph::GetNode(const ID& id) const
   {
      if (mDirector)
      {
         const Node* node = mDirector->GetNode(id);

         // If this node exists in the script, make sure it is within this graph.
         if (node)
         {
            const DirectorGraph* parent = node->GetGraph();
            while (parent)
            {
               if (parent == this)
               {
                  return node;
               }

               parent = parent->GetParent();
            }
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorGraph::GetNodes(const std::string& name, const std::string& category, std::vector<Node*>& outNodes, bool searchSubGraphs, bool searchImportedGraphs /*= false*/)
   {
      if (searchImportedGraphs)
      {
         std::vector<DirectorGraph*> importedGraphs = GetImportedGraphs();
         int count = (int)importedGraphs.size();
         for (int index = 0; index < count; ++index)
         {
            DirectorGraph* importedGraph = importedGraphs[index];
            if (importedGraph)
            {
               importedGraph->GetNodes(name, category, outNodes, searchSubGraphs, searchImportedGraphs);
            }
         }
      }

      int count = (int)mEventNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mEventNodes[index]->GetType().GetName() == name &&
            mEventNodes[index]->GetType().GetCategory() == category)
         {
             outNodes.push_back(mEventNodes[index]);
         }
      }

      count = (int)mActionNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mActionNodes[index]->GetType().GetName() == name &&
            mActionNodes[index]->GetType().GetCategory() == category)
         {
            outNodes.push_back(mActionNodes[index]);
         }
      }

      count = (int)mValueNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mValueNodes[index]->GetType().GetName() == name &&
            mValueNodes[index]->GetType().GetCategory() == category)
         {
            outNodes.push_back(mValueNodes[index]);
         }
      }

      if (searchSubGraphs)
      {
         count = (int)mSubGraphs.size();
         for (int index = 0; index < count; index++)
         {
            DirectorGraph* graph = mSubGraphs[index];
            if (graph)
            {
               graph->GetNodes(name, category, outNodes);
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorGraph::GetNodes(const std::string& name, const std::string& category, const std::string& property, const std::string& value, std::vector<Node*>& outNodes, bool searchSubGraphs, bool searchImportedGraphs /*= false*/)
   {
      if (searchImportedGraphs)
      {
         std::vector<DirectorGraph*> importedGraphs = GetImportedGraphs();
         int count = (int)importedGraphs.size();
         for (int index = 0; index < count; ++index)
         {
            DirectorGraph* importedGraph = importedGraphs[index];
            if (importedGraph)
            {
               importedGraph->GetNodes(name, category, property, value, outNodes, searchSubGraphs, searchImportedGraphs);
            }
         }
      }

      std::vector<Node*> nodes;
      GetNodes(name, category, nodes, searchSubGraphs);
      int count = (int)nodes.size();
      for (int index = 0; index < count; index++)
      {
         Node* node = nodes[index];
         dtCore::ActorProperty* prop = node->GetProperty(property);
         if (prop && prop->ToString() == value)
         {
            outNodes.push_back(node);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorGraph::GetAllNodes(std::vector<Node*>& outNodes, bool searchSubGraphs, bool searchImportedGraphs)
   {
      if (searchImportedGraphs)
      {
         std::vector<DirectorGraph*> importedGraphs = GetImportedGraphs();
         int count = (int)importedGraphs.size();
         for (int index = 0; index < count; ++index)
         {
            DirectorGraph* importedGraph = importedGraphs[index];
            if (importedGraph)
            {
               importedGraph->GetAllNodes(outNodes, false, false);
            }
         }
      }

      int count = (int)mEventNodes.size();
      for (int index = 0; index < count; index++)
      {
         outNodes.push_back(mEventNodes[index]);
      }

      count = (int)mActionNodes.size();
      for (int index = 0; index < count; index++)
      {
         outNodes.push_back(mActionNodes[index]);
      }

      count = (int)mValueNodes.size();
      for (int index = 0; index < count; index++)
      {
         outNodes.push_back(mValueNodes[index]);
      }

      if (searchSubGraphs)
      {
         count = (int)mSubGraphs.size();
         for (int index = 0; index < count; index++)
         {
            DirectorGraph* graph = mSubGraphs[index];
            if (graph)
            {
               graph->GetAllNodes(outNodes);
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   ValueNode* DirectorGraph::GetValueNode(const std::string& name, bool searchSubgraphs, bool searchImportedGraph)
   {
      if (!GetDirector())
      {
         return NULL;
      }

      if (searchImportedGraph && (IsImported() || mDirector->GetGraphRoot() == this))
      {
         std::vector<DirectorGraph*> importedGraphs = GetImportedGraphs();
         int count = (int)importedGraphs.size();
         for (int index = 0; index < count; ++index)
         {
            DirectorGraph* importedGraph = importedGraphs[index];
            if (importedGraph)
            {
               ValueNode* foundNode = importedGraph->GetValueNode(name, searchSubgraphs, true);
               if (foundNode)
               {
                  return foundNode;
               }
            }
         }
      }

      int count = (int)mValueNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mValueNodes[index] && mValueNodes[index]->GetName() == name)
         {
            return mValueNodes[index];
         }
      }

      if (searchSubgraphs)
      {
         count = (int)mSubGraphs.size();
         for (int index = 0; index < count; index++)
         {
            DirectorGraph* graph = mSubGraphs[index];
            if (graph)
            {
               ValueNode* node = graph->GetValueNode(name, true, searchImportedGraph);
               if (node) return node;
            }
         }
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorGraph::SetParent(DirectorGraph* parent)
   {
      if (mDirector)
      {
         mDirector->MasterListRemoveGraph(this);
      }

      mParent = parent;

      if (mParent)
      {
         mDirector = mParent->GetDirector();

         if (mDirector)
         {
            mDirector->MasterListAddGraph(this);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DirectorGraph::AddNode(Node* node)
   {
      bool result = false;
      ValueNode* valueNode = node->AsValueNode();
      if (valueNode)
      {
         mValueNodes.push_back(valueNode);
         result = true;
      }

      EventNode* eventNode = node->AsEventNode();
      if (eventNode)
      {
         mEventNodes.push_back(eventNode);
         result = true;
      }

      ActionNode* actionNode = node->AsActionNode();
      if (actionNode)
      {
         mActionNodes.push_back(actionNode);
         result = true;
      }

      if (mDirector)
      {
         mDirector->MasterListAddNode(node);
      }

      return result;
   }

   //////////////////////////////////////////////////////////////////////////
   bool DirectorGraph::DeleteGraph(const ID& id)
   {
      // Check sub graphs
      int count = (int)mSubGraphs.size();
      for (int index = 0; index < count; index++)
      {
         DirectorGraph* graph = mSubGraphs[index];
         if (graph && graph->GetID() == id)
         {
            // Remove the sub graph.
            if (mDirector)
            {
               mDirector->MasterListRemoveGraph(graph);
            }

            mSubGraphs.erase(mSubGraphs.begin() + index);

            return true;
         }

         // Recurse through sub graphs.
         if (graph->DeleteGraph(id))
         {
            return true;
         }
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool DirectorGraph::DeleteNode(const ID& id)
   {
      // Search for the node and remove it from the list.
      int count = (int)mEventNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mEventNodes[index]->GetID() == id)
         {
            if (mDirector)
            {
               mDirector->MasterListRemoveNode(mEventNodes[index]);
            }
            mEventNodes.erase(mEventNodes.begin() + index);
            return true;
         }
      }

      count = (int)mActionNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mActionNodes[index]->GetID() == id)
         {
            if (mDirector)
            {
               mDirector->MasterListRemoveNode(mActionNodes[index]);
            }
            mActionNodes.erase(mActionNodes.begin() + index);
            return true;
         }
      }

      count = (int)mValueNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mValueNodes[index]->GetID() == id)
         {
            if (mDirector)
            {
               mDirector->MasterListRemoveNode(mValueNodes[index]);
            }
            mValueNodes.erase(mValueNodes.begin() + index);
            return true;
         }
      }

      // Check sub graphs
      count = (int)mSubGraphs.size();
      for (int index = 0; index < count; index++)
      {
         if (mSubGraphs[index]->DeleteNode(id))
         {
            return true;
         }
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   DirectorGraph* DirectorGraph::AddGraph(DirectorGraph* graph)
   {
      if (!graph)
      {
         graph = new DirectorGraph(GetDirector());
         graph->BuildPropertyMap();
      }

      // First remove the graph from its parent.
      if (graph->mParent)
      {
         graph->mParent->RemoveGraph(graph);
      }

      graph->SetParent(this);
      mSubGraphs.push_back(graph);

      return graph;
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorGraph::RemoveGraph(DirectorGraph* graph)
   {
      if (!graph) return;

      if (graph->mParent == this)
      {
         int count = (int)mSubGraphs.size();
         for (int index = 0; index < count; index++)
         {
            if (mSubGraphs[index] == graph)
            {
               graph->SetParent(NULL);
               mSubGraphs.erase(mSubGraphs.begin() + index);
               return;
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   std::vector<dtCore::RefPtr<EventNode> > DirectorGraph::GetInputNodes()
   {
      std::vector<dtCore::RefPtr<EventNode> > nodes;

      // Search all event nodes for input event nodes.
      int count = (int)mEventNodes.size();
      for (int index = 0; index < count; index++)
      {
         EventNode* node = mEventNodes[index].get();
         if (node && node->GetType().GetFullName() == "Core.Input Link")
         {
            bool bFound = false;
            int sortCount = (int)nodes.size();
            for (int sortIndex = 0; sortIndex < sortCount; sortIndex++)
            {
               EventNode* sortNode = nodes[sortIndex];
               if (sortNode && sortNode->GetPosition().y() >= node->GetPosition().y())
               {
                  bFound = true;
                  nodes.insert(nodes.begin() + sortIndex, node);
                  break;
               }
            }

            if (!bFound) nodes.push_back(node);
         }
      }

      return nodes;
   }

   //////////////////////////////////////////////////////////////////////////
   std::vector<dtCore::RefPtr<ActionNode> > DirectorGraph::GetOutputNodes()
   {
      std::vector<dtCore::RefPtr<ActionNode> > nodes;

      // Search all action nodes for output action nodes.
      int count = (int)mActionNodes.size();
      for (int index = 0; index < count; index++)
      {
         ActionNode* node = mActionNodes[index].get();
         if (node && node->GetType().GetFullName() == "Core.Output Link")
         {
            bool bFound = false;
            int sortCount = (int)nodes.size();
            for (int sortIndex = 0; sortIndex < sortCount; sortIndex++)
            {
               ActionNode* sortNode = nodes[sortIndex];
               if (sortNode && sortNode->GetPosition().y() >= node->GetPosition().y())
               {
                  bFound = true;
                  nodes.insert(nodes.begin() + sortIndex, node);
                  break;
               }
            }

            if (!bFound) nodes.push_back(node);
         }
      }

      return nodes;
   }

   //////////////////////////////////////////////////////////////////////////
   std::vector<dtCore::RefPtr<ValueNode> > DirectorGraph::GetExternalValueNodes()
   {
      std::vector<dtCore::RefPtr<ValueNode> > nodes;

      // Search all value nodes for external value nodes.
      int count = (int)mValueNodes.size();
      for (int index = 0; index < count; index++)
      {
         // An external ValueLink is a ValueNode-derivate with node-type LINK_NODE
         ValueNode* node = mValueNodes[index].get();
         if (node && node->GetType().GetNodeType() == NodeType::LINK_NODE)
         {
            bool bFound = false;
            int sortCount = (int)nodes.size();
            for (int sortIndex = 0; sortIndex < sortCount; sortIndex++)
            {
               ValueNode* sortNode = nodes[sortIndex];
               if (sortNode && sortNode->GetPosition().x() >= node->GetPosition().x())
               {
                  bFound = true;
                  nodes.insert(nodes.begin() + sortIndex, node);
                  break;
               }
            }

            if (!bFound) nodes.push_back(node);
         }
      }

      return nodes;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DirectorGraph::IsImported() const
   {
      return mIsImported;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DirectorGraph::IsReadOnly() const
   {
      return mIsReadOnly;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<DirectorGraph*> DirectorGraph::GetImportedGraphs()
   {
      std::vector<DirectorGraph*> importedGraphs;

      const std::vector<dtCore::RefPtr<Director> >& importedScripts = mDirector->GetImportedScriptList();
      int count = (int)importedScripts.size();

      if (mDirector->GetGraphRoot() == this)
      {
         for (int index = 0; index < count; ++index)
         {
            Director* importedScript = importedScripts[index];
            if (importedScript)
            {
               DirectorGraph* importedGraph = importedScript->GetGraphRoot();
               if (importedGraph)
               {
                  importedGraphs.push_back(importedGraph);

                  // Now recurse up the import chain.
                  std::vector<DirectorGraph*> recursedGraphs = importedGraph->GetImportedGraphs();
                  if (!recursedGraphs.empty())
                  {
                     importedGraphs.insert(importedGraphs.end(), recursedGraphs.begin(), recursedGraphs.end());
                  }
               }
            }
         }
      }
      else
      {
         ID id = GetID();
         id.index = -1;

         for (int index = 0; index < count; ++index)
         {
            Director* importedScript = importedScripts[index];
            if (importedScript)
            {
               DirectorGraph* importedGraph = importedScript->GetGraph(id);
               if (importedGraph)
               {
                  importedGraphs.push_back(importedGraph);

                  // Now recurse up the import chain.
                  std::vector<DirectorGraph*> recursedGraphs = importedGraph->GetImportedGraphs();
                  if (!recursedGraphs.empty())
                  {
                     importedGraphs.insert(importedGraphs.end(), recursedGraphs.begin(), recursedGraphs.end());
                  }
               }
            }
         }
      }

      return importedGraphs;
   }
}

//////////////////////////////////////////////////////////////////////////
