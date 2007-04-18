#include <dtHLAGM/ddmregioncalculatorgroup.h>
#include <dtHLAGM/ddmregioncalculator.h>

namespace dtHLAGM
{

   DDMRegionCalculatorGroup::DDMRegionCalculatorGroup()
   {
   }
   DDMRegionCalculatorGroup::~DDMRegionCalculatorGroup()
   {
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   const DDMRegionCalculator* DDMRegionCalculatorGroup::GetCalculator(const std::string& name) const
   {
      for (unsigned i = 0; i < mCalculators.size(); ++i)
      {
         if (mCalculators[i]->GetName() == name)
            return mCalculators[i].get();
      }
      
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   DDMRegionCalculator* DDMRegionCalculatorGroup::GetCalculator(const std::string& name)
   {
      for (unsigned i = 0; i < mCalculators.size(); ++i)
      {
         if (mCalculators[i]->GetName() == name)
            return mCalculators[i].get();
      }
      
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::vector<dtCore::RefPtr<DDMRegionCalculator> >& DDMRegionCalculatorGroup::GetCalculators() const 
   { 
      return mCalculators; 
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool DDMRegionCalculatorGroup::AddCalculator(DDMRegionCalculator& newCalc)
   {
      if (GetCalculator(newCalc.GetName()) == NULL)
      {
         mCalculators.push_back(&newCalc);
         return true;
      }
      
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DDMRegionCalculatorGroup::RemoveCalculator(DDMRegionCalculator& calc)
   {
      for (unsigned i = 0; i < mCalculators.size(); ++i)
      {
         if (mCalculators[i] == &calc)
            mCalculators.erase(mCalculators.begin() + i);
      }      
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DDMRegionCalculatorGroup::RemoveCalculator(const std::string& name)
   {
      for (unsigned i = 0; i < mCalculators.size(); ++i)
      {
         if (mCalculators[i]->GetName() == name)
            mCalculators.erase(mCalculators.begin() + i);
      }            
   }
}
