// dtcharbindings.cpp: Python bindings for dtChar library.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>

#include <dtCore/dt.h>
#include <dtChar/dtchar.h>

void initCharacterBindings();

BOOST_PYTHON_MODULE(PyDtChar)
{
   initCharacterBindings();
}
