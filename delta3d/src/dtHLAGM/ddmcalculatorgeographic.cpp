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
#include <dtHLAGM/ddmgeographicregiondata.h>
#include <dtHLAGM/ddmregiondata.h>
#include <dtHLAGM/ddmutil.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/functor.h>

namespace dtHLAGM
{   
   
   const std::string DDMCalculatorGeographic::PROP_FRIENDLY_GROUND_REGION_TYPE("Friendly Ground Region Type");
   const std::string DDMCalculatorGeographic::PROP_ENEMY_GROUND_REGION_TYPE("Enemy Ground Region Type");
   const std::string DDMCalculatorGeographic::PROP_NEUTRAL_GROUND_REGION_TYPE("Neutral Ground Region Type");

   const std::string DDMCalculatorGeographic::PROP_FRIENDLY_AIR_REGION_TYPE("Friendly Air Region Type");
   const std::string DDMCalculatorGeographic::PROP_ENEMY_AIR_REGION_TYPE("Enemy Air Region Type");
   const std::string DDMCalculatorGeographic::PROP_NEUTRAL_AIR_REGION_TYPE("Neutral Air Region Type");

   const std::string DDMCalculatorGeographic::PROP_FRIENDLY_SEA_REGION_TYPE("Friendly Sea Region Type");
   const std::string DDMCalculatorGeographic::PROP_ENEMY_SEA_REGION_TYPE("Enemy Sea Region Type");
   const std::string DDMCalculatorGeographic::PROP_NEUTRAL_SEA_REGION_TYPE("Neutral Sea Region Type");
   
   const std::string DDMCalculatorGeographic::PROP_FRIENDLY_LIFEFORM_REGION_TYPE("Friendly Lifeform Region Type");
   const std::string DDMCalculatorGeographic::PROP_ENEMY_LIFEFORM_REGION_TYPE("Enemy Lifeform Region Type");
   const std::string DDMCalculatorGeographic::PROP_NEUTRAL_LIFEFORM_REGION_TYPE("Neutral Lifeform Region Type");

   const std::string DDMCalculatorGeographic::PROP_FRIENDLY_GROUND_APPSPACE("Friendly Ground App Space");
   const std::string DDMCalculatorGeographic::PROP_ENEMY_GROUND_APPSPACE("Enemy Ground App Space");
   const std::string DDMCalculatorGeographic::PROP_NEUTRAL_GROUND_APPSPACE("Neutral Ground App Space");

   const std::string DDMCalculatorGeographic::PROP_FRIENDLY_AIR_APPSPACE("Friendly Air App Space");
   const std::string DDMCalculatorGeographic::PROP_ENEMY_AIR_APPSPACE("Enemy Air App Space");
   const std::string DDMCalculatorGeographic::PROP_NEUTRAL_AIR_APPSPACE("Neutral Air App Space");

   const std::string DDMCalculatorGeographic::PROP_FRIENDLY_SEA_APPSPACE("Friendly Sea App Space");
   const std::string DDMCalculatorGeographic::PROP_ENEMY_SEA_APPSPACE("Enemy Sea App Space");
   const std::string DDMCalculatorGeographic::PROP_NEUTRAL_SEA_APPSPACE("Neutral Sea App Space");
   
   const std::string DDMCalculatorGeographic::PROP_FRIENDLY_LIFEFORM_APPSPACE("Friendly Lifeform App Space");
   const std::string DDMCalculatorGeographic::PROP_ENEMY_LIFEFORM_APPSPACE("Enemy Lifeform App Space");
   const std::string DDMCalculatorGeographic::PROP_NEUTRAL_LIFEFORM_APPSPACE("Neutral Lifeform App Space");

   const std::string DDMCalculatorGeographic::PROP_APP_SPACE_MIN("App Space Min");
   const std::string DDMCalculatorGeographic::PROP_APP_SPACE_MAX("App Space Man");
   
   IMPLEMENT_ENUM(DDMCalculatorGeographic::RegionCalculationType);
   DDMCalculatorGeographic::RegionCalculationType DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE("GEOGRAPHIC_SPACE");
   DDMCalculatorGeographic::RegionCalculationType DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY("APP_SPACE_ONLY");

   DDMCalculatorGeographic::RegionCalculationType::RegionCalculationType(const std::string &name) : dtUtil::Enumeration(name)
   {
      AddInstance(this);
   }

   IMPLEMENT_ENUM(DDMCalculatorGeographic::DDMForce);
   DDMCalculatorGeographic::DDMForce DDMCalculatorGeographic::DDMForce::FORCE_FRIENDLY("FORCE_FRIENDLY", 1);
   DDMCalculatorGeographic::DDMForce DDMCalculatorGeographic::DDMForce::FORCE_ENEMY("FORCE_ENEMY", 2);
   DDMCalculatorGeographic::DDMForce DDMCalculatorGeographic::DDMForce::FORCE_NEUTRAL("FORCE_NEUTRAL", 3);

   DDMCalculatorGeographic::DDMForce::DDMForce(const std::string &name, int id) : dtUtil::Enumeration(name)
   {
      AddInstance(this);
      mId = id;
   }

   DDMCalculatorGeographic::DDMCalculatorGeographic(): 
      mFriendlyGroundRegionType(&DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE),
      mFriendlyAirRegionType(&DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE),
      mFriendlySeaRegionType(&DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE),
      mFriendlyLifeformRegionType(&DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE),
      mEnemyGroundRegionType(&DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE),
      mEnemyAirRegionType(&DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE),
      mEnemySeaRegionType(&DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE),
      mEnemyLifeformRegionType(&DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE),
      mNeutralGroundRegionType(&DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE),
      mNeutralAirRegionType(&DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE),
      mNeutralSeaRegionType(&DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE),
      mNeutralLifeformRegionType(&DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE),

      mFriendlyGroundAppSpace(7),
      mFriendlyAirAppSpace(1),
      mFriendlySeaAppSpace(4),
      mFriendlyLifeformAppSpace(10),
      mEnemyGroundAppSpace(8),
      mEnemyAirAppSpace(2),
      mEnemySeaAppSpace(5),
      mEnemyLifeformAppSpace(10),
      mNeutralGroundAppSpace(9),
      mNeutralAirAppSpace(3),
      mNeutralSeaAppSpace(6),
      mNeutralLifeformAppSpace(10),

      mAppSpaceMin(0), 
      mAppSpaceMax(99)
   {
      mCoordinates.SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::GEODETIC);

      AddProperty(*new dtDAL::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>(PROP_FRIENDLY_SEA_REGION_TYPE, PROP_FRIENDLY_SEA_REGION_TYPE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetFriendlySeaRegionType),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetFriendlySeaRegionType)
            ));
      AddProperty(*new dtDAL::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>(PROP_FRIENDLY_AIR_REGION_TYPE, PROP_FRIENDLY_AIR_REGION_TYPE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetFriendlyAirRegionType),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetFriendlyAirRegionType)
            ));
      AddProperty(*new dtDAL::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>(PROP_FRIENDLY_GROUND_REGION_TYPE, PROP_FRIENDLY_GROUND_REGION_TYPE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetFriendlyGroundRegionType),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetFriendlyGroundRegionType)
            ));
      AddProperty(*new dtDAL::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>(PROP_FRIENDLY_LIFEFORM_REGION_TYPE, PROP_FRIENDLY_LIFEFORM_REGION_TYPE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetFriendlyLifeformRegionType),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetFriendlyLifeformRegionType)
            ));

      AddProperty(*new dtDAL::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>(PROP_ENEMY_SEA_REGION_TYPE, PROP_ENEMY_SEA_REGION_TYPE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetEnemySeaRegionType),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetEnemySeaRegionType)
            ));
      AddProperty(*new dtDAL::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>(PROP_ENEMY_AIR_REGION_TYPE, PROP_ENEMY_AIR_REGION_TYPE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetEnemyAirRegionType),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetEnemyAirRegionType)
            ));
      AddProperty(*new dtDAL::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>(PROP_ENEMY_GROUND_REGION_TYPE, PROP_ENEMY_GROUND_REGION_TYPE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetEnemyGroundRegionType),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetEnemyGroundRegionType)
            ));
      AddProperty(*new dtDAL::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>(PROP_ENEMY_LIFEFORM_REGION_TYPE, PROP_ENEMY_LIFEFORM_REGION_TYPE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetEnemyLifeformRegionType),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetEnemyLifeformRegionType)
            ));
      
      AddProperty(*new dtDAL::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>(PROP_NEUTRAL_SEA_REGION_TYPE, PROP_NEUTRAL_SEA_REGION_TYPE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetNeutralSeaRegionType),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetNeutralSeaRegionType)
            ));
      AddProperty(*new dtDAL::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>(PROP_NEUTRAL_AIR_REGION_TYPE, PROP_NEUTRAL_AIR_REGION_TYPE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetNeutralAirRegionType),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetNeutralAirRegionType)
            ));
      AddProperty(*new dtDAL::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>(PROP_NEUTRAL_GROUND_REGION_TYPE, PROP_NEUTRAL_GROUND_REGION_TYPE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetNeutralGroundRegionType),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetNeutralGroundRegionType)
            ));
      AddProperty(*new dtDAL::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>(PROP_NEUTRAL_LIFEFORM_REGION_TYPE, PROP_NEUTRAL_LIFEFORM_REGION_TYPE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetNeutralLifeformRegionType),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetNeutralLifeformRegionType)
            ));

      
      AddProperty(*new dtDAL::IntActorProperty(PROP_FRIENDLY_SEA_APPSPACE, PROP_FRIENDLY_SEA_APPSPACE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetFriendlySeaAppSpace),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetFriendlySeaAppSpace)
            ));
      AddProperty(*new dtDAL::IntActorProperty(PROP_FRIENDLY_AIR_APPSPACE, PROP_FRIENDLY_AIR_APPSPACE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetFriendlyAirAppSpace),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetFriendlyAirAppSpace)
            ));
      AddProperty(*new dtDAL::IntActorProperty(PROP_FRIENDLY_GROUND_APPSPACE, PROP_FRIENDLY_GROUND_APPSPACE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetFriendlyGroundAppSpace),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetFriendlyGroundAppSpace)
            ));
      AddProperty(*new dtDAL::IntActorProperty(PROP_FRIENDLY_LIFEFORM_APPSPACE, PROP_FRIENDLY_LIFEFORM_APPSPACE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetFriendlyLifeformAppSpace),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetFriendlyLifeformAppSpace)
            ));

      AddProperty(*new dtDAL::IntActorProperty(PROP_ENEMY_SEA_APPSPACE, PROP_ENEMY_SEA_APPSPACE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetEnemySeaAppSpace),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetEnemySeaAppSpace)
            ));
      AddProperty(*new dtDAL::IntActorProperty(PROP_ENEMY_AIR_APPSPACE, PROP_ENEMY_AIR_APPSPACE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetEnemyAirAppSpace),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetEnemyAirAppSpace)
            ));
      AddProperty(*new dtDAL::IntActorProperty(PROP_ENEMY_GROUND_APPSPACE, PROP_ENEMY_GROUND_APPSPACE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetEnemyGroundAppSpace),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetEnemyGroundAppSpace)
            ));
      AddProperty(*new dtDAL::IntActorProperty(PROP_ENEMY_LIFEFORM_APPSPACE, PROP_ENEMY_LIFEFORM_APPSPACE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetEnemyLifeformAppSpace),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetEnemyLifeformAppSpace)
            ));
      
      AddProperty(*new dtDAL::IntActorProperty(PROP_NEUTRAL_SEA_APPSPACE, PROP_NEUTRAL_SEA_APPSPACE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetNeutralSeaAppSpace),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetNeutralSeaAppSpace)
            ));
      AddProperty(*new dtDAL::IntActorProperty(PROP_NEUTRAL_AIR_APPSPACE, PROP_NEUTRAL_AIR_APPSPACE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetNeutralAirAppSpace),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetNeutralAirAppSpace)
            ));
      AddProperty(*new dtDAL::IntActorProperty(PROP_NEUTRAL_GROUND_APPSPACE, PROP_NEUTRAL_GROUND_APPSPACE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetNeutralGroundAppSpace),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetNeutralGroundAppSpace)
            ));
      AddProperty(*new dtDAL::IntActorProperty(PROP_NEUTRAL_LIFEFORM_APPSPACE, PROP_NEUTRAL_LIFEFORM_APPSPACE,
            dtDAL::MakeFunctor(*this, &DDMCalculatorGeographic::SetNeutralLifeformAppSpace),
            dtDAL::MakeFunctorRet(*this, &DDMCalculatorGeographic::GetNeutralLifeformAppSpace)
            ));
      
      
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

   void DDMCalculatorGeographic::SetCoordinateConverter(const dtUtil::Coordinates& coord) 
   { 
      mCoordinates = coord; 
      mCoordinates.SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::GEODETIC); 
   }

   dtCore::RefPtr<DDMRegionData> DDMCalculatorGeographic::CreateRegionData() const
   {
      return new DDMGeographicRegionData;
   }
   
   bool DDMCalculatorGeographic::UpdateRegionData(DDMRegionData& ddmData) const
   {
      DDMGeographicRegionData* data = dynamic_cast<DDMGeographicRegionData*>(&ddmData);
      if (data == NULL)
      {
         LOG_ERROR("Unable to cast DDMRegionData to a DDMGeographicRegionData.  This implies a code error. Ignoring data.")
         return false;         
      }
      return true;
   }
   
   unsigned long DDMCalculatorGeographic::MapAppSpaceValue(unsigned spaceNumber) const
   {
      return DDMUtil::MapEnumerated(spaceNumber, mAppSpaceMin, mAppSpaceMax);
   }
}
