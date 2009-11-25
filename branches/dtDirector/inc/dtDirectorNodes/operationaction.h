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

#ifndef OPERATION_ACTION_NODE
#define OPERATION_ACTION_NODE

////////////////////////////////////////////////////////////////////////////////

#include <dtDirector/actionnode.h>
#include <dtDirectorNodes/nodelibraryexport.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   class NODE_LIBRARY_EXPORT OperationAction: public ActionNode
   {
   public:

      /**
       * Constructor.
       */
      OperationAction();

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
       * Event handler when an input of this node has
       * been activated.
       *
       * @param[in]  inputIndex  The input that was activated.
       *
       * @note  This should be overloaded to provide functionality
       *         of the node when an input has been activated.
       */
      virtual void OnInputActivated(int inputIndex);

      /**
       * Accessors for property values.
       */
      void SetA(int value);
      int GetA();

      void SetB(int value);
      int GetB();

      void SetResult(int value);
      int GetResult();


   protected:

      /**
       * Destructor.
       */
      ~OperationAction();

   private:

      int mValueA;
      int mValueB;
      int mResult;
   };
}

////////////////////////////////////////////////////////////////////////////////

#endif // OPERATION_ACTION_NODE
