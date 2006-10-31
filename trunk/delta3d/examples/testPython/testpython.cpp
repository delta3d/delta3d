#include <dtScript/scriptmanager.h>
#include <dtCore/dt.h>
#include <dtABC/dtabc.h>

using namespace dtCore;
using namespace dtABC;

class TestPythonApp : public Application
{
   DECLARE_MANAGEMENT_LAYER(TestPythonApp)

public:

   TestPythonApp( const std::string& configFile = "config.xml" )
      :  Application( configFile ),
         mScriptManager( 0 )
   {     
      Object* obj = new Object( "UH-1N" );

      obj->LoadFile( "models/uh-1n.ive" );

      Transform transform( 0.0f, 50.0f, 0.0f );
      obj->SetTransform( transform );

      AddDrawable(obj);

      mScriptManager = new dtScript::ScriptManager();
      
      // Pre-load the Python script
      std::string filename = dtCore::FindFileInPathList( "flyhelo.py" );

      if( !filename.empty() )
      {
         mScriptManager->Load( filename );
      }

   }

protected:

   virtual ~TestPythonApp() {}

public:
   
   virtual bool KeyPressed(const dtCore::Keyboard* keyboard, 
                              Producer::KeyboardKey key, 
                              Producer::KeyCharacter character )
   {
      bool verdict(false);
      switch( key )
      {
         case Producer::Key_Escape:
         {
            // Make sure to stop the thread before exiting
            mScriptManager->Stop();
            Quit();
            verdict = true;
            break;
         }
         case Producer::Key_S:
         {
            // Kick off a Python script in its own thread
            mScriptManager->Run();
            verdict = true;
            break;
         }
         default:
         {         
         }
      }

      return verdict;
   }
   
private:

   dtCore::RefPtr< dtScript::ScriptManager > mScriptManager;
};

IMPLEMENT_MANAGEMENT_LAYER(TestPythonApp)

int main()
{
   SetDataFilePathList( GetDeltaRootPath() + "/examples/testPython/;" +
                        GetDeltaDataPathList()  );

   dtCore::RefPtr< TestPythonApp > app = new TestPythonApp( "config.xml" );

   app->Config();
   app->Run();

   return 0;
}
