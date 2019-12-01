/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006 Alion Science and Technology, BMH Operation
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

#include <dtHLAGM/ddmutil.h>
#include <dtUtil/mathdefines.h>

namespace dtHLAGM
{
   unsigned int DDMUtil::mMinExtent = 0;
   unsigned int DDMUtil::mMaxExtent = UINT_MAX;

   unsigned int DDMUtil::GetMinExtent() { return mMinExtent; }
   void DDMUtil::SetMinExtent(unsigned int val) { mMinExtent = val; }
   unsigned int DDMUtil::GetMaxExtent() { return mMaxExtent; }
   void DDMUtil::SetMaxExtent(unsigned int val) { mMaxExtent = val; }


   unsigned int DDMUtil::MapEnumerated(unsigned value, unsigned min, unsigned max)
   {
      dtUtil::Clamp(value, min, max);
      
      // scale indicates how "big" each bin is when we divy up the RTI
      // extent range..  The addition of one to the denominator adjusts
      // for the fact that we are mapping into a closed range (min <= ev
      // <= max) and therefore the number of bins in the range is one
      // more than the difference..
      double scale = double(mMaxExtent - mMinExtent) /
                     double(max - min + 1);

      // now, the value we are looking for is the value exactly in the
      // center of the appropriate bin.  the last part of the following
      // equation (scale/2) moves the value into the center of that bin.
      double result = double(mMinExtent) + scale * double(value-min) + scale/2.0;
      return (unsigned int)(result);
   }

   unsigned int DDMUtil::MapPartitioned(double value, const std::vector<double> partitionValues)
   {
      for (unsigned i = 0; i < partitionValues.size() - 1; ++i)
      {
        if ((value >= partitionValues[i]) && (value < partitionValues[i+1]))
           return(MapEnumerated(i, 0, (unsigned int)(partitionValues.size() - 2)));
      }
      return 0;
   }
   
   unsigned int DDMUtil::MapLinear(double value, double min, double max)
   {
      // Crop to valid range..    for linear mappings, we clip all
      // values outside the range to either the min or max..
      dtUtil::Clamp(value, min, max);
      // scale in this case represents a scaler used to put the value
      // in the right place in the linear mapping..
      double scale = double(mMaxExtent - mMinExtent) /
                     double(max - min);

      double result = double(mMinExtent) + scale*(value-min);
      
      return (unsigned int)(result);
   }
}
