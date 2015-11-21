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

#include <prefix/dtcoreprefix.h>
#include <dtCore/actorproperty.h>
#include <dtCore/namedparameter.h>
#include <dtCore/objecttype.h>
#include <iostream>

#include <dtCore/refptr.h>

namespace dtCore
{
   ActorProperty::ActorProperty(dtCore::DataType& dataType,
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
      , mMultipleEdit(true)
      , mSendInPartialUpdate(false)
      , mSendInFullUpdate(true)
      , mAdvanced(false)
      , mIgnoreWhenSaving(false)
      , mAlwaysSave(false)

   {
      groupName->empty() ? SetGroupName("Base") : SetGroupName(groupName);
   }

   ////////////////////////////////////////
   ActorProperty::~ActorProperty() { }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorProperty::InitDefault(ObjectType& type)
   {
      // Don't store defaults for properties that are read only.
      if (IsReadOnly())
      {
         return;
      }

      dtCore::RefPtr<NamedParameter> param = NamedParameter::CreateFromType(
         GetDataType(), GetName());
      if (param)
      {
         param->SetFromProperty(*this);
         type.SetDefaultValue(GetName(), *param);
      }
   }

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
      dtCore::RefPtr<NamedParameter> param = NamedParameter::CreateFromType(GetDataType(), GetName(), false);
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

      dtCore::RefPtr<NamedParameter> param = NamedParameter::CreateFromType(GetDataType(), GetName(), false);
      bool result = param->FromDataStream(stream);
      if (result)
      {
         param->ApplyValueToProperty(*this);
      }
      return result;
   }

   ////////////////////////////////////////
   void ActorProperty::CopyMetadata(const ActorProperty& otherProp)
   {
      mReadOnly = otherProp.mReadOnly;
      mMultipleEdit = otherProp.mMultipleEdit;
      mSendInPartialUpdate = otherProp.mSendInPartialUpdate;
      mSendInFullUpdate = otherProp.mSendInFullUpdate;
      mAdvanced = otherProp.mAdvanced;
      mIgnoreWhenSaving = otherProp.mIgnoreWhenSaving;
      mAlwaysSave = otherProp.mAlwaysSave;
   }

   ////////////////////////////////////////
   // C++ 11 version.  This avoids warnings uninitialized variable warnings on clang and in eclipse, but won't build in VS 2010
//   ActorProperty::ActorProperty(const ActorProperty& toCopy):
//      ActorProperty(toCopy.GetDataType(), toCopy.GetName(), toCopy.GetLabel(), toCopy.GetDescription(), toCopy.GetGroupName(), toCopy.IsReadOnly()) {  }

   ////////////////////////////////////////
   // This one works in VS 2010, remove once it is no longer supported.
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
