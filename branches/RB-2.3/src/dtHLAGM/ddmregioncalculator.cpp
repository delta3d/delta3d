
#include <dtHLAGM/ddmregioncalculator.h>
#include <dtHLAGM/ddmregiondata.h>
#include <dtDAL/enginepropertytypes.h>

namespace dtHLAGM
{
   const std::string DDMRegionCalculator::PROP_FIRST_DIMENSION_NAME("First Dimension Name");
   const std::string DDMRegionCalculator::PROP_SECOND_DIMENSION_NAME("Second Dimension Name");
   const std::string DDMRegionCalculator::PROP_THIRD_DIMENSION_NAME("Third Dimension Name");

   //////////////////////////////////////////////////////////////
   DDMRegionCalculator::DDMRegionCalculator():
      mFirstDimensionName("subspace"),
      mSecondDimensionName("one"),
      mThirdDimensionName("two")
   {
      AddProperty(*new dtDAL::StringActorProperty(PROP_FIRST_DIMENSION_NAME, PROP_FIRST_DIMENSION_NAME,
            dtDAL::MakeFunctor(*this, &DDMRegionCalculator::SetFirstDimensionName),
            dtDAL::MakeFunctorRet(*this, &DDMRegionCalculator::GetFirstDimensionNameByCopy)
            ));
      AddProperty(*new dtDAL::StringActorProperty(PROP_SECOND_DIMENSION_NAME, PROP_SECOND_DIMENSION_NAME,
            dtDAL::MakeFunctor(*this, &DDMRegionCalculator::SetSecondDimensionName),
            dtDAL::MakeFunctorRet(*this, &DDMRegionCalculator::GetSecondDimensionNameByCopy)
            ));
      AddProperty(*new dtDAL::StringActorProperty(PROP_THIRD_DIMENSION_NAME, PROP_THIRD_DIMENSION_NAME,
            dtDAL::MakeFunctor(*this, &DDMRegionCalculator::SetThirdDimensionName),
            dtDAL::MakeFunctorRet(*this, &DDMRegionCalculator::GetThirdDimensionNameByCopy)
            ));
   }
   
   //////////////////////////////////////////////////////////////
   dtDAL::ActorProperty* DDMRegionCalculator::GetProperty(const std::string& name)
   {
      std::map<std::string, dtCore::RefPtr<dtDAL::ActorProperty> >::iterator i = mProperties.find(name);
      if (i == mProperties.end())
         return NULL;
      
      return i->second.get();
   }
   
   //////////////////////////////////////////////////////////////
   const dtDAL::ActorProperty* DDMRegionCalculator::GetProperty(const std::string& name) const
   {
      std::map<std::string, dtCore::RefPtr<dtDAL::ActorProperty> >::const_iterator i = mProperties.find(name);
      if (i == mProperties.end())
         return NULL;
      
      return i->second.get();      
   }

   //////////////////////////////////////////////////////////////
   void DDMRegionCalculator::GetAllProperties(std::vector<dtDAL::ActorProperty*> toFill)
   {
      toFill.clear();
      toFill.reserve(mProperties.size());

      std::map<std::string, dtCore::RefPtr<dtDAL::ActorProperty> >::iterator i;
      for (i = mProperties.begin(); i != mProperties.end(); ++i)
      {
         toFill.push_back(i->second.get());
      }
   }
   
   //////////////////////////////////////////////////////////////
   void DDMRegionCalculator::AddProperty(dtDAL::ActorProperty& newProperty)
   {
      mProperties.insert(std::make_pair(newProperty.GetName(), &newProperty));
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
