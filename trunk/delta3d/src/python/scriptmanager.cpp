#include "python/scriptmanager.h"

using namespace dtScript;

IMPLEMENT_MANAGEMENT_LAYER(ScriptManager)

ScriptManager::ScriptManager()
: mFileObject(0)
{
   Py_Initialize();
}

ScriptManager::~ScriptManager()
{
   Py_Finalize();
}

void 
ScriptManager::Load( std::string filename )
{
   mFilename = filename;
   mFileObject = PyFile_FromString( const_cast<char*>(mFilename.c_str()), "r" );
}

void 
ScriptManager::Run()
{ 
   PyRun_SimpleFile( PyFile_AsFile(mFileObject), const_cast<char*>(mFilename.c_str()) );
}

void 
ScriptManager::Run( std::string filename )
{ 
   Load( filename );
   Run();
}
