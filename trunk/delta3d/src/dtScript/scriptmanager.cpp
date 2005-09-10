#include <dtScript/scriptmanager.h>
#include <osgDB/FileUtils>

using namespace dtScript;

IMPLEMENT_MANAGEMENT_LAYER(ScriptManager)

ScriptManager::ScriptManager( const std::string& name )
:  Base( name ),
   mFileObject( 0 )
{
   RegisterInstance( this );

   Py_Initialize();
}

ScriptManager::~ScriptManager()
{
   DeregisterInstance( this );
}

void ScriptManager::Load( const std::string& filename )
{
   mFilename = filename;
   mFileObject = PyFile_FromString( const_cast< char* >( mFilename.c_str() ), "r" );
}

void ScriptManager::Run( const std::string& filename )
{ 
   if( osgDB::fileExists( filename ) )
   {
      Load( filename );
   }

   start();
}

void ScriptManager::run()
{
   try
   {
      PyRun_SimpleFile( PyFile_AsFile( mFileObject ), const_cast< char* >( mFilename.c_str() ) );
   }
   catch (...)
   {
      PyErr_Print();
   }
}
