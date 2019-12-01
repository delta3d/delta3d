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

#ifndef TRIGGER_VOLUME_EVENT_NODE
#define TRIGGER_VOLUME_EVENT_NODE

#include <dtDirector/eventnode.h>
#include <dtDirectorNodes/nodelibraryexport.h>
#include <dtActors/triggervolumeactor.h>

namespace dtDirector
{
   /**
    * Event handler for when a transformable has entered or exited a
    * Trigger Volume Actor.
    *
    * @note
    *      Node objects must be created through the NodePluginRegistry or
    *      the NodeManager. If they are not created in this fashion,
    *      the node types will not be set correctly.
    */
   class NODE_LIBRARY_EXPORT TriggerVolumeEvent : public EventNode
   {
   public:

      /**
       * Constructs the Node.
       */
      TriggerVolumeEvent();

      /**
       * Initializes the Node.
       *
       * @param[in]  nodeType  The node type.
       * @param[in]  graph     The graph that owns this node.
       */
      virtual void Init(const NodeType& nodeType, DirectorGraph* graph);

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
      * Event handler for an enter or leave event from any volumes.
      *
      * @param[in]  instigator  The instigator of the event.
      * @param[in]  eventType   Event type.
      */
      void OnTriggerEvent(dtCore::Transformable* instigator, dtActors::TriggerVolumeActor::TriggerEventType eventType);

      /**
       * This event is called by value nodes that are linked via
       * value links when that value has changed.
       *
       * @param[in]  linkName  The name of the value link that is changing.
       */
      virtual void OnLinkValueChanged(const std::string& linkName);

      /**
       * Accessors for property values.
       */
      void SetTriggerVolume(const dtCore::UniqueId& value);
      dtCore::UniqueId GetTriggerVolume();

   protected:

      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~TriggerVolumeEvent();

   private:

      dtCore::UniqueId mTriggerVolume;

      std::vector<dtCore::ObserverPtr<dtActors::TriggerVolumeActor> > mVolumes; 
   };
}

#endif
