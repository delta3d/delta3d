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

#include <dtDirector/node.h>
#include <dtDirector/export.h>


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
        void Disconnect(ValueLink* valueLink = NULL);

        friend class ValueLink;

    protected:

        /**
         *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
         */
        virtual ~ValueNode();

        dtDAL::ActorProperty* mProperty;

        std::vector<ValueLink*> mLinks;
    };
}

#endif
