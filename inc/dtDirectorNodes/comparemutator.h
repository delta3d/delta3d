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

#ifndef COMPARE_MUTATOR_NODE
#define COMPARE_MUTATOR_NODE

////////////////////////////////////////////////////////////////////////////////

#include <dtDirector/mutatornode.h>
#include <dtDirectorNodes/nodelibraryexport.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   class NODE_LIBRARY_EXPORT CompareMutator: public MutatorNode
   {
   public:

      /**
       * Constructor.
       */
      CompareMutator();

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
       * This event is called by value nodes that are linked via
       * value links when that value has changed.
       *
       * @param[in]  linkName  The name of the value link that is changing.
       */
      virtual void OnLinkValueChanged(const std::string& linkName);

      /**
       * Determines whether a value link on this node can connect
       * to a given value.
       *
       * @param[in]  link   The link.
       * @param[in]  value  The value to connect to.
       *
       * @return     True if a connection can be made.
       */
      virtual bool CanConnectValue(ValueLink* link, ValueNode* value);

      /**
       * Performs the compare operation.
       *
       * @param[in]  left   The left value.
       * @param[in]  right  The right value.
       *
       * @return     The result.
       */
      virtual bool Compare(const osg::Vec4& left, const osg::Vec4f& right) = 0;
      virtual bool Compare(const std::string& left, const std::string& right) {return false;}

      /**
       * Accessors for property values.
       */
      void SetA(double value) {mValueA = value;}
      double GetA() const     {return mValueA;}

      void SetB(double value) {mValueB = value;}
      double GetB() const     {return mValueB;}

      void SetResult(bool value) {mResult = value;}
      bool GetResult() const     {return mResult;}

   protected:

      /**
       * Destructor.
       */
      ~CompareMutator();

   private:

      double mValueA;
      double mValueB;

      bool   mResult;
   };
}

////////////////////////////////////////////////////////////////////////////////

#endif // COMPARE_MUTATOR_NODE
