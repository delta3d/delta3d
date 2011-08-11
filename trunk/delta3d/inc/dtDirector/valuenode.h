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

namespace dtCore
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
       * Retrieves the value node, if valid.
       */
      virtual ValueNode* AsValueNode();

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
       * Checks if a given property is already set to its default value.
       *
       * @param[in]  prop  The property.
       *
       * @return     True if the given property is already set to default.
       */
      virtual bool IsPropertyDefault(const dtCore::ActorProperty& prop) const;

      /**
       * Resets the value of the given property to its default value.
       *
       * @param[in]   prop  The property to reset.
       */
      virtual void ResetProperty(dtCore::ActorProperty& prop);

      /**
       * Checks if a given property should be saved out to file data.
       *
       * @param[in]  prop  The property.
       *
       * @return     True if the given property should be saved.
       */
      virtual bool ShouldPropertySave(const dtCore::ActorProperty& prop) const;

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
       * Sets the enabled status of the node.
       *
       * @param[in]  enabled   True to enable.
       */
      virtual void SetEnabled(bool enabled);

      /**
       * Accessors for the name of the node.
       */
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
       * Retrieves the string formatted representation for the initial value.
       */
      virtual std::string GetFormattedInitialValue();

      /**
       * Sets the formatted string initial value.
       */
      virtual void SetFormattedInitialValue(const std::string& value);

      /**
       * Retrieves the property for this value.
       *
       * @param[in]   index    The property index, in case this is an array value.
       * @param[out]  outNode  The owner node of the retrieved property.
       *
       * @return  The property.
       */
      dtCore::ActorProperty* GetProperty(ValueNode** outNode = NULL);
      virtual dtCore::ActorProperty* GetProperty(int index, ValueNode** outNode = NULL);

      /**
       * Retrieves the total number of values linked to a value link.
       *
       * @param[in]  name  The name of the value link.
       *
       * @return     The count.
       */
      virtual int GetPropertyCount(const std::string& name = "Value");

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
       * Retrieves the links list.
       */
      std::vector<ValueLink*>& GetLinks() {return mLinks;}

      /**
       * Event handler when the value has changed.  This will
       * notify all nodes that it is linked to that this value
       * has been changed.
       *
       * @note  Inherited classes should call this function after
       *        the property value has changed.
       */
      void OnValueChanged();

      /**
       * Event handler when the initial value property has changed.
       */
      virtual void OnInitialValueChanged(const std::string& oldValue);

      /**
       * Event handler when the value has been retrieved.
       */
      void OnValueRetrieved();

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

      /**
       * Exposes the initial value property for this value.
       */
      virtual void ExposeInitialValue();

      /**
       * Accessors for the name of the input node.
       */
      virtual void SetName(const std::string& name);

      /**
       * Accessors for the global flag.
       */
      void SetGlobal(bool value);
      bool GetGlobal() const;

      friend class ValueLink;

   protected:

      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~ValueNode();

      dtCore::RefPtr<dtCore::ActorProperty> mProperty;
      dtCore::RefPtr<dtCore::ActorProperty> mInitialProperty;

      bool mHasInitialValue;
      bool mIsGlobal;

      std::vector<ValueLink*> mLinks;
   };
}

#endif
