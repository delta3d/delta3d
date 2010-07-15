/*
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
 * @author David Guthrie
 */

#include <prefix/dtdalprefix.h>

#include <dtDAL/namedcontainerparameter.h>

#include <dtDAL/containeractorproperty.h>

namespace dtDAL
{
   ///////////////////////////////////////////////////////////////////////////////
   NamedContainerParameter::NamedContainerParameter(const dtUtil::RefString& name)
      : NamedGenericParameter<std::string>(DataType::CONTAINER, name, "", false)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedContainerParameter::NamedContainerParameter(DataType& dataType, const dtUtil::RefString& name)
      : NamedGenericParameter<std::string>(dataType, name, "", false)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedContainerParameter::~NamedContainerParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string NamedContainerParameter::ToString() const
   {
      return GetValue();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedContainerParameter::FromString(const std::string& value)
   {
      SetValue(value);
      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedContainerParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::ContainerActorProperty* ap = static_cast<const dtDAL::ContainerActorProperty*> (&property);
      SetValue(ap->ToString());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedContainerParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::ContainerActorProperty *ap = static_cast<dtDAL::ContainerActorProperty*> (&property);
      ap->FromString(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
}
