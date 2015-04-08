/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2012, MOVES Institute
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
 * Jeff P. Houde
 */

#include <prefix/dtcoreprefix.h>

#include <dtCore/namedcontainerselectorparameter.h>
#include <dtCore/containerselectoractorproperty.h>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   NamedContainerSelectorParameter::NamedContainerSelectorParameter(const dtUtil::RefString& name)
      : NamedGenericParameter<std::string>(DataType::CONTAINER_SELECTOR, name, "", false)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedContainerSelectorParameter::NamedContainerSelectorParameter(DataType& dataType, const dtUtil::RefString& name)
      : NamedGenericParameter<std::string>(dataType, name, "", false)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedContainerSelectorParameter::~NamedContainerSelectorParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string NamedContainerSelectorParameter::ToString() const
   {
      return GetValue();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedContainerSelectorParameter::FromString(const std::string& value)
   {
      SetValue(value);
      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedContainerSelectorParameter::SetFromProperty(const dtCore::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtCore::ContainerSelectorActorProperty* ap = static_cast<const dtCore::ContainerSelectorActorProperty*> (&property);
      SetValue(ap->ToString());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedContainerSelectorParameter::ApplyValueToProperty(dtCore::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtCore::ContainerSelectorActorProperty *ap = static_cast<dtCore::ContainerSelectorActorProperty*> (&property);
      ap->FromString(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
}
