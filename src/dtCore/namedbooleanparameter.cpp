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
#include <dtCore/namedbooleanparameter.h>

#include <dtCore/booleanactorproperty.h>

#include <dtUtil/stringutils.h>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   NamedBooleanParameter::NamedBooleanParameter(const dtUtil::RefString& name, bool defaultValue, bool isList)
      : NamedPODParameter<bool>(dtCore::DataType::BOOLEAN, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedBooleanParameter::~NamedBooleanParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string NamedBooleanParameter::ToString() const
   {
      if (IsList())
      {
         std::ostringstream stream;
         const std::vector<bool> &values = GetValueList();

         if (values.empty())
         {
         }
         else if (values[0])
         {
            stream << "true";
         }
         else
         {
            stream << "false";
         }

         for (unsigned int i=1; i<values.size(); i++)
         {
            stream << GetParamDelimeter();
            if (values[i])
               stream << "true";
            else
               stream << "false";
         }

         return stream.str();
      }
      else
      {
         const char* result = GetValue() ? "true" : "false";
         return result;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedBooleanParameter::FromString(const std::string &value)
   {
      if (IsList())
      {
         std::vector<std::string> result;
         dtUtil::IsDelimeter delimCheck(GetParamDelimeter());
         dtUtil::StringTokenizer<dtUtil::IsDelimeter>::tokenize(result,value,delimCheck);

         GetValueList().clear();
         for (unsigned int i=0; i<result.size(); i++)
         {
            const std::string &str = result[i];

            bool bResult = dtUtil::ToType<bool>(str);
            GetValueList().push_back(bResult);
         }
      }
      else
      {
         bool bResult = dtUtil::ToType<bool>(value);
         SetValue(bResult);
      }

      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedBooleanParameter::SetFromProperty(const dtCore::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtCore::BooleanActorProperty *bap = static_cast<const dtCore::BooleanActorProperty*> (&property);
      SetValue(bap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedBooleanParameter::ApplyValueToProperty(dtCore::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtCore::BooleanActorProperty *bap = static_cast<dtCore::BooleanActorProperty*> (&property);
      bap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedBooleanParameter::operator==(const ActorProperty& toCompare) const
   {
      if (toCompare.GetDataType() == GetDataType())
      {
         return GetValue() == static_cast<const dtCore::BooleanActorProperty&>(toCompare).GetValue();
      }
      return false;
   }
}

