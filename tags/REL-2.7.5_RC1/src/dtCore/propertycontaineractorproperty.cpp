/* -*-c++-*-
 * Delta3D
 * Copyright 2010, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */

#include <prefix/dtcoreprefix.h>
#include <dtCore/propertycontaineractorproperty.h>
#include <dtCore/namedparameter.h>
#include <dtCore/datatype.h>

namespace dtCore
{

   ///////////////////////////////////////////////////////////////////////
   BasePropertyContainerActorProperty::BasePropertyContainerActorProperty(
            const dtUtil::RefString& name,
            const dtUtil::RefString& label,
            const dtUtil::RefString& desc,
            const dtUtil::RefString& groupName)
   : ActorProperty(dtCore::DataType::PROPERTY_CONTAINER, name, label, desc, groupName, false)
   {
   }

   ///////////////////////////////////////////////////////////////////////
   BasePropertyContainerActorProperty::~BasePropertyContainerActorProperty() {}

   ///////////////////////////////////////////////////////////////////////
   void BasePropertyContainerActorProperty::CopyFrom(const ActorProperty& otherProp)
   {
      if (GetDataType() != otherProp.GetDataType())
      {
         LOG_ERROR("Property types are incompatible. Cannot make copy.");
         return;
      }

      const BasePropertyContainerActorProperty* prop =
         dynamic_cast<const BasePropertyContainerActorProperty* >(&otherProp);


      if (prop != NULL)
      {
         PropertyContainer* pcFrom = prop->GetValue();
         if (pcFrom == NULL)
         {
            SetValue(NULL);
         }
         else
         {
            PropertyContainer* pcTo = GetValue();
            if (pcTo == NULL)
            {
               // TODO Copy type from the other property
               CreateNew();
               pcTo = GetValue();
            }

            if (pcTo != NULL)
            {
               pcTo->CopyPropertiesFrom(*pcFrom);
            }

         }
      }
      else
      {
         LOG_ERROR("Property types are incompatible. Cannot make copy.");
      }
   }

   ///////////////////////////////////////////////////////////////////////
   const std::string BasePropertyContainerActorProperty::ToString() const
   {
      dtCore::RefPtr<NamedParameter> param = NamedParameter::CreateFromType(GetDataType(), GetName(), false);
      param->SetFromProperty(*this);
      return param->ToString();
   }

   ///////////////////////////////////////////////////////////////////////
   bool BasePropertyContainerActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      dtCore::RefPtr<NamedParameter> param = NamedParameter::CreateFromType(GetDataType(), GetName(), false);
      bool result = param->FromString(value);
      if (result)
      {
         param->ApplyValueToProperty(*this);
      }
      return result;
   }
}
