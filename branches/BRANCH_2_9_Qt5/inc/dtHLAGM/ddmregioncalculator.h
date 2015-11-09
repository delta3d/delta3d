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

#include <dtCore/refptr.h>
#include <dtCore/propertycontainer.h>
#include <dtCore/actorproperty.h>
#include <dtHLAGM/export.h>
#include <map>

namespace dtCore
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
   class DT_HLAGM_EXPORT DDMRegionCalculator : public dtCore::PropertyContainer
   {
      public:
         static const std::string PROP_FIRST_DIMENSION_NAME;
         static const std::string PROP_SECOND_DIMENSION_NAME;
         static const std::string PROP_THIRD_DIMENSION_NAME;

         DDMRegionCalculator();

         // TODO, this should be handled like an actor type and the various sub types should have their own.
         // It would be better to make this actors and be able to load them dynamically from the HLA config.
         static dtCore::RefPtr<dtCore::ObjectType> TYPE;

         const dtCore::ObjectType& GetObjectType() const override;

         /**
          * Sets the name of this instance.
          *
          * @param name the new name
          */
         void SetName(const std::string& name);

         /**
          * Returns the name of this instance.
          *
          * @return the current name
          */
         const std::string& GetName() const;

         /**
          * Creates and returns a pointer to a region data object for this calculator.
          * This is basically a factory method.
          * This will be called on the calculator when it is creating regions for published
          * entities.
          * @return the new region data object.
          */
         virtual dtCore::RefPtr<DDMRegionData> CreateRegionData() const = 0;

         /**
          * Creates and returns a pointer to a region data object for this calculator.
          * This is basically a factory method. This will be called on the calculator
          * when it is creating regions for subscription.  The calculator may handle multiple regions, so
          * it will fill a vector.
          */
         virtual void CreateSubscriptionRegionData(std::vector<dtCore::RefPtr<DDMRegionData> >& toFill) const = 0;

         /**
          * Updates the subscription region
          * @param region the region to update.
          */
         virtual bool UpdateRegionData(DDMRegionData& ddmData) const = 0;

         const std::string& GetFirstDimensionName() const { return mFirstDimensionName; }
         void SetFirstDimensionName(const std::string& newName) { mFirstDimensionName = newName; }

         const std::string& GetSecondDimensionName() const { return mSecondDimensionName; }
         void SetSecondDimensionName(const std::string& newName) { mSecondDimensionName = newName; }

         const std::string& GetThirdDimensionName() const { return mThirdDimensionName; }
         void SetThirdDimensionName(const std::string& newName) { mThirdDimensionName = newName; }

      protected:
         /**
          * This helper method should be called by subclasses when update a region because it
          * will check to see if the dimension actually changed before creates a new one and assigns it, which
          * requires some data copying.  Also it helps one check to see if any of the dimensions changed so that the
          * UpdateRegionData method can know what to return.
          * @return true if the dimension was changed.
          * @param ddmData the region data object to update.
          * @param index the index of the dimension
          * @param name the name of the dimension
          * @param min  the minimum extent value
          * @param max the maximum extent value
          */
         static bool UpdateDimension(DDMRegionData& ddmData, unsigned index, const std::string& name, unsigned int min, unsigned int max);

         std::string GetFirstDimensionNameByCopy() const { return mFirstDimensionName; }
         std::string GetSecondDimensionNameByCopy() const { return mSecondDimensionName; }
         std::string GetThirdDimensionNameByCopy() const { return mThirdDimensionName; }
      private:
         ///< The name of this instance.
         dtUtil::RefString mName;
         std::string mFirstDimensionName;
         std::string mSecondDimensionName;
         std::string mThirdDimensionName;
   };
}

#endif /*DELTA_DDM_REGION_CALCULATOR*/
