// pythonbindings.cpp: Python binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>


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


BOOST_PYTHON_MODULE(P51)
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
