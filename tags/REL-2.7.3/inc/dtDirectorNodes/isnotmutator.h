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

#ifndef IS_NOT_MUTATOR_NODE
#define IS_NOT_MUTATOR_NODE

////////////////////////////////////////////////////////////////////////////////

#include <dtDirector/mutatornode.h>
#include <dtDirectorNodes/nodelibraryexport.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   class NODE_LIBRARY_EXPORT IsNotMutator: public MutatorNode
   {
   public:

      /**
       * Constructor.
       */
      IsNotMutator();

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
       * Accessors for the name of the node.
       */
      virtual std::string GetValueLabel();

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
      void SetValue(bool value) {mValue = value;}
      bool GetValue() const     {return mValue;}

      void SetResult(bool value) {mResult = value;}
      bool GetResult() const     {return mResult;}

   protected:

      /**
       * Destructor.
       */
      ~IsNotMutator();

   private:

      bool mValue;
      bool mResult;
   };
}

////////////////////////////////////////////////////////////////////////////////

#endif // IS_NOT_MUTATOR_NODE
