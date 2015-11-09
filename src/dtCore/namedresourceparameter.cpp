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

#include <dtCore/namedresourceparameter.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/datatype.h>

#include <dtCore/resourceactorproperty.h>

#include <dtUtil/datastream.h>
#include <dtUtil/stringutils.h>

#include <sstream>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   NamedResourceParameter::NamedResourceParameter(dtCore::DataType& type,
      const dtUtil::RefString& name, bool isList)
      : NamedParameter(type, name,isList)
   {
      if (IsList())
         mValueList = new std::vector<dtCore::ResourceDescriptor>();
      else
         mValueList = NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedResourceParameter::~NamedResourceParameter()
   {
      delete mValueList;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedResourceParameter::ToDataStream(dtUtil::DataStream &stream) const
   {
      if (IsList())
      {
         stream << unsigned(mValueList->size());
         for (unsigned int i=0; i<mValueList->size(); i++)
         {
            stream << (*mValueList)[i].GetResourceIdentifier();
            stream << (*mValueList)[i].GetDisplayName();
         }
      }
      else
      {
         stream << mDescriptor.GetResourceIdentifier();
         stream << mDescriptor.GetDisplayName();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedResourceParameter::FromDataStream(dtUtil::DataStream &stream)
   {
      bool okay = true;

      std::string resourceId;
      std::string resourceDisplayName;

      if (IsList())
      {
         mValueList->clear();

         unsigned int listSize;
         stream >> listSize;
         for (unsigned int i=0; i<listSize; i++)
         {
            stream >> resourceId;
            stream >> resourceDisplayName;
            mValueList->push_back(dtCore::ResourceDescriptor(resourceDisplayName, resourceId));
         }
      }
      else
      {
         stream >> resourceId;
         stream >> resourceDisplayName;
         mDescriptor = dtCore::ResourceDescriptor(resourceDisplayName, resourceId);
      }

      return okay;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string NamedResourceParameter::ToString() const
   {
      std::ostringstream stream;

      if (IsList())
      {
         const std::vector<dtCore::ResourceDescriptor> &values = GetValueList();
         if (!values.empty())
         {
            stream << values[0].GetDisplayName() << "/" << values[0].GetResourceIdentifier();
         }

         for (unsigned int i=1; i<values.size(); i++)
         {
            stream << GetParamDelimeter() << values[i].GetDisplayName() <<
               "/" << values[i].GetResourceIdentifier();
         }
      }
      else
      {
         const dtCore::ResourceDescriptor r = GetValue();
         if (r.IsEmpty() == false)
            stream << r.GetDisplayName() << "/" << r.GetResourceIdentifier();
      }

      return stream.str();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedResourceParameter::FromString(const std::string& value)
   {
      bool result = true;
      if (value.empty() || value == "NULL")
      {
         if (IsList())
            mValueList->clear();
         else
            SetValue(dtCore::ResourceDescriptor::NULL_RESOURCE);

         return true;
      }

      std::string displayName;
      std::string identifier;
      std::vector<std::string> tokens;
      if (IsList())
      {
         std::vector<std::string> result;
         unsigned int i;
         dtUtil::IsDelimeter delimCheck(GetParamDelimeter());
         dtUtil::StringTokenizer<dtUtil::IsDelimeter>::tokenize(result,value,delimCheck);

         mValueList->clear();
         for (i=0; i<result.size(); i++)
         {
            tokens.clear();

            dtUtil::StringTokenizer<dtUtil::IsSlash>::tokenize(tokens,result[i]);
            if (tokens.size() == 2)
            {
               displayName = tokens[0];
               identifier = tokens[1];
            }
            else
            {
               //assume the value is a descriptor and use it for both the
               //data and the display name.
               displayName = tokens[0];
               identifier = tokens[0];
            }

            dtUtil::Trim(identifier);
            dtUtil::Trim(displayName);
            mValueList->push_back(dtCore::ResourceDescriptor(displayName, identifier));
         }
      }
      else
      {
         dtUtil::StringTokenizer<dtUtil::IsSlash>::tokenize(tokens,value);
         if (tokens.size() == 2)
         {
            displayName = tokens[0];
            identifier = tokens[1];
         }
         else
         {
            //assume the value is a descriptor and use it for both the
            //data and the display name.
            displayName = tokens[0];
            identifier = tokens[0];
         }

         dtUtil::Trim(identifier);
         dtUtil::Trim(displayName);

         dtCore::ResourceDescriptor descriptor(displayName,identifier);
         SetValue(descriptor);
      }

      return result;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedResourceParameter::CopyFrom(const NamedParameter& otherParam)
   {
      const NamedResourceParameter *param =
         dynamic_cast<const NamedResourceParameter*>(&otherParam);


      //First make sure this parameter does not have a list if the
      //other parameter does and vice versa.
      if ((IsList() && !otherParam.IsList()) ||(!IsList() && otherParam.IsList()))
         throw dtCore::BaseException(
         "Cannot assign two parameters with one being a list of values and the other not.",
         __FILE__, __LINE__);

      if (param != NULL)
      {
         if (!IsList())
            SetValue(param->GetValue());
         else
            SetValueList(param->GetValueList());
      }
      else
      {
         if (!FromString(otherParam.ToString()))
            LOGN_ERROR("MessageParameter", "Parameter types are incompatible. Cannot copy the value.");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NamedResourceParameter::SetValue(const dtCore::ResourceDescriptor& descriptor)
   {
      if (IsList())
         throw dtCore::BaseException(
         "Cannot call SetValue() on message parameter with a list of values.", __FILE__, __LINE__);

      mDescriptor = descriptor;

   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtCore::ResourceDescriptor NamedResourceParameter::GetValue() const
   {
      if (IsList())
         throw dtCore::BaseException(
         "Cannot call GetValue() on message parameter with a list of values.", __FILE__, __LINE__);

      return mDescriptor;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::vector<dtCore::ResourceDescriptor> &NamedResourceParameter::GetValueList() const
   {
      if (!IsList())
         throw dtCore::BaseException(
         "Cannot retrieve the parameters value list.  Parameter does not contain a list.", __FILE__, __LINE__);
      return *mValueList;
   }

   ///////////////////////////////////////////////////////////////////////////////
   std::vector<dtCore::ResourceDescriptor> &NamedResourceParameter::GetValueList()
   {
      if (!IsList())
         throw dtCore::BaseException(
         "Cannot retrieve the parameters value list.  Parameter does not contain a list.", __FILE__, __LINE__);
      return *mValueList;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedResourceParameter::SetValueList(const std::vector<dtCore::ResourceDescriptor> &newValues)
   {
      if (!IsList())
         throw dtCore::BaseException(
         "Cannot set a list of new values on a parameter that is not a list.", __FILE__, __LINE__);

      *mValueList = newValues;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedResourceParameter::SetFromProperty(const dtCore::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtCore::ResourceActorProperty *ap = static_cast<const dtCore::ResourceActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedResourceParameter::ApplyValueToProperty(dtCore::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtCore::ResourceActorProperty *vap = static_cast<dtCore::ResourceActorProperty*> (&property);
      vap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedResourceParameter::operator==(const NamedParameter& toCompare) const
   {
      if (GetDataType() == toCompare.GetDataType())
      {
         const NamedResourceParameter& rpToCompare = static_cast<const NamedResourceParameter&>(toCompare);

         return GetValue() == rpToCompare.GetValue();
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
}
