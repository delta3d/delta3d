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

      protected:
         virtual ~DDMCameraCalculatorGeographic();
         
      private:
         dtCore::RefPtr<dtCore::Camera> mCamera;
   };
}

#endif /*DELTA_DDM_CAMERA_GEOGRAPHIC_CALCULATOR*/
