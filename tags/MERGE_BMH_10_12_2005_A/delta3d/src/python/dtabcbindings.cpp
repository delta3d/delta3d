// dtabcbindings.cpp: Python bindings for dtABC library.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>

#include <dtCore/dt.h>
#include <dtABC/dtabc.h>

void initBaseABCBindings();
void initApplicationBindings();
void initWeatherBindings();
void initWidgetBindings();

BOOST_PYTHON_MODULE(PyDtABC)
{
   initBaseABCBindings();
   initApplicationBindings();
   initWeatherBindings();
   initWidgetBindings();
}
