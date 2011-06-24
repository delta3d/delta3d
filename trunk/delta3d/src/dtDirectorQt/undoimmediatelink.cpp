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
#include <dtDirectorQt/undoimmediatelink.h>
#include <dtDirectorQt/directoreditor.h>

#include <dtDirector/director.h>
#include <dtDirector/directorgraph.h>
#include <dtDirector/outputlink.h>

namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   UndoImmediateLinkEvent::UndoImmediateLinkEvent(DirectorEditor* editor, const dtCore::UniqueId& nodeID, int linkType, const std::string& linkName, bool enabled)
      : UndoEvent(editor)
      , mID(nodeID)
      , mLinkType(linkType)
      , mLinkName(linkName)
      , mEnabled(enabled)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   UndoImmediateLinkEvent::~UndoImmediateLinkEvent()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoImmediateLinkEvent::Undo()
   {
      switch (mLinkType)
      {
      case 0:
         {
            InputLink* link = GetInputLink();
            if (link)
            {
            }
            break;
         }
      case 1:
         {
            OutputLink* link = GetOutputLink();
            if (link)
            {
               link->SetImmediate(!mEnabled);
            }
            break;
         }
      case 2:
         {
            ValueLink* link = GetValueLink();
            if (link)
            {
            }
            break;
         }
      }

      Node* node = mEditor->GetDirector()->GetNode(mID);
      if (node)
      {
         mEditor->RefreshNode(node);
      }
      else
      {
         DirectorGraph* macro = mEditor->GetDirector()->GetGraph(mID);
         if (macro)
         {
            mEditor->RefreshGraph(macro);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoImmediateLinkEvent::Redo()
   {
      switch (mLinkType)
      {
      case 0:
         {
            InputLink* link = GetInputLink();
            if (link)
            {
            }
            break;
         }
      case 1:
         {
            OutputLink* link = GetOutputLink();
            if (link)
            {
               link->SetImmediate(mEnabled);
            }
            break;
         }
      case 2:
         {
            ValueLink* link = GetValueLink();
            if (link)
            {
            }
            break;
         }
      }
      
      Node* node = mEditor->GetDirector()->GetNode(mID);
      if (node)
      {
         mEditor->RefreshNode(node);
      }
      else
      {
         DirectorGraph* macro = mEditor->GetDirector()->GetGraph(mID);
         if (macro)
         {
            mEditor->RefreshGraph(macro);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   InputLink* UndoImmediateLinkEvent::GetInputLink()
   {
      Node* node = mEditor->GetDirector()->GetNode(mID);
      if (node)
      {
         return node->GetInputLink(mLinkName);
      }
      else
      {
         DirectorGraph* macro = mEditor->GetDirector()->GetGraph(mID);
         if (macro)
         {
            int count = (int)macro->GetInputNodes().size();
            for (int index = 0; index < count; ++index)
            {
               if (mLinkName == macro->GetInputNodes()[index]->GetName())
               {
                  return &macro->GetInputNodes()[index]->GetInputLinks()[0];
               }
            }
         }
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   OutputLink* UndoImmediateLinkEvent::GetOutputLink()
   {
      Node* node = mEditor->GetDirector()->GetNode(mID);
      if (node)
      {
         return node->GetOutputLink(mLinkName);
      }
      else
      {
         DirectorGraph* macro = mEditor->GetDirector()->GetGraph(mID);
         if (macro)
         {
            int count = (int)macro->GetOutputNodes().size();
            for (int index = 0; index < count; ++index)
            {
               if (mLinkName == macro->GetOutputNodes()[index]->GetName())
               {
                  return &macro->GetOutputNodes()[index]->GetOutputLinks()[0];
               }
            }
         }
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ValueLink* UndoImmediateLinkEvent::GetValueLink()
   {
      Node* node = mEditor->GetDirector()->GetNode(mID);
      if (node)
      {
         return node->GetValueLink(mLinkName);
      }
      else
      {
         DirectorGraph* macro = mEditor->GetDirector()->GetGraph(mID);
         if (macro)
         {
            int count = (int)macro->GetValueNodes().size();
            for (int index = 0; index < count; ++index)
            {
               if (mLinkName == macro->GetValueNodes()[index]->GetName())
               {
                  return &macro->GetValueNodes()[index]->GetValueLinks()[0];
               }
            }
         }
      }

      return NULL;
   }
}

//////////////////////////////////////////////////////////////////////////
