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

#include <dtCore/namedstringparameter.h>

#include <dtCore/stringactorproperty.h>

#include <dtUtil/stringutils.h>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   NamedStringParameter::NamedStringParameter(const dtUtil::RefString& name, const std::string& defaultValue,
      bool isList)
      : NamedGenericParameter<std::string>(DataType::STRING, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedStringParameter::NamedStringParameter(DataType& dataType, const dtUtil::RefString& name,
      const std::string& defaultValue, bool isList)
      : NamedGenericParameter<std::string>(dataType, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedStringParameter::~NamedStringParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string NamedStringParameter::ToString() const
   {
      if (IsList())
      {
         const std::vector<std::string> &values = GetValueList();
         std::ostringstream stream;
         if (values.empty())
         {
            stream << "null";
         }
         else if (values[0].empty())
         {
            stream << "null";
         }
         else
         {
            stream << values[0];
         }

         for (unsigned int i=1; i<values.size(); i++)
         {
            if (values[i].empty())
               stream << GetParamDelimeter() << "null";
            else
               stream << GetParamDelimeter() << values[i];
         }

         return stream.str();
      }
      else
      {
         return GetValue();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedStringParameter::FromString(const std::string& value)
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
               GetValueList().push_back("");
            else
               GetValueList().push_back(result[i]);
         }
      }
      else
      {
         SetValue(value);
      }

      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedStringParameter::SetFromProperty(const dtCore::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtCore::StringActorProperty *ap = static_cast<const dtCore::StringActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedStringParameter::ApplyValueToProperty(dtCore::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtCore::StringActorProperty *ap = static_cast<dtCore::StringActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
}
