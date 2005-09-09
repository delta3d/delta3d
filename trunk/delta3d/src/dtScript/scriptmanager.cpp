#include <dtScript/scriptmanager.h>
#include <osgDB/FileUtils>

using namespace dtScript;

IMPLEMENT_MANAGEMENT_LAYER(ScriptManager)

ScriptManager::ScriptManager()
:  mFileObject(0),
   mThreadState(0)
{
   RegisterInstance( this );

   SetName("ScriptManager");

   Py_Initialize(); // start python thread
}

ScriptManager::~ScriptManager()
{
   if ( mThreadState != 0 )
   {
      Py_EndInterpreter( mThreadState );
   }

   Py_Finalize(); // kill python thread

   DeregisterInstance( this );
}

void ScriptManager::Load( const std::string& filename )
{
   if( mThreadState != 0 )
   {
      Py_EndInterpreter( mThreadState );
   }

   mThreadState = Py_NewInterpreter();

   mFilename = filename;
   mFileObject = PyFile_FromString( const_cast< char* >( mFilename.c_str() ), "r" );
}

void ScriptManager::Run( const std::string& filename )
{ 
   if( osgDB::fileExists( filename ) )
   {
      Load( filename);
   }
   
   try
   {
      PyRun_SimpleFile( PyFile_AsFile( mFileObject ), const_cast< char* >( mFilename.c_str() ) );
   }
   catch (...)
   {
      PyErr_Print();
   }
}
