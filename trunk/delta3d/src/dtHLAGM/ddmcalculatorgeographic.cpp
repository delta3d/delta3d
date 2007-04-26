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
 * @author David Guthrie
 */
#include <dtHLAGM/ddmcalculatorgeographic.h>
#include <dtHLAGM/ddmregiondata.h>
#include <dtHLAGM/ddmutil.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/functor.h>


namespace dtHLAGM
{   

   const std::string DDMCalculatorGeographic::PROP_APP_SPACE_MIN("App Space Min");
   const std::string DDMCalculatorGeographic::PROP_APP_SPACE_MAX("App Space Man");

   
   IMPLEMENT_ENUM(DDMCalculatorGeographic::RegionCalculationType);
   const DDMCalculatorGeographic::RegionCalculationType DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE("GEOGRAPHIC_SPACE");
   const DDMCalculatorGeographic::RegionCalculationType DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY("APP_SPACE_ONLY");

   DDMCalculatorGeographic::RegionCalculationType::RegionCalculationType(const std::string &name) : dtUtil::Enumeration(name)
   {
      AddInstance(this);
   }

   IMPLEMENT_ENUM(DDMCalculatorGeographic::DDMForce);
   const DDMCalculatorGeographic::DDMForce DDMCalculatorGeographic::DDMForce::FORCE_FRIENDLY("FORCE_FRIENDLY", 1);
   const DDMCalculatorGeographic::DDMForce DDMCalculatorGeographic::DDMForce::FORCE_ENEMY("FORCE_ENEMY", 2);
   const DDMCalculatorGeographic::DDMForce DDMCalculatorGeographic::DDMForce::FORCE_NEUTRAL("FORCE_NEUTRAL", 3);

   DDMCalculatorGeographic::DDMForce::DDMForce(const std::string &name, int id) : dtUtil::Enumeration(name)
   {
      AddInstance(this);
      mId = id;
   }

   DDMCalculatorGeographic::DDMCalculatorGeographic(): mAppSpaceMin(0), mAppSpaceMax(99)
   {
      AddProperty(*new dtDAL::LongActorProperty(PROP_APP_SPACE_MIN, PROP_APP_SPACE_MIN,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetAppSpaceMinimum),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetAppSpaceMinimum)
            ));
      AddProperty(*new dtDAL::LongActorProperty(PROP_APP_SPACE_MAX, PROP_APP_SPACE_MAX,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetAppSpaceMaximum),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetAppSpaceMaximum)
            ));
   }
   
   DDMCalculatorGeographic::~DDMCalculatorGeographic()
   {
   }

   dtCore::RefPtr<DDMRegionData> DDMCalculatorGeographic::CreateRegionData() const
   {
      return NULL;
   }
   
   bool DDMCalculatorGeographic::UpdateRegionData(DDMRegionData& ddmData) const
   {
      return true;
   }
   
   unsigned long DDMCalculatorGeographic::MapAppSpaceValue(unsigned spaceNumber) 
   {
      return DDMUtil::MapEnumerated(spaceNumber, mAppSpaceMin, mAppSpaceMax);
   }
}
