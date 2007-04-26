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
#ifndef DELTA_DDMCALCULATORGEOGRAPHIC
#define DELTA_DDMCALCULATORGEOGRAPHIC

#include <dtHLAGM/export.h>
#include <dtHLAGM/ddmregioncalculator.h>
#include <dtUtil/enumeration.h>
#include <dtUtil/coordinates.h>

namespace dtHLAGM
{

   class DT_HLAGM_EXPORT DDMCalculatorGeographic : public dtHLAGM::DDMRegionCalculator
   {
      public:
         
         static const std::string PROP_APP_SPACE_MIN;
         static const std::string PROP_APP_SPACE_MAX;
         
         class RegionCalculationType : public dtUtil::Enumeration
         {
            DECLARE_ENUM(RegionCalculationType);
            public:
               static const RegionCalculationType GEOGRAPHIC_SPACE;
               static const RegionCalculationType APP_SPACE_ONLY;
            protected:
               RegionCalculationType(const std::string &name);

         };

         class DDMForce : public dtUtil::Enumeration
         {
            DECLARE_ENUM(DDMForce);
            public:
               static const DDMForce FORCE_FRIENDLY;
               static const DDMForce FORCE_ENEMY;
               static const DDMForce FORCE_NEUTRAL;
            protected:
               DDMForce(const std::string& name, int id);
            private:
               int mId;
         };
         
         DDMCalculatorGeographic();

         /**
          * Creates and returns a pointer to a region data object for this calculator.
          * This is basically a factory method.  The using code will be required to call delete on
          * the resulting object.
          * @return the new region data object.
          */
         virtual dtCore::RefPtr<DDMRegionData> CreateRegionData() const;
         
         /**
          * Updates the subscription region 
          * @param region the region to update.
          */
         virtual bool UpdateRegionData(DDMRegionData& ddmData) const;
                 
         void SetCoordinateConverter(dtUtil::Coordinates& coord) { mCoordinates = coord; }
         dtUtil::Coordinates& GetCoordinateConverter() { return mCoordinates; }
         const dtUtil::Coordinates& GetCoordinateConverter() const { return mCoordinates; }
         
         /// @return the minimum encoded app space value.  It defaults to 0
         long GetAppSpaceMinimum() const { return mAppSpaceMin; }
         void SetAppSpaceMinimum(long newMin) { mAppSpaceMin = newMin; }
         
         /// @return the maximum encoded app space value.  It defaults to 99
         long GetAppSpaceMaximum() const { return mAppSpaceMax; }
         void SetAppSpaceMaximum(long newMax) { mAppSpaceMax = newMax; }
         
         unsigned long MapAppSpaceValue(unsigned spaceNumber);

      protected:
         virtual ~DDMCalculatorGeographic();
         
         long mAppSpaceMin;
         long mAppSpaceMax;
         
         dtUtil::Coordinates mCoordinates;
   };

}

#endif /* DELTA_DDMCALCULATORGEOGRAPHIC*/
