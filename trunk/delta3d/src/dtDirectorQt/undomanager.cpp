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
#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/propertyeditor.h>
#include <dtDirectorQt/editorscene.h>

#include <dtUtil/log.h>

namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   UndoManager::UndoManager(DirectorEditor* editor)
      : mEditor(editor)
      , mModifyIndex(0)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   UndoManager::~UndoManager()
   {
      Clear();
   }

   //////////////////////////////////////////////////////////////////////////
   bool UndoManager::IsModified()
   {
      if (mModifyIndex == -1) return true;

      if (mModifyIndex != (int)mUndoEvents.size()) return true;

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoManager::OnSaved()
   {
      // We set the modify index to the undo size so it remembers
      // where the state was saved.
      mModifyIndex = mUndoEvents.size();
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoManager::Revert()
   {
      if (IsModified() && mModifyIndex > -1)
      {
         while (mModifyIndex < (int)mUndoEvents.size())
         {
            Undo();
         }

         while (mModifyIndex > (int)mUndoEvents.size())
         {
            Redo();
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoManager::Clear()
   {
      while (!mUndoEvents.empty()) mUndoEvents.pop();
      while (!mRedoEvents.empty()) mRedoEvents.pop();
      while (!mMultipleEventStack.empty()) mMultipleEventStack.pop();
      mModifyIndex = 0;
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoManager::Undo()
   {
      if (mUndoEvents.empty()) return;

      dtCore::RefPtr<UndoEvent> event = mUndoEvents.top();
      mUndoEvents.pop();

      if (event.valid())
      {
         event->Undo();
         mRedoEvents.push(event);
      }

      mEditor->RefreshButtonStates();
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

      mEditor->RefreshButtonStates();
   }

   //////////////////////////////////////////////////////////////////////////
   std::string UndoManager::GetUndoDescription() const
   {
      if (!mUndoEvents.empty())
      {
         return mUndoEvents.top()->GetDescription();
      }

      return "";
   }

   //////////////////////////////////////////////////////////////////////////
   std::string UndoManager::GetRedoDescription() const
   {
      if (!mRedoEvents.empty())
      {
         return mRedoEvents.top()->GetDescription();
      }

      return "";
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoManager::BeginMultipleEvents(const std::string& description)
   {
      dtCore::RefPtr<UndoMultipleEvent> event = new UndoMultipleEvent(mEditor);
      if (event.valid())
      {
         event->SetDescription(description);
         mMultipleEventStack.push(event.get());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoManager::EndMultipleEvents()
   {
      if (mMultipleEventStack.empty())
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
            "Attempted to end a multiple event stack when there is none started.");

         return;
      }

      dtCore::RefPtr<UndoMultipleEvent> event = mMultipleEventStack.top();
      mMultipleEventStack.pop();

      // Only add the multiple event if it contains events.
      if (event->HasEvents()) AddEvent(event.get());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void UndoManager::UndoCurrentMultipleEvent()
   {
      if (mMultipleEventStack.empty())
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
            "Attempted to clear a multiple event stack when there is none started.");

         return;
      }

      dtCore::RefPtr<UndoMultipleEvent> event = mMultipleEventStack.top();
      mMultipleEventStack.pop();

      event->Undo();
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoManager::AddEvent(UndoEvent* event)
   {
      if (!event) return;

      // If we had redo events, since they will be cleared now,
      // there is no way to restore your graphs back to their
      // last saved state, so we set the modify index to -1
      // so the IsModified method will always return true.
      if (!mRedoEvents.empty() &&
         mModifyIndex > (int)mUndoEvents.size()) mModifyIndex = -1;

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
      mEditor->RefreshButtonStates();
   }
}

//////////////////////////////////////////////////////////////////////////
