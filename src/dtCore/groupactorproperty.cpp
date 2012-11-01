/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
 */
#include <prefix/dtcoreprefix.h>

#include <dtUtil/log.h>

#include <dtCore/groupactorproperty.h>
#include <dtCore/datatype.h>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////
   GroupActorProperty::GroupActorProperty(const std::string& name,
                                          const std::string& label,
                                          SetFuncType set,
                                          GetFuncType get,
                                          const std::string& desc,
                                          const std::string& groupName,
                                          const std::string& editorType,
                                          bool readOnly):
                                          ActorProperty(DataType::GROUP,name,label,desc,groupName, readOnly),
                                          mSetPropFunctor(set),
                                          mGetPropFunctor(get),
                                          mEditorType(editorType)
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   GroupActorProperty::~GroupActorProperty() {}

   ////////////////////////////////////////////////////////////////////////////
   bool GroupActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      NamedGroupParameter param(GetName());
      bool result = param.FromString(value);
      if (result)
         SetValue(param);

      return result;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string GroupActorProperty::ToString() const
   {
      dtCore::RefPtr<NamedGroupParameter> param = GetValue();
      return param->ToString();
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroupActorProperty::SetValue(const NamedGroupParameter& value)
   {
      if (!IsReadOnly())
      {
         mSetPropFunctor(value);
      }
      else
      {
         LOG_WARNING("SetValue has been called on a property that is read only.");
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<NamedGroupParameter> GroupActorProperty::GetValue() const
   {
      return mGetPropFunctor();
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroupActorProperty::CopyFrom(const ActorProperty& otherProp)
   {
      if (GetDataType() != otherProp.GetDataType())
      {
         LOG_ERROR("Property types are incompatible. Cannot make copy.");
         return;
      }

      const GroupActorProperty& prop =
         static_cast<const GroupActorProperty& >(otherProp);

      SetValue(*prop.GetValue());
   }
}

