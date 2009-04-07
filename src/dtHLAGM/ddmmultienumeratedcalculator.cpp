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

#include <dtHLAGM/ddmregiondata.h>
#include <dtHLAGM/ddmmultienumeratedcalculator.h>
#include <dtHLAGM/ddmutil.h>

#include <dtDAL/enginepropertytypes.h>

namespace dtHLAGM
{
   const std::string DDMMultiEnumeratedCalculator::PROP_SECOND_DIMENSION_MIN("Second Dimension Min");
   const std::string DDMMultiEnumeratedCalculator::PROP_SECOND_DIMENSION_MAX("Second Dimension Max");
   const std::string DDMMultiEnumeratedCalculator::PROP_SECOND_DIMENSION_VALUE("Second Dimension Value");
   const std::string DDMMultiEnumeratedCalculator::PROP_SECOND_DIMENSION_ENABLED("Second Dimension Enabled");

   const std::string DDMMultiEnumeratedCalculator::PROP_THIRD_DIMENSION_MIN("First Dimension Min");
   const std::string DDMMultiEnumeratedCalculator::PROP_THIRD_DIMENSION_MAX("First Dimension Max");
   const std::string DDMMultiEnumeratedCalculator::PROP_THIRD_DIMENSION_VALUE("First Dimension Value");
   const std::string DDMMultiEnumeratedCalculator::PROP_THIRD_DIMENSION_ENABLED("First Dimension Enabled");

   DDMMultiEnumeratedCalculator::DDMMultiEnumeratedCalculator():
      mSecondDimensionMin(0),
      mSecondDimensionMax(1),
      mSecondDimensionValue(0),
      mSecondDimensionEnabled(false),
      mThirdDimensionMin(0),
      mThirdDimensionMax(1),
      mThirdDimensionValue(0),
      mThirdDimensionEnabled(false)
   {
      AddProperty(*new dtDAL::LongActorProperty(PROP_SECOND_DIMENSION_MIN, PROP_SECOND_DIMENSION_MIN,
            dtDAL::MakeFunctor(*this, &DDMMultiEnumeratedCalculator::SetSecondDimensionMinimum),
            dtDAL::MakeFunctorRet(*this, &DDMMultiEnumeratedCalculator::GetSecondDimensionMinimum)
            ));
      AddProperty(*new dtDAL::LongActorProperty(PROP_SECOND_DIMENSION_MAX, PROP_SECOND_DIMENSION_MAX,
            dtDAL::MakeFunctor(*this, &DDMMultiEnumeratedCalculator::SetSecondDimensionMaximum),
            dtDAL::MakeFunctorRet(*this, &DDMMultiEnumeratedCalculator::GetSecondDimensionMaximum)
            ));

      AddProperty(*new dtDAL::LongActorProperty(PROP_SECOND_DIMENSION_VALUE, PROP_SECOND_DIMENSION_VALUE,
            dtDAL::MakeFunctor(*this, &DDMMultiEnumeratedCalculator::SetSecondDimensionValue),
            dtDAL::MakeFunctorRet(*this, &DDMMultiEnumeratedCalculator::GetSecondDimensionValue)
            ));
      AddProperty(*new dtDAL::BooleanActorProperty(PROP_SECOND_DIMENSION_ENABLED, PROP_SECOND_DIMENSION_ENABLED,
            dtDAL::MakeFunctor(*this, &DDMMultiEnumeratedCalculator::SetSecondDimensionEnabled),
            dtDAL::MakeFunctorRet(*this, &DDMMultiEnumeratedCalculator::IsSecondDimensionEnabled)
            ));

      AddProperty(*new dtDAL::LongActorProperty(PROP_THIRD_DIMENSION_MIN, PROP_THIRD_DIMENSION_MIN,
            dtDAL::MakeFunctor(*this, &DDMMultiEnumeratedCalculator::SetThirdDimensionMinimum),
            dtDAL::MakeFunctorRet(*this, &DDMMultiEnumeratedCalculator::GetThirdDimensionMinimum)
            ));
      AddProperty(*new dtDAL::LongActorProperty(PROP_THIRD_DIMENSION_MAX, PROP_THIRD_DIMENSION_MAX,
            dtDAL::MakeFunctor(*this, &DDMMultiEnumeratedCalculator::SetThirdDimensionMaximum),
            dtDAL::MakeFunctorRet(*this, &DDMMultiEnumeratedCalculator::GetThirdDimensionMaximum)
            ));

      AddProperty(*new dtDAL::LongActorProperty(PROP_THIRD_DIMENSION_VALUE, PROP_THIRD_DIMENSION_VALUE,
            dtDAL::MakeFunctor(*this, &DDMMultiEnumeratedCalculator::SetThirdDimensionValue),
            dtDAL::MakeFunctorRet(*this, &DDMMultiEnumeratedCalculator::GetThirdDimensionValue)
            ));
      AddProperty(*new dtDAL::BooleanActorProperty(PROP_THIRD_DIMENSION_ENABLED, PROP_THIRD_DIMENSION_ENABLED,
            dtDAL::MakeFunctor(*this, &DDMMultiEnumeratedCalculator::SetThirdDimensionEnabled),
            dtDAL::MakeFunctorRet(*this, &DDMMultiEnumeratedCalculator::IsThirdDimensionEnabled)
            ));
   }

   DDMMultiEnumeratedCalculator::~DDMMultiEnumeratedCalculator()
   {
   }
   
   dtCore::RefPtr<DDMRegionData> DDMMultiEnumeratedCalculator::CreateRegionData() const
   {
      return new DDMRegionData;
   }
   
   void DDMMultiEnumeratedCalculator::CreateSubscriptionRegionData(std::vector<dtCore::RefPtr<DDMRegionData> >& toFill) const
   {
      toFill.resize(1);
      toFill[0] = new DDMRegionData;
   }
   
   bool DDMMultiEnumeratedCalculator::UpdateRegionData(DDMRegionData& ddmData) const
   {
      //if (!BaseClass::UpdateRegionData(ddmData))
      //   return false;
      dtUtil::Log& logger = dtUtil::Log::GetInstance("ddmmultienumeratedcalculator.cpp");

      unsigned long mappedValue = MapAppSpaceValue(GetDefaultAppSpace());

      if (logger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Using app space number "
               "[%d].  It maps to [%u].", GetDefaultAppSpace(), mappedValue);
      }
      
      bool updated = UpdateDimension(ddmData, 0, GetFirstDimensionName(), mappedValue, mappedValue);

      if (IsSecondDimensionEnabled())
      {
         mappedValue = DDMUtil::MapEnumerated(GetSecondDimensionValue(), GetSecondDimensionMinimum(), GetSecondDimensionMaximum());
         if (UpdateDimension(ddmData, 1, GetSecondDimensionName(), mappedValue, mappedValue))
         {
            updated = true;
         }
         
         if (IsThirdDimensionEnabled())
         {
            mappedValue = DDMUtil::MapEnumerated(GetThirdDimensionValue(), GetThirdDimensionMinimum(), GetThirdDimensionMaximum());
            if (UpdateDimension(ddmData, 2, GetThirdDimensionName(), mappedValue, mappedValue))
            {
               updated = true;
            }
         }
      }
      return updated;
   }
}
