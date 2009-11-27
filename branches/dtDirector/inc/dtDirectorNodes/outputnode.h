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

#ifndef DIRECTOR_OUTPUT_NODE
#define DIRECTOR_OUTPUT_NODE

#include <dtDirectorNodes/nodelibraryexport.h>
#include <dtDirector/actionnode.h>


namespace dtDirector
{
   /**
    * This node, when used inside a sub tier, will expose
    * output links when viewing that tier from the outside.
    *
    * @note
    *      Node objects must be created through the NodePluginRegistry or
    *      the NodeManager. If they are not created in this fashion,
    *      the node types will not be set correctly.
    */
   class NODE_LIBRARY_EXPORT OutputNode : public ActionNode
   {
   public:

      /**
       * Constructs the Node.
       */
      OutputNode();

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
       * Accessors for the name of the input node.
       */
      void SetName(const std::string& name) {mName = name;}
      std::string GetName() {return mName;}

   protected:

      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~OutputNode();

   private:

      std::string mName;
   };
}

#endif
