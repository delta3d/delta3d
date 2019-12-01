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

#include <dtCore/namedactorparameter.h>

#include <dtCore/actorproperty.h>

#include <dtUtil/stringutils.h>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   NamedActorParameter::NamedActorParameter(const dtUtil::RefString& name,
      const dtCore::UniqueId& defaultValue, bool isList)
      : NamedGenericParameter<dtCore::UniqueId>(dtCore::DataType::ACTOR, name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedActorParameter::~NamedActorParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string NamedActorParameter::ToString() const
   {
      if (IsList())
      {
         std::ostringstream stream;
         const std::vector<dtCore::UniqueId> &values = GetValueList();

         if (values.empty())
         {
            stream << "null";
         }
         else if (values[0].ToString().empty())
         {
            stream << "null";
         }
         else
         {
            stream << values[0].ToString();
         }

         for (unsigned int i=1; i<values.size(); i++)
         {
            if (values[i].ToString().empty())
               stream << GetParamDelimeter() << "null";
            else
               stream << GetParamDelimeter() << values[i].ToString();
         }

         return stream.str();
      }
      else
         return GetValue().ToString();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedActorParameter::FromString(const std::string& value)
   {
      if (IsList())
      {
         std::vector<std::string> result;
         dtUtil::IsDelimeter delimCheck(GetParamDelimeter());
         dtUtil::StringTokenizer<dtUtil::IsDelimeter>::tokenize(result,value,delimCheck);

         GetValueList().clear();
         for (unsigned int i=0; i<result.size(); i++)
         {
            if (result[i] == "null")
               GetValueList().push_back(dtCore::UniqueId(""));
            else
               GetValueList().push_back(dtCore::UniqueId(result[i]));
         }
      }
      else
      {
         SetValue(dtCore::UniqueId(value));
      }

      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedActorParameter::SetFromProperty(const dtCore::ActorProperty& property)
   {
      ValidatePropertyType(property);

      if (property.GetDataType() == DataType::ACTOR)
      {
         dtCore::UniqueId id(property.ToString());
         SetValue(id);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedActorParameter::ApplyValueToProperty(dtCore::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      if (property.GetDataType() == DataType::ACTOR)
      {
         std::string id(GetValue().ToString());
         property.FromString(id);
      }
      else
      {
         throw dtCore::InvalidParameterException(
            "Cannot call ApplyValueToProperty()on an ActorMessageParameter.  See the GameActor::ApplyActorUpdate() for an example of how to do this.",
            __FILE__, __LINE__);
      }
   }
}
