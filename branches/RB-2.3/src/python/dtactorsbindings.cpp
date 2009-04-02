// dtactorsbindings.cpp: Python bindings for dtActors library.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>

void initWaypointVolumeBindings();

BOOST_PYTHON_MODULE(PyDtActors)
{
   initWaypointVolumeBindings();
}
