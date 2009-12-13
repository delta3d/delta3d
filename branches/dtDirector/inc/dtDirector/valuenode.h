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

#ifndef DIRECTOR_VALUE_NODE
#define DIRECTOR_VALUE_NODE

#include <dtDirector/export.h>
#include <dtDirector/node.h>

namespace dtDAL
{
   class ActorProperty;
}

namespace dtDirector
{
   class ValueLink;

   /**
    * This is the base class for all value nodes.
    *
    * @note
    *      Node objects must be created through the NodePluginRegistry or
    *      the NodeManager. If they are not created in this fashion,
    *      the node types will not be set correctly.
    */
   class DT_DIRECTOR_EXPORT ValueNode : public Node
   {
   public:

      /**
       * Constructs the Node.
       */
      ValueNode();

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
       * Connects this node to a specified value link.
       *
       * @param[in]  valueLink  The value link to connect to.
       *
       * @return     True if the connection was made.  Connection
       *              can fail based on type checking.
       */
      bool Connect(ValueLink* valueLink);

      /**
       * Disconnects this node from a specified value link.
       *
       * @param[in]  valueLink  The value link to disconnect from.
       *                         NULL to disconnect all.
       */
      virtual bool Disconnect();
      bool Disconnect(ValueLink* valueLink);

      /**
       * Event handler when a connection has changed.
       */
      virtual void OnConnectionChange();

      /**
       * Accessors for the name of the node.
       */
      virtual void SetName(const std::string& name) {mName = name;}
      virtual const std::string& GetName() {return mName;}

      /**
       * Retrieves the property for this value.
       *
       * @return  The property.
       */
      virtual dtDAL::ActorProperty* GetProperty();

      /**
       * Retrieves whether the value can be a specified type.
       *
       * @param[in]  type  The type to check.
       *
       * @return  True if this value is the proper type.
       */
      virtual bool CanBeType(dtDAL::DataType& type);

      /**
       * Retrieves the property type of this value.
       *
       * @return  The type.
       */
      virtual dtDAL::DataType& GetPropertyType();

      /**
       * Retrieves the links list.
       */
      std::vector<ValueLink*>& GetLinks() {return mLinks;}

      /**
       * Retrieves whether the UI should expose input links
       * assigned to this node.
       *
       * @return  True to expose inputs.
       */
      virtual bool InputsExposed();

      /**
       * Retrieves whether the UI should expose output links
       * assigned to this node.
       *
       * @return  True to expose outputs.
       */
      virtual bool OutputsExposed();

      /**
       * Retrieves whether the UI should expose the value links
       * assigned to this node.
       *
       * @return  True to expose values.
       */
      virtual bool ValuesExposed();

      friend class ValueLink;

   protected:

      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~ValueNode();

      std::string    mName;

      dtDAL::ActorProperty* mProperty;

      std::vector<ValueLink*> mLinks;
   };
}

#endif
