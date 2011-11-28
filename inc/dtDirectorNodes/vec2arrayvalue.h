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

#ifndef DIRECTOR_VEC2_ARRAY_VALUE_NODE
#define DIRECTOR_VEC2_ARRAY_VALUE_NODE

#include <dtDirector/arrayvaluenode.h>
#include <dtDirectorNodes/nodelibraryexport.h>


namespace dtDirector
{
   /**
    * This is the base class for all value nodes.
    *
    * @note
    *      Node objects must be created through the NodePluginRegistry or
    *      the NodeManager. If they are not created in this fashion,
    *      the node types will not be set correctly.
    */
   class NODE_LIBRARY_EXPORT Vec2ArrayValue : public ArrayValueNode
   {
   public:

      /**
       * Constructs the Node.
       */
      Vec2ArrayValue();

      /**
       *  Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~Vec2ArrayValue();

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


   private:

      /**
       * Sets the value.
       *
       * @param[in]  value  The value.
       */
      void SetValue(const osg::Vec2& value);

      /**
       * Retrieves the value.
       *
       * @return  The value.
       */
      osg::Vec2 GetValue();

      /**
       * Retrieves the default value for a new array element.
       *
       * @return  The default value.
       */
      osg::Vec2 GetDefaultValue();

      /**
       * Retrieves the value array.
       *
       * @return  The array.
       */
      std::vector<osg::Vec2> GetArray();

      /**
       * Sets the current value array.
       *
       * @param[in]  value  The new value.
       */
      void SetArray(const std::vector<osg::Vec2>& value);

      /**
       * Sets the value.
       *
       * @param[in]  value  The value.
       */
      void SetInitialValue(const osg::Vec2& value);

      /**
       * Retrieves the value.
       *
       * @return  The value.
       */
      osg::Vec2 GetInitialValue();

      /**
       * Retrieves the value array.
       *
       * @return  The array.
       */
      std::vector<osg::Vec2> GetInitialArray();

      /**
       * Sets the current value array.
       *
       * @param[in]  value  The new value.
       */
      void SetInitialArray(const std::vector<osg::Vec2>& value);

      std::vector<osg::Vec2> mValues;
      std::vector<osg::Vec2> mInitialValues;
   };
}

#endif
