/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * Jeff P. Houde
 */
#include <dtDirector/messagegmcomponent.h>
#include <dtDirector/director.h>
#include <dtGame/message.h>
#include <dtGame/messagetype.h>

namespace dtDirector
{
   //////////////////////////////////////////////
   MessageGMComponent::MessageGMComponent()
      : dtGame::GMComponent("DirectorMessageGMComponent")
   {
   }

   //////////////////////////////////////////////
   MessageGMComponent::~MessageGMComponent()
   {
      mRegisteredCallbacks.clear();
   }

   //////////////////////////////////////////////
   void MessageGMComponent::DispatchNetworkMessage(const dtGame::Message& message)
   {
      ProcessMessage(message);
   }

   //////////////////////////////////////////////
   void MessageGMComponent::ProcessMessage(const dtGame::Message& message)
   {
      std::map<std::string, std::map<dtDirector::Node*, MsgFunc> >::iterator i =
         mRegisteredCallbacks.find(message.GetMessageType().GetName());

      if (i != mRegisteredCallbacks.end())
      {
         std::map<dtDirector::Node*, MsgFunc>& callbacks = i->second;

         for (std::map<dtDirector::Node*, MsgFunc>::iterator a = callbacks.begin();
            a != callbacks.end(); ++a)
         {
            if( a->first->GetDirector()->GetActive() )
               a->second(message);
         }
      }

      // Also send this message to all registered messages registered
      // under a blank message name.
      i = mRegisteredCallbacks.find("");
      if (i != mRegisteredCallbacks.end())
      {
         std::map<dtDirector::Node*, MsgFunc>& callbacks = i->second;

         for (std::map<dtDirector::Node*, MsgFunc>::iterator a = callbacks.begin();
            a != callbacks.end(); ++a)
         {
            if( a->first->GetDirector()->GetActive() )
               a->second(message);
         }
      }
   }

   //////////////////////////////////////////////
   void MessageGMComponent::OnAddedToGM()
   {
   }

   //////////////////////////////////////////////
   void MessageGMComponent::OnRemovedFromGM()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageGMComponent::RegisterMessage(const std::string& msgType, dtDirector::Node* node, MsgFunc callback)
   {
      std::map<std::string, std::map<dtDirector::Node*, MsgFunc> >::iterator i =
         mRegisteredCallbacks.find(msgType);

      // Register the message type if it isn't.
      if (i == mRegisteredCallbacks.end())
      {
         std::map<dtDirector::Node*, MsgFunc> msgList;
         i = mRegisteredCallbacks.insert(mRegisteredCallbacks.end(), std::make_pair<std::string, std::map<dtDirector::Node*, MsgFunc> >(msgType, msgList));
      }

      // Register this callback for that message type.
      if (i != mRegisteredCallbacks.end())
      {
         i->second.insert(i->second.end(), std::make_pair<dtDirector::Node*, MsgFunc>(node, callback));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageGMComponent::UnRegisterMessage(const std::string& msgType, dtDirector::Node* node)
   {
      std::map<std::string, std::map<dtDirector::Node*, MsgFunc> >::iterator i =
         mRegisteredCallbacks.find(msgType);

      // Find the callback.
      if (i != mRegisteredCallbacks.end())
      {
         std::map<dtDirector::Node*, MsgFunc>::iterator a =
            i->second.find(node);

         if (a != i->second.end())
         {
            i->second.erase(a);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageGMComponent::UnRegisterMessages(dtDirector::Node* node)
   {
      std::map<std::string, std::map<dtDirector::Node*, MsgFunc> >::iterator i;
      for (i = mRegisteredCallbacks.begin(); i != mRegisteredCallbacks.end(); ++i)
      {
         std::map<dtDirector::Node*, MsgFunc>::iterator found = i->second.find(node);

         if (found != i->second.end())
         {
            i->second.erase(found);
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////

