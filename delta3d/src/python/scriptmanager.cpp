#include "python/scriptmanager.h"

using namespace dtScript;

ScriptManager::ScriptManager()
{
   Py_Initialize();
}

ScriptManager::~ScriptManager()
{
   Py_Finalize();
}

	   