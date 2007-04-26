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
#include <dtHLAGM/export.h>

namespace dtDAL
{
   class ActorProperty;
}

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
   class DT_HLAGM_EXPORT DDMRegionCalculator : public dtCore::Base
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
         
         dtDAL::ActorProperty* GetProperty(const std::string& name);

         template <class PropertyType>
         void GetProperty(const std::string& name, PropertyType*& toSet)
         {
            dtDAL::ActorProperty* prop = GetProperty(name);
            toSet = static_cast<PropertyType*>(prop);
         }
         
         const dtDAL::ActorProperty* GetProperty(const std::string& name) const;

         template <class PropertyType>
         void GetProperty(const std::string& name, PropertyType*& toSet) const
         {
            const dtDAL::ActorProperty* prop = GetProperty(name);
            toSet = static_cast<PropertyType*>(prop);
         }

         void GetAllProperties(std::vector<dtDAL::ActorProperty*> toFill);
         
      protected:
         void AddProperty(dtDAL::ActorProperty& newProperty);
      private:
         std::map<std::string, dtCore::RefPtr<dtDAL::ActorProperty> > mProperties;
   };
}

#endif /*DELTA_DDM_REGION_CALCULATOR*/
