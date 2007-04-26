
#include <dtHLAGM/ddmregioncalculator.h>
#include <dtDAL/actorproperty.h>

namespace dtHLAGM
{
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