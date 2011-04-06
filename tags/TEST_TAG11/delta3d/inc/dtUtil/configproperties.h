/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008, Alion Science and Technology
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
#ifndef DELTA_CONFIGPROPERTIES
#define DELTA_CONFIGPROPERTIES

#include <string>

namespace dtUtil
{
   /// Interface for having configuration properties. 
   class ConfigProperties
   {
      public:
         /// @return a string value that is paired with the given name.  The default is returned if the property is not set.
         virtual const std::string& GetConfigPropertyValue(const std::string& name, const std::string& defaultValue = "") const = 0;

         /// Sets the value of a given config property.
         virtual void SetConfigPropertyValue(const std::string& name, const std::string& value) = 0;

         /// Removes a property with the given name
         virtual void RemoveConfigPropertyValue(const std::string& name) = 0;

   };
}

#endif /*DELTA_CONFIGPROPERTIES*/
