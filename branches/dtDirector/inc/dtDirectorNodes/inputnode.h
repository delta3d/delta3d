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

#ifndef DIRECTOR_INPUT_NODE
#define DIRECTOR_INPUT_NODE

#include <dtDirectorNodes/nodelibraryexport.h>
#include <dtDirector/eventnode.h>


namespace dtDirector
{
   /**
    * This node, when used inside a sub tier, will expose
    * input links when viewing that tier from the outside.
   *
   * @note
   *      Node objects must be created through the NodePluginRegistry or
   *      the NodeManager. If they are not created in this fashion,
   *      the node types will not be set correctly.
   */
   class NODE_LIBRARY_EXPORT InputNode : public EventNode
   {
   public:

      /**
       * Constructs the Node.
       */
      InputNode();

      /**
       * Initializes the Node.
       *
       * @param[in]  nodeType  The node type.
       * @param[in]  graph     The graph that owns this node.
       */
      virtual void Init(const NodeType& nodeType, DirectorGraphData* graph);

      /**
       * Triggers the event.
       *
       * @param[in]  outputIndex  The output to trigger.
       * @param[in]  instigator   The instigating actor.
       */
      virtual void Trigger(int outputIndex, const dtDAL::ActorProxy* instigator = NULL);

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
       * Updates the node.
       *
       * @param[in]  simDelta  The simulation time step.
       * @param[in]  delta     The real time step.
       */
      virtual void Update(float simDelta, float delta);

      /**
       * Accessors for the name of the input node.
       */
      void SetName(const std::string& name);
      virtual const std::string& GetName() {return mName;}

      /**
       * Retrieves whether this Event uses an instigator.
       */
      virtual bool UsesInstigator();

   protected:

      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~InputNode();

   private:

      std::string mName;
  };
}

#endif
