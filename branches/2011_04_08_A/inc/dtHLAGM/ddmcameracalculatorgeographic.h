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
#ifndef DELTA_DDM_CAMERA_GEOGRAPHIC_CALCULATOR
#define DELTA_DDM_CAMERA_GEOGRAPHIC_CALCULATOR

#include <dtCore/refptr.h>
#include <dtHLAGM/export.h>
#include <dtHLAGM/ddmcalculatorgeographic.h>

namespace dtCore
{
   class Camera;
}

namespace dtHLAGM
{
   /**
    * This calculator is intended for generating subscription regions based on the view of the camera.
    */
   class DT_HLAGM_EXPORT DDMCameraCalculatorGeographic : public dtHLAGM::DDMCalculatorGeographic
   {
      public:
         
         typedef dtHLAGM::DDMCalculatorGeographic BaseClass;
         
         static const std::string PROP_X_SUBSCRIPTION_RANGE;
         static const std::string PROP_Y_SUBSCRIPTION_RANGE;
         static const std::string PROP_MIN_TIME_BETWEEN_UPDATES;

         DDMCameraCalculatorGeographic();
         
         /// Sets the camera this calculator is using for generating the region.
         void SetCamera(dtCore::Camera* camera);

         /// @return the camera this calculator is using for generating the region.
         dtCore::Camera* GetCamera();
                  
         /**
          * Updates the subscription region 
          * @param region the region to update.
          */
         virtual bool UpdateRegionData(DDMRegionData& ddmData) const;

         /// @return the length in the X direction around the camera the calculator should subscribe to.
         float GetXSubscriptionRange() const { return mXRange; }
         /// Sets  the length in the X direction the camera the calculator should subscribe to.
         void SetXSubscriptionRange(float range);

         /// @return the length in the Y direction around the camera the calculator should subscribe to.
         float GetYSubscriptionRange() const { return mYRange; }
         /// Sets  the length in the Y direction the camera the calculator should subscribe to.
         void SetYSubscriptionRange(float range);
         
         /// @return the minimum time between updates.
         float GetMinTimeBetweenUpdates() const { return mMinTimeBetweenUpdates; }
         /// sets the minimum time between updates.
         void SetMinTimeBetweenUpdates(float minTime);
         
      protected:
         virtual ~DDMCameraCalculatorGeographic();
         
         double FixLongitudeRange(double longitude) const;

      private:
         dtCore::RefPtr<dtCore::Camera> mCamera;
         float mXRange;
         float mYRange;
         float mMinTimeBetweenUpdates;
   };
}

#endif /*DELTA_DDM_CAMERA_GEOGRAPHIC_CALCULATOR*/
