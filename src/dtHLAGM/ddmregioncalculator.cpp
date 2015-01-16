
#include <dtHLAGM/ddmregioncalculator.h>

#include <dtCore/datatype.h>
#include <dtCore/stringactorproperty.h>

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
      AddProperty(new dtCore::StringActorProperty(PROP_FIRST_DIMENSION_NAME, PROP_FIRST_DIMENSION_NAME,
            dtCore::StringActorProperty::SetFuncType(this, &DDMRegionCalculator::SetFirstDimensionName),
            dtCore::StringActorProperty::GetFuncType(this, &DDMRegionCalculator::GetFirstDimensionNameByCopy)
            ));
      AddProperty(new dtCore::StringActorProperty(PROP_SECOND_DIMENSION_NAME, PROP_SECOND_DIMENSION_NAME,
            dtCore::StringActorProperty::SetFuncType(this, &DDMRegionCalculator::SetSecondDimensionName),
            dtCore::StringActorProperty::GetFuncType(this, &DDMRegionCalculator::GetSecondDimensionNameByCopy)
            ));
      AddProperty(new dtCore::StringActorProperty(PROP_THIRD_DIMENSION_NAME, PROP_THIRD_DIMENSION_NAME,
            dtCore::StringActorProperty::SetFuncType(this, &DDMRegionCalculator::SetThirdDimensionName),
            dtCore::StringActorProperty::GetFuncType(this, &DDMRegionCalculator::GetThirdDimensionNameByCopy)
            ));
   }

   dtCore::RefPtr<dtCore::ObjectType> DDMRegionCalculator::TYPE(new dtCore::ObjectType("DDMRegionCalculator", "dtHLAGM"));

   /*override*/ const dtCore::ObjectType& DDMRegionCalculator::GetObjectType() const { return *TYPE; }

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
   bool DDMRegionCalculator::UpdateDimension(DDMRegionData& ddmData, unsigned index, const std::string& name, unsigned int min, unsigned int max)
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
