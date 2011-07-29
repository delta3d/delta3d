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
#include <dtHLAGM/ddmcalculatorgeographic.h>

#include <dtCore/enumactorproperty.h>
#include <dtCore/longactorproperty.h>

#include <dtHLAGM/ddmgeographicregiondata.h>
#include <dtHLAGM/ddmregiondata.h>

namespace dtHLAGM
{
   const std::string DDMCalculatorGeographic::PROP_CALCULATOR_OBJECT_KIND("Object Kind");

   const std::string DDMCalculatorGeographic::PROP_FRIENDLY_REGION_TYPE("Friendly Region Type");
   const std::string DDMCalculatorGeographic::PROP_ENEMY_REGION_TYPE("Enemy Region Type");
   const std::string DDMCalculatorGeographic::PROP_NEUTRAL_REGION_TYPE("Neutral Region Type");

   const std::string DDMCalculatorGeographic::PROP_DEFAULT_REGION_TYPE("Default Region Type");

   const std::string DDMCalculatorGeographic::PROP_FRIENDLY_APPSPACE("Friendly App Space");
   const std::string DDMCalculatorGeographic::PROP_ENEMY_APPSPACE("Enemy App Space");
   const std::string DDMCalculatorGeographic::PROP_NEUTRAL_APPSPACE("Neutral App Space");

   IMPLEMENT_ENUM(DDMCalculatorGeographic::RegionCalculationType);
   DDMCalculatorGeographic::RegionCalculationType DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE("GEOGRAPHIC_SPACE");
   DDMCalculatorGeographic::RegionCalculationType DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY("APP_SPACE_ONLY");

   DDMCalculatorGeographic::RegionCalculationType::RegionCalculationType(const std::string &name) : dtUtil::Enumeration(name)
   {
      AddInstance(this);
   }

   IMPLEMENT_ENUM(DDMCalculatorGeographic::DDMObjectKind);
   DDMCalculatorGeographic::DDMObjectKind DDMCalculatorGeographic::DDMObjectKind::OBJECT_KIND_ENTITY("OBJECT_KIND_GROUND");
   DDMCalculatorGeographic::DDMObjectKind DDMCalculatorGeographic::DDMObjectKind::OBJECT_KIND_OTHER("OBJECT_KIND_OTHER");

   DDMCalculatorGeographic::DDMObjectKind::DDMObjectKind(const std::string& name) : dtUtil::Enumeration(name)
   {
      AddInstance(this);
   }

   IMPLEMENT_ENUM(DDMCalculatorGeographic::DDMForce);
   DDMCalculatorGeographic::DDMForce DDMCalculatorGeographic::DDMForce::FORCE_FRIENDLY("FORCE_FRIENDLY", 1);
   DDMCalculatorGeographic::DDMForce DDMCalculatorGeographic::DDMForce::FORCE_ENEMY("FORCE_ENEMY", 2);
   DDMCalculatorGeographic::DDMForce DDMCalculatorGeographic::DDMForce::FORCE_NEUTRAL("FORCE_NEUTRAL", 3);

   DDMCalculatorGeographic::DDMForce::DDMForce(const std::string& name, int id) : dtUtil::Enumeration(name)
   {
      AddInstance(this);
      mId = id;
   }

   DDMCalculatorGeographic::DDMCalculatorGeographic():
      mCalculatorObjectKind(&DDMCalculatorGeographic::DDMObjectKind::OBJECT_KIND_ENTITY),

      mFriendlyRegionType(&DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE),
      mEnemyRegionType(&DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE),
      mNeutralRegionType(&DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE),

      mDefaultRegionType(&DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY),

      mFriendlyAppSpace(1),
      mEnemyAppSpace(2),
      mNeutralAppSpace(3)

   {
      mCoordinates.SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::GEODETIC);

      AddProperty(new dtCore::EnumActorProperty<DDMCalculatorGeographic::DDMObjectKind>(PROP_CALCULATOR_OBJECT_KIND, PROP_CALCULATOR_OBJECT_KIND,
            dtCore::EnumActorProperty<DDMCalculatorGeographic::DDMObjectKind>::SetFuncType(this, &DDMCalculatorGeographic::SetCalculatorObjectKind),
            dtCore::EnumActorProperty<DDMCalculatorGeographic::DDMObjectKind>::GetFuncType(this, &DDMCalculatorGeographic::GetCalculatorObjectKind)
            ));

      AddProperty(new dtCore::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>(PROP_FRIENDLY_REGION_TYPE, PROP_FRIENDLY_REGION_TYPE,
            dtCore::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>::SetFuncType(this, &DDMCalculatorGeographic::SetFriendlyRegionType),
            dtCore::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>::GetFuncType(this, &DDMCalculatorGeographic::GetFriendlyRegionType)
            ));
      AddProperty(new dtCore::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>(PROP_ENEMY_REGION_TYPE, PROP_ENEMY_REGION_TYPE,
            dtCore::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>::SetFuncType(this, &DDMCalculatorGeographic::SetEnemyRegionType),
            dtCore::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>::GetFuncType(this, &DDMCalculatorGeographic::GetEnemyRegionType)
            ));
      AddProperty(new dtCore::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>(PROP_NEUTRAL_REGION_TYPE, PROP_NEUTRAL_REGION_TYPE,
            dtCore::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>::SetFuncType(this, &DDMCalculatorGeographic::SetNeutralRegionType),
            dtCore::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>::GetFuncType(this, &DDMCalculatorGeographic::GetNeutralRegionType)
            ));

      AddProperty(new dtCore::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>(PROP_DEFAULT_REGION_TYPE, PROP_DEFAULT_REGION_TYPE,
            dtCore::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>::SetFuncType(this, &DDMCalculatorGeographic::SetDefaultRegionType),
            dtCore::EnumActorProperty<DDMCalculatorGeographic::RegionCalculationType>::GetFuncType(this, &DDMCalculatorGeographic::GetDefaultRegionType)
            ));


      AddProperty(new dtCore::LongActorProperty(PROP_FRIENDLY_APPSPACE, PROP_FRIENDLY_APPSPACE,
            dtCore::LongActorProperty::SetFuncType(this, &DDMCalculatorGeographic::SetFriendlyAppSpace),
            dtCore::LongActorProperty::GetFuncType(this, &DDMCalculatorGeographic::GetFriendlyAppSpace)
            ));
      AddProperty(new dtCore::LongActorProperty(PROP_ENEMY_APPSPACE, PROP_ENEMY_APPSPACE,
            dtCore::LongActorProperty::SetFuncType(this, &DDMCalculatorGeographic::SetEnemyAppSpace),
            dtCore::LongActorProperty::GetFuncType(this, &DDMCalculatorGeographic::GetEnemyAppSpace)
            ));
      AddProperty(new dtCore::LongActorProperty(PROP_NEUTRAL_APPSPACE, PROP_NEUTRAL_APPSPACE,
            dtCore::LongActorProperty::SetFuncType(this, &DDMCalculatorGeographic::SetNeutralAppSpace),
            dtCore::LongActorProperty::GetFuncType(this, &DDMCalculatorGeographic::GetNeutralAppSpace)
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

   void DDMCalculatorGeographic::CreateSubscriptionRegionData(std::vector<dtCore::RefPtr<DDMRegionData> >& toFill) const
   {
      DDMGeographicRegionData* newData;

      if (GetCalculatorObjectKind() == DDMCalculatorGeographic::DDMObjectKind::OBJECT_KIND_ENTITY)
      {
         toFill.resize(3);

         newData = new DDMGeographicRegionData;
         toFill[0] = newData;
         newData->SetForce(DDMCalculatorGeographic::DDMForce::FORCE_FRIENDLY);

         newData = new DDMGeographicRegionData;
         toFill[1] = newData;
         newData->SetForce(DDMCalculatorGeographic::DDMForce::FORCE_NEUTRAL);

         newData = new DDMGeographicRegionData;
         toFill[2] = newData;
         newData->SetForce(DDMCalculatorGeographic::DDMForce::FORCE_ENEMY);
      }
      else
      {
         toFill.resize(1);
         newData = new DDMGeographicRegionData;
         toFill[0] = newData;
         newData->SetForce(DDMCalculatorGeographic::DDMForce::FORCE_NEUTRAL);
      }
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

   std::pair<DDMCalculatorGeographic::RegionCalculationType*, long> DDMCalculatorGeographic::GetAppSpaceValues(DDMCalculatorGeographic::DDMForce& force, DDMCalculatorGeographic::DDMObjectKind& kind) const
   {
      std::pair<DDMCalculatorGeographic::RegionCalculationType*, long> result = std::make_pair(&DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY ,0L);

      if (kind == DDMCalculatorGeographic::DDMObjectKind::OBJECT_KIND_OTHER)
      {
         result = std::make_pair(&GetDefaultRegionType(), GetDefaultAppSpace());
      }
      else if (kind == DDMCalculatorGeographic::DDMObjectKind::OBJECT_KIND_ENTITY)
      {
         if (force == DDMCalculatorGeographic::DDMForce::FORCE_ENEMY)
         {
            result = std::make_pair(&GetEnemyRegionType(), GetEnemyAppSpace());
         }
         else if (force == DDMCalculatorGeographic::DDMForce::FORCE_FRIENDLY)
         {
            result = std::make_pair(&GetFriendlyRegionType(), GetFriendlyAppSpace());
         }
         else if (force == DDMCalculatorGeographic::DDMForce::FORCE_NEUTRAL)
         {
            result = std::make_pair(&GetNeutralRegionType(), GetNeutralAppSpace());
         }
      }

      return result;
   }

}
