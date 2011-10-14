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
#include <dtDirectorQt/undopropertyevent.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/propertyeditor.h>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   UndoPropertyEvent::UndoPropertyEvent(DirectorEditor* editor,
                                        const dtDirector::ID& id,
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
   dtCore::ActorProperty* UndoPropertyEvent::GetProperty()
   {
      // If the ID is blank, then it belongs to the director.
      if (mID.id.ToString() == "")
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
      dtCore::ActorProperty* prop = GetProperty();

      if (prop)
      {
         prop->FromString(mOldValue);

         mEditor->Refresh();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoPropertyEvent::Redo()
   {
      dtCore::ActorProperty* prop = GetProperty();

      if (prop)
      {
         prop->FromString(mNewValue);

         mEditor->Refresh();
      }
   }
}

//////////////////////////////////////////////////////////////////////////
