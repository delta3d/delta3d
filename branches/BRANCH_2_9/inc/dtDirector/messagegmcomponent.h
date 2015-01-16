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

#ifndef MESSAGE_COMPONENT
#define MESSAGE_COMPONENT

#include <dtDirector/export.h>
#include <dtDirector/eventnode.h>
#include <dtDirector/actionnode.h>

#include <dtCore/refptr.h>

#include <dtGame/gamemanager.h>
#include <dtGame/gmcomponent.h>

#include <dtUtil/functor.h>

#include <vector>
#include <string>

namespace dtDirector
{
   class Message;

   class DT_DIRECTOR_EXPORT MessageGMComponent: public dtGame::GMComponent
   {
   public:

      typedef dtUtil::Functor<void, TYPELIST_1(const dtGame::Message&)> MsgFunc;

      static const dtCore::RefPtr<dtCore::SystemComponentType> TYPE;

      /// Constructor
      /// @param name client code must supply a unique name for this instance.
      MessageGMComponent(dtCore::SystemComponentType& type = *TYPE);

   protected:
      /// Destructor
      virtual ~MessageGMComponent();

   public:
      /**
       * handles a sent a message
       * @param The message
       */
      virtual void DispatchNetworkMessage(const dtGame::Message& message);

      /**
       * handles a processed a message
       * @param The message
       */
      virtual void ProcessMessage(const dtGame::Message& message);

      /**
       * Called immediately after a component is added to the GM. Override this
       * to do init type behavior that needs access to the GameManager.
       */
      virtual void OnAddedToGM();

      /**
       * Called immediately after a component is removed from the GM. This is
       * where any previously allocated memory should be deallocated, files unloaded,
       * resources free'd, etc.  This gets called when the MessageGMComponent gets removed
       * from the GameManager and when the GameManager gets shut down.
       * @see GameManager::RemoveComponent()
       * @see GameManager::Shutdown()
       */
      virtual void OnRemovedFromGM();

      /**
       * Registers a Message Callback.
       *
       * @param[in]  msgType   The message type to register.
       * @param[in]  node      The node.
       * @param[in]  callback  The callback functor.
       */
      void RegisterMessage(const std::string& msgType, dtDirector::Node* node, MsgFunc callback);
      void RegisterMessage(const dtGame::MessageType& msgType, dtDirector::Node* node, MsgFunc callback);

      /**
       * Un-registers a Message Callback.
       *
       * @param[in]  msgType   The message type to unregister.
       * @param[in]  node      The node.
       */
      void UnRegisterMessage(const std::string& msgType, dtDirector::Node* node);
      void UnRegisterMessage(const dtGame::MessageType& msgType, dtDirector::Node* node);

      /**
       * Un-registers all messages connected to a given node.
       *
       * @param[in]  node  The Node.
       */
      void UnRegisterMessages(dtDirector::Node* node);

   private:

      struct deleteQueue
      {
         std::string msgType;
         dtDirector::Node* node;
      };

      std::map<std::string, std::map<dtDirector::Node*, MsgFunc> > mRegisteredCallbacks;

      bool mIsProcessingMessages;
      std::vector<deleteQueue> mDeleteQueue;
   };

} // namespace dtDirector

#endif // MESSAGE_COMPONENT
