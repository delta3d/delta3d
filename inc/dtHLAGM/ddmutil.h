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
#ifndef DELTA_DDMUTIL
#define DELTA_DDMUTIL

#include <dtHLAGM/export.h>
#include <vector>
#include <string>

namespace dtHLAGM
{

   class DT_HLAGM_EXPORT DDMUtil
   {
      public:
         
         static const std::string DDM_SPACE_LABEL;
         static const std::string DDM_SUBSPACE_EXTENT_LABEL;
         static const std::string DDM_FIRST_EXTENT_LABEL;
         static const std::string DDM_SECOND_EXTENT_LABEL;
         
         //Since the hla libraries are loaded and not direct dependencies now, this has to be set
         // on connection to the federation, so these allow that
         static unsigned int GetMinExtent();
         static void SetMinExtent(unsigned int);
         static unsigned int GetMaxExtent();
         static void SetMaxExtent(unsigned int);

         /**
          * Maps an enumerated value into the MIN_EXTENT MAX_EXTENT range given the known min and max.
          * @param value the value to map
          * @param min the minimum enumerated value.
          * @param max the maximum enumerated value.
          */
         static unsigned int MapEnumerated(unsigned value, unsigned min, unsigned max);
         
         /**
          * Maps to a value in the MIN_EXTENT/MAX_EXTENT range where value would match a set of precomputed
          * valid values. The partitionValues vector should contain a list of evenly incrementing values. 
          * @param value the value to map
          * @param partitionValues a vector of evenly incrementing values that define the list of partitions.
          */
         static unsigned int MapPartitioned(double value, const std::vector<double> partitionValues);
         
         /**
          * Maps a double value linearly in the MIN_EXTENT/MAX_EXTENT range using the passed in min and max
          * value.  The value parameter will be clamped between min and max.
          * @param value the value to map.  It will be clamped between min and max.
          * @param min the minimum value, it should be less than max.
          * @param max the maximum value, it should be greater than min.
          */
         static unsigned int MapLinear(double value, double min, double max);

      private:
         static unsigned int mMinExtent;
         static unsigned int mMaxExtent;

   };

}

#endif /*DELTA_DDMUTIL*/
