#include "python/scriptmanager.h"

using namespace dtScript;

ScriptManager::ScriptManager()
: mFileObject(0), mFilename(0)
{
   Py_Initialize();
}

ScriptManager::~ScriptManager()
{
   Py_Finalize();
}

	   