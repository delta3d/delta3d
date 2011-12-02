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

#ifndef DELTA_NAMED_ARRAY_PARAMETER
#define DELTA_NAMED_ARRAY_PARAMETER

#include <dtCore/namedparameter.h>
#include <vector>
#include <algorithm>

namespace dtCore
{
   /**
   * @class ArrayMessageParameter
   */
   class DT_CORE_EXPORT NamedArrayParameter: public NamedParameter
   {
      public:
         typedef std::vector<dtCore::RefPtr<NamedParameter> > ParameterList;
         NamedArrayParameter(const dtUtil::RefString& name);

         virtual void CopyFrom(const NamedParameter& otherParam);

         virtual void ToDataStream(dtUtil::DataStream& stream) const;
         virtual bool FromDataStream(dtUtil::DataStream& stream);

         virtual const std::string ToString() const;
         virtual bool FromString(const std::string& value);

         virtual void SetFromProperty(const dtCore::ActorProperty& property);
         virtual void ApplyValueToProperty(dtCore::ActorProperty& property) const;

         virtual bool operator==(const ActorProperty& toCompare) const;

         /**
          * Adds a parameter to the group
          * @param name The name of the parameter to add
          * @param type The type of parameter it is, corresponding with dtCore::DataType
          * @param createAsList true if the parameter should be a list parameter.
          * @return A pointer to the parameter
          * @see dtCore::DataType
          * @throws dtUtil::Exception if the name specified is already used.
          */
         NamedParameter* AddParameter(const dtUtil::RefString& name, dtCore::DataType& type, bool createAsList = false);

         NamedParameter* GetParameter(unsigned index);
         const NamedParameter* GetParameter(unsigned index) const;
         void AddParameter(NamedParameter& param);
         void InsertParameter(unsigned index, NamedParameter& param);
         void RemoveParameter(unsigned index);
         size_t GetSize() const;

         virtual bool operator==(const NamedParameter& toCompare) const;

         template <class UnaryFunction>
         void ForEachParameter(UnaryFunction function) const
         {
            std::for_each(mParameterList.begin(), mParameterList.end(), function);
         }

      protected:
         NamedArrayParameter(DataType& dataType, const dtUtil::RefString& name);
         virtual ~NamedArrayParameter();

         ParameterList mParameterList;
   };
}

#endif //DELTA_NAMED_ARRAY_PARAMETER
