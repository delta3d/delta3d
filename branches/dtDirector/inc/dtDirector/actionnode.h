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

#ifndef DIRECTOR_ACTION_NODE
#define DIRECTOR_ACTION_NODE

#include <dtDirector/node.h>
#include <dtDirector/export.h>


namespace dtDirector
{
   /**
    * This is the base class for all action nodes.
    *
    * @note
    *      Node objects must be created through the NodePluginRegistry or
    *      the NodeManager. If they are not created in this fashion,
    *      the node types will not be set correctly.
    */
   class DT_DIRECTOR_EXPORT ActionNode : public Node
   {
   public:

      /**
       * Constructs the Node.
       */
      ActionNode();

      /**
       * Initializes the Node.
       *
       * @param[in]  nodeType  The node type.
       */
      virtual void Init(const NodeType& nodeType);

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
       *
       * @note  This should not be overloaded, instead, use the
       *         ActiveUpdate method.
       *
       * @note  This method will test all inputs and call
       *         the OnInputActivated method for all
       *         active inputs.
       *
       * @note  Any time an input is active, the mActive flag
       *         on the node will also become active.  While
       *         the node is active, this will call the ActiveUpdate
       *         method until that method returns false.
       */
      virtual void Update(float simDelta, float delta);

      /**
       * Updates the node while it is active.
       *
       * @param[in]  simDelta  The simulation time step.
       * @param[in]  delta     The real time step.
       *
       * @return     True if the node should remain active.
       *
       * @note  This method is called by the Update method
       *         only if the bActive flag is set true.
       *
       * @note  By default, this call will return false to
       *         deactivate the node.
       *
       * @note  This method can be overloaded by custom nodes
       *         to provide continuous updates while the node
       *         is active.
       */
      virtual bool ActiveUpdate(float simDelta, float delta);

      /**
       * Event handler when an input of this node has
       * been activated.
       *
       * @param[in]  inputIndex  The input that was activated.
       *
       * @note  This should be overloaded to provide functionality
       *         of the node when an input has been activated.
       *
       * @note  By default, this method will activate the first
       *         output.  Inherited actions can choose not to
       *         perform this parent implementation if this result
       *         is not desired.
       */
      virtual void OnInputActivated(int inputIndex);

   protected:

      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~ActionNode();

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

      bool    mActive;

   private:

   };
}

#endif
