// dtcorebindings.cpp: Python bindings for dtCore library.
//
//////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>

#include "dt.h"


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
