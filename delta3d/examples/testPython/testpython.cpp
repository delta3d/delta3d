#include <dtCore/dt.h>
#include <dtABC/dtabc.h>
#include <dtScript/scriptmanager.h>

#include <OpenThreads/Thread>
#include <boost/python/detail/wrap_python.hpp>

using namespace dtCore;
using namespace dtABC;

/**
* The Python test application.
*/
class TestPythonApp : public Application,
   public OpenThreads::Thread
{
   DECLARE_MANAGEMENT_LAYER(TestPythonApp)

public:

   /**
   * Constructor.
   */
   TestPythonApp( std::string configFile = "config.xml" )
      :  Application( configFile ),
         mScriptManager( 0 )
   {

      Object* obj = new Object("UH-1N");

      obj->LoadFile("models/uh-1n.ive");

      Transform transform(0, 50, 0);

      obj->SetTransform(&transform);

      AddDrawable(obj);

      mScriptManager = new dtScript::ScriptManager();

      // Start the interactive python prompt      
      Py_Initialize();
      start();
   }
   
   virtual void KeyPressed(   dtCore::Keyboard* keyboard, 
                              Producer::KeyboardKey key, 
                              Producer::KeyCharacter character )
   {
      switch( key )
      {
         case Producer::Key_Escape:
         {
            Quit();
            break;
         }
         case Producer::Key_S:
         {
            mScriptManager->Run( "../../../examples/testPython/flyhelo.py" );
            break;
         }
         default:
         {
         
         }
      }   
   }
   
   /**
   * The run thread.
   */
   virtual void run()
   {
      PyRun_InteractiveLoop(stdin, "???");

      Quit();
   }
   
   private:
   
      dtCore::RefPtr< dtScript::ScriptManager > mScriptManager;
};

IMPLEMENT_MANAGEMENT_LAYER(TestPythonApp)

int main( int argc, char **argv )
{
   SetDataFilePathList( GetDeltaRootPath() + "/examples/testPython/;" +
                        GetDeltaDataPathList()  );

   TestPythonApp* app = new TestPythonApp( "config.xml" );

   app->Config();
   app->Run();

   delete app;

   return 0;
}
