#include "dtCore/dt.h"
#include "dtABC/dtabc.h"

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
      : Application( configFile )
   {

      Object* obj = new Object("AH-1W");

      obj->LoadFile("ah-1w.ive");

      Transform transform(0, 50, 0);

      obj->SetTransform(&transform);

      GetScene()->AddDrawable(obj);

      Py_Initialize();

      start();
   }

   /**
   * The run thread.
   */
   virtual void run()
   {
      PyRun_InteractiveLoop(stdin, "???");

      Quit();
   }
};

IMPLEMENT_MANAGEMENT_LAYER(TestPythonApp)

int main( int argc, char **argv )
{
   SetDataFilePathList("..;" + GetDeltaDataPathList());

   TestPythonApp* app = new TestPythonApp( "config.xml" );

   app->Config();
   app->Run();

   delete app;

   return 0;
}
