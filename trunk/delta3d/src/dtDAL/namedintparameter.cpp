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

#include <dtDAL/namedintparameter.h>

#include <dtDAL/intactorproperty.h>

namespace dtDAL
{
   ///////////////////////////////////////////////////////////////////////////////
   NamedIntParameter::NamedIntParameter(const dtUtil::RefString& name, int defaultValue, bool isList)
      : NamedPODParameter<int>(dtDAL::DataType::INT, name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedIntParameter::~NamedIntParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedIntParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::IntActorProperty *ap = static_cast<const dtDAL::IntActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedIntParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::IntActorProperty *ap = static_cast<dtDAL::IntActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedIntParameter::operator==(const ActorProperty& toCompare) const
   {
      if (toCompare.GetDataType() == GetDataType())
      {
         return GetValue() == static_cast<const dtDAL::IntActorProperty&>(toCompare).GetValue();
      }
      return false;
   }
}
