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
#include <dtHLAGM/ddmregioncalculatorgroup.h>
#include <dtHLAGM/ddmregioncalculator.h>

#include <dtCore/actorproperty.h>
#include <algorithm>
#include <vector>

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
   void DDMRegionCalculatorGroup::GetCalculators(std::vector<DDMRegionCalculator*>& toFill) 
   {  
      toFill.resize(mCalculators.size(), NULL);
      std::transform(mCalculators.begin(), mCalculators.end(), toFill.begin(), dtCore::ConvertToPointerUnary<DDMRegionCalculator>());
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
