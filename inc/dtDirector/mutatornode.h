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

#ifndef MUTATOR_VALUE_NODE
#define MUTATOR_VALUE_NODE

////////////////////////////////////////////////////////////////////////////////

#include <dtDirector/valuenode.h>
#include <dtDirector/export.h>
#include <dtCore/actorproperty.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   class DT_DIRECTOR_EXPORT MutatorNode: public ValueNode
   {
   public:

      /**
       * Constructor.
       */
      MutatorNode();

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
       * Retrieves a property of the given name.  This is overloaded
       * to provide functionality of redirected properties (from the
       * use of ValueLink's).
       *
       * @param[in]  name     The name of the property.
       * @param[in]  index    The property index, in case of multiple linking.
       * @param[in]  outNode  If provided, will retrieve the value node that owns this property (if any).
       *
       * @return     A pointer to the property, NULL if none found.
       *
       * @note  All properties used within nodes should be retrieved
       *         via this method instead of directly to ensure that
       *         the desired property is being used.
       */
      virtual dtCore::ActorProperty* GetProperty(const std::string& name, int index = 0, ValueNode** outNode = NULL);

      /**
       * Retrieves the total number of values linked to a value link.
       *
       * @param[in]  name  The name of the value link.
       *
       * @return     The count.
       */
      virtual int GetPropertyCount(const std::string& name = "Value");

      /**
       * Retrieves whether the UI should expose the value links
       * assigned to this node.
       *
       * @return  True to expose values.
       */
      virtual bool ValuesExposed() {return true;}

   protected:

      /**
       * Destructor.
       */
      ~MutatorNode();

   private:

   };
}

////////////////////////////////////////////////////////////////////////////////

#endif // MUTATOR_VALUE_NODE
