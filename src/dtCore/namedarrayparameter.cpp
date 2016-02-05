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
#include <dtCore/datatype.h>
#include <dtCore/namedarrayparameter.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/arrayactorproperty.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/datastream.h>
#include <dtUtil/stringutils.h>
namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   NamedArrayParameter::NamedArrayParameter(const dtUtil::RefString& name)
      : NamedParameter(DataType::ARRAY, name, false)
      , mPackData(true)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedArrayParameter::NamedArrayParameter(DataType& dataType, const dtUtil::RefString& name)
      : NamedParameter(dataType, name, false)
      , mPackData(true)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedArrayParameter::~NamedArrayParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR(NamedArrayParameter, bool, PackData)

   ///////////////////////////////////////////////////////////////////////////////
   void NamedArrayParameter::CopyFrom(const NamedParameter& otherParam)
   {
      if (otherParam.GetDataType() != GetDataType())
         throw dtCore::InvalidParameterException(
         "The msg parameter must be of type ARRY.", __FILE__, __LINE__);

      const NamedArrayParameter& apm = static_cast<const NamedArrayParameter&>(otherParam);

      Resize(apm.GetSize());

      unsigned idx = 0;
      //copy parameters
      NamedArrayParameter::ParameterList::const_iterator i = apm.mParameterList.begin();
      NamedArrayParameter::ParameterList::const_iterator end = apm.mParameterList.end();
      for (; i != end; ++i, ++idx)
      {
         NamedParameter* cur = *i;

         // Don't touch the parameters that are NULL.
         if (cur != NULL)
         {
            dtCore::RefPtr<NamedParameter> newParameter = dtCore::NamedParameter::CreateFromType(cur->GetDataType(), cur->GetName(), cur->IsList());

            newParameter->CopyFrom(*cur);
            SetParameter(idx, *newParameter);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedArrayParameter::ToDataStream(dtUtil::DataStream& stream) const
   {
      // Write out the size of the list so we know how many times to loop in FromDataStream
      stream << (unsigned int)mParameterList.size();

      if (mParameterList.size() > 10240U)
      {
         LOGN_WARNING("namedarrayparameter.cpp", "The array parameter is huge: " + dtUtil::ToString(mParameterList.size()));
      }

      NamedArrayParameter::ParameterList::const_iterator i = mParameterList.begin();
      NamedArrayParameter::ParameterList::const_iterator end = mParameterList.end();
      for (; i != end; ++i)
      {
         const NamedParameter* param = *i;
         if (param != NULL)
         {
            stream << param->GetDataType().GetTypeId();
            if (!mPackData)
               stream << param->GetName();
            stream << param->IsList();
            param->ToDataStream(stream);
         }
         else
         {
            stream << (unsigned char)(dtCore::DataType::UNKNOWN_ID);
         }

      }
   }

   static const dtUtil::RefString DEFAULT_ELEMENT_NAME("elem");

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedArrayParameter::FromDataStream(dtUtil::DataStream& stream)
   {
      bool okay = true;

      // Read in the size of the stream
      unsigned int size;
      stream >> size;

      if (size > 10240U)
      {
         LOGN_WARNING("namedarrayparameter.cpp", "The array parameter stream has a huge size parameter: " + dtUtil::ToString(size));
      }
      Resize(size);

      // To reuse the buffer space.
      std::string name;
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
         if (type == NULL)
         {
            throw dtCore::BaseException( "The datatype was not found in the stream", __FILE__, __LINE__);
            okay = false;
         }

         // don't touch the index if it's the unknown type.
         if (*type != dtCore::DataType::UNKNOWN)
         {
            if (!mPackData)
               stream >> name;

            bool isList;
            stream >> isList;

            dtCore::RefPtr<NamedParameter> param = dtCore::NamedParameter::CreateFromType(*type, mPackData ? DEFAULT_ELEMENT_NAME : dtUtil::RefString(name), isList);

            okay = okay && param != NULL && param->FromDataStream(stream);
            if (okay) SetParameter(i, *param);
         }
      }

      return okay;
   }

   static const char OPEN_CHAR = '{';
   static const char CLOSE_CHAR = '}';

   ///////////////////////////////////////////////////////////////////////////////
   const std::string NamedArrayParameter::ToString() const
   {
      std::ostringstream ss;
      ss << OPEN_CHAR << GetSize() << CLOSE_CHAR;

      NamedArrayParameter::ParameterList::const_iterator i = mParameterList.begin();
      NamedArrayParameter::ParameterList::const_iterator end = mParameterList.end();
      for (; i!= end; ++i)
      {
         NamedParameter* param = i->get();
         if (param != NULL)
         {
            ss << OPEN_CHAR << param->GetDataType().GetName() << CLOSE_CHAR;
            if (!mPackData)
               ss << OPEN_CHAR << param->GetName() << CLOSE_CHAR;
            ss << OPEN_CHAR << param->IsList() << CLOSE_CHAR;
            ss << OPEN_CHAR << param->ToString() << CLOSE_CHAR;
         }
         else
         {
            ss << OPEN_CHAR << dtCore::DataType::UNKNOWN.GetName()<< CLOSE_CHAR;
         }
      }
      return ss.str();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedArrayParameter::FromString(const std::string& value)
   {
      bool result = true;

      std::string data = value;

      dtUtil::Trim(data);


      // First read the total size of the array.
      std::string name, datatype, isList, item;

      if (dtUtil::TakeToken(data, item, OPEN_CHAR, CLOSE_CHAR))
      {
         size_t elementCount = dtUtil::ToType<size_t>(item);
         if (elementCount > 10000U)
         {
            LOGN_ERROR("namedarrayparameter.cpp", "For safety sake, the size of this array parameter is being limited to 10000");
            elementCount = 10000U;
         }

         Resize(elementCount);
         for (unsigned i = 0; i < elementCount && !data.empty(); ++i)
         {
            result = dtUtil::TakeToken(data, datatype, OPEN_CHAR, CLOSE_CHAR);

            dtCore::DataType* dt = NULL;
            if (result)
            {
               dt = dtCore::DataType::GetValueForName(datatype);
               if (dt == NULL || *dt == dtCore::DataType::UNKNOWN)
               {
                  // Unknown appears by itself make elements as to leave unchanged.
                  continue;
               }
            }

            if (!mPackData)
               result = result && dtUtil::TakeToken(data, name, OPEN_CHAR, CLOSE_CHAR);

            result = result &&
                  dtUtil::TakeToken(data, isList, OPEN_CHAR, CLOSE_CHAR) &&
                  dtUtil::TakeToken(data, item, OPEN_CHAR, CLOSE_CHAR);

            if (result && dt != NULL)
            {
               dtCore::RefPtr<NamedParameter> newParameter = dtCore::NamedParameter::CreateFromType(*dt, mPackData ? DEFAULT_ELEMENT_NAME: dtUtil::RefString(name), dtUtil::ToType<bool>(isList));
               result = newParameter->FromString(item);
               if (result)
               {
                  SetParameter(i, *newParameter);
               }
            }
            dtUtil::Trim(data);
         }
      }

      return result;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedArrayParameter::SetFromProperty(const dtCore::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtCore::ArrayActorPropertyBase* ap = static_cast<const dtCore::ArrayActorPropertyBase*> (&property);
      const dtCore::ActorProperty* internalProp = ap->GetArrayProperty();
      unsigned arraySize = unsigned(ap->GetArraySize());
      Resize(arraySize);
      dtCore::DataType& type = internalProp->GetDataType();
      dtUtil::RefString paramName = internalProp->GetName();
      for (unsigned i = 0; i < arraySize; ++i)
      {
         ap->SetIndex(int(i));
         dtCore::RefPtr<NamedParameter> newParameter = dtCore::NamedParameter::CreateFromType(type, paramName, false);
         newParameter->SetFromProperty(*internalProp);
         SetParameter(i, *newParameter);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedArrayParameter::ApplyValueToProperty(dtCore::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtCore::ArrayActorPropertyBase* ap = static_cast<dtCore::ArrayActorPropertyBase*> (&property);
      dtCore::ActorProperty* internalProp = ap->GetArrayProperty();
      unsigned numParams = mParameterList.size();
      unsigned arraySize = unsigned(ap->Resize(numParams));

      // If the sizes don't match, then only copy over the minimum of the two.
      numParams = dtUtil::Min(arraySize, numParams);

      for (unsigned i = 0; i < numParams ; ++i)
      {
         ap->SetIndex(int(i));
         const NamedParameter* param = GetParameter(i);
         // Null items mean don't change the value.
         if (param != NULL)
            param->ApplyValueToProperty(*internalProp);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedArrayParameter::operator==(const ActorProperty& toCompare) const
   {
      bool result = true;
      if (toCompare.GetDataType() == GetDataType())
      {
         const dtCore::ArrayActorPropertyBase* ap = static_cast<const dtCore::ArrayActorPropertyBase*> (&toCompare);
         const dtCore::ActorProperty* internalProp = ap->GetArrayProperty();
         unsigned numParams = mParameterList.size();
         if (numParams != unsigned(ap->GetArraySize()))
         {
            result = false;
         }

         for (unsigned i = 0; result && i < numParams; ++i)
         {
            ap->SetIndex(int(i));
            const NamedParameter* param = GetParameter(i);
            if (param == NULL)
            {
               result = false;
               break;
            }
            result = result && (*param) == (*internalProp);
         }
      }
      return result;
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedParameter* NamedArrayParameter::GetParameter(unsigned index)
   {
      if (index < mParameterList.size())
      {
         return mParameterList[index];
      }
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const NamedParameter* NamedArrayParameter::GetParameter(unsigned index) const
   {
      if (index < mParameterList.size())
      {
         return mParameterList[index];
      }
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedArrayParameter::SetParameter(unsigned index, NamedParameter& param)
   {
      if (index >= mParameterList.size())
      {
         mParameterList.resize(index+1);
      }
      if (index < mParameterList.size())
      {
         mParameterList[index] = &param;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedParameter* NamedArrayParameter::AddParameter(const dtUtil::RefString& name,
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
   void NamedArrayParameter::AddEmptyIndex()
   {
      mParameterList.push_back(NULL);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedArrayParameter::SetEmptyIndex(unsigned index)
   {
      if (index < mParameterList.size())
      {
         mParameterList[index] = nullptr;
      }
   }


   ///////////////////////////////////////////////////////////////////////////////
   void NamedArrayParameter::AddParameter(NamedParameter& param)
   {
      mParameterList.push_back(&param);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedArrayParameter::InsertParameter(unsigned index, NamedParameter& param)
   {
      if (index > mParameterList.size())
      {
         mParameterList.resize(index);
      }
      if (index <= mParameterList.size())
      {
         mParameterList.insert(mParameterList.begin() + index, &param);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedArrayParameter::RemoveParameter(unsigned index)
   {
      if (index < mParameterList.size())
      {
         mParameterList.erase(mParameterList.begin() + index);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   size_t NamedArrayParameter::GetSize() const
   {
      return mParameterList.size();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedArrayParameter::operator==(const NamedParameter& toCompare) const
   {
      if (GetDataType() == toCompare.GetDataType())
      {
         const NamedArrayParameter& arrayToCompare = static_cast<const NamedArrayParameter&>(toCompare);
         //if the size doesn't match. it's not equal.
         if (mParameterList.size() != arrayToCompare.mParameterList.size())
         {
            return false;
         }
         NamedArrayParameter::ParameterList::const_iterator itor = mParameterList.begin();
         NamedArrayParameter::ParameterList::const_iterator itorComp = arrayToCompare.mParameterList.begin();
         NamedArrayParameter::ParameterList::const_iterator end = mParameterList.end();
         for (; itor != end; ++itor)
         {
            //spin through the params and return false if one is not equal.
            if (**itor != **itorComp)
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
}
