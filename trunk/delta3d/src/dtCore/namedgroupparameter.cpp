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
#include <dtCore/namedgroupparameter.h>
#include <dtCore/groupactorproperty.h>
#include <dtCore/exceptionenum.h>
#include <dtUtil/datastream.h>
#include <dtUtil/stringutils.h>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   NamedGroupParameter::NamedGroupParameter(const dtUtil::RefString& name)
   : NamedParameter(dtCore::DataType::GROUP, name, false)
   {}

   ///////////////////////////////////////////////////////////////////////////////
   NamedGroupParameter::NamedGroupParameter(dtCore::DataType& newDataType, const dtUtil::RefString& name)
   : NamedParameter(newDataType, name, false)
   {}

   ///////////////////////////////////////////////////////////////////////////////
   NamedGroupParameter::NamedGroupParameter(const NamedGroupParameter& toCopy)
   : NamedParameter(dtCore::DataType::GROUP, toCopy.GetName(), false)
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
         dtCore::DataType* type = NULL;

         for (unsigned int j = 0; j < dtCore::DataType::EnumerateType().size(); j++)
         {
            dtCore::DataType* d = dtCore::DataType::EnumerateType()[j];
            if (d->GetTypeId() == id)
            {
               type = d;
               break;
            }
         }
         if (type == NULL) //|| type == &dtCore::DataType::UNKNOWN)
         {
            throw dtCore::BaseException( "The datatype was not found in the stream", __FILE__, __LINE__);
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

   static const char OPEN_CHAR = '{';
   static const char CLOSE_CHAR = '}';

   ///////////////////////////////////////////////////////////////////////////////
   const std::string NamedGroupParameter::ToString() const
   {
       std::string toFill;

       NamedGroupParameter::ParameterList::const_iterator i = mParameterList.begin();
       NamedGroupParameter::ParameterList::const_iterator end = mParameterList.end();
       for (; i!= end; ++i)
       {
          NamedParameter& param = *i->second;
          toFill.append(1, OPEN_CHAR);
          toFill.append(param.GetName());
          toFill.append(1, CLOSE_CHAR);
          toFill.append(1, OPEN_CHAR);
          toFill.append(dtUtil::ToString(param.GetDataType().GetName()));
          toFill.append(1, CLOSE_CHAR);
          // output this boolean as "true" or "false" in the string
          toFill.append(1, OPEN_CHAR);
          bool isList = param.IsList();
          toFill.append(isList ? "true": "false");
          toFill.append(1, CLOSE_CHAR);

          toFill.append(1, OPEN_CHAR);
          toFill.append(param.ToString());
          toFill.append(1, CLOSE_CHAR);
       }
       return toFill;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedGroupParameter::FromString(const std::string& value)
   {
      bool result = true;

      std::string data = value;
      dtUtil::Trim(data);


      // First read the total size of the array.
      std::string name, datatype, isList, item;


      while (!data.empty())
      {
         result = dtUtil::TakeToken(data, name, OPEN_CHAR, CLOSE_CHAR) &&
                  dtUtil::TakeToken(data, datatype, OPEN_CHAR, CLOSE_CHAR) &&
                  dtUtil::TakeToken(data, isList, OPEN_CHAR, CLOSE_CHAR) &&
                  dtUtil::TakeToken(data, item, OPEN_CHAR, CLOSE_CHAR);
         dtCore::DataType* dt = dtCore::DataType::GetValueForName(datatype);
         if (result && dt != NULL)
         {
            dtCore::RefPtr<NamedParameter> newParameter =
                     AddParameter(name, *dt, dtUtil::ToType<bool>(isList));

            result = newParameter->FromString(item);
         }
         dtUtil::Trim(data);
      }

      return result;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::CopyFrom(const NamedParameter& otherParam)
   {
      if (otherParam.GetDataType() != GetDataType())
         throw dtCore::InvalidParameterException(
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
      dtCore::DataType& type, bool createAsList)
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
         throw dtCore::InvalidParameterException(
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
   void NamedGroupParameter::SetFromProperty(const dtCore::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtCore::GroupActorProperty* gap = static_cast<const dtCore::GroupActorProperty*>(&property);
      dtCore::RefPtr<NamedGroupParameter> createdParam = gap->GetValue();
      if (createdParam.valid())
      {
         CopyFrom(*createdParam);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::ApplyValueToProperty(dtCore::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtCore::GroupActorProperty* gap = static_cast<dtCore::GroupActorProperty*>(&property);
      gap->SetValue(*this);
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedGroupParameter::operator==(const ActorProperty& toCompare) const
   {
      if (toCompare.GetDataType() == GetDataType())
      {
         const dtCore::GroupActorProperty* gap = static_cast<const dtCore::GroupActorProperty*>(&toCompare);
         dtCore::RefPtr<NamedGroupParameter> createdParam = gap->GetValue();
         if (createdParam.valid())
         {
            return (*this) == (*createdParam);
         }
      }
      return false;
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
