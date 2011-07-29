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

#ifndef DELTA_NAMED_POD_PARAMETER
#define DELTA_NAMED_POD_PARAMETER

#include <dtCore/namedgenericparameter.h>

namespace dtCore
{
   /**
    * @class NamedPODParameter
    * This is a template for the single piece of data types to make reading data to and from a string easier.
    */
   template <class ParamType>
   class NamedPODParameter: public NamedGenericParameter<ParamType>
   {
      public:
         NamedPODParameter(DataType& dataType, const dtUtil::RefString& name, const ParamType& defaultValue, bool isList = false)
            : NamedGenericParameter<ParamType>(dataType, name, defaultValue, isList)
         {
         }

         virtual const std::string ToString() const
         {
            std::ostringstream stream;
            stream.precision(NamedGenericParameter<ParamType>::GetNumberPrecision());
            if (NamedGenericParameter<ParamType>::IsList())
            {
               const std::vector<ParamType>& values =
                  NamedGenericParameter<ParamType>::GetValueList();

               if (!values.empty())
               {
                  stream << values[0];
               }

               for (unsigned int i = 1; i < values.size(); ++i)
               {
                  stream << NamedGenericParameter<ParamType>::GetParamDelimeter()
                     << values[i];
               }
            }
            else
            {
              stream << NamedGenericParameter<ParamType>::GetValue();
            }

            return stream.str();
         }

         virtual bool FromString(const std::string& value)
         {
            std::istringstream stream;
            stream.precision(NamedGenericParameter<ParamType>::GetNumberPrecision());

            if (NamedGenericParameter<ParamType>::IsList())
            {
               std::vector<ParamType>& result =
                  NamedGenericParameter<ParamType>::GetValueList();

               ParamType actual;
               result.clear();
               stream.str(value);
               stream >> actual;
               result.push_back(actual);
               while (stream)
               {
                  char sep;
                  stream >> sep;
                  if (!stream)
                  {
                     break;
                  }

                  stream >> actual;
                  result.push_back(actual);
               }
            }
            else
            {
               ParamType newValue;
               stream.str(value);
               stream >> newValue;
               NamedGenericParameter<ParamType>::SetValue(newValue);
            }

            return true;
         }

      protected:
         virtual ~NamedPODParameter() {}
   };
}

#endif //DELTA_NAMED_POD_PARAMETER
