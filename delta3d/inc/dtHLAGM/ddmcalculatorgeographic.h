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
#ifndef DELTA_DDMCALCULATORGEOGRAPHIC
#define DELTA_DDMCALCULATORGEOGRAPHIC

#include <dtHLAGM/export.h>
#include <dtHLAGM/ddmregioncalculator.h>
#include <dtUtil/enumeration.h>
#include <dtUtil/coordinates.h>

namespace dtHLAGM
{

   class DT_HLAGM_EXPORT DDMCalculatorGeographic : public dtHLAGM::DDMRegionCalculator
   {
      public:
         typedef dtHLAGM::DDMRegionCalculator BaseClass;

         static const std::string PROP_CALCULATOR_ENTITY_KIND;
         ;
         static const std::string PROP_FRIENDLY_GROUND_REGION_TYPE;
         static const std::string PROP_ENEMY_GROUND_REGION_TYPE;
         static const std::string PROP_NEUTRAL_GROUND_REGION_TYPE;

         static const std::string PROP_FRIENDLY_AIR_REGION_TYPE;
         static const std::string PROP_ENEMY_AIR_REGION_TYPE;
         static const std::string PROP_NEUTRAL_AIR_REGION_TYPE;

         static const std::string PROP_FRIENDLY_SEA_REGION_TYPE;
         static const std::string PROP_ENEMY_SEA_REGION_TYPE;
         static const std::string PROP_NEUTRAL_SEA_REGION_TYPE;
         
         static const std::string PROP_FRIENDLY_LIFEFORM_REGION_TYPE;
         static const std::string PROP_ENEMY_LIFEFORM_REGION_TYPE;
         static const std::string PROP_NEUTRAL_LIFEFORM_REGION_TYPE;

         static const std::string PROP_FRIENDLY_GROUND_APPSPACE;
         static const std::string PROP_ENEMY_GROUND_APPSPACE;
         static const std::string PROP_NEUTRAL_GROUND_APPSPACE;

         static const std::string PROP_FRIENDLY_AIR_APPSPACE;
         static const std::string PROP_ENEMY_AIR_APPSPACE;
         static const std::string PROP_NEUTRAL_AIR_APPSPACE;

         static const std::string PROP_FRIENDLY_SEA_APPSPACE;
         static const std::string PROP_ENEMY_SEA_APPSPACE;
         static const std::string PROP_NEUTRAL_SEA_APPSPACE;
         
         static const std::string PROP_FRIENDLY_LIFEFORM_APPSPACE;
         static const std::string PROP_ENEMY_LIFEFORM_APPSPACE;
         static const std::string PROP_NEUTRAL_LIFEFORM_APPSPACE;

         static const std::string PROP_APP_SPACE_MAX;
         static const std::string PROP_APP_SPACE_MIN;
         
         class DT_HLAGM_EXPORT RegionCalculationType : public dtUtil::Enumeration
         {
            DECLARE_ENUM(RegionCalculationType);
            public:
               static RegionCalculationType GEOGRAPHIC_SPACE;
               static RegionCalculationType APP_SPACE_ONLY;
            protected:
               RegionCalculationType(const std::string &name);

         };

         class DT_HLAGM_EXPORT DDMForce : public dtUtil::Enumeration
         {
            DECLARE_ENUM(DDMForce);
            public:
               static DDMForce FORCE_FRIENDLY;
               static DDMForce FORCE_ENEMY;
               static DDMForce FORCE_NEUTRAL;
               
               int GetId() const { return mId; }
            protected:
               DDMForce(const std::string& name, int id);
            private:
               int mId;
         };
         
         class DT_HLAGM_EXPORT DDMEntityKind : public dtUtil::Enumeration
         {
            DECLARE_ENUM(DDMEntityKind);
            public:
               static DDMEntityKind ENTITY_KIND_GROUND;
               static DDMEntityKind ENTITY_KIND_AIR;
               static DDMEntityKind ENTITY_KIND_SEA;
               static DDMEntityKind ENTITY_KIND_LIFEFORM;
            protected:
               DDMEntityKind(const std::string& name);

         };
         
         DDMCalculatorGeographic();

         virtual dtCore::RefPtr<DDMRegionData> CreateRegionData() const;
         virtual void CreateSubscriptionRegionData(std::vector<dtCore::RefPtr<DDMRegionData> >& toFill) const;
         
         /**
          * Updates the subscription region 
          * @param region the region to update.
          */
         virtual bool UpdateRegionData(DDMRegionData& ddmData) const;
                 
         void SetCoordinateConverter(const dtUtil::Coordinates& coord);
         dtUtil::Coordinates& GetCoordinateConverter() { return mCoordinates; }
         const dtUtil::Coordinates& GetCoordinateConverter() const { return mCoordinates; }
         
         DDMEntityKind& GetCalculatorEntityKind() const { return *mCalculatorEntityKind; }
         void SetCalculatorEntityKind(DDMEntityKind& newKind)   { mCalculatorEntityKind = &newKind; }
         
         RegionCalculationType& GetFriendlyGroundRegionType() const { return *mFriendlyGroundRegionType; }
         void SetFriendlyGroundRegionType(RegionCalculationType& newType)   { mFriendlyGroundRegionType = &newType; }

         RegionCalculationType& GetFriendlyAirRegionType() const { return *mFriendlyAirRegionType; }
         void SetFriendlyAirRegionType(RegionCalculationType& newType)      { mFriendlyAirRegionType = &newType; }

         RegionCalculationType& GetFriendlySeaRegionType() const { return *mFriendlySeaRegionType; }
         void SetFriendlySeaRegionType(RegionCalculationType& newType)      { mFriendlySeaRegionType = &newType; }

         RegionCalculationType& GetFriendlyLifeformRegionType() const { return *mFriendlyLifeformRegionType; }
         void SetFriendlyLifeformRegionType(RegionCalculationType& newType) { mFriendlyLifeformRegionType = &newType; }

         
         RegionCalculationType& GetEnemyGroundRegionType() const { return *mEnemyGroundRegionType; }
         void SetEnemyGroundRegionType(RegionCalculationType& newType)   { mEnemyGroundRegionType = &newType; }

         RegionCalculationType& GetEnemyAirRegionType() const { return *mEnemyAirRegionType; }
         void SetEnemyAirRegionType(RegionCalculationType& newType)      { mEnemyAirRegionType = &newType; }
         
         RegionCalculationType& GetEnemySeaRegionType() const { return *mEnemySeaRegionType; }
         void SetEnemySeaRegionType(RegionCalculationType& newType)      { mEnemySeaRegionType = &newType; }

         RegionCalculationType& GetEnemyLifeformRegionType() const { return *mEnemyLifeformRegionType; }
         void SetEnemyLifeformRegionType(RegionCalculationType& newType) { mEnemyLifeformRegionType = &newType; }


         RegionCalculationType& GetNeutralGroundRegionType() const { return *mNeutralGroundRegionType; }
         void SetNeutralGroundRegionType(RegionCalculationType& newType)   { mNeutralGroundRegionType = &newType; }

         RegionCalculationType& GetNeutralAirRegionType() const { return *mNeutralAirRegionType; }
         void SetNeutralAirRegionType(RegionCalculationType& newType)      { mNeutralAirRegionType = &newType; }

         RegionCalculationType& GetNeutralSeaRegionType() const { return *mNeutralSeaRegionType; }
         void SetNeutralSeaRegionType(RegionCalculationType& newType)      { mNeutralSeaRegionType = &newType; }

         RegionCalculationType& GetNeutralLifeformRegionType() const { return *mNeutralLifeformRegionType; }
         void SetNeutralLifeformRegionType(RegionCalculationType& newType) { mNeutralLifeformRegionType = &newType; }

         
         
         int GetFriendlyGroundAppSpace() const { return mFriendlyGroundAppSpace; }
         void SetFriendlyGroundAppSpace(int newSpace)   { mFriendlyGroundAppSpace = newSpace; }

         int GetFriendlyAirAppSpace() const { return mFriendlyAirAppSpace; }
         void SetFriendlyAirAppSpace(int newSpace)      { mFriendlyAirAppSpace = newSpace; }

         int GetFriendlySeaAppSpace() const { return mFriendlySeaAppSpace; }
         void SetFriendlySeaAppSpace(int newSpace)      { mFriendlySeaAppSpace = newSpace; }

         int GetFriendlyLifeformAppSpace() const { return mFriendlyLifeformAppSpace; }
         void SetFriendlyLifeformAppSpace(int newSpace) { mFriendlyLifeformAppSpace = newSpace; }

         
         int GetEnemyGroundAppSpace() const { return mEnemyGroundAppSpace; }
         void SetEnemyGroundAppSpace(int newSpace)   { mEnemyGroundAppSpace = newSpace; }

         int GetEnemyAirAppSpace() const { return mEnemyAirAppSpace; }
         void SetEnemyAirAppSpace(int newSpace)      { mEnemyAirAppSpace = newSpace; }
         
         int GetEnemySeaAppSpace() const { return mEnemySeaAppSpace; }
         void SetEnemySeaAppSpace(int newSpace)      { mEnemySeaAppSpace = newSpace; }

         int GetEnemyLifeformAppSpace() const { return mEnemyLifeformAppSpace; }
         void SetEnemyLifeformAppSpace(int newSpace) { mEnemyLifeformAppSpace = newSpace; }


         int GetNeutralGroundAppSpace() const { return mNeutralGroundAppSpace; }
         void SetNeutralGroundAppSpace(int newSpace)   { mNeutralGroundAppSpace = newSpace; }

         int GetNeutralAirAppSpace() const { return mNeutralAirAppSpace; }
         void SetNeutralAirAppSpace(int newSpace)      { mNeutralAirAppSpace = newSpace; }

         int GetNeutralSeaAppSpace() const { return mNeutralSeaAppSpace; }
         void SetNeutralSeaAppSpace(int newSpace)      { mNeutralSeaAppSpace = newSpace; }

         int GetNeutralLifeformAppSpace() const { return mNeutralLifeformAppSpace; }
         void SetNeutralLifeformAppSpace(int newSpace) { mNeutralLifeformAppSpace = newSpace; }
         
         /// @return the minimum encoded app space value.  It defaults to 0
         long GetAppSpaceMinimum() const { return mAppSpaceMin; }
         void SetAppSpaceMinimum(long newMin) { mAppSpaceMin = newMin; }
         
         /// @return the maximum encoded app space value.  It defaults to 99
         long GetAppSpaceMaximum() const { return mAppSpaceMax; }
         void SetAppSpaceMaximum(long newMax) { mAppSpaceMax = newMax; }
         
         unsigned long MapAppSpaceValue(unsigned spaceNumber) const;
         std::pair<RegionCalculationType*, int> GetAppSpaceValues(DDMForce& force, DDMEntityKind& kind) const;
      protected:
         virtual ~DDMCalculatorGeographic();
         
         DDMEntityKind* mCalculatorEntityKind;
         
         RegionCalculationType* mFriendlyGroundRegionType;
         RegionCalculationType* mFriendlyAirRegionType;
         RegionCalculationType* mFriendlySeaRegionType;
         RegionCalculationType* mFriendlyLifeformRegionType;
         
         RegionCalculationType* mEnemyGroundRegionType;
         RegionCalculationType* mEnemyAirRegionType;
         RegionCalculationType* mEnemySeaRegionType;
         RegionCalculationType* mEnemyLifeformRegionType;

         RegionCalculationType* mNeutralGroundRegionType;
         RegionCalculationType* mNeutralAirRegionType;
         RegionCalculationType* mNeutralSeaRegionType;
         RegionCalculationType* mNeutralLifeformRegionType;

         int mFriendlyGroundAppSpace;
         int mFriendlyAirAppSpace;
         int mFriendlySeaAppSpace;
         int mFriendlyLifeformAppSpace;
         
         int mEnemyGroundAppSpace;
         int mEnemyAirAppSpace;
         int mEnemySeaAppSpace;
         int mEnemyLifeformAppSpace;

         int mNeutralGroundAppSpace;
         int mNeutralAirAppSpace;
         int mNeutralSeaAppSpace;
         int mNeutralLifeformAppSpace;

         long mAppSpaceMin;
         long mAppSpaceMax;
         
         //This is a bit of a hack, but some of the methods to do conversions are non-const.
         mutable dtUtil::Coordinates mCoordinates;
   };

}

#endif /* DELTA_DDMCALCULATORGEOGRAPHIC*/
