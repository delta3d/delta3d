/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2007, Alion Science and Technology, BMH Operation.
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
#ifndef DELTA_DDMAPPSPACECALCULATOR
#define DELTA_DDMAPPSPACECALCULATOR

#include <dtHLAGM/ddmregioncalculator.h>

namespace dtHLAGM
{

   class DT_HLAGM_EXPORT DDMAppSpaceCalculator : public dtHLAGM::DDMRegionCalculator
   {
      public:
         static const std::string PROP_APP_SPACE_MAX;
         static const std::string PROP_APP_SPACE_MIN;

         static const std::string PROP_DEFAULT_APP_SPACE;

         /// @return the minimum encoded app space value.  It defaults to 0
         long GetAppSpaceMinimum() const { return mAppSpaceMin; }
         void SetAppSpaceMinimum(long newMin) { mAppSpaceMin = newMin; }
         
         /// @return the maximum encoded app space value.  It defaults to 30
         long GetAppSpaceMaximum() const { return mAppSpaceMax; }
         void SetAppSpaceMaximum(long newMax) { mAppSpaceMax = newMax; }

         /// @return the default appspace to encode.  It defaults to 0.
         long GetDefaultAppSpace() const { return mDefaultAppSpace; }
         void SetDefaultAppSpace(long newSpace) { mDefaultAppSpace = newSpace; }
         
         unsigned int MapAppSpaceValue(unsigned spaceNumber) const;
         
         DDMAppSpaceCalculator();
         virtual ~DDMAppSpaceCalculator();
         
      private:
         long mAppSpaceMin;
         long mAppSpaceMax;

         long mDefaultAppSpace;
                  
   };

}

#endif /*DELTA_DDMAPPSPACECALCULATOR*/
