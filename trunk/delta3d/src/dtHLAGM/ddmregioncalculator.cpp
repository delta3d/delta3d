
#include <dtHLAGM/ddmregioncalculator.h>
#include <dtDAL/enginepropertytypes.h>
namespace dtHLAGM
{
   const std::string DDMRegionCalculator::PROP_FIRST_DIMENSION_NAME("First Dimension Name");
   const std::string DDMRegionCalculator::PROP_SECOND_DIMENSION_NAME("Second Dimension Name");
   const std::string DDMRegionCalculator::PROP_THIRD_DIMENSION_NAME("Third Dimension Name");

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
   
   dtDAL::ActorProperty* DDMRegionCalculator::GetProperty(const std::string& name)
   {
      std::map<std::string, dtCore::RefPtr<dtDAL::ActorProperty> >::iterator i = mProperties.find(name);
      if (i == mProperties.end())
         return NULL;
      
      return i->second.get();
   }
   
   const dtDAL::ActorProperty* DDMRegionCalculator::GetProperty(const std::string& name) const
   {
      std::map<std::string, dtCore::RefPtr<dtDAL::ActorProperty> >::const_iterator i = mProperties.find(name);
      if (i == mProperties.end())
         return NULL;
      
      return i->second.get();      
   }

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
   
   void DDMRegionCalculator::AddProperty(dtDAL::ActorProperty& newProperty)
   {
      mProperties.insert(std::make_pair(newProperty.GetName(), &newProperty));
   }

}