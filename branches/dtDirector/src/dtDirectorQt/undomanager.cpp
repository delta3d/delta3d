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

#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/propertyeditor.h>
#include <dtDirectorQt/editorscene.h>

#include <dtUtil/log.h>

namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   UndoEvent::UndoEvent(DirectorEditor* editor)
      : mEditor(editor)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   UndoEvent::~UndoEvent()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoEvent::Undo()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoEvent::Redo()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   UndoMultipleEvent::UndoMultipleEvent(DirectorEditor* editor)
      : UndoEvent(editor)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   UndoMultipleEvent::~UndoMultipleEvent()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoMultipleEvent::Undo()
   {
      int count = (int)mEvents.size();
      for (int index = 0; index < count; index++)
      {
         mEvents[index]->Undo();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoMultipleEvent::Redo()
   {
      int count = (int)mEvents.size();
      for (int index = 0; index < count; index++)
      {
         mEvents[index]->Redo();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoMultipleEvent::AddEvent(UndoEvent* event)
   {
      if (!event) return;

      mEvents.push_back(event);
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   UndoPropertyEvent::UndoPropertyEvent(DirectorEditor* editor,
                                        const dtCore::UniqueId& id,
                                        const std::string& propName,
                                        const std::string& oldValue,
                                        const std::string& newValue)
      : UndoEvent(editor)
      , mID(id)
      , mPropName(propName)
      , mOldValue(oldValue)
      , mNewValue(newValue)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   UndoPropertyEvent::~UndoPropertyEvent()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProperty* UndoPropertyEvent::GetProperty()
   {
      // If the ID is blank, then it belongs to the director.
      if (mID.ToString() == "")
      {
         return mEditor->GetDirector()->GetProperty(mPropName);
      }
      else
      {
         // Check if the ID belongs to a graph.
         DirectorGraph* graph = mEditor->GetDirector()->GetGraph(mID);
         if (graph)
         {
            return graph->GetProperty(mPropName);
         }
         else
         {
            // Check if the ID belongs to a node.
            Node* node = mEditor->GetDirector()->GetNode(mID);
            if (node)
            {
               // Restore the property back to its old value.
               return node->GetProperty(mPropName);
            }
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoPropertyEvent::Undo()
   {
      dtDAL::ActorProperty* prop = GetProperty();

      if (prop)
      {
         prop->FromString(mOldValue);

         mEditor->Refresh();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoPropertyEvent::Redo()
   {
      dtDAL::ActorProperty* prop = GetProperty();

      if (prop)
      {
         prop->FromString(mNewValue);

         mEditor->Refresh();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   UndoManager::UndoManager(DirectorEditor* editor)
      : mEditor(editor)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   UndoManager::~UndoManager()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoManager::Undo()
   {
      if (mUndoEvents.empty()) return;

      dtCore::RefPtr<UndoEvent> event = mUndoEvents.top().get();
      mUndoEvents.pop();

      if (event.valid())
      {
         event->Undo();
         mRedoEvents.push(event);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoManager::Redo()
   {
      if (mRedoEvents.empty()) return;

      dtCore::RefPtr<UndoEvent> event = mRedoEvents.top().get();
      mRedoEvents.pop();

      if (event.valid())
      {
         event->Redo();
         mUndoEvents.push(event.get());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoManager::BeginMultipleEvents()
   {
      dtCore::RefPtr<UndoMultipleEvent> event = new UndoMultipleEvent(mEditor);
      if (event.valid())
      {
         AddEvent(event.get());
         mMultipleEventStack.push(event.get());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoManager::EndMultipleEvents()
   {
      if (mMultipleEventStack.empty())
      {
         dtUtil::Log* logger = mEditor->GetDirector()->GetLogger();

         logger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
            "Attempted to end a multiple event stack when there is none started.");

         return;
      }

      mMultipleEventStack.pop();
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoManager::PropertyChangeEvent(const dtCore::UniqueId& id, const std::string& propName, const std::string& oldValue, const std::string& newValue)
   {
      dtCore::RefPtr<UndoPropertyEvent> event = new UndoPropertyEvent(mEditor, id, propName, oldValue, newValue);
      AddEvent(event.get());
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoManager::AddEvent(UndoEvent* event)
   {
      if (!event) return;

      // Any time we add new events, we need to clear any redo events
      // we may have.
      while (!mRedoEvents.empty()) mRedoEvents.pop();

      // First check if we have any multiple events in the stack.
      if (!mMultipleEventStack.empty())
      {
         UndoMultipleEvent* multipleEvent = mMultipleEventStack.top();
         if (multipleEvent)
         {
            multipleEvent->AddEvent(event);
            return;
         }
      }

      // If not, add it to the normal stack.
      mUndoEvents.push(event);
   }
}

//////////////////////////////////////////////////////////////////////////
