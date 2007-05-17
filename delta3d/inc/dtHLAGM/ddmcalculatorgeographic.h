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
#include <dtHLAGM/ddmappspacecalculator.h>
#include <dtUtil/enumeration.h>
#include <dtUtil/coordinates.h>

namespace dtHLAGM
{

   class DT_HLAGM_EXPORT DDMCalculatorGeographic : public dtHLAGM::DDMAppSpaceCalculator
   {
      public:
         typedef dtHLAGM::DDMAppSpaceCalculator BaseClass;

         static const std::string PROP_CALCULATOR_OBJECT_KIND;
         
         static const std::string PROP_FRIENDLY_REGION_TYPE;
         static const std::string PROP_ENEMY_REGION_TYPE;
         static const std::string PROP_NEUTRAL_REGION_TYPE;

         static const std::string PROP_DEFAULT_REGION_TYPE;
         
         static const std::string PROP_FRIENDLY_APPSPACE;
         static const std::string PROP_ENEMY_APPSPACE;
         static const std::string PROP_NEUTRAL_APPSPACE;
         
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
         
         class DT_HLAGM_EXPORT DDMObjectKind : public dtUtil::Enumeration
         {
            DECLARE_ENUM(DDMObjectKind);
            public:
               static DDMObjectKind OBJECT_KIND_ENTITY;
               static DDMObjectKind OBJECT_KIND_OTHER;
            protected:
               DDMObjectKind(const std::string& name);

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
         
         DDMObjectKind& GetCalculatorObjectKind() const { return *mCalculatorObjectKind; }
         void SetCalculatorObjectKind(DDMObjectKind& newKind)   { mCalculatorObjectKind = &newKind; }
         
         RegionCalculationType& GetFriendlyRegionType() const { return *mFriendlyRegionType; }
         void SetFriendlyRegionType(RegionCalculationType& newType)   { mFriendlyRegionType = &newType; }
         
         RegionCalculationType& GetEnemyRegionType() const { return *mEnemyRegionType; }
         void SetEnemyRegionType(RegionCalculationType& newType)   { mEnemyRegionType = &newType; }

         RegionCalculationType& GetNeutralRegionType() const { return *mNeutralRegionType; }
         void SetNeutralRegionType(RegionCalculationType& newType)   { mNeutralRegionType = &newType; }

         RegionCalculationType& GetDefaultRegionType() const { return *mDefaultRegionType; }
         void SetDefaultRegionType(RegionCalculationType& newType) { mDefaultRegionType = &newType; }

         long GetFriendlyAppSpace() const { return mFriendlyAppSpace; }
         void SetFriendlyAppSpace(long newSpace)   { mFriendlyAppSpace = newSpace; }
         
         long GetEnemyAppSpace() const { return mEnemyAppSpace; }
         void SetEnemyAppSpace(long newSpace)   { mEnemyAppSpace = newSpace; }

         long GetNeutralAppSpace() const { return mNeutralAppSpace; }
         void SetNeutralAppSpace(long newSpace)   { mNeutralAppSpace = newSpace; }
         
         std::pair<RegionCalculationType*, long> GetAppSpaceValues(DDMForce& force, DDMObjectKind& kind) const;
      protected:
         virtual ~DDMCalculatorGeographic();
         
         DDMObjectKind* mCalculatorObjectKind;
         
         RegionCalculationType* mFriendlyRegionType;
         RegionCalculationType* mEnemyRegionType;
         RegionCalculationType* mNeutralRegionType;

         RegionCalculationType* mDefaultRegionType;

         long mFriendlyAppSpace;
         long mEnemyAppSpace;
         long mNeutralAppSpace;

         //This is a bit of a hack, but some of the methods to do conversions are non-const.
         mutable dtUtil::Coordinates mCoordinates;
   };

}

#endif /* DELTA_DDMCALCULATORGEOGRAPHIC*/
