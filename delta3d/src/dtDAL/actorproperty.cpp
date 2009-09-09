/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009, Alion Science and Technology.
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
 * David Guthrie
 * Matthew W. Campbell
 */

#include <dtDAL/actorproperty.h>
#include <iostream>

//For initial ToDataStream solution. Eventually this should be removed when all properties can do the work themselves.
#include <dtDAL/namedparameter.h>

namespace dtDAL
{
   ActorProperty::ActorProperty(dtDAL::DataType& dataType,
                 const dtUtil::RefString& name,
                 const dtUtil::RefString& label,
                 const dtUtil::RefString& desc,
                 const dtUtil::RefString& groupName,
                 bool  readOnly)
      : AbstractParameter(dataType, name)
      , mLabel(label)
      , mDescription(desc)
      , mNumberPrecision(16)
      , mReadOnly(readOnly)
   {
      groupName->empty() ? SetGroupName("Base") : SetGroupName(groupName);
   }

   ////////////////////////////////////////
   ActorProperty::~ActorProperty() { }

   ////////////////////////////////////////
   void ActorProperty::SetNumberPrecision(unsigned int precision)
   {
      mNumberPrecision = precision;
   }

   ////////////////////////////////////////
   unsigned int ActorProperty::GetNumberPrecision() const
   {
      return mNumberPrecision;
   }

   ////////////////////////////////////////
   void ActorProperty::ToDataStream(dtUtil::DataStream& stream) const
   {
      dtCore::RefPtr<dtDAL::NamedParameter> param = NamedParameter::CreateFromType(GetDataType(), GetName(), false);
      param->SetFromProperty(*this);
      param->ToDataStream(stream);
   }

   ////////////////////////////////////////
   bool ActorProperty::FromDataStream(dtUtil::DataStream& stream)
   {
      if (IsReadOnly())
      {
         return false;
      }

      dtCore::RefPtr<dtDAL::NamedParameter> param = NamedParameter::CreateFromType(GetDataType(), GetName(), false);
      bool result = param->FromDataStream(stream);
      if (result)
      {
         param->ApplyValueToProperty(*this);
      }
      return result;
   }

   ////////////////////////////////////////
   ActorProperty::ActorProperty(const ActorProperty& toCopy):
      AbstractParameter(GetDataType(), GetName()) { }

   ////////////////////////////////////////
   ActorProperty& ActorProperty::operator=(const ActorProperty&) { return *this; }

   ////////////////////////////////////////
   std::ostream& operator << (std::ostream& o, const ActorProperty& prop)
   {
      o << prop.ToString();
      return o;
   }

   ////////////////////////////////////////
   std::istream& operator >> (std::istream& i, ActorProperty& prop)
   {
      std::string value;
      i >> value;
      prop.FromString(value);
      return i;
   }
}
