#include "python/script.h"

using namespace dtScript;

Script::Script()
{
   Py_Initialize();
}

Script::~Script()
{
   Py_Finalize();
}

	   