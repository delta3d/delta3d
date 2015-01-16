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

#include <dtCore/namedenumparameter.h>

#include <dtCore/actorproperty.h>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   NamedEnumParameter::NamedEnumParameter(const dtUtil::RefString& name,
      const std::string& defaultValue, bool isList)
      : NamedStringParameter(dtCore::DataType::ENUMERATION, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedEnumParameter::NamedEnumParameter(const dtUtil::RefString& name, const dtUtil::Enumeration& enumValue)
   : NamedStringParameter(dtCore::DataType::ENUMERATION, name, enumValue.GetName(), false)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedEnumParameter::~NamedEnumParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedEnumParameter::SetFromProperty(const dtCore::ActorProperty& property)
   {
      ValidatePropertyType(property);

      SetValue(property.ToString());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedEnumParameter::ApplyValueToProperty(dtCore::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      if (!property.FromString(GetValue()))
         LOG_ERROR(("Failed to set the enum value \"" + GetValue() + "\" on property \"" + GetName() + "\".").c_str());
   }

   ///////////////////////////////////////////////////////////////////////////////
}
