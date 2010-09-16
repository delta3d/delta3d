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

#ifndef DELTA_NAMED_RESOURCE_PARAMETER
#define DELTA_NAMED_RESOURCE_PARAMETER

#include <dtDAL/namedparameter.h>

#include <dtDAL/resourcedescriptor.h>

#include <dtUtil/deprecationmgr.h>

#include <vector>

namespace dtDAL
{
   /**
    * @class ResourceMessageParameter
    */
   class DT_DAL_EXPORT NamedResourceParameter: public NamedParameter
   {
      public:
         NamedResourceParameter(dtDAL::DataType& type, const dtUtil::RefString& name,
            bool isList = false);

         virtual void ToDataStream(dtUtil::DataStream& stream) const;

         virtual bool FromDataStream(dtUtil::DataStream& stream);

         virtual const std::string ToString() const;

         virtual bool FromString(const std::string& value);

         virtual void CopyFrom(const NamedParameter& otherParam);

         /**
          * Sets the resource descriptor.  This value copied if not null.
          * You should create one on the stack, and then pass it with &.
          * The method takes NULL so that the parameter can have an empty value.
          */
         void SetValue(const dtDAL::ResourceDescriptor& descriptor);

         /**
          * This returns a pointer to a stack created data member, so you should copy it
          * via copy constructor and hold onto it that way.  If you hold onto the
          * point, you could get deleted out from under you.
          * @return the resource descriptor or NULL.
          */
         const dtDAL::ResourceDescriptor GetValue() const;

         const std::vector<dtDAL::ResourceDescriptor>& GetValueList() const;

         std::vector<dtDAL::ResourceDescriptor>& GetValueList();

         void SetValueList(const std::vector<dtDAL::ResourceDescriptor>& newValues);

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

         virtual bool operator==(const NamedParameter& toCompare) const;

         ///deprecated 12/11/09
         DEPRECATE_FUNC void SetValue(const dtDAL::ResourceDescriptor* descriptor); 
      protected:
         virtual ~NamedResourceParameter();

      private:
         const dtDAL::DataType* mDataType;
         std::vector<dtDAL::ResourceDescriptor>* mValueList;
         dtDAL::ResourceDescriptor mDescriptor;
   };
}

#endif //DELTA_NAMED_RESOURCE_PARAMETER
