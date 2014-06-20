/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * David Guthrie
 */
#ifndef _TEST_COMPONENT_H_
#define _TEST_COMPONENT_H_
#include <dtCore/refptr.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/messagetype.h>
#include <dtGame/messagefactory.h>

// for std::exception
#include <exception>

#include <dtCore/refptr.h>

#include <vector>

namespace dtGame
{
   class Message;
   class MessageType;

   class TestComponent: public GMComponent
   {
   public:
      typedef GMComponent BaseClass;

      TestComponent(const std::string& name = "TestComponent")
      : BaseClass(name)
      , mWasOnAddedToGMCalled(false)
      , mWasOnRemovedFromGMCalled(false)
      {}

      std::vector<dtCore::RefPtr<const dtGame::Message> >& GetReceivedProcessMessages()
                  { return mReceivedProcessMessages; }
      std::vector<dtCore::RefPtr<const dtGame::Message> >& GetReceivedDispatchNetworkMessages()
                  { return mReceivedDispatchNetworkMessages; }

      virtual void OnAddedToGM()
      {
         mWasOnAddedToGMCalled = true;
      }

      virtual void OnRemovedFromGM()
      {
         if (GetGameManager() == NULL)
         {
         }
         mWasOnRemovedFromGMCalled = true;
      }

      virtual void ProcessMessage(const dtGame::Message& msg)
      {
         mReceivedProcessMessages.push_back(&msg);
      }
      virtual void DispatchNetworkMessage(const dtGame::Message& msg)
      {
         mReceivedDispatchNetworkMessages.push_back(&msg);
      }

      void reset()
      {
         mReceivedDispatchNetworkMessages.clear();
         mReceivedProcessMessages.clear();
      }

      dtCore::RefPtr<const dtGame::Message> FindProcessMessageOfType(const dtGame::MessageType& type)
      {
         for (unsigned i = 0; i < mReceivedProcessMessages.size(); ++i)
         {
            if (mReceivedProcessMessages[i]->GetMessageType() == type)
               return mReceivedProcessMessages[i];
         }
         return NULL;
      }

      dtCore::RefPtr<const dtGame::Message> FindDispatchNetworkMessageOfType(const dtGame::MessageType& type)
      {
         for (unsigned i = 0; i < mReceivedDispatchNetworkMessages.size(); ++i)
         {
            if (mReceivedDispatchNetworkMessages[i]->GetMessageType() == type)
               return mReceivedDispatchNetworkMessages[i];
         }
         return NULL;
      }

      bool mWasOnAddedToGMCalled;
      bool mWasOnRemovedFromGMCalled;

   protected:

      virtual ~TestComponent()
      {
      }

      std::vector<dtCore::RefPtr<const dtGame::Message> > mReceivedProcessMessages;
      std::vector<dtCore::RefPtr<const dtGame::Message> > mReceivedDispatchNetworkMessages;
   };
}

#endif
