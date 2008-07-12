// dtcorebindings.cpp: Python bindings for dtCore library.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/notify.h>
#include <dtCore/globals.h>

using namespace boost::python;
using namespace dtCore;

void initBaseBindings();
void initCameraBindings();
void initCloudDomeBindings();
void initCloudPlaneBindings();
void initCompassBindings();
void initDeltaDrawableBindings();
void initDeltaWinBindings();
void initEffectManagerBindings();
void initEnvEffectBindings();
void initEnvironmentBindings();
void initFlyMotionModelBindings();
void initFPSMotionModelBindings();
void initInfiniteLightBindings();
void initInfiniteTerrainBindings();
void initInputDeviceBindings();
void initInputMapperBindings();
void initIsectorBindings();
void initKeyboardBindings();
void initLightBindings();
void initLoadableBindings();
void initLogicalInputDeviceBindings();
void initMotionModelBindings();
void initMouseBindings();
void initObjectBindings();
void initOrbitMotionModelBindings();
void initCollisionMotionModelBindings();
void initViewBindings();


//#ifndef WIN32
void initOSGVec2();
void initOSGVec3();
void initOSGVec4();
void initOSGMatrix();
void initOSGMath();
//#endif //WIN32

void initParticleSystemBindings();
void initPhysicalBindings();
void initPointAxisBindings();
void initPositionalLightBindings();
//void initRecorderBindings();
void initSceneBindings();
void initSpotLightBindings();
void initSkyBoxBindings();
void initSkyDomeBindings();
void initSystemBindings();
void initTransformBindings();
void initTransformableBindings();
void initTripodBindings();
void initUFOMotionModelBindings();
void initWalkMotionModelBindings();
void initFPSColliderBindings();


void NotifyWrap(NotifySeverity ns, const char* msg)
{
   Notify(ns, msg);
}

BOOST_PYTHON_MODULE(PyDtCore)
{
   def("SetDataFilePathList", SetDataFilePathList);
   def("GetDataFilePathList", GetDataFilePathList);
   def("GetDeltaDataPathList", GetDeltaDataPathList);
   def("GetDeltaRootPath", GetDeltaRootPath);
   def("GetEnvironment", GetEnvironment);

   def("SetNotifyLevel", SetNotifyLevel);
   def("Notify", NotifyWrap);
   
   enum_<NotifySeverity>("NotifySeverity")
      .value("ALWAYS", ALWAYS)
      .value("FATAL", FATAL)
      .value("WARN", WARN)
      .value("NOTICE", NOTICE)
      .value("INFO", INFO)
      .value("DEBUG_INFO", DEBUG_INFO)
      .export_values();


   initOSGVec2();
   initOSGVec3();
   initOSGVec4();
   initOSGMatrix();
   initOSGMath();

   initBaseBindings();
   initSystemBindings();
   initSceneBindings();
   initTransformBindings();
   initLoadableBindings();

   initDeltaDrawableBindings();
   initTransformableBindings();

   initEnvEffectBindings();
   initCloudDomeBindings();
   initCloudPlaneBindings();
   initSkyBoxBindings();
   initSkyDomeBindings();
   initEnvironmentBindings();

   initPhysicalBindings();
   initObjectBindings();

   initCameraBindings();
   initTripodBindings();
   initInputDeviceBindings();
   initKeyboardBindings();
   initMouseBindings();
   initLogicalInputDeviceBindings();
   initInputMapperBindings();
   initDeltaWinBindings();
   initParticleSystemBindings();
   initIsectorBindings();
   initInfiniteTerrainBindings();

   initMotionModelBindings();
   initWalkMotionModelBindings();
   initFlyMotionModelBindings();
   initUFOMotionModelBindings();
   initOrbitMotionModelBindings();
   initCollisionMotionModelBindings();

   initFPSColliderBindings();

   initCompassBindings();
   initPointAxisBindings();
   //initRecorderBindings();
   initEffectManagerBindings();

   initLightBindings();
   initInfiniteLightBindings();
   initPositionalLightBindings();
   initSpotLightBindings();

   initViewBindings();
}
