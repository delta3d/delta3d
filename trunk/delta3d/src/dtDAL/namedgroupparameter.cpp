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
#include <dtDAL/namedgroupparameter.h>
#include <dtDAL/groupactorproperty.h>
#include <dtDAL/exceptionenum.h>
#include <dtUtil/datastream.h>
#include <dtUtil/stringutils.h>

namespace dtDAL
{
   ///////////////////////////////////////////////////////////////////////////////
   NamedGroupParameter::NamedGroupParameter(const dtUtil::RefString& name)
   : NamedParameter(dtDAL::DataType::GROUP, name, false)
   {}

   ///////////////////////////////////////////////////////////////////////////////
   NamedGroupParameter::NamedGroupParameter(dtDAL::DataType& newDataType, const dtUtil::RefString& name)
   : NamedParameter(newDataType, name, false)
   {}

   ///////////////////////////////////////////////////////////////////////////////
   NamedGroupParameter::NamedGroupParameter(const NamedGroupParameter& toCopy)
   : NamedParameter(dtDAL::DataType::GROUP, toCopy.GetName(), false)
   {
      CopyFrom(toCopy);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::ToDataStream(dtUtil::DataStream& stream) const
   {
      // Write out the size of the list so we know how many times to loop in FromDataStream
      stream << (unsigned int)mParameterList.size();

      NamedGroupParameter::ParameterList::const_iterator i = mParameterList.begin();
      NamedGroupParameter::ParameterList::const_iterator end = mParameterList.end();
      for (; i != end; ++i)
      {
         stream << i->second->GetDataType().GetTypeId();
         stream << i->second->GetName();
         stream << i->second->IsList();
         i->second->ToDataStream(stream);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedGroupParameter::FromDataStream(dtUtil::DataStream& stream)
   {
      bool okay = true;

      // Read in the size of the stream
      unsigned int size;
      stream >> size;

      for (unsigned short int i = 0; i < size; i++)
      {
         unsigned char id;
         stream >> id;
         dtDAL::DataType* type = NULL;

         for (unsigned int j = 0; j < dtDAL::DataType::EnumerateType().size(); j++)
         {
            dtDAL::DataType* d = dtDAL::DataType::EnumerateType()[j];
            if (d->GetTypeId() == id)
            {
               type = d;
               break;
            }
         }
         if (type == NULL) //|| type == &dtDAL::DataType::UNKNOWN)
         {
            throw dtDAL::BaseException( "The datatype was not found in the stream", __FILE__, __LINE__);
            okay = false;
         }

         std::string name;
         stream >> name;

         bool isList;
         stream >> isList;

         dtCore::RefPtr<NamedParameter> param = GetParameter(name);
         if (param == NULL)
         {
            param = AddParameter(name, *type, isList);
         }

         okay = okay && param != NULL && param->FromDataStream(stream);
      }

      return okay;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string NamedGroupParameter::ToString() const
   {
      std::string toFill;
      NamedGroupParameter::ParameterList::const_iterator i = mParameterList.begin();
      NamedGroupParameter::ParameterList::const_iterator end = mParameterList.end();
      for (; i!= end; ++i)
      {
         NamedParameter& param = *i->second;
         toFill.append(param.GetName());
         toFill.append(" ");
         toFill.append(dtUtil::ToString(param.GetDataType().GetName()));
         toFill.append(" ");
         // output this boolean as "true" or "false" in the string
         bool isList = param.IsList();
         toFill.append(isList ? "true": "false");

         toFill.append(" ");
         toFill.append(param.ToString());
         toFill.append(1, '\n');
      }
      return toFill;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedGroupParameter::FromString(const std::string& value)
   {
      std::istringstream iss(value);

      std::string paramName;
      std::string typeString;
      std::string isListString;
      std::string paramValue;

      // get values
      std::getline(iss, paramName, ' ');
      std::getline(iss, typeString, ' ');
      std::getline(iss, isListString, ' ');
      std::getline(iss, paramValue);

      dtDAL::DataType *type = dtDAL::DataType::GetValueForName(typeString);

      if (type == NULL)
         return false;

      // isList
      bool isList = isListString == "true";

      // try and retrieve the parameter
      dtCore::RefPtr<NamedParameter> param = GetParameter(paramName);

      if (param == NULL)
      { // add it if it does not exist
         param = AddParameter(paramName, *type, isList);
      }

      param->FromString(paramValue);
      return true;

   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::CopyFrom(const NamedParameter& otherParam)
   {
      if (otherParam.GetDataType() != GetDataType())
         throw dtDAL::InvalidParameterException(
         "The msg parameter must be of type GROUP.", __FILE__, __LINE__);

      const NamedGroupParameter& gpm = static_cast<const NamedGroupParameter&>(otherParam);

      //wipe out any existing parameters.  It's easier to just recreate them.
      mParameterList.clear();

      //copy parameters
      NamedGroupParameter::ParameterList::const_iterator i = gpm.mParameterList.begin();
      NamedGroupParameter::ParameterList::const_iterator end = gpm.mParameterList.end();
      for (; i != end; ++i)
      {
         NamedParameter& cur = *i->second;

         dtCore::RefPtr<NamedParameter> newParameter =
                  AddParameter(cur.GetName(), cur.GetDataType(), cur.IsList());

         newParameter->CopyFrom(cur);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedParameter* NamedGroupParameter::AddParameter(const dtUtil::RefString& name,
      dtDAL::DataType& type, bool createAsList)
   {
      dtCore::RefPtr<NamedParameter> param = CreateFromType(type, name);
      if (param.valid())
      {
         AddParameter(*param);
         return param.get();
      }
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<NamedParameter> NamedGroupParameter::RemoveParameter(const dtUtil::RefString& name)
   {
      NamedGroupParameter::ParameterList::iterator itor = mParameterList.find(name);
      if (itor != mParameterList.end())
      {
         dtCore::RefPtr<NamedParameter> param = itor->second;
         mParameterList.erase(itor);
         return param;
      }
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::RemoveAllParameters()
   {
      mParameterList.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::AddParameter(NamedParameter& newParam)
   {
      if (!mParameterList.insert(std::make_pair(newParam.GetName(), &newParam)).second)
      {
         throw dtDAL::InvalidParameterException(
         "Could not add new parameter: "+ newParam.GetName() +
         ". A parameter with that name already exists.", __FILE__, __LINE__);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedParameter* NamedGroupParameter::GetParameter(const dtUtil::RefString& name)
   {
      NamedGroupParameter::ParameterList::iterator itor = mParameterList.find(name);
      return itor == mParameterList.end() ? NULL : itor->second.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   const NamedParameter* NamedGroupParameter::GetParameter(const dtUtil::RefString& name) const
   {
      NamedGroupParameter::ParameterList::const_iterator itor = mParameterList.find(name);
      return itor == mParameterList.end() ? NULL : itor->second.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::GetParameters(std::vector<NamedParameter*>& toFill)
   {
      toFill.clear();
      toFill.reserve(mParameterList.size());
      NamedGroupParameter::ParameterList::iterator i = mParameterList.begin();
      NamedGroupParameter::ParameterList::iterator end = mParameterList.end();
      for (;i != end; ++i)
      {
         toFill.push_back(i->second.get());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::GetParameters(std::vector<const NamedParameter*>& toFill) const
   {
      toFill.clear();
      toFill.reserve(mParameterList.size());
      NamedGroupParameter::ParameterList::const_iterator i = mParameterList.begin();
      NamedGroupParameter::ParameterList::const_iterator end = mParameterList.end();
      for (;i != end; ++i)
      {
         toFill.push_back(i->second.get());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned int NamedGroupParameter::GetParameterCount() const
   {
      return mParameterList.size();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::GroupActorProperty* gap = static_cast<const dtDAL::GroupActorProperty*>(&property);
      if (gap->GetValue().valid())
      {
         CopyFrom(*gap->GetValue());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::GroupActorProperty* gap = static_cast<dtDAL::GroupActorProperty*>(&property);
      gap->SetValue(*this);
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedGroupParameter::operator==(const NamedParameter& toCompare) const
   {
      if (GetDataType() == toCompare.GetDataType())
      {
         const NamedGroupParameter& groupToCompare = static_cast<const NamedGroupParameter&>(toCompare);
         //if the size doesn't match. it's not equal.
         if (mParameterList.size() != groupToCompare.mParameterList.size())
         {
            return false;
         }
         NamedGroupParameter::ParameterList::const_iterator itor = mParameterList.begin();
         NamedGroupParameter::ParameterList::const_iterator itorComp = groupToCompare.mParameterList.begin();
         NamedGroupParameter::ParameterList::const_iterator end = mParameterList.end();
         for (; itor != end; ++itor)
         {
            //spin through the params and return false if one is not equal.
            if (*itor->second != *itorComp->second)
            {
               return false;
            }

            ++itorComp;
         }
         //all params are equal.
         return true;
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
}
