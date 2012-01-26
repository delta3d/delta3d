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

#ifndef DIRECTOR_ARRAY_VALUE_NODE
#define DIRECTOR_ARRAY_VALUE_NODE

#include <dtDirector/export.h>
#include <dtDirector/valuenode.h>

#include <dtCore/arrayactorpropertybase.h>

namespace dtCore
{
   class ActorProperty;
}

namespace dtDirector
{
   class ValueLink;

   /**
    * This is the base class for all value nodes that contain an array of values.
    *
    * @note
    *      ValueNode objects must be created through the NodePluginRegistry or
    *      the NodeManager. If they are not created in this fashion,
    *      the node types will not be set correctly.
    */
   class DT_DIRECTOR_EXPORT ArrayValueNode : public ValueNode
   {
   public:

      /**
       * Constructs the ValueNode.
       */
      ArrayValueNode();

      /**
       * Initializes the ValueNode.
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
      virtual dtCore::ActorProperty* GetProperty(int index, ValueNode** outNode = NULL);

      /**
       * Retrieves the total number of values linked to a property.
       *
       * @param[in]  name  The name of the property.
       *
       * @return     The count.
       */
      virtual int GetPropertyCount(const std::string& name = "Value");

      /**
       * Event handler when the initial value property has changed.
       */
      virtual void OnInitialValueChanged(const std::string& oldValue);

      /**
       * Exposes the initial value property for this value.
       */
      virtual void ExposeInitialValue();

      /**
       * Sets the current property index.
       *
       * @param[in]  index  The index.
       */
      void SetPropertyIndex(int index);

      /**
       * Sets the current property index.
       *
       * @param[in]  index  The index.
       */
      void SetInitialPropertyIndex(int index);

      /**
       * Accessor to the array property.
       *
       * @return  The array property.
       */
      dtCore::ArrayActorPropertyBase* GetArrayProperty();

      /**
       * Accessor to the array property.
       *
       * @return  The array property.
       */
      dtCore::ArrayActorPropertyBase* GetInitialArrayProperty();

   protected:


      /**
       *    Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~ArrayValueNode();

      int   mPropertyIndex;
      int   mInitialPropertyIndex;
      dtCore::RefPtr<dtCore::ArrayActorPropertyBase> mArrayProperty;
      dtCore::RefPtr<dtCore::ArrayActorPropertyBase> mInitialArrayProperty;
   };
}

#endif
