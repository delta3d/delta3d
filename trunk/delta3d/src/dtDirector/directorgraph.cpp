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
   //////////////////////////////////////////////////////////////////////////
   DirectorGraph::DirectorGraph(Director* director)
      : mDirector(director)
      , mParent(NULL)
      , mComment("")
   {
      SetColorRGB(Colors::GREEN);
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

      count = (int)mEventNodes.size();
      for (int index = 0; index < count; ++index)
      {
         Node* node = mEventNodes[index];
         if (node)
         {
            node->SetGraph(NULL);
         }
      }
      mEventNodes.clear();

      count = (int)mActionNodes.size();
      for (int index = 0; index < count; ++index)
      {
         Node* node = mActionNodes[index];
         if (node)
         {
            node->SetGraph(NULL);
         }
      }
      mActionNodes.clear();

      count = (int)mValueNodes.size();
      for (int index = 0; index < count; ++index)
      {
         Node* node = mValueNodes[index];
         if (node)
         {
            node->SetGraph(NULL);
         }
      }
      mValueNodes.clear();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string DirectorGraph::GetDefaultPropertyKey() const
   {
      return "Director Graph";
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

   //////////////////////////////////////////////////////////////////////////
   DirectorGraph* DirectorGraph::GetGraph(const dtCore::UniqueId& id)
   {
      if (GetID() == id) return this;

      int count = (int)mSubGraphs.size();
      for (int index = 0; index < count; index++)
      {
         DirectorGraph* result = mSubGraphs[index]->GetGraph(id);
         if (result) return result;
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   Node* DirectorGraph::GetNode(const dtCore::UniqueId& id)
   {
      int count = (int)mEventNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mEventNodes[index]->GetID() == id)
         {
            return mEventNodes[index];
         }
      }

      count = (int)mActionNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mActionNodes[index]->GetID() == id)
         {
            return mActionNodes[index];
         }
      }

      count = (int)mValueNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mValueNodes[index]->GetID() == id)
         {
            return mValueNodes[index];
         }
      }

      count = (int)mSubGraphs.size();
      for (int index = 0; index < count; index++)
      {
         DirectorGraph* graph = mSubGraphs[index];
         if (graph)
         {
            Node* node = graph->GetNode(id);
            if (node) return node;
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   const Node* DirectorGraph::GetNode(const dtCore::UniqueId& id) const
   {
      int count = (int)mEventNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mEventNodes[index]->GetID() == id)
         {
            return mEventNodes[index];
         }
      }

      count = (int)mActionNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mActionNodes[index]->GetID() == id)
         {
            return mActionNodes[index];
         }
      }

      count = (int)mValueNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mValueNodes[index]->GetID() == id)
         {
            return mValueNodes[index];
         }
      }

      count = (int)mSubGraphs.size();
      for (int index = 0; index < count; index++)
      {
         DirectorGraph* graph = mSubGraphs[index];
         if (graph)
         {
            Node* node = graph->GetNode(id);
            if (node) return node;
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorGraph::GetNodes(const std::string& name, const std::string& category, std::vector<Node*>& outNodes, bool searchSubGraphs)
   {
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
   void DirectorGraph::GetNodes(const std::string& name, const std::string& category, const std::string& property, const std::string& value, std::vector<Node*>& outNodes, bool searchSubGraphs)
   {
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
   void DirectorGraph::GetAllNodes(std::vector<Node*>& outNodes, bool searchSubGraphs)
   {
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
   ValueNode* DirectorGraph::GetValueNode(const std::string& name, bool searchSubgraphs)
   {
      int count = (int)mValueNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mValueNodes[index]->GetName() == name)
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
               ValueNode* node = graph->GetValueNode(name);
               if (node) return node;
            }
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   bool DirectorGraph::AddNode(Node* node)
   {
      ValueNode* valueNode = node->AsValueNode();
      if (valueNode)
      {
         mValueNodes.push_back(valueNode);
         return true;
      }

      EventNode* eventNode = node->AsEventNode();
      if (eventNode)
      {
         mEventNodes.push_back(eventNode);
         return true;
      }

      ActionNode* actionNode = node->AsActionNode();
      if (actionNode)
      {
         mActionNodes.push_back(actionNode);
         return true;
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool DirectorGraph::DeleteGraph(const dtCore::UniqueId& id)
   {
      // Check sub graphs
      int count = (int)mSubGraphs.size();
      for (int index = 0; index < count; index++)
      {
         DirectorGraph* graph = mSubGraphs[index];
         if (graph && graph->GetID() == id)
         {
            // Remove the sub graph.
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
   bool DirectorGraph::DeleteNode(const dtCore::UniqueId& id)
   {
      // Search for the node and remove it from the list.
      int count = (int)mEventNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mEventNodes[index]->GetID() == id)
         {
            mEventNodes.erase(mEventNodes.begin() + index);
            return true;
         }
      }

      count = (int)mActionNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mActionNodes[index]->GetID() == id)
         {
            mActionNodes.erase(mActionNodes.begin() + index);
            return true;
         }
      }

      count = (int)mValueNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mValueNodes[index]->GetID() == id)
         {
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

      // First remove the graph from their parents.
      if (graph->mParent)
      {
         graph->mParent->RemoveGraph(graph);
      }

      graph->mParent = this;
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
               mSubGraphs.erase(mSubGraphs.begin() + index);
               graph->mParent = NULL;
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
}

//////////////////////////////////////////////////////////////////////////
