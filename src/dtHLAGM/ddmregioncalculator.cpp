
#include <dtHLAGM/ddmregioncalculator.h>

#include <dtDAL/datatype.h>
#include <dtDAL/stringactorproperty.h>

#include <dtHLAGM/ddmregiondata.h>

namespace dtHLAGM
{
   const std::string DDMRegionCalculator::PROP_FIRST_DIMENSION_NAME("First Dimension Name");
   const std::string DDMRegionCalculator::PROP_SECOND_DIMENSION_NAME("Second Dimension Name");
   const std::string DDMRegionCalculator::PROP_THIRD_DIMENSION_NAME("Third Dimension Name");

   //////////////////////////////////////////////////////////////
   DDMRegionCalculator::DDMRegionCalculator()
   : mFirstDimensionName("subspace")
   ,  mSecondDimensionName("one")
   ,  mThirdDimensionName("two")
   {
      AddProperty(new dtDAL::StringActorProperty(PROP_FIRST_DIMENSION_NAME, PROP_FIRST_DIMENSION_NAME,
            dtDAL::StringActorProperty::SetFuncType(this, &DDMRegionCalculator::SetFirstDimensionName),
            dtDAL::StringActorProperty::GetFuncType(this, &DDMRegionCalculator::GetFirstDimensionNameByCopy)
            ));
      AddProperty(new dtDAL::StringActorProperty(PROP_SECOND_DIMENSION_NAME, PROP_SECOND_DIMENSION_NAME,
            dtDAL::StringActorProperty::SetFuncType(this, &DDMRegionCalculator::SetSecondDimensionName),
            dtDAL::StringActorProperty::GetFuncType(this, &DDMRegionCalculator::GetSecondDimensionNameByCopy)
            ));
      AddProperty(new dtDAL::StringActorProperty(PROP_THIRD_DIMENSION_NAME, PROP_THIRD_DIMENSION_NAME,
            dtDAL::StringActorProperty::SetFuncType(this, &DDMRegionCalculator::SetThirdDimensionName),
            dtDAL::StringActorProperty::GetFuncType(this, &DDMRegionCalculator::GetThirdDimensionNameByCopy)
            ));
   }

   //////////////////////////////////////////////////////////////
   void DDMRegionCalculator::SetName(const std::string& name)
   {
      mName = name;
   }

   //////////////////////////////////////////////////////////////
   const std::string& DDMRegionCalculator::GetName() const
   {
      return mName;
   }

   //////////////////////////////////////////////////////////////
   bool DDMRegionCalculator::UpdateDimension(DDMRegionData& ddmData, unsigned index, const std::string& name, unsigned long min, unsigned long max)
   {
      const DDMRegionData::DimensionValues* dv = ddmData.GetDimensionValue(index);
      bool result = false;
      if (dv != NULL)
      {
         if (name != dv->mName)
            result = true;
         if (min != dv->mMin)
            result = true;
         if (max != dv->mMax)
            result = true;
      }
      else
      {
         result = true;
      }

      if (result)
      {
         dtUtil::Log& logger = dtUtil::Log::GetInstance("ddmcameracalculatorgeographic.cpp");
         if (logger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Updating dimension [%u] "
                  "with name \"%s\" to min [%u] max [%u].", index, name.c_str(), min, max);
         }

         DDMRegionData::DimensionValues newDv;
         newDv.mName = name;
         newDv.mMin = min;
         newDv.mMax = max;
         ddmData.SetDimensionValue(index, newDv);
      }
      return result;
   }

}
