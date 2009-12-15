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

#include <dtDirectorQt/clipboard.h>
#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undocreateevent.h>

#include <dtDirector/nodemanager.h>
#include <dtDirector/directorgraph.h>

#include <dtDAL/enginepropertytypes.h>


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
   void Clipboard::AddObject(dtDAL::PropertyContainer* object)
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
      dtDAL::Vec2ActorProperty* prop = dynamic_cast<dtDAL::Vec2ActorProperty*>
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
   void Clipboard::RemoveObject(dtDAL::PropertyContainer* object)
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
   std::vector<dtDAL::PropertyContainer*> Clipboard::PasteObjects(DirectorGraph* parent, UndoManager* undoManager, const osg::Vec2& position)
   {
      std::vector<dtDAL::PropertyContainer*> result;

      if (!parent) return result;

      int count = (int)mCopied.size();
      for (int index = 0; index < count; index++)
      {
         dtDAL::PropertyContainer* object = mCopied[index].get();
         if (object)
         {
            dtDAL::PropertyContainer* newObject = CopyObject(object, parent, position);
            if (newObject) result.push_back(newObject);
         }
      }

      // Now link all the new objects together.
      count = (int)mPasted.size();
      for (int index = 0; index < count; index++)
      {
         LinkNode(mPasted[index], parent);
      }

      // Now add all created nodes to the undo manager.
      if (undoManager && (!mPasted.empty() || !mPastedGraphs.empty()))
      {
         undoManager->BeginMultipleEvents();

         for (int index = 0; index < count; index++)
         {
            Node* node = mPasted[index];
            if (node)
            {
               dtCore::RefPtr<UndoCreateEvent> event = new UndoCreateEvent(
                  undoManager->GetEditor(), node->GetID(), node->GetGraph()->GetID());
               undoManager->AddEvent(event);
            }
         }

         count = mPastedGraphs.size();
         for (int index = 0; index < count; index++)
         {
            DirectorGraph* graph = mPastedGraphs[index];
            if (graph)
            {
               dtCore::RefPtr<UndoCreateEvent> event = new UndoCreateEvent(
                  undoManager->GetEditor(), graph->GetID(), graph->mParent->GetID());
               undoManager->AddEvent(event);
            }
         }

         undoManager->EndMultipleEvents();
      }

      mIDNewToOld.clear();
      mIDOldToNew.clear();
      mPasted.clear();
      mPastedGraphs.clear();

      return result;
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::PropertyContainer* Clipboard::CopyObject(dtDAL::PropertyContainer* object, DirectorGraph* parent, const osg::Vec2& position)
   {
      if (!object || !parent) return NULL;

      NodeManager& nodeManager = NodeManager::GetInstance();

      // Find out the object's type.
      Node* node = dynamic_cast<Node*>(object);
      if (node)
      {
         dtCore::RefPtr<Node> newNode = nodeManager.CreateNode(node->GetType(), parent);
         newNode->CopyPropertiesFrom(*node);

         // Map the ID between the two.
         mIDNewToOld[newNode->GetID()] = node;
         mIDOldToNew[node->GetID()] = newNode;
         mPasted.push_back(newNode.get());

         // Offset the position of the new node based on our offset and position.
         newNode->SetPosition(newNode->GetPosition() - mOffset + position);
         return newNode.get();
      }
      else
      {
         DirectorGraph* graph = dynamic_cast<DirectorGraph*>(object);
         if (graph)
         {
            dtCore::RefPtr<DirectorGraph> newGraph = parent->AddGraph();
            if (newGraph.valid())
            {
               newGraph->CopyPropertiesFrom(*graph);
               mPastedGraphs.push_back(newGraph.get());

               // Offset the position of the new node based on our offset and position.
               newGraph->SetPosition(newGraph->GetPosition() - mOffset + position);

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

               count = (int)graph->GetSubGraphs().size();
               for (int index = 0; index < count; index++)
               {
                  DirectorGraph* subGraph = graph->GetSubGraphs()[index].get();
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
   void Clipboard::LinkNode(Node* node, DirectorGraph* parent)
   {
      if (!node || !parent) return;

      Node* fromNode = mIDNewToOld[node->GetID()];
      if (!fromNode) return;

      // Link outputs to inputs.
      int count = (int)fromNode->GetInputLinks().size();
      for (int index = 0; index < count; index++)
      {
         InputLink* fromLink = &fromNode->GetInputLinks()[index];
         InputLink* link = &node->GetInputLinks()[index];
         LinkInputs(link, fromLink, parent);
      }

      // Link outputs to inputs.
      count = (int)fromNode->GetOutputLinks().size();
      for (int index = 0; index < count; index++)
      {
         OutputLink* fromLink = &fromNode->GetOutputLinks()[index];
         OutputLink* link = &node->GetOutputLinks()[index];
         LinkOutputs(link, fromLink, parent);
      }

      // Link values.
      count = (int)fromNode->GetValueLinks().size();
      for (int index = 0; index < count; index++)
      {
         ValueLink* fromLink = &fromNode->GetValueLinks()[index];
         ValueLink* link = &node->GetValueLinks()[index];
         LinkValues(link, fromLink, parent);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void Clipboard::LinkInputs(InputLink* link, InputLink* fromLink, DirectorGraph* parent)
   {
      if (!link || !fromLink || !parent) return;

      int count = (int)fromLink->GetLinks().size();
      for (int index = 0; index < count; index++)
      {
         Node* owner = fromLink->GetLinks()[index]->GetOwner();
         if (owner)
         {
            Node* newOwner = NULL;
            if (mIDOldToNew.find(owner->GetID()) == mIDOldToNew.end())
            {
               // If we did not copy the node that it is linked to, then link it
               // to the old one instead only if the old node is within the same graph.
               if (owner->GetGraph() == parent ||
                  owner->GetGraph()->mParent == parent ||
                  parent->mParent == owner->GetGraph())
                  newOwner = owner;
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
   }

   //////////////////////////////////////////////////////////////////////////
   void Clipboard::LinkOutputs(OutputLink* link, OutputLink* fromLink, DirectorGraph* parent)
   {
      if (!link || !fromLink || !parent) return;

      int count = (int)fromLink->GetLinks().size();
      for (int index = 0; index < count; index++)
      {
         Node* owner = fromLink->GetLinks()[index]->GetOwner();
         if (owner)
         {
            Node* newOwner = NULL;
            if (mIDOldToNew.find(owner->GetID()) == mIDOldToNew.end())
            {
               // If we did not copy the node that it is linked to, then link it
               // to the old one instead only if the old node is within the same graph.
               if (owner->GetGraph() == parent ||
                  owner->GetGraph()->mParent == parent ||
                  parent->mParent == owner->GetGraph())
                  newOwner = owner;
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
   }

   //////////////////////////////////////////////////////////////////////////
   void Clipboard::LinkValues(ValueLink* link, ValueLink* fromLink, DirectorGraph* parent)
   {
      if (!link || !fromLink || !parent) return;

      int count = (int)fromLink->GetLinks().size();
      for (int index = 0; index < count; index++)
      {
         Node* owner = fromLink->GetLinks()[index];
         if (owner)
         {
            ValueNode* newOwner = NULL;
            if (mIDOldToNew.find(owner->GetID()) == mIDOldToNew.end())
            {
               // If we did not copy the node that it is linked to, then link it
               // to the old one instead only if the old node is within the same graph.
               if (owner->GetGraph() == parent ||
                  owner->GetGraph()->mParent == parent ||
                  parent->mParent == owner->GetGraph())
                  newOwner = dynamic_cast<ValueNode*>(owner);
            }
            else
            {
               newOwner = dynamic_cast<ValueNode*>(mIDOldToNew[owner->GetID()]);
            }

            if (newOwner) link->Connect(newOwner);
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
