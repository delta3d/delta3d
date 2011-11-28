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
#include <dtDirectorQt/undolinkevent.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/propertyeditor.h>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   UndoLinkEvent::UndoLinkEvent(DirectorEditor* editor,
                                LinkType type,
                                const dtDirector::ID& sourceID,
                                const dtDirector::ID& destID,
                                const std::string& sourceLinkName,
                                const std::string& destLinkName,
                                bool connecting)
      : UndoEvent(editor)
      , mType(type)
      , mSourceID(sourceID)
      , mDestID(destID)
      , mSource(sourceLinkName)
      , mDest(destLinkName)
      , mConnecting(connecting)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   UndoLinkEvent::~UndoLinkEvent()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoLinkEvent::Undo()
   {
      if (mConnecting) DisconnectLink();
      else             ConnectLink();

      mEditor->Refresh();
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoLinkEvent::Redo()
   {
      if (mConnecting) ConnectLink();
      else             DisconnectLink();

      mEditor->Refresh();
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoLinkEvent::ConnectLink()
   {
      switch (mType)
      {
      // Inputs.
      case INPUT_LINK:
         {
            Node* sourceNode = mEditor->GetDirector()->GetNode(mSourceID, true);
            Node* destNode = mEditor->GetDirector()->GetNode(mDestID, true);

            if (sourceNode && destNode)
            {
               InputLink* sourceLink = sourceNode->GetInputLink(mSource);
               OutputLink* destLink = destNode->GetOutputLink(mDest);

               if (sourceLink && destLink)
               {
                  sourceLink->Connect(destLink);
               }
            }

            break;
         }

      // Values.
      case VALUE_LINK:
         {
            Node* sourceNode = mEditor->GetDirector()->GetNode(mSourceID, true);
            Node* destNode = mEditor->GetDirector()->GetNode(mDestID, true);

            if (sourceNode && destNode)
            {
               ValueLink* sourceLink = sourceNode->GetValueLink(mSource);

               if (sourceLink)
               {
                  sourceLink->Connect(destNode->AsValueNode());
               }
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoLinkEvent::DisconnectLink()
   {
      switch (mType)
      {
      // Inputs.
      case INPUT_LINK:
         {
            Node* sourceNode = mEditor->GetDirector()->GetNode(mSourceID, true);
            Node* destNode = mEditor->GetDirector()->GetNode(mDestID, true);

            if (sourceNode && destNode)
            {
               InputLink* sourceLink = sourceNode->GetInputLink(mSource);
               OutputLink* destLink = destNode->GetOutputLink(mDest);

               if (sourceLink && destLink)
               {
                  sourceLink->Disconnect(destLink);
               }
            }

            break;
         }

      // Values.
      case VALUE_LINK:
         {
            Node* sourceNode = mEditor->GetDirector()->GetNode(mSourceID, true);
            Node* destNode = mEditor->GetDirector()->GetNode(mDestID, true);

            if (sourceNode && destNode)
            {
               ValueLink* sourceLink = sourceNode->GetValueLink(mSource);

               if (sourceLink)
               {
                  sourceLink->Disconnect(destNode->AsValueNode());
               }
            }
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
