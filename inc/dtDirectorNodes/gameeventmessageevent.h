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

#ifndef DIRECTOR_GAME_EVENT_MESSAGE_EVENT_NODE
#define DIRECTOR_GAME_EVENT_MESSAGE_EVENT_NODE

#include <dtDirector/eventnode.h>
#include <dtDirectorNodes/nodelibraryexport.h>

#include <dtGame/message.h>

namespace dtCore
{
   class GameEvent;
}

namespace dtDirector
{
   class MessageGMComponent;

   /**
    * This is the base class for all event nodes.
    *
    * @note
    *      Node objects must be created through the NodePluginRegistry or
    *      the NodeManager. If they are not created in this fashion,
    *      the node types will not be set correctly.
    */
   class NODE_LIBRARY_EXPORT GameEventMessageEvent : public EventNode
   {
   public:

      /**
       * Constructs the Node.
       */
      GameEventMessageEvent();

      /**
       * This method is called in init, which instructs the node
       * to create its properties.  Methods implementing this should
       * be sure to call their parent class's buildPropertyMap method to
       * ensure all properties in the proxy inheritance hierarchy are
       * correctly added to the property map.
       *
       * @see GetDeprecatedProperty to handle old properties that need
       *       to be removed.
       */
      virtual void BuildPropertyMap();

      /**
       * Retrieves whether this Event uses an instigator.
       */
      virtual bool UsesActorFilters();

      /**
       * Determines whether a value link on this node can connect
       * to a given value.
       *
       * @param[in]  link   The link.
       * @param[in]  value  The value to connect to.
       *
       * @return     True if a connection can be made.
       */
      virtual bool CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value);

      /**
       * This event is called by value nodes that are linked via
       * value links when that value has changed.
       *
       * @param[in]  linkName  The name of the value link that is changing.
       */
      virtual void OnLinkValueChanged(const std::string& linkName);

      /**
       * Registration function for registering message types this
       * node will receive.
       * Note: This should be rewritten in inherited classes.
       */
      virtual void RegisterMessages();
      virtual void UnRegisterMessages();

      /**
       * Accessor for the message type property.
       */
      void SetEvent(dtCore::GameEvent* value);
      dtCore::GameEvent* GetEvent() const;

      /**
       * Callback to receive message events.
       *
       * @param[in]  message  The message data.
       */
      void OnMessage(const dtGame::Message& message);

      /**
       * Updates the label.
       */
      void UpdateName();

   protected:

      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~GameEventMessageEvent();

   private:
      dtCore::GameEvent* mEvent;
   };
}

#endif
