/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * Matthew W. Campbell
 * David Guthrie
 * William E. Johnson II
 * Eric Heine
 */

#ifndef ABSTRACT_ENUM_ACTOR_PROPERTY
#define ABSTRACT_ENUM_ACTOR_PROPERTY

#include <dtCore/actorproperty.h>
#include <dtCore/datatype.h>
#include <dtCore/export.h>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents an abstract enumeration data member.
    * @note This abstract class exists so that all enumerations can be cast to this and worked with generically.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT AbstractEnumActorProperty
   {
      public:

         /**
          * @return the datatype for this enum actor property.
          */
         virtual DataType& GetPropertyType() const = 0;

         /**
          * @return the value as a generic enumeration
          */
         virtual dtUtil::Enumeration& GetEnumValue() const = 0;

         /**
          * Sets the value as a generic enumeration, but it must be the actual type stored
          * in the implementation of this pure virtual class.
          * @param value
          */
         virtual void SetEnumValue(dtUtil::Enumeration& value) = 0;

         /**
          * @return A list of the possibilities for this enumeration property
          */
         virtual const std::vector<dtUtil::Enumeration*>& GetList() const = 0;

         /**
          * Sets the value of the property based on the string name of the enumeration, i.e. getName()
          * on Enumeration.  If it fails, the value will remain unchanged.
          * @param name The name of the value.
          * @return true if it was successful in matching an name to a value.
          * @see Enumeration#getName
          */
         virtual bool SetValueFromString(const std::string& name) = 0;

         /**
          * This is a convenience method and enforces that only actor properties can implement this interface.
          * @return the property as an actor property.
          */
         virtual ActorProperty* AsActorProperty();

         /**
          * This is a convenience method and enforces that only actor properties can implement this interface.
          * @return the property as an actor property.
          */
         virtual const ActorProperty* AsActorProperty() const;

      protected:
         virtual ~AbstractEnumActorProperty() { }
   };
}

#endif //ABSTRACT_ENUM_ACTOR_PROPERTY
