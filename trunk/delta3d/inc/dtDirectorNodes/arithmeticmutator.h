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

#ifndef ARITHMETIC_MUTATOR_NODE
#define ARITHMETIC_MUTATOR_NODE

////////////////////////////////////////////////////////////////////////////////

#include <dtDirector/mutatornode.h>
#include <dtDirectorNodes/nodelibraryexport.h>


namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   class NODE_LIBRARY_EXPORT ArithmeticMutator: public MutatorNode
   {
   public:

      /**
       * Constructor.
       */
      ArithmeticMutator();

      /**
       * Initializes the Node.
       *
       * @param[in]  nodeType  The node type.
       * @param[in]  graph     The graph that owns this node.
       */
      virtual void Init(const NodeType& nodeType, DirectorGraph* graph);

      /**
       * Event handler called when a script has finished loading.
       */
      virtual void OnFinishedLoading();

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
       * Retrieves whether the value can be a specified type.
       *
       * @param[in]  type  The type to check.
       *
       * @return  True if this value is the proper type.
       */
      virtual bool CanBeType(dtCore::DataType& type);

      /**
       * Retrieves the property type of this value.
       *
       * @return  The type.
       */
      virtual dtCore::DataType& GetPropertyType();

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
       * Performs the arithmetic operation.
       *
       * @param[in]  left   The left value.
       * @param[in]  right  The right value.
       *
       * @return     The result.
       */
      virtual osg::Vec4 PerformOperation(const osg::Vec4& left, const osg::Vec4& right) = 0;

      /**
       * Retrieves the stronger data type.
       *
       * @param[in]  type1  The first type.
       * @param[in]  type2  The second type.
       *
       * @return     The stronger type.
       */
      dtCore::DataType& GetStrongerType(dtCore::DataType& type1, dtCore::DataType& type2);

      /**
       * Accessors for property values.
       */
      void SetA(double value);
      double GetA() const;

      void SetB(double value);
      double GetB() const;

      void SetResult(const std::string& value);
      const std::string& GetResult() const;

   protected:

      /**
       * Destructor.
       */
      ~ArithmeticMutator();

   private:

      double mValueA;
      double mValueB;

      std::string mResult;

      dtCore::DataType* mResultType;
   };
}

////////////////////////////////////////////////////////////////////////////////

#endif // ARITHMETIC_MUTATOR_NODE
