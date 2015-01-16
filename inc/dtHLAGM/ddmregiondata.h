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
#ifndef DELTA_DDM_REGION_DATA
#define DELTA_DDM_REGION_DATA

#include <dtHLAGM/rtiregion.h>

#include <dtHLAGM/export.h>
#include <osg/Referenced>

#include <dtCore/refptr.h>

#include <vector>

namespace dtHLAGM
{
   /**
    * @brief An abstract class that will be extended by DDMRegionCalculator implementations
    *        to store object instance data used when updating the region for a given
    *        object.
    * 
    * This abstract exists because calculating the region uses data in attributes that
    * may not be updated every time an update message is received, but the values are required
    * to update the region.  
    * 
    * This class is not referenced because the region this class holds onto must be manually 
    * deleted by the rti ambassador.  At that point the instance is no longer valid and should
    * be deleted itself.  Since deletion can't be guaranteed with a refptr, is not referenced.
    */
   class DT_HLAGM_EXPORT DDMRegionData: public osg::Referenced
   {
      public:
         
         class DT_HLAGM_EXPORT DimensionValues
         {
            public:
               DimensionValues():mMin(), mMax() {}
               std::string mName;
               unsigned int mMin;
               unsigned int mMax;
               
               bool operator==(const DimensionValues& toComp) const;
               
         };
         
         DDMRegionData(): mRegion() {}
         
         /// Sets the region associated with this data class.
         void SetRegion(RTIRegion* region) { mRegion = region; }
         
         /// @return the region this data object supports. 
         RTIRegion* GetRegion() { return mRegion; }

         /// @return the region this data object supports. 
         const RTIRegion* GetRegion() const { return mRegion; }
         
         /// @return the number of dimensions/extents for this class.
         unsigned GetNumberOfExtents() const { return mDimensions.size(); }
         
         void SetDimensionValue(unsigned extent, const DimensionValues& dimVal);
         const DimensionValues* GetDimensionValue(unsigned extent) const;

         void ClearDimensions() { mDimensions.clear(); }
      protected:
         virtual ~DDMRegionData() {};
         
      private:
         //Note that this region must be deleted by the RTI.
         dtCore::RefPtr<RTIRegion> mRegion;
         std::vector<DimensionValues> mDimensions;
   };
}

#endif /*DELTA_DDM_REGION_DATA*/
