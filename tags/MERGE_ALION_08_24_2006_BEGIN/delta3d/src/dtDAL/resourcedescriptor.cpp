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

#include <dtDAL/resourcedescriptor.h>

namespace dtDAL 
{
   const std::string ResourceDescriptor::GetExtension() const 
   {
      std::string ext;
      std::size_t index = mResourceIdentifier.find_last_of('.');
      //we want everything AFTER the ., so we need to see if the
      //the result is between the 0 and next to the last element.
      if (index >= 0 && index < mResourceIdentifier.size()-1)
         ext = mResourceIdentifier.substr(index+1);
      return ext;
   }

   const std::string ResourceDescriptor::GetResourceName() const 
   {
      std::string name;
      std::size_t index = mDisplayName.find_last_of(DESCRIPTOR_SEPARATOR);
      //we want everything AFTER the separator, so we need to see if the
      //the result is between the 0 and next to the last element.
      if (index >= 0 && index < mDisplayName.size()-1)
         name = mDisplayName.substr(index+1);
      return name;
   }

}
