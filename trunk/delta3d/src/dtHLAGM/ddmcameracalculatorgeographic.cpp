#include <dtHLAGM/ddmcameracalculatorgeographic.h>
#include <dtHLAGM/ddmgeographicregiondata.h>
#include <dtHLAGM/ddmregiondata.h>
#include <dtHLAGM/ddmutil.h>
#include <dtCore/camera.h>
#include <dtCore/transform.h>

#include <osg/Matrix>
#include <osg/Vec3>
#include <osg/Vec3d>

namespace dtHLAGM
{
   //////////////////////////////////////////////////////////////
   DDMCameraCalculatorGeographic::DDMCameraCalculatorGeographic()
   {
   }
   
   //////////////////////////////////////////////////////////////
   DDMCameraCalculatorGeographic::~DDMCameraCalculatorGeographic()
   {
   }
   
   //////////////////////////////////////////////////////////////
   bool DDMCameraCalculatorGeographic::UpdateRegionData(DDMRegionData& ddmData) const
   {
      if (!BaseClass::UpdateRegionData(ddmData))
         return false;
      
      if (!mCamera.valid())
         return false;

      
      dtCore::Transform xform;
      mCamera->GetTransform(xform, dtCore::Transformable::ABS_CS);
      
      const osg::Vec3& pos = xform.GetTranslation();
      
      const osg::Vec3d latLonElev = mCoordinates.ConvertToRemoteTranslation(pos);
      
      DDMGeographicRegionData* data = static_cast<DDMGeographicRegionData*>(&ddmData);

      const DDMRegionData::DimensionValues* dv = data->GetDimensionValue(0);
      
      if (dv == NULL)
      {
         DDMRegionData::DimensionValues subspacedv;   
         subspacedv.mName = GetFirstDimensionName();
         subspacedv.mMin = MapAppSpaceValue(GetAppSpaceMinimum());
         subspacedv.mMax = MapAppSpaceValue(GetAppSpaceMaximum());
         data->SetDimensionValue(0, subspacedv);
      }
           
      DDMRegionData::DimensionValues onedv;   
      onedv.mName = GetSecondDimensionName();
      onedv.mMin = DDMUtil::MapLinear(latLonElev.x() - 0.20 , -75.0, 75.0);
      onedv.mMax = DDMUtil::MapLinear(latLonElev.x() + 0.20, -75.0, 75.0);
      data->SetDimensionValue(1, onedv);

      DDMRegionData::DimensionValues twodv;   
      twodv.mName = GetThirdDimensionName();
      
      double lon = latLonElev.y();
      if (lon < 0)
         lon += 360.0;
      else if (lon > 360.0)
         lon -= 360.0;
      
      twodv.mMin = DDMUtil::MapLinear(lon - 0.20, 0.0, 360.0);
      twodv.mMax = DDMUtil::MapLinear(lon + 0.20, 0.0, 360.0);
      data->SetDimensionValue(2, twodv);

      return true;
   }

   //////////////////////////////////////////////////////////////
   void DDMCameraCalculatorGeographic::SetCamera(dtCore::Camera* camera)
   {
      mCamera = camera;
   }

   //////////////////////////////////////////////////////////////
   dtCore::Camera* DDMCameraCalculatorGeographic::GetCamera()
   {
      return mCamera.get();
   }
}
