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

#ifndef ENUM_ACTOR_PROPERTY
#define ENUM_ACTOR_PROPERTY

#include <dtCore/abstractenumactorproperty.h>
#include <dtCore/export.h>
#include <dtCore/genericactorproperty.h>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents an enumeration data member.
    */
   ////////////////////////////////////////////////////////////////////////////
   template <typename T>
   class EnumActorProperty : public GenericActorProperty<T&, T&>, public AbstractEnumActorProperty
   {
      public:
         typedef GenericActorProperty<T&, T&> BaseClass;
         typedef typename BaseClass::SetFuncType SetFuncType;
         typedef typename BaseClass::GetFuncType GetFuncType;

         EnumActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           const dtUtil::RefString& desc = "", const dtUtil::RefString& groupName = "")
            : BaseClass(DataType::ENUMERATION, name, label, set, get, desc, groupName)
         {
         }

         ///@note this method exists to reconcile the two same-named methods on the superclases.
         DataType& GetPropertyType() const { return GenericActorProperty<T&, T&>::GetPropertyType(); }

         virtual const std::vector<dtUtil::Enumeration*>& GetList() const
         {
            return T::Enumerate();
         }

         virtual const std::vector<T*>& GetListOfType() const
         {
            return T::EnumerateType();
         }

         virtual bool SetValueFromString(const std::string& name)
         {
            dtUtil::Enumeration* e = T::GetValueForName(name);
            if (e != NULL)
            {
               BaseClass::SetValue(*static_cast<T*>(e));
               return true;
            }

            return false;
         }

         virtual dtUtil::Enumeration& GetEnumValue() const
         {
            return static_cast<dtUtil::Enumeration&>(GenericActorProperty<T&, T&>::GetValue());
         }

         virtual void SetEnumValue(dtUtil::Enumeration& value)
         {
            T* val = dynamic_cast<T*>(&value);
            if (val != NULL)
            {
               BaseClass::SetValue(*val);
            }
         }

         /**
          * the same as SetValueFromString
          * @see #SetValueFromString
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true if the value could be parsed.
          */
         virtual bool FromString(const std::string& value) { return SetValueFromString(value); }

         /**
          * @return a string version of the data.  This value can be used when calling FromString.
          * @see #FromString
          */
         virtual const std::string ToString() const { return GetEnumValue().GetName(); }

      protected:
         virtual ~EnumActorProperty() { }
   };
}

#endif //ENUM_ACTOR_PROPERTY
