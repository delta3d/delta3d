// dtcharbindings.cpp: Python bindings for dtChar library.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"

#include "dtCore/dt.h"
#include "dtChar/dtchar.h"


// The individual class bindings

void initCharacterBindings();


BOOST_PYTHON_MODULE(dtChar)
{
   initCharacterBindings();
}
