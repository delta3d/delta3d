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
#include <dtDirectorQt/customeditortool.h>

#include <dtDirector/nodemanager.h>

#include <dtDirectorQt/undomanager.h>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////////
   CustomEditorTool::CustomEditorTool(const std::string& name)
      : mIsOpen(false)
      , mEditor(NULL)
      , mGraph(NULL)
      , mRowHeight(0)
   {
      mToolName = name;

      DirectorEditor::RegisterCustomEditorTool(this);
   }

   ////////////////////////////////////////////////////////////////////////////////
   CustomEditorTool::~CustomEditorTool()
   {
      DirectorEditor::UnRegisterCustomEditorTool(this);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool CustomEditorTool::IsDirectorSupported(Director* director) const
   {
      if (director->IsLibraryTypeSupported("Core"))
      {
         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CustomEditorTool::Open(DirectorEditor* editor, DirectorGraph* graph)
   {
      if (mIsOpen)
      {
         Close();
      }

      mEditor = editor;
      mGraph = graph;
      mIsOpen = true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CustomEditorTool::Close()
   {
      mEditor = NULL;
      mGraph = NULL;
      mIsOpen = false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool CustomEditorTool::BeginSave()
   {
      if (!GetGraph() || !GetEditor())
      {
         return false;
      }

      mInputConnections.clear();
      mOutputConnections.clear();
      mValueConnections.clear();
      mChainedNodeMap.clear();
      mRowHeight = 0;

      // Prepare the undo manager for multiple events.
      GetEditor()->GetUndoManager()->BeginMultipleEvents("\'" + mToolName + "\' editor tool application.");

      std::vector<dtDirector::Node*> nodes;
      GetGraph()->GetAllNodes(nodes);

      int nodeCount = (int)nodes.size();
      for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
      {
         dtDirector::Node* node = nodes[nodeIndex];
         if (!node)
         {
            continue;
         }

         // Save all input link connections.
         if (node->GetType().GetFullName() == "Core.Input Link")
         {
            std::string source = node->GetString("Name");
            dtDirector::InputLink& link = node->GetInputLinks()[0];

            std::vector<dtDirector::OutputLink*>& connectedLinks = link.GetLinks();
            int linkCount = (int)connectedLinks.size();
            if (!link.GetVisible())
            {
               ConnectionData data;
               data.visible = false;
               data.sourceNode = source;
               data.destNode = NULL;

               mInputConnections.push_back(data);
            }
            else
            {
               for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
               {
                  dtDirector::OutputLink* destLink = connectedLinks[linkIndex];
                  if (!destLink)
                  {
                     continue;
                  }

                  ConnectionData data;
                  data.visible = true;
                  data.sourceNode = source;
                  data.destNode = destLink->GetOwner();
                  data.destNodeLink = destLink->GetName();

                  mInputConnections.push_back(data);
               }
            }
         }
         else if (node->GetType().GetFullName() == "Core.Output Link")
         {
            std::string source = node->GetString("Name");
            dtDirector::OutputLink& link = node->GetOutputLinks()[0];

            std::vector<dtDirector::InputLink*>& connectedLinks = link.GetLinks();
            int linkCount = (int)connectedLinks.size();
            if (!link.GetVisible())
            {
               ConnectionData data;
               data.visible = false;
               data.sourceNode = source;
               data.destNode = NULL;

               mOutputConnections.push_back(data);
            }
            else
            {
               for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
               {
                  dtDirector::InputLink* destLink = connectedLinks[linkIndex];
                  if (!destLink)
                  {
                     continue;
                  }

                  ConnectionData data;
                  data.visible = true;
                  data.sourceNode = source;
                  data.destNode = destLink->GetOwner();
                  data.destNodeLink = destLink->GetName();

                  mOutputConnections.push_back(data);
               }
            }
         }
         else if (node->GetType().GetFullName() == "Core.Value Link")
         {
            std::string source = node->GetString("Name");
            dtDirector::ValueLink& link = node->GetValueLinks()[0];

            std::vector<dtDirector::ValueNode*>& connectedLinks = link.GetLinks();
            int linkCount = (int)connectedLinks.size();
            if (!link.GetVisible())
            {
               ConnectionData data;
               data.visible = false;
               data.sourceNode = source;
               data.destNode = NULL;

               mValueConnections.push_back(data);
            }
            else
            {
               for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
               {
                  dtDirector::ValueNode* destNode = connectedLinks[linkIndex];
                  if (!destNode)
                  {
                     continue;
                  }

                  ConnectionData data;
                  data.visible = true;
                  data.sourceNode = source;
                  data.destNode = destNode;
                  data.destNodeLink = "";

                  mValueConnections.push_back(data);
               }
            }
         }

         // Delete the node.
         GetEditor()->DeleteNode(node->GetID());
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool CustomEditorTool::EndSave()
   {
      if (!GetGraph() || !GetEditor())
      {
         return false;
      }

      // Re-connect all input connections.
      int connectionCount = (int)mInputConnections.size();
      for (int connectionIndex = 0; connectionIndex < connectionCount; ++connectionIndex)
      {
         ConnectionData& data = mInputConnections[connectionIndex];

         // Find the source node.
         std::vector<dtDirector::Node*> nodes;
         GetGraph()->GetNodes("Input Link", "Core", "Name", data.sourceNode, nodes);

         if (!nodes.empty())
         {
            if (!data.visible)
            {
               nodes[0]->GetInputLinks()[0].SetVisible(false);
            }
            else
            {
               // Connect the source node with the dest node.
               Connect(data.destNode, nodes[0], data.destNodeLink, data.sourceNode);
            }
         }
      }

      // Re-connect all output connections.
      connectionCount = (int)mOutputConnections.size();
      for (int connectionIndex = 0; connectionIndex < connectionCount; ++connectionIndex)
      {
         ConnectionData& data = mOutputConnections[connectionIndex];

         // Find the source node.
         std::vector<dtDirector::Node*> nodes;
         GetGraph()->GetNodes("Output Link", "Core", "Name", data.sourceNode, nodes);

         if (!nodes.empty())
         {
            if (!data.visible)
            {
               nodes[0]->GetOutputLinks()[0].SetVisible(false);
            }
            else
            {
               // Connect the source node with the dest node.
               Connect(nodes[0], data.destNode, data.sourceNode, data.destNodeLink);
            }
         }
      }

      // Re-connect all value connections.
      connectionCount = (int)mValueConnections.size();
      for (int connectionIndex = 0; connectionIndex < connectionCount; ++connectionIndex)
      {
         ConnectionData& data = mValueConnections[connectionIndex];

         // Find the source node.
         std::vector<dtDirector::Node*> nodes;
         GetGraph()->GetNodes("Value Link", "Core", "Name", data.sourceNode, nodes);

         if (!nodes.empty())
         {
            if (!data.visible)
            {
               nodes[0]->GetValueLinks()[0].SetVisible(false);
            }
            else
            {
               // Connect the source node with the dest node.
               dtDirector::ValueLink* valueLink = nodes[0]->GetValueLink(data.sourceNode);

               if (!valueLink)
               {
                  continue;
               }

               valueLink->Connect(dynamic_cast<dtDirector::ValueNode*>(data.destNode));
            }
         }
      }

      // Allow the undo manager to know about all new nodes.
      std::vector<dtDirector::Node*> nodes;
      GetGraph()->GetAllNodes(nodes);

      int nodeCount = (int)nodes.size();
      for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
      {
         dtDirector::Node* node = nodes[nodeIndex];
         if (node)
         {
            GetEditor()->OnNodeCreated(node);
         }
      }

      // Finalize the undo events.
      GetEditor()->GetUndoManager()->EndMultipleEvents();

      GetEditor()->Refresh();

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDirector::Node* CustomEditorTool::CreateNode(const std::string& name, const std::string& category, dtDirector::Node* chainedNode, int nodeHeight)
   {
      // Create our new node.
      dtDirector::Node* newNode = dtDirector::NodeManager::GetInstance().CreateNode(name, category, GetGraph());

      if (newNode)
      {
         newNode->OnFinishedLoading();
         if (GetGraph()->GetDirector() && GetGraph()->GetDirector()->HasStarted())
         {
            newNode->OnStart();
         }

         // Value nodes are ignored.
         if (newNode->GetType().GetNodeType() != dtDirector::NodeType::VALUE_NODE &&
            newNode->GetType().GetFullName() != "Core.Value Link")
         {
            // Position the new node in it's own empty row.
            if (!chainedNode)
            {
               newNode->SetPosition(osg::Vec2(0, mRowHeight));
               mRowHeight += nodeHeight;
            }
            // Position the new node relative to the chained node.
            else
            {
               // Find out if this chained node is already mapped.
               std::map<dtDirector::Node*, ChainData>::iterator iter = mChainedNodeMap.find(chainedNode);

               float offset = 0;
               if (iter == mChainedNodeMap.end())
               {
                  ChainData chainData;
                  chainData.node = chainedNode;
                  chainData.parent = NULL;
                  chainData.offset = osg::Vec2();
                  mChainedNodeMap[chainedNode] = chainData;
                  iter = mChainedNodeMap.find(chainedNode);
               }
               else
               {
                  offset = iter->second.offset.y();
               }

               ChainData chainData;
               chainData.node = newNode;
               chainData.parent = chainedNode;
               chainData.offset = osg::Vec2();
               mChainedNodeMap[newNode] = chainData;

               osg::Vec2 chainedPos = chainedNode->GetPosition();
               newNode->SetPosition(chainedPos + osg::Vec2(400, offset));

               offset += nodeHeight;
               if (iter != mChainedNodeMap.end())
               {
                  iter->second.offset.y() = offset;
                  dtDirector::Node* parent = iter->second.parent;

                  while (parent)
                  {
                     ChainData& chainData = mChainedNodeMap[parent];
                     if (chainData.node->GetPosition().y() + chainData.offset.y() < newNode->GetPosition().y() + offset)
                     {
                        chainData.offset.y() = newNode->GetPosition().y() + offset - chainData.node->GetPosition().y();
                     }

                     parent = chainData.parent;
                  }
               }

               // Update the max row height if needed.
               if (chainedPos.y() + offset > mRowHeight)
               {
                  mRowHeight = chainedPos.y() + offset;
               }
            }
         }
      }

      return newNode;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool CustomEditorTool::Connect(dtDirector::Node* node1, dtDirector::Node* node2, const std::string& outputName, const std::string& inputName)
   {
      if (!node1 || !node2)
      {
         return false;
      }

      dtDirector::OutputLink* outputLink = node1->GetOutputLink(outputName);
      dtDirector::InputLink*  inputLink  = node2->GetInputLink(inputName);

      if (!outputLink || !inputLink)
      {
         return false;
      }

      return outputLink->Connect(inputLink);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool CustomEditorTool::Connect(dtDirector::Node* node, dtDirector::Node* valueNode, const std::string& linkName)
   {
      if (!node || !valueNode)
      {
         return false;
      }

      dtDirector::ValueLink* valueLink = node->GetValueLink(linkName);

      if (!valueLink)
      {
         return false;
      }

      if (!valueLink->GetVisible() || !valueLink->GetExposed())
      {
         valueLink->SetExposed(true);
         valueLink->SetVisible(true);
      }

      bool result = valueLink->Connect(dynamic_cast<dtDirector::ValueNode*>(valueNode));

      // Find out if this chained node is already mapped.
      std::map<dtDirector::Node*, ChainData>::iterator iter = mChainedNodeMap.find(node);

      float offset = 0;
      if (iter == mChainedNodeMap.end())
      {
         ChainData chainData;
         chainData.node = node;
         chainData.parent = NULL;
         chainData.offset = osg::Vec2();
         mChainedNodeMap[node] = chainData;
         iter = mChainedNodeMap.find(node);
      }
      else
      {
         offset = iter->second.offset.x();
      }

      if (mChainedNodeMap.find(valueNode) == mChainedNodeMap.end())
      {
         ChainData chainData;
         chainData.node = valueNode;
         chainData.parent = node;
         chainData.offset = osg::Vec2();
         mChainedNodeMap[valueNode] = chainData;
      }

      // Position the value node beneath the node.
      osg::Vec2 pos = node->GetPosition();
      valueNode->SetPosition(pos + osg::Vec2(offset + 10, 200));

      offset += 80;
      if (iter != mChainedNodeMap.end())
      {
         iter->second.offset.x() = offset;

         dtDirector::Node* parent = iter->second.parent;
         while (parent)
         {
            ChainData& chainData = mChainedNodeMap[parent];
            if (chainData.node->GetPosition().y() + chainData.offset.y() < node->GetPosition().y() + 350)
            {
               chainData.offset.y() = node->GetPosition().y() + 350 - chainData.node->GetPosition().y();
            }

            parent = chainData.parent;
         }
      }

      // Update the max row height if needed.
      if (pos.y() + 350 > mRowHeight)
      {
         mRowHeight = pos.y() + 350;
      }

      return result;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool CustomEditorTool::GetNext(dtDirector::Node* node, const std::string& linkName, std::vector<dtDirector::InputLink*>& outLinks)
   {
      if (!node)
      {
         return false;
      }

      dtDirector::OutputLink* link = node->GetOutputLink(linkName);
      if (!link)
      {
         return false;
      }

      std::vector<dtDirector::InputLink*>& links = link->GetLinks();
      int count = (int)links.size();
      for (int index = 0; index < count; ++index)
      {
         dtDirector::InputLink* inputLink = links[index];
         if (inputLink && inputLink->GetOwner())
         {
            outLinks.push_back(inputLink);
         }
      }

      return !outLinks.empty();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool CustomEditorTool::GetPrev(dtDirector::Node* node, const std::string& linkName, std::vector<dtDirector::OutputLink*>& outLinks)
   {
      if (!node)
      {
         return false;
      }

      dtDirector::InputLink* link = node->GetInputLink(linkName);
      if (!link)
      {
         return false;
      }

      std::vector<dtDirector::OutputLink*>& links = link->GetLinks();
      int count = (int)links.size();
      for (int index = 0; index < count; ++index)
      {
         dtDirector::OutputLink* outputLink = links[index];
         if (outputLink && outputLink->GetOwner())
         {
            outLinks.push_back(outputLink);
         }
      }

      return !outLinks.empty();
   }

} // namespace dtDirector

//////////////////////////////////////////////////////////////////////////
