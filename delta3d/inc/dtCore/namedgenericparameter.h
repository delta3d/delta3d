/* -*-c++-*-
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
 * Matthew W. Campbell
 * David Guthrie
 */

#ifndef DELTA_NAMED_GENERIC_PARAMETER
#define DELTA_NAMED_GENERIC_PARAMETER

#include <dtCore/namedparameter.h>

#include <dtCore/datatype.h>
#include <dtCore/exceptionenum.h>

#include <dtUtil/datastream.h>

#include <sstream>
#include <vector>

namespace dtCore
{
   template <class ParamType>
   class NamedGenericParameter : public NamedParameter
   {
      public:
         typedef ParamType value_type;

         NamedGenericParameter(DataType& dataType, const dtUtil::RefString& name, const ParamType& defaultValue, bool isList = false)
            : NamedParameter(dataType, name, isList)
            , mValue(defaultValue)
            , mNumberPrecision(17)
         {
            if (IsList())
            {
               mValueList = new std::vector<ParamType>();
               mValueList->push_back(defaultValue);
            }
            else
            {
               mValueList = NULL;
            }
         }

         /**
          * Sets the precision of floating point number as they are
          * converted to and from string values.
          * @param precision The amount of numeric percision.
          * @note This value defaults to 16.
          */
         inline void SetNumberPrecision(unsigned int precision)
         {
            mNumberPrecision = precision;
         }

         /**
          * Gets the current floating point precision value on this
          * parameter.
          * @return The current number precision.
          */
         inline unsigned int GetNumberPrecision() const
         {
            return mNumberPrecision;
         }

         inline virtual void CopyFrom(const NamedParameter& otherParam)
         {
            const NamedGenericParameter<ParamType>* param =
                static_cast<const NamedGenericParameter<ParamType>*>(&otherParam);

            //First make sure this parameter does not have a list if the
            //other parameter does and vice versa.
            if ((IsList() && !otherParam.IsList()) ||(!IsList() && otherParam.IsList()))
            {
               throw dtCore::BaseException(
                  "Cannot assign two parameters with one being a list of values and the other not.", __FILE__, __LINE__);
            }

            if (param != NULL)
            {
               if (!IsList())
               {
                 SetValue(param->GetValue());
               }
               else
               {
                  SetValueList(param->GetValueList());
               }
            }
            else
            {
               if (!FromString(otherParam.ToString()))
               {
                  LOGN_ERROR("MessageParameter", "Parameter types are incompatible. Cannot copy the value.");
               }
            }
         }

         inline virtual void SetValue(const ParamType& value)
         {
            if (IsList())
            {
               throw dtCore::BaseException(
                  "Cannot call SetValue() on message parameter with a list of values.", __FILE__, __LINE__);
            }

            mValue = value;
         }

         inline virtual const ParamType& GetValue() const
         {
            if (IsList())
            {
               throw dtCore::BaseException(
                  "Cannot call GetValue() on message parameter with a list of values.", __FILE__, __LINE__);
            }

            return mValue;
         }

         inline const std::vector<ParamType>& GetValueList() const
         {
            if (!IsList())
            {
               throw dtCore::BaseException(
                  "Cannot retrieve the parameters value list.  Parameter does not contain a list.", __FILE__, __LINE__);
            }
            return *mValueList;
         }

         inline std::vector<ParamType>& GetValueList()
         {
            if (!IsList())
            {
               throw dtCore::BaseException(
                  "Cannot retrieve the parameters value list.  Parameter does not contain a list.", __FILE__, __LINE__);
            }
            return *mValueList;
         }

         inline void SetValueList(const std::vector<ParamType>& newValues)
         {
            if (!IsList())
            {
               throw dtCore::BaseException(
                  "Cannot set a list of new values on a parameter that is not a list.", __FILE__, __LINE__);
            }

            *mValueList = newValues;
         }

         inline virtual void ToDataStream(dtUtil::DataStream& stream) const
         {
            if (IsList())
            {
               stream << unsigned(mValueList->size());
               for (unsigned int i=0; i<mValueList->size(); i++)
               {
                  stream << (*mValueList)[i];
               }
            }
            else
            {
               stream << mValue;
            }
         }

         inline virtual bool FromDataStream(dtUtil::DataStream& stream)
         {
            bool okay = true;

            if (IsList())
            {
               mValueList->clear();

               unsigned int listSize;
               stream >> listSize;
               for (unsigned int i = 0; i < listSize; ++i)
               {
                  ParamType newElement;
                  stream >> newElement;
                  mValueList->push_back(newElement);
               }
            }
            else
            {
               stream >> mValue;
            }

            return okay;
         }

         virtual const std::string ToString() const { return std::string(); }
         virtual bool FromString(const std::string& /*value*/) { return false; }

         virtual bool operator==(const NamedParameter& toCompare) const
         {
            if (GetDataType() == toCompare.GetDataType())
            {
               return GetValue() == static_cast<const NamedGenericParameter<ParamType>&>(toCompare).GetValue();
            }
            return false;
         }

      protected:
         ///Keep destructors protected to ensure property smart pointer management.
         virtual ~NamedGenericParameter() { delete mValueList; }

      private:
         std::vector<ParamType>* mValueList;
         ParamType mValue;
         unsigned int mNumberPrecision;
   };
}

#endif //DELTA_NAMED_GENERIC_PARAMETER
