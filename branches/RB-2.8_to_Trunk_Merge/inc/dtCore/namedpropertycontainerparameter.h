/* -*-c++-*-
 * Delta3D
 * Copyright 2010, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 * 
 * David Guthrie
 */

#ifndef NAMEDPROPERTYCONTAINERPARAMETER_H_
#define NAMEDPROPERTYCONTAINERPARAMETER_H_

#include <dtCore/namedgroupparameter.h>

namespace dtCore
{

   /**
    * Defines the named parameter to match with a property container actor property.
    */
   class DT_CORE_EXPORT NamedPropertyContainerParameter : public dtCore::NamedGroupParameter
   {
   public:
      NamedPropertyContainerParameter(const dtUtil::RefString& name);
      NamedPropertyContainerParameter(const NamedPropertyContainerParameter& toCopy);

      template <typename ParameterContainer >
      NamedPropertyContainerParameter(const dtUtil::RefString& name, const ParameterContainer& parameters)
      : NamedGroupParameter(dtCore::DataType::GROUP, name)
      {
         typename ParameterContainer::const_iterator i, iend;
         i = parameters.begin();
         iend = parameters.end();
         for (; i != iend; ++i)
         {
            NamedParameter& cur = **i;

            dtCore::RefPtr<NamedParameter> newParameter =
                     AddParameter(cur.GetName(), cur.GetDataType(), cur.IsList());

            newParameter->CopyFrom(cur);
         }
      }

      virtual void SetFromProperty(const dtCore::ActorProperty& property);

      virtual void ApplyValueToProperty(dtCore::ActorProperty& property) const;

      virtual bool operator==(const ActorProperty& toCompare) const;

   protected:
      NamedPropertyContainerParameter(dtCore::DataType& newDataType, const dtUtil::RefString& name);

      virtual ~NamedPropertyContainerParameter();
   };

}

#endif /* NAMEDPROPERTYCONTAINERPARAMETER_H_ */
