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

#ifndef DELTA_DDMMULTIENUMERATEDCALCULATOR
#define DELTA_DDMMULTIENUMERATEDCALCULATOR

#include <dtHLAGM/export.h>
#include <dtHLAGM/ddmappspacecalculator.h>

namespace dtHLAGM
{

   class DT_HLAGM_EXPORT DDMMultiEnumeratedCalculator : public dtHLAGM::DDMAppSpaceCalculator
   {
      public:
         typedef dtHLAGM::DDMAppSpaceCalculator BaseClass;
         
         DDMMultiEnumeratedCalculator();
         virtual ~DDMMultiEnumeratedCalculator();

         static const std::string PROP_SECOND_DIMENSION_MIN;
         static const std::string PROP_SECOND_DIMENSION_MAX;
         static const std::string PROP_SECOND_DIMENSION_VALUE;
         static const std::string PROP_SECOND_DIMENSION_ENABLED;

         static const std::string PROP_THIRD_DIMENSION_MIN;
         static const std::string PROP_THIRD_DIMENSION_MAX;
         static const std::string PROP_THIRD_DIMENSION_VALUE;
         static const std::string PROP_THIRD_DIMENSION_ENABLED;
         
         virtual dtCore::RefPtr<DDMRegionData> CreateRegionData() const;
         virtual void CreateSubscriptionRegionData(std::vector<dtCore::RefPtr<DDMRegionData> >& toFill) const;
         virtual bool UpdateRegionData(DDMRegionData& ddmData) const;

         /// @return the minimum second enumerated value.  It defaults to 0
         long GetSecondDimensionMinimum() const { return mSecondDimensionMin; }
         void SetSecondDimensionMinimum(long newMin) { mSecondDimensionMin = newMin; }
         
         /// @return the maximum second enumerated value.  It defaults to 1
         long GetSecondDimensionMaximum() const { return mSecondDimensionMax; }
         void SetSecondDimensionMaximum(long newMax) { mSecondDimensionMax = newMax; }

         /// @return value to encode in the second enumerated space to encode.  It defaults to 0.
         long GetSecondDimensionValue() const { return mSecondDimensionValue; }
         void SetSecondDimensionValue(long newValue) { mSecondDimensionValue = newValue; }

         /// @return value to encode in the second enumerated space to encode.  It defaults to 0.
         bool IsSecondDimensionEnabled() const { return mSecondDimensionEnabled; }
         void SetSecondDimensionEnabled(bool newEnabled) { mSecondDimensionEnabled = newEnabled; }

         /// @return the minimum third enumerated value.  It defaults to 0
         long GetThirdDimensionMinimum() const { return mThirdDimensionMin; }
         void SetThirdDimensionMinimum(long newMin) { mThirdDimensionMin = newMin; }
         
         /// @return the maximum third enumerated value.  It defaults to 1
         long GetThirdDimensionMaximum() const { return mThirdDimensionMax; }
         void SetThirdDimensionMaximum(long newMax) { mThirdDimensionMax = newMax; }

         /// @return value to encode in the third enumerated space to encode.  It defaults to 0.
         long GetThirdDimensionValue() const { return mThirdDimensionValue; }
         void SetThirdDimensionValue(long newValue) { mThirdDimensionValue = newValue; }

         /// @return value to encode in the third enumerated space to encode.  It defaults to 0.
         bool IsThirdDimensionEnabled() const { return mThirdDimensionEnabled; }
         void SetThirdDimensionEnabled(bool newEnabled) { mThirdDimensionEnabled = newEnabled; }

      private:
         
         long mSecondDimensionMin;
         long mSecondDimensionMax;
         long mSecondDimensionValue;
         bool mSecondDimensionEnabled;

         long mThirdDimensionMin;
         long mThirdDimensionMax;
         long mThirdDimensionValue;
         bool mThirdDimensionEnabled;
   };

}

#endif /*DELTA_DDMMULTIENUMERATEDCALCULATOR*/
