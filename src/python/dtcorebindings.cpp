// dtcorebindings.cpp: Python bindings for dtCore library.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>

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
void initRTSMotionModelBindings();
void initODEBodyWrapBindings();
void initShaderParameterBindings();
void initShaderParamFloatBindings();
void initShaderParamIntBindings();
void initShaderParamVec4Bindings();
void initShaderParamTextureBindings();
void initShaderParamTexture2DBindings();
void initShaderManagerBindings();
void initShaderProgramBindings();
void initOSGNodeBindings();


//#ifndef WIN32
void initOSGVec2();
void initOSGVec3();
void initOSGVec4();
void initOSGMatrix();
void initOSGMath();
//#endif //WIN32

void initODE();

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

void initActorPropertyBindings();
void initActorProxyBindings();
void initDataTypeBindings();
void initMapBindings();
void initProjectBindings();

BOOST_PYTHON_MODULE(PyDtCore)
{
   initOSGVec2();
   initOSGVec3();
   initOSGVec4();
   initOSGMatrix();
   initOSGMath();

   initODE();

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
   initRTSMotionModelBindings();

   initODEBodyWrapBindings();
   initShaderParameterBindings();
   initShaderParamFloatBindings();
   initShaderParamIntBindings();
   initShaderParamVec4Bindings();
   initShaderParamTextureBindings();
   initShaderParamTexture2DBindings();
   initShaderManagerBindings();
   initShaderProgramBindings();
   initOSGNodeBindings();

   initActorPropertyBindings();
   initActorProxyBindings();
   initDataTypeBindings();
   initMapBindings();
   initProjectBindings();

}
