// dtcorebindings.cpp: Python bindings for dtCore library.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"

#include "dt.h"
#include "globals.h"

using namespace boost::python;
using namespace dtCore;


// The individual class bindings

void initBaseBindings();
void initDrawableBindings();
void initCameraBindings();
void initObjectBindings();
void initSceneBindings();
void initSoundBindings();
void initSystemBindings();
void initTransformBindings();
void initTransformableBindings();
void initWindowBindings();


BOOST_PYTHON_MODULE(dtCore)
{
   def("SetDataFilePathList", SetDataFilePathList);
   
   initBaseBindings();
   initSystemBindings();
   initSceneBindings();
   initTransformBindings();
   initTransformableBindings();
   initDrawableBindings();
   initObjectBindings();
   initCameraBindings();
   initWindowBindings();
   initSoundBindings();
}
