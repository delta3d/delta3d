// dtabcbindings.cpp: Python bindings for dtABC library.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>

#include <dtCore/dt.h>
#include <dtABC/dtabc.h>

void initBaseABCBindings();
void initApplicationBindings();
void initProximityTriggerBindings();
void initTriggerBindings();
void initWeatherBindings();
void initWidgetBindings();

BOOST_PYTHON_MODULE(PyDtABC)
{
   initBaseABCBindings();
   initApplicationBindings();
   initProximityTriggerBindings();
   initTriggerBindings();
   initWeatherBindings();
   initWidgetBindings();
}
