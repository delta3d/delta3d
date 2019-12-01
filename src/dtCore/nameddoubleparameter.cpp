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

#include <prefix/dtcoreprefix.h>

#include <dtCore/nameddoubleparameter.h>

#include <dtCore/doubleactorproperty.h>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   NamedDoubleParameter::NamedDoubleParameter(const dtUtil::RefString& name, double defaultValue,
      bool isList)
      : NamedPODParameter<double>(dtCore::DataType::DOUBLE, name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedDoubleParameter::~NamedDoubleParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedDoubleParameter::SetFromProperty(const dtCore::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtCore::DoubleActorProperty *ap = static_cast<const dtCore::DoubleActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedDoubleParameter::ApplyValueToProperty(dtCore::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtCore::DoubleActorProperty *ap = static_cast<dtCore::DoubleActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedDoubleParameter::operator==(const ActorProperty& toCompare) const
   {
      if (toCompare.GetDataType() == GetDataType())
      {
         return GetValue() == static_cast<const dtCore::DoubleActorProperty&>(toCompare).GetValue();
      }
      return false;
   }
}
