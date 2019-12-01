/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology, Inc.
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

#include <dtHLAGM/ddmregiondata.h>

#include <cstring>

namespace dtHLAGM
{
   void DDMRegionData::SetDimensionValue(unsigned extent, const DimensionValues& dimVal)
   {
      // filling in the ones that don't exist yet.
      if (int(extent) > int(mDimensions.size()) - 1)
      {
         mDimensions.insert(mDimensions.end(), unsigned(int(extent) - (int(mDimensions.size()) - 1)), DimensionValues());
      }
      
      mDimensions[extent] = dimVal;
   }

   const DDMRegionData::DimensionValues* DDMRegionData::GetDimensionValue(unsigned extent) const
   {
      if (mDimensions.size() <= extent)
         return NULL;
      
      return &mDimensions[extent];
   }

   bool DDMRegionData::DimensionValues::operator == (const DDMRegionData::DimensionValues& toComp) const
   {
      if (this == &toComp)
         return true;

      // TODO, figure out why you can't do == or != on 2 strings
      if(strcmp(mName.c_str(), toComp.mName.c_str()) != 0)
         return false;
      
      return mMin == toComp.mMin && mMax == toComp.mMax; 
   }
}
