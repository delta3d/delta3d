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
 * David Guthrie
 */

#ifndef DELTA_ABSTRACT_PARAMETER
#define DELTA_ABSTRACT_PARAMETER

#include <dtUtil/refstring.h>
#include <dtCore/export.h>
#include <osg/Referenced>

namespace dtUtil
{
   class DataStream;
}

namespace dtCore
{
   class DataType;

   /**
    * @class AbstractParameter
    * @brief a common base class for actor property and named parameter, especially helpful in the load/save process.
    * More is expected to be added to this over time as features are unified between the two sets of classes.
    */
   class DT_CORE_EXPORT AbstractParameter : public osg::Referenced
   {
      public:

         AbstractParameter(DataType& dataType, const dtUtil::RefString& name):
            mDataType(dataType), mName(name)
         {}

         ///@return the type of this parameter.
         DataType& GetDataType() const { return mDataType; };

         /**
          * Gets the unique name assigned to this parameter.
          */
         const dtUtil::RefString& GetName() const { return mName; }

         virtual const std::string ToString() const = 0;

         /**
          * @return true if it was able to assign the value based on the string or false if not.
          * @param value the string value to assign the parameter to.
          */
         virtual bool FromString(const std::string& value) = 0;

         /// write the data to a data stream
         virtual void ToDataStream(dtUtil::DataStream& stream) const = 0;

         /// read the data from a data stream.
         virtual bool FromDataStream(dtUtil::DataStream& stream) = 0;

         void* operator new(size_t num_bytes);


         void operator delete(void*);

      protected:

         virtual ~AbstractParameter() { }
         DataType& mDataType;
         dtUtil::RefString mName;
   };
}
#endif
