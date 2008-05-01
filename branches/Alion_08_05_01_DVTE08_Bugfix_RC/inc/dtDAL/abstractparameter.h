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

#include <string>
#include <osg/Referenced>

namespace dtDAL
{
   class DataType;
   
   /**
    * @class AbstractParameter
    * @brief a common base class for actor property and named parameter, especially helpful in the load/save process.
    * More is expected to be added to this over time as features are unified between the two sets of classes.
    */
   class AbstractParameter : public osg::Referenced
   {
      public:

         ///@return the name of this message parameter
         virtual const std::string &GetName() const = 0;

         ///@return 
         virtual const dtDAL::DataType &GetDataType() const = 0;

         virtual const std::string ToString() const = 0;

         /**
          * @return true if it was able to assign the value based on the string or false if not.
          * @param value the string value to assign the parameter to.
          */
         virtual bool FromString(const std::string &value) = 0;

      protected:

         virtual ~AbstractParameter() { }
   };
}
#endif
