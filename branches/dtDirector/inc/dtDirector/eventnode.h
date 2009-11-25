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


namespace dtDAL
{
    class ActorProxy;
}

namespace dtDirector
{
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
       */
      virtual void Init(const NodeType& nodeType);

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

   protected:

      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~EventNode();

      /**
       * Retrieves the total number of values linked to a property.
       *
       * @param[in]  name  The name of the property.
       *
       * @return     The count.
       */
      virtual int GetPropertyCount(const std::string& name);

      /**
       * Retrieves a property of the given name.  This is overloaded
       * to provide functionality of redirected properties (from the
       * use of ValueLink's).
       *
       * @param[in]  name   The name of the property.
       * @param[in]  index  The property index, in case of multiple linking.
       *
       * @return     A pointer to the property, NULL if none found.
       *
       * @note  All properties used within nodes should be retrieved
       *         via this method instead of directly to ensure that
       *         the desired property is being used.
       */
      virtual dtDAL::ActorProperty* GetProperty(const std::string& name, int index = 0);

   private:

  };
}

#endif
