/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology, Inc.
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
#ifndef DELTA_DDM_REGION_CALCULATOR
#define DELTA_DDM_REGION_CALCULATOR

#ifndef RTI_USES_STD_FSTREAM
#define RTI_USES_STD_FSTREAM
#endif
#include <RTI.hh>

#include <dtCore/refptr.h>
#include <dtCore/base.h>

namespace dtGame
{
   class ActorUpdateMessage;
}

namespace dtHLAGM
{
   class DDMRegionData;
   
   /**
    * This abstract calculator class provides the interface used to write concrete region calculators.
    * That is, it modifies the RTI region based on implemented algoriths using the state data provided
    * on the DDMRegionData object.
    */
   class DDMRegionCalculator : public dtCore::Base
   {
      public:
         /**
          * Creates and returns a pointer to a region data object for this calculator.
          * This is basically a factory method.  The using code will be required to call delete on
          * the resulting object.
          * @return the new region data object.
          */
         virtual dtCore::RefPtr<DDMRegionData> CreateRegionData() const = 0;
         
         /**
          * Updates the subscription region 
          * @param region the region to update.
          */
         virtual bool UpdateRegionData(DDMRegionData& ddmData) const = 0;
         
         /**
          * Sets a property value on the region calculator.  These properties are defined by the class, but 
          * can be set generically using this method so that they can be put in the xml config file.
          * @param name the name of the property to set
          * @param value the value to set
          */
         virtual void SetProperty(const std::string& name, const std::string& value) = 0;

         /**
          * @param name the name of the property to fetch.
          * @param valueToFill an output parameter string to fill with the value of the property. It's value
          *                    is undefined if the method returns false.
          * @return true if the property exists or false if not.
          */
         virtual bool GetProperty(const std::string& name, std::string& valueToFill) = 0;
        
   };
}

#endif /*DELTA_DDM_REGION_CALCULATOR*/
