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

#ifndef DIRECTOR_EVENT_NODE
#define DIRECTOR_EVENT_NODE

#include <dtDirector/node.h>
#include <dtDirector/export.h>
#include <dtCore/actorproperty.h>

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
   class DT_DIRECTOR_EXPORT EventNode : public Node
   {
   public:

      /**
       * Constructs the Node.
       */
      EventNode();

      /**
       * Initializes the Node.
       *
       * @param[in]  nodeType  The node type.
       * @param[in]  graph     The graph that owns this node.
       */
      virtual void Init(const NodeType& nodeType, DirectorGraph* graph);

      /**
       * Retrieves the event node, if valid.
       */
      virtual EventNode* AsEventNode();

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
       * Triggers the event.
       *
       * @param[in]  outputName    The output to trigger.
       * @param[in]  instigator    The instigating actor ID.
       * @param[in]  countTrigger  True if you want this trigger to count towards the max trigger count.
       *
       * @return     The thread ID that was created (-1 if no thread was created).
       */
      int Trigger(const std::string& outputName = "Out", const dtCore::UniqueId* instigator = NULL, bool countTrigger = true);

      /**
       * Tests whether the event should trigger.
       *
       * @param[in]  outputName  The output to trigger.
       * @param[in]  instigator  The instigating actor ID.
       */
      virtual bool Test(const std::string& outputName, const dtCore::UniqueId* instigator = NULL, bool countTrigger = true);

      /**
       * Updates the node.
       * @note  Parent implementation will auto activate any trigger
       *        with the "Out" label by default.
       *
       * @param[in]  simDelta     The simulation time step.
       * @param[in]  delta        The real time step.
       * @param[in]  input        The index to the input that is active.
       * @param[in]  firstUpdate  True if this input was just activated,
       *
       * @return     True if the current node should remain active.
       */
      virtual bool Update(float simDelta, float delta, int input, bool firstUpdate);

      /**
       * Retrieves whether the UI should expose input links
       * assigned to this node.
       *
       * @return  True to expose inputs.
       */
      virtual bool InputsExposed();

      /**
       * Retrieves whether this Event uses an instigator.
       */
      virtual bool UsesActorFilters();

      /**
       * Accessors for the trigger count property.
       */
      void SetTriggerCount(int value) {mMaxTriggerCount = value;}
      int GetTriggerCount() {return mMaxTriggerCount;}

      /**
       * Accessors for the instigator property.
       */
      void SetInstigator(const dtCore::UniqueId& id);
      dtCore::UniqueId GetInstigator();

      /**
       * Accessors for the actor filters property.
       */
      void SetActorFilters(const dtCore::UniqueId& id);
      dtCore::UniqueId GetActorFilters();

   protected:

      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~EventNode();

   private:
      int mMaxTriggerCount;
      int mTriggerCount;
      dtCore::UniqueId mInstigator;
      dtCore::UniqueId mActorFilter;

      dtCore::RefPtr<dtCore::ActorProperty> mInstigatorProp;
  };
}

#endif
