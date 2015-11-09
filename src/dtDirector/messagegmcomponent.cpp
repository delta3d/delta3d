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

   const dtCore::RefPtr<dtCore::SystemComponentType> MessageGMComponent::TYPE(new dtCore::SystemComponentType("DirectorMessageComponent", "GMComponents",
         "Passes messages back and forth between dtDirector and the Game Manager."));

   //////////////////////////////////////////////
   MessageGMComponent::MessageGMComponent(dtCore::SystemComponentType& type)
      : dtGame::GMComponent(type)
      , mIsProcessingMessages(false)
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
      mIsProcessingMessages = true;
      std::map<std::string, std::map<dtDirector::Node*, MsgFunc> >::iterator i =
         mRegisteredCallbacks.find(message.GetMessageType().GetName());

      if (i != mRegisteredCallbacks.end())
      {
         std::map<dtDirector::Node*, MsgFunc>& callbacks = i->second;

         for (std::map<dtDirector::Node*, MsgFunc>::iterator a = callbacks.begin();
            a != callbacks.end(); ++a)
         {
            if( a->first->IsEnabled() && a->first->GetDirector()->GetActive() )
            {
               a->second(message);
            }
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
            if( a->first->IsEnabled() && a->first->GetDirector()->GetActive() )
            {
               a->second(message);
            }
         }
      }
      mIsProcessingMessages = false;

      // Now clear all unregistered messages from the listing.
      if (!mDeleteQueue.empty())
      {
         for (size_t q = 0; q < mDeleteQueue.size(); ++q)
         {
            const std::string& msgType = mDeleteQueue[q].msgType;

            i = mRegisteredCallbacks.begin();
            while (i != mRegisteredCallbacks.end())
            {
               std::map<dtDirector::Node*, MsgFunc>::iterator a = i->second.begin();

               while(a != i->second.end())
               {
                  if (msgType == "" || msgType == i->first)
                  {
                     if (mDeleteQueue[q].node == a->first)
                     {
                        i->second.erase(a++);
                        break;
                     }
                  }

                  ++a;
               }

               ++i;
            }
         }

         mDeleteQueue.clear();
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
         i = mRegisteredCallbacks.insert(
            mRegisteredCallbacks.end(), std::make_pair(msgType, std::map<dtDirector::Node*, MsgFunc>()));
      }

      // Register this callback for that message type.
      if (i != mRegisteredCallbacks.end())
      {
         i->second.insert(i->second.end(), std::make_pair(node, callback));

         // If this message type is queued to be removed, make sure we
         // no longer queue it because it is being registered again.
         if (!mDeleteQueue.empty())
         {
            for (size_t q = 0; q < mDeleteQueue.size(); ++q)
            {
               if (msgType == mDeleteQueue[q].msgType &&
                   node == mDeleteQueue[q].node)
               {
                  mDeleteQueue.erase(mDeleteQueue.begin() + q);
                  break;
               }
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageGMComponent::RegisterMessage(const dtGame::MessageType& msgType, dtDirector::Node* node, MsgFunc callback)
   {
      RegisterMessage(msgType.GetName(), node, callback);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageGMComponent::UnRegisterMessage(const std::string& msgType, dtDirector::Node* node)
   {
      if (mIsProcessingMessages)
      {
         deleteQueue item;
         item.msgType = msgType;
         item.node = node;
         mDeleteQueue.push_back(item);
         return;
      }

      std::map<std::string, std::map<dtDirector::Node*, MsgFunc> >::iterator i = mRegisteredCallbacks.begin();
      while (i != mRegisteredCallbacks.end())
      {
         std::map<dtDirector::Node*, MsgFunc>::iterator a = i->second.begin();

         while(a != i->second.end())
         {
            if (msgType == "" || msgType == i->first)
            {
               if (node == a->first)
               {
                  i->second.erase(a++);
                  break;
               }
            }

            ++a;
         }

         ++i;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageGMComponent::UnRegisterMessage(const dtGame::MessageType& msgType, dtDirector::Node* node)
   {
      UnRegisterMessage(msgType.GetName(), node);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MessageGMComponent::UnRegisterMessages(dtDirector::Node* node)
   {
      UnRegisterMessage("", node);
   }
}

////////////////////////////////////////////////////////////////////////////////

