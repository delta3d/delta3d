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

#include <dtCore/namedfloatparameter.h>

#include <dtCore/floatactorproperty.h>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   NamedFloatParameter::NamedFloatParameter(const dtUtil::RefString& name, float defaultValue,
      bool isList)
      : NamedPODParameter<float>(dtCore::DataType::FLOAT, name, defaultValue, isList)
   {
      SetNumberPrecision(9);
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedFloatParameter::~NamedFloatParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedFloatParameter::SetFromProperty(const dtCore::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtCore::FloatActorProperty *ap = static_cast<const dtCore::FloatActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedFloatParameter::ApplyValueToProperty(dtCore::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtCore::FloatActorProperty *ap = static_cast<dtCore::FloatActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedFloatParameter::operator==(const ActorProperty& toCompare) const
   {
      if (toCompare.GetDataType() == GetDataType())
      {
         return GetValue() == static_cast<const dtCore::FloatActorProperty&>(toCompare).GetValue();
      }
      return false;
   }
}
