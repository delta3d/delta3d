// dtcorebindings.cpp: Python bindings for dtCore library.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"

#include "dt.h"

using namespace boost::python;
using namespace dtCore;

// The individual class bindings

void initBaseBindings();
void initDrawableBindings();
void initCameraBindings();
void initInputDeviceBindings();
void initObjectBindings();
void initJoystickBindings();
void initKeyboardBindings();
void initMouseBindings();
void initPhysicalBindings();
void initSceneBindings();
void initSoundBindings();
void initSystemBindings();
void initTrackerBindings();
void initTransformBindings();
void initTransformableBindings();
void initTripodBindings();
void initWindowBindings();

void NotifyWrap(NotifySeverity ns, const char* msg)
{
   Notify(ns, msg);
}

BOOST_PYTHON_MODULE(dtCore)
{
   def("SetDataFilePathList", SetDataFilePathList);
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
      
   initBaseBindings();
   initSystemBindings();
   initSceneBindings();
   initTransformBindings();
   initTransformableBindings();
   initDrawableBindings();
   initPhysicalBindings();
   initObjectBindings();
   initCameraBindings();
   initTripodBindings();
   initInputDeviceBindings();
   initKeyboardBindings();
   initMouseBindings();
   initJoystickBindings();
   initTrackerBindings();
   initWindowBindings();
   initSoundBindings();
}
