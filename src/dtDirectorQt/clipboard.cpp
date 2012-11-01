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
#include <dtDirectorQt/clipboard.h>

#include <dtCore/vectoractorproperties.h>

#include <dtDirector/director.h>
#include <dtDirector/directorgraph.h>
#include <dtDirector/nodemanager.h>

#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/undocreateevent.h>
#include <dtDirectorQt/undoaddlibraryevent.h>
#include <dtDirectorQt/undomanager.h>

namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   Clipboard* Clipboard::mInstance = NULL;

   //////////////////////////////////////////////////////////////////////////
   Clipboard& Clipboard::GetInstance()
   {
      if (!mInstance)
      {
         mInstance = new Clipboard();
      }

      return *mInstance;
   }

   //////////////////////////////////////////////////////////////////////////
   void Clipboard::Clear()
   {
      mCopied.clear();
      mOffset.set(0, 0);
      mFirstObject = true;
   }

   //////////////////////////////////////////////////////////////////////////
   void Clipboard::AddObject(dtCore::PropertyContainer* object)
   {
      if (!object) return;

      // Make sure the object isn't already added.
      int count = (int)mCopied.size();
      for (int index = 0; index < count; index++)
      {
         if (mCopied[index].get() == object)
         {
            return;
         }
      }

      // Add the object.
      mCopied.push_back(object);

      // Calculate the offset.
      dtCore::Vec2ActorProperty* prop = dynamic_cast<dtCore::Vec2ActorProperty*>
         (object->GetProperty("Position"));
      if (prop)
      {
         osg::Vec2 pos = prop->GetValue();

         // The offset is always set to the first objects position.
         if (mFirstObject)
         {
            mOffset = pos;
            mFirstObject = false;
         }
         // Offset is always the top/left most object.
         else
         {
            if (mOffset.x() > pos.x())
            {
               mOffset.x() = pos.x();
            }
            if (mOffset.y() > pos.y())
            {
               mOffset.y() = pos.y();
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void Clipboard::RemoveObject(dtCore::PropertyContainer* object)
   {
      if (!object) return;

      // Find and remove the object.
      int count = (int)mCopied.size();
      for (int index = 0; index < count; index++)
      {
         if (mCopied[index].get() == object)
         {
            mCopied.erase(mCopied.begin() + index);
            return;
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   std::vector<dtCore::PropertyContainer*> Clipboard::PasteObjects(DirectorGraph* graph, UndoManager* undoManager, const osg::Vec2& position, bool createLinks, bool linkExternal)
   {
      std::vector<dtCore::PropertyContainer*> result;

      if (!graph) return result;

      mAddedLibraries.clear();
      int count = (int)mCopied.size();
      for (int index = 0; index < count; index++)
      {
         dtCore::PropertyContainer* object = mCopied[index].get();
         if (object)
         {
            dtCore::PropertyContainer* newObject = CopyObject(object, graph, position);
            if (newObject) result.push_back(newObject);
         }
      }

      // Now link all the new objects together.
      count = (int)mPasted.size();
      for (int index = 0; index < count; index++)
      {
         LinkNode(mPasted[index], graph, undoManager, result, createLinks, linkExternal);
      }

      // Now add all created nodes to the undo manager.
      if (undoManager && !result.empty())
      {
         Node* node = dynamic_cast<Node*>(result[0]);
         DirectorGraph* addedGraph = dynamic_cast<DirectorGraph*>(result[0]);

         std::string undoDescription = "Paste operation of ";
         if (result.size() == 1)
         {
            if (node)
            {
               undoDescription += "Node \'" + node->GetTypeName() + "\'.";
            }
            else if (addedGraph)
            {
               if (addedGraph->GetEditor().empty())
               {
                  undoDescription += "Macro Node \'" + addedGraph->GetName() + "\'.";
               }
               else
               {
                  undoDescription += "\'" + addedGraph->GetEditor() + "\' Macro Node \'" +
                     addedGraph->GetName() + "\'.";
               }
            }
         }
         else
         {
            undoDescription += "multiple Nodes.";
         }
         undoManager->BeginMultipleEvents(undoDescription);

         count = (int)result.size();
         for (int index = 0; index < count; index++)
         {
            dtDirector::ID id;
            dtDirector::ID parentId;

            Node* node = dynamic_cast<Node*>(result[index]);
            if (node)
            {
               id = node->GetID();
               parentId = node->GetGraph()->GetID();
            }
            else
            {
               DirectorGraph* addedGraph = dynamic_cast<DirectorGraph*>(result[index]);
               if (addedGraph)
               {
                  id = addedGraph->GetID();
                  parentId = addedGraph->GetParent()->GetID();
               }
               else
               {
                  continue;
               }
            }

            dtCore::RefPtr<UndoCreateEvent> event = new UndoCreateEvent(
               undoManager->GetEditor(), id, parentId);
            undoManager->AddEvent(event);
         }

         // Added Libraries.
         int libCount = (int)graph->GetDirector()->GetAllLibraries().size();
         count = (int)mAddedLibraries.size();
         for (int index = 0; index < count; ++index)
         {
            std::string libraryName = mAddedLibraries[index];

            dtCore::RefPtr<UndoAddLibraryEvent> event = new UndoAddLibraryEvent(
               undoManager->GetEditor(), libraryName, libCount + index - (int)mAddedLibraries.size());
            undoManager->AddEvent(event);
         }

         undoManager->EndMultipleEvents();
      }

      mPasted.clear();
      mPastedGraphs.clear();
      mAddedLibraries.clear();
      mIDNewToOld.clear();
      mIDOldToNew.clear();

      return result;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::PropertyContainer* Clipboard::CopyObject(dtCore::PropertyContainer* object, DirectorGraph* parent, const osg::Vec2& position)
   {
      if (!object || !parent) return NULL;

      NodeManager& nodeManager = NodeManager::GetInstance();

      // Find out the object's type.
      Node* node = dynamic_cast<Node*>(object);
      if (node)
      {
         // Make sure the node we found is a type valid for this script.
         NodePluginRegistry* reg = NodeManager::GetInstance().GetRegistryForType(node->GetType());
         if (!reg || !parent->GetDirector()->HasLibrary(reg->GetName()))
         {
            // If the library does not exist, attempt to add it first.
            if (!parent->GetDirector()->AddLibrary(reg->GetName()))
            {
               return NULL;
            }

            mAddedLibraries.push_back(reg->GetName());
         }

         dtCore::RefPtr<Node> newNode = nodeManager.CreateNode(node->GetType(), parent);
         newNode->CopyPropertiesFrom(*node);

         // Map the ID between the two.
         mIDNewToOld[newNode->GetID()] = node;
         mIDOldToNew[node->GetID()] = newNode;
         mPasted.push_back(newNode.get());

         // Offset the position of the new node based on our offset and position.
         newNode->SetPosition(newNode->GetPosition() - mOffset + position);

         newNode->OnFinishedLoading();
         if (parent->GetDirector() && parent->GetDirector()->HasStarted())
         {
            newNode->OnStart();
         }

         return newNode.get();
      }
      else
      {
         DirectorGraph* copiedGraph = dynamic_cast<DirectorGraph*>(object);
         if (copiedGraph)
         {
            dtCore::RefPtr<DirectorGraph> newGraph = parent->AddGraph();
            if (newGraph.valid())
            {
               newGraph->CopyPropertiesFrom(*copiedGraph);
               mPastedGraphs.push_back(newGraph.get());

               // Offset the position of the new node based on our offset and position.
               newGraph->SetPosition(newGraph->GetPosition() - mOffset + position);

               std::vector<DirectorGraph*> graphList = copiedGraph->GetImportedGraphs();
               graphList.push_back(copiedGraph);
               int graphCount = (int)graphList.size();
               for (int graphIndex = 0; graphIndex < graphCount; ++graphIndex)
               {
                  DirectorGraph* graph = graphList[graphIndex];

                  // Now we need to copy the entire contents of the children as well.
                  int count = (int)graph->GetEventNodes().size();
                  for (int index = 0; index < count; index++)
                  {
                     Node* node = graph->GetEventNodes()[index].get();
                     if (node)
                     {
                        CopyObject(node, newGraph.get(), mOffset);
                     }
                  }

                  count = (int)graph->GetActionNodes().size();
                  for (int index = 0; index < count; index++)
                  {
                     Node* node = graph->GetActionNodes()[index].get();
                     if (node)
                     {
                        CopyObject(node, newGraph.get(), mOffset);
                     }
                  }

                  count = (int)graph->GetValueNodes().size();
                  for (int index = 0; index < count; index++)
                  {
                     Node* node = graph->GetValueNodes()[index].get();
                     if (node)
                     {
                        CopyObject(node, newGraph.get(), mOffset);
                     }
                  }
               }

               int count = (int)copiedGraph->GetSubGraphs().size();
               for (int index = 0; index < count; index++)
               {
                  DirectorGraph* subGraph = copiedGraph->GetSubGraphs()[index].get();
                  if (subGraph)
                  {
                     CopyObject(subGraph, newGraph.get(), mOffset);
                  }
               }

               return newGraph.get();
            }
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   void Clipboard::LinkNode(Node* node, DirectorGraph* parent, UndoManager* undoManager, std::vector<dtCore::PropertyContainer*>& linkNodes, bool createLinks, bool linkExternal)
   {
      if (!node || !parent) return;

      Node* fromNode = mIDNewToOld[node->GetID()];
      if (!fromNode) return;

      // Link outputs to inputs.
      int count = (int)std::min( fromNode->GetInputLinks().size(), node->GetInputLinks().size() );
      for (int index = 0; index < count; index++)
      {
         InputLink* fromLink = &fromNode->GetInputLinks()[index];
         InputLink* link = &node->GetInputLinks()[index];

         link->SetVisible(fromLink->GetVisible());
         LinkInputs(link, fromLink, parent, undoManager, linkNodes, createLinks, linkExternal);
      }

      // Link outputs to inputs.
      count = (int)std::min( fromNode->GetOutputLinks().size(), node->GetOutputLinks().size() );
      for (int index = 0; index < count; index++)
      {
         OutputLink* fromLink = &fromNode->GetOutputLinks()[index];
         OutputLink* link = &node->GetOutputLinks()[index];

         link->SetVisible(fromLink->GetVisible());
         LinkOutputs(link, fromLink, parent, undoManager, linkNodes, createLinks, linkExternal);
      }

      // Link values.
      count = (int)std::min( fromNode->GetValueLinks().size(), node->GetValueLinks().size() );
      for (int index = 0; index < count; index++)
      {
         ValueLink* fromLink = &fromNode->GetValueLinks()[index];
         ValueLink* link = &node->GetValueLinks()[index];

         link->SetVisible(fromLink->GetVisible());
         link->SetExposed(fromLink->GetExposed());
         LinkValues(link, fromLink, parent, undoManager, linkNodes, createLinks, linkExternal);
      }

      // Link value nodes.
      ValueNode* valueNode = fromNode->AsValueNode();
      if (valueNode)
      {
         LinkValueNode(node->AsValueNode(),
            valueNode, parent, undoManager, linkNodes, createLinks, linkExternal);
      }

      return;
   }

   //////////////////////////////////////////////////////////////////////////
   void Clipboard::LinkInputs(InputLink* link, InputLink* fromLink, DirectorGraph* parent, UndoManager* undoManager, std::vector<dtCore::PropertyContainer*>& linkNodes, bool createLinks, bool linkExternal)
   {
      if (!link || !fromLink || !parent) return;

      dtCore::RefPtr<Node> rampNode = NULL;

      int count = (int)fromLink->GetLinks().size();
      for (int index = 0; index < count; index++)
      {
         Node* owner = fromLink->GetLinks()[index]->GetOwner();
         if (owner)
         {
            Node* newOwner = NULL;
            if ((linkExternal || createLinks) && mIDOldToNew.find(owner->GetID()) == mIDOldToNew.end())
            {
               DirectorGraph* myNodeGraph = link->GetOwner()->GetGraph();

               ID otherGraphID = owner->GetGraph()->GetID();
               otherGraphID.index = -1;
               DirectorGraph* otherNodeGraph = undoManager->GetEditor()->GetDirector()->GetGraph(otherGraphID);

               // If my new node is an input link node, it can only
               // connect its input to a node that is on its parent graph.
               if (link->GetOwner()->GetType().GetFullName() == "Core.Input Link")
               {
                  myNodeGraph = myNodeGraph->GetParent();
               }
               // If the other node is an output link node, it can only
               // connect its output to a node that is on its parent graph.
               if (owner->GetType().GetFullName() == "Core.Output Link")
               {
                  otherNodeGraph = otherNodeGraph->GetParent();
               }

               // The two nodes can only link together if their connecting
               // links are on the same graph.
               if (myNodeGraph == otherNodeGraph)
               {
                  newOwner = owner;
               }

               // If we are creating new link nodes, then create
               // a new input link node and connect them together.
               if (createLinks && !newOwner &&
                  otherNodeGraph == parent->GetParent())
               {
                  if (!rampNode.valid())
                  {
                     rampNode = NodeManager::GetInstance().CreateNode("Input Link", "Core", parent);

                     if (rampNode.valid())
                     {
                        rampNode->OnFinishedLoading();
                        if (parent->GetDirector() && parent->GetDirector()->HasStarted())
                        {
                           rampNode->OnStart();
                        }

                        OutputLink* output = rampNode->GetOutputLink("Out");
                        if (output) link->Connect(output);

                        // Position the new node to the left of the current.
                        osg::Vec2 position = link->GetOwner()->GetPosition();
                        position.x() -= 300.0f;

                        int linkCount = (int)link->GetOwner()->GetInputLinks().size();
                        for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
                        {
                           if (link == &link->GetOwner()->GetInputLinks()[linkIndex])
                           {
                              position.y() += (55 * linkIndex);
                           }
                        }

                        rampNode->SetPosition(position);

                        // Copy the name of the input to the input node.
                        dtCore::ActorProperty* prop = rampNode->GetProperty("Name");
                        if (prop) prop->FromString(link->GetName());
                     }
                  }

                  if (rampNode.valid())
                  {
                     // Link them together.
                     OutputLink* output = owner->GetOutputLink(fromLink->GetLinks()[index]->GetName());
                     rampNode->GetInputLinks()[0].Connect(output);
                  }
               }
            }
            else
            {
               newOwner = mIDOldToNew[owner->GetID()];
            }

            if (newOwner)
            {
               OutputLink* output = newOwner->GetOutputLink(fromLink->GetLinks()[index]->GetName());
               if (output) link->Connect(output);
            }
         }
      }

      // Create an undo event for the creation of the input link node.
      if (rampNode.valid() && undoManager)
      {
         dtCore::RefPtr<UndoCreateEvent> event = new UndoCreateEvent(undoManager->GetEditor(), rampNode->GetID(), parent->GetID());
         undoManager->AddEvent(event);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void Clipboard::LinkOutputs(OutputLink* link, OutputLink* fromLink, DirectorGraph* parent, UndoManager* undoManager, std::vector<dtCore::PropertyContainer*>& linkNodes, bool createLinks, bool linkExternal)
   {
      if (!link || !fromLink || !parent) return;

      dtCore::RefPtr<Node> rampNode = NULL;

      int count = (int)fromLink->GetLinks().size();
      for (int index = 0; index < count; index++)
      {
         Node* owner = fromLink->GetLinks()[index]->GetOwner();
         if (owner)
         {
            Node* newOwner = NULL;
            if ((linkExternal || createLinks) && mIDOldToNew.find(owner->GetID()) == mIDOldToNew.end())
            {
               DirectorGraph* myNodeGraph = link->GetOwner()->GetGraph();

               ID otherGraphID = owner->GetGraph()->GetID();
               otherGraphID.index = -1;
               DirectorGraph* otherNodeGraph = undoManager->GetEditor()->GetDirector()->GetGraph(otherGraphID);

               // If my new node is an output link node, it can only
               // connect its outputs to a node that is on its parent graph.
               if (link->GetOwner()->GetType().GetFullName() == "Core.Output Link")
               {
                  myNodeGraph = myNodeGraph->GetParent();
               }
               // If the other node is an input link node, it can only
               // connect its input to a node that is on its parent graph.
               if (owner->GetType().GetFullName() == "Core.Input Link")
               {
                  otherNodeGraph = otherNodeGraph->GetParent();
               }

               // The two nodes can only link together if their connecting
               // links are on the same graph.
               if (myNodeGraph == otherNodeGraph)
               {
                  newOwner = owner;
               }

               // If we are creating new link nodes, then create
               // a new output link node and connect them together.
               if (createLinks && !newOwner &&
                  otherNodeGraph == parent->GetParent())
               {
                  if (!rampNode.valid())
                  {
                     rampNode = NodeManager::GetInstance().CreateNode("Output Link", "Core", parent);

                     if (rampNode.valid())
                     {
                        rampNode->OnFinishedLoading();
                        if (parent->GetDirector() && parent->GetDirector()->HasStarted())
                        {
                           rampNode->OnStart();
                        }

                        InputLink* input = rampNode->GetInputLink("In");
                        if (input) link->Connect(input);

                        // Position the new node to the right of the current.
                        osg::Vec2 position = link->GetOwner()->GetPosition();
                        position.x() += 500.0f;

                        int linkCount = (int)link->GetOwner()->GetOutputLinks().size();
                        for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
                        {
                           if (link == &link->GetOwner()->GetOutputLinks()[linkIndex])
                           {
                              position.y() += (55 * linkIndex);
                           }
                        }

                        rampNode->SetPosition(position);

                        // Copy the name of the output to the output node.
                        dtCore::ActorProperty* prop = rampNode->GetProperty("Name");
                        if (prop) prop->FromString(link->GetName());
                     }
                  }

                  if (rampNode.valid())
                  {
                     // Link them together.
                     InputLink* input = owner->GetInputLink(fromLink->GetLinks()[index]->GetName());
                     rampNode->GetOutputLinks()[0].Connect(input);
                  }
               }
            }
            else
            {
               newOwner = mIDOldToNew[owner->GetID()];
            }

            if (newOwner)
            {
               InputLink* input = newOwner->GetInputLink(fromLink->GetLinks()[index]->GetName());
               if (input) link->Connect(input);
            }
         }
      }

      // Create an undo event for the creation of the output link node.
      if (rampNode.valid() && undoManager)
      {
         dtCore::RefPtr<UndoCreateEvent> event = new UndoCreateEvent(undoManager->GetEditor(), rampNode->GetID(), parent->GetID());
         undoManager->AddEvent(event);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void Clipboard::LinkValues(ValueLink* link, ValueLink* fromLink, DirectorGraph* parent, UndoManager* undoManager, std::vector<dtCore::PropertyContainer*>& linkNodes, bool createLinks, bool linkExternal)
   {
      if (!link || !fromLink || !parent) return;

      dtCore::RefPtr<ValueNode> rampNode = NULL;

      int count = (int)fromLink->GetLinks().size();
      for (int index = 0; index < count; index++)
      {
         Node* owner = fromLink->GetLinks()[index];
         if (owner)
         {
            ValueNode* newOwner = NULL;
            if ((linkExternal || createLinks) && mIDOldToNew.find(owner->GetID()) == mIDOldToNew.end())
            {
               DirectorGraph* myNodeGraph = link->GetOwner()->GetGraph();

               ID otherGraphID = owner->GetGraph()->GetID();
               otherGraphID.index = -1;
               DirectorGraph* otherNodeGraph = undoManager->GetEditor()->GetDirector()->GetGraph(otherGraphID);

               // If my new node is a value link node, it can only
               // connect its value to a node that is on its parent graph.
               if (link->GetOwner()->GetType().GetFullName() == "Core.Value Link")
               {
                  myNodeGraph = myNodeGraph->GetParent();
               }

               // The two nodes can only link together if their connecting
               // links are on the same graph.
               if (myNodeGraph == otherNodeGraph)
               {
                  newOwner = owner->AsValueNode();
               }

               // If we are creating new link nodes, then create
               // a new input link node and connect them together.
               if (createLinks && !newOwner &&
                  otherNodeGraph == parent->GetParent() &&
                  !link->GetOwner()->AsValueNode())
               {
                  if (!rampNode.valid())
                  {
                     rampNode = dynamic_cast<ValueNode*>(NodeManager::GetInstance().CreateNode("Value Link", "Core", parent).get());

                     if (rampNode.valid())
                     {
                        rampNode->OnFinishedLoading();
                        if (parent->GetDirector() && parent->GetDirector()->HasStarted())
                        {
                           rampNode->OnStart();
                        }

                        link->Connect(rampNode);

                        // Position the new node to the bottom of the current.
                        osg::Vec2 position = link->GetOwner()->GetPosition();
                        position.y() += 300.0f;

                        int linkCount = (int)link->GetOwner()->GetValueLinks().size();
                        for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
                        {
                           if (link == &link->GetOwner()->GetValueLinks()[linkIndex])
                           {
                              position.x() += (55 * linkIndex);
                           }
                        }

                        rampNode->SetPosition(position);

                        // Copy the name of the value link to the value node.
                        dtCore::ActorProperty* prop = ((Node*)rampNode)->GetProperty("Name");
                        if (prop) prop->FromString(link->GetName());
                     }
                  }

                  if (rampNode.valid())
                  {
                     // Link them together.
                     ValueNode* ownerValue = owner->AsValueNode();
                     rampNode->GetValueLinks()[0].Connect(ownerValue);
                  }
               }
            }
            else
            {
               Node* node = mIDOldToNew[owner->GetID()];
               if (node)
               {
                  newOwner = node->AsValueNode();
               }
            }

            if (newOwner) link->Connect(newOwner);
         }
      }

      // Create an undo event for this creation event.
      if (rampNode.valid() && undoManager)
      {
         dtCore::RefPtr<UndoCreateEvent> event = new UndoCreateEvent(undoManager->GetEditor(), rampNode->GetID(), parent->GetID());
         undoManager->AddEvent(event);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void Clipboard::LinkValueNode(ValueNode* node, ValueNode* fromNode, DirectorGraph* parent, UndoManager* undoManager, std::vector<dtCore::PropertyContainer*>& linkNodes, bool createLinks, bool linkExternal)
   {
      if (!node || !fromNode || !parent) return;

      for (int index = 0; index < (int)fromNode->GetLinks().size(); index++)
      {
         ValueLink* valueLink = fromNode->GetLinks()[index];
         if (valueLink)
         {
            Node* newOwner = NULL;
            if ((linkExternal || createLinks) && mIDOldToNew.find(valueLink->GetOwner()->GetID()) == mIDOldToNew.end())
            {
               DirectorGraph* myNodeGraph = node->GetGraph();
               DirectorGraph* otherNodeGraph = valueLink->GetOwner()->GetGraph();

               // If the other node is a value link node, it can only
               // connect its value to a node that is on its parent graph.
               if (valueLink->GetOwner()->GetType().GetFullName() == "Core.Value Link")
               {
                  otherNodeGraph = otherNodeGraph->GetParent();
               }

               // The two nodes can only link together if their connecting
               // links are on the same graph.
               if (myNodeGraph == otherNodeGraph)
               {
                  newOwner = valueLink->GetOwner();
               }
            }
            else
            {
               newOwner = mIDOldToNew[valueLink->GetOwner()->GetID()];
            }

            if (newOwner)
            {
               valueLink = newOwner->GetValueLink(fromNode->GetLinks()[index]->GetName());
               if (valueLink) valueLink->Connect(node);
            }
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
