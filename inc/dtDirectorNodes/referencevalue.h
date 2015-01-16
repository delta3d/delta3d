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

#ifndef DIRECTOR_REFERENCE_VALUE_NODE
#define DIRECTOR_REFERENCE_VALUE_NODE

#include <dtDirectorNodes/nodelibraryexport.h>
#include <dtDirector/valuenode.h>

namespace dtCore
{
   class ActorProperty;
}

namespace dtDirector
{
   class ValueLink;

   /**
    * This node finds and references another script value node
    * found elsewhere in the Director graph.  If multiple
    * value nodes with the same name are found, it will choose
    * the first one (top most tier) it finds.  This can reference
    * any value found in the entire Director graph including all
    * sub-graphs.
    *
    * @note
    *      Node objects must be created through the NodePluginRegistry or
    *      the NodeManager. If they are not created in this fashion,
    *      the node types will not be set correctly.
    */
   class NODE_LIBRARY_EXPORT ReferenceValue : public ValueNode
   {
   public:

      /**
       * Constructs the Node.
       */
      ReferenceValue();

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
       * Accessors for the name of the node.
       */
      virtual void SetName(const std::string& name);
      virtual std::string GetValueLabel();

      /**
       * Retrieves the string formatted representation for this value.
       */
      virtual std::string GetFormattedValue();

      /**
       * Sets the formatted string value.
       */
      virtual void SetFormattedValue(const std::string& value);

      /**
       * Event handler when a connection has changed.
       */
      virtual void OnConnectionChange();

      /**
       * Retrieves the total number of values linked to a property.
       *
       * @param[in]  name  The name of the property.
       *
       * @return     The count.
       */
      virtual int GetPropertyCount(const std::string& name = "Value");

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
      virtual dtCore::ActorProperty* GetProperty(const std::string& name, int index = 0, ValueNode** outNode = NULL);

      /**
       * Retrieves the property for this value.
       *
       * @param[in]   index    The property index, in case this is an array value.
       * @param[out]  outNode  The owner node of the retrieved property.
       *
       * @return  The property.
       */
      virtual dtCore::ActorProperty* GetProperty(int index, ValueNode** outNode = NULL);

      /**
       * Retrieves whether the value can be a specified type.
       *
       * @param[in]  type  The type to check.
       *
       * @return  True if this value is the proper type.
       */
      virtual bool CanBeType(dtCore::DataType& type);

      /**
       * Retrieves the type of this value.
       *
       * @return  The type.
       */
      virtual dtCore::DataType& GetPropertyType();

      /**
       * Retrieves the type of this value from the action or event node it
       * is connected to.
       *
       * @return  The type.
       */
      dtCore::DataType& GetNonReferencedPropertyType();

      /**
       *	Returns the referenced value node.
       */
      ValueNode* GetReferencedValue();

   protected:

      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~ReferenceValue();

      void UpdateLinkType();

      /**
       * Sets the referenced value.
       *
       * @param[in]  value  The referenced value name.
       */
      void SetReference(const std::string& value);

      /**
       * Retrieves the current referenced value name.
       */
      const std::string& GetReference();

      /**
       * Updates the reference node we are linked to.
       */
      void UpdateReference();

      std::string mReference;
   };
}

#endif
