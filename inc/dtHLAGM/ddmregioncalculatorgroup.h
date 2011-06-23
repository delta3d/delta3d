#ifndef DELTA_DDM_REGION_CALCULATOR_GROUP
#define DELTA_DDM_REGION_CALCULATOR_GROUP

#include <osg/Referenced>
#include <dtCore/refptr.h>
#include <dtHLAGM/export.h>
#include <vector>

namespace dtHLAGM
{
   class DDMRegionCalculator;
   
   class DT_HLAGM_EXPORT DDMRegionCalculatorGroup
   {
      public:
      	DDMRegionCalculatorGroup();
         ~DDMRegionCalculatorGroup();
      	
         /// @return the list of DDM region publishing calculators used by this component.
         const DDMRegionCalculator* GetCalculator(const std::string& name) const;
         DDMRegionCalculator* GetCalculator(const std::string& name);

         /// @return the list of DDM calculators used by this component.
         const std::vector<dtCore::RefPtr<DDMRegionCalculator> >& GetCalculators() const;

         /// @param toFill a vector to fill with pointers to all the calculators in the group. 
         void GetCalculators(std::vector<DDMRegionCalculator*>& toFill);

         /// Adds a new custom DDM region calculator to this group.
         bool AddCalculator(DDMRegionCalculator& newCalc);

         /// Removes a custom DDM region calculator to this group.
         void RemoveCalculator(DDMRegionCalculator& calc);

         /// Removes a custom DDM region calculator to this group by name.
         void RemoveCalculator(const std::string& name);

         unsigned GetSize() const { return mCalculators.size(); }
         
         bool IsEmpty() const { return mCalculators.empty(); }
         
         DDMRegionCalculator* operator[](unsigned index) { return mCalculators[index].get(); }
         const DDMRegionCalculator* operator[](unsigned index) const { return mCalculators[index].get(); }
         
      private:
         std::vector<dtCore::RefPtr<DDMRegionCalculator> > mCalculators;
   };
}

#endif /*DELTA_DDM_REGION_CALCULATOR_GROUP*/
