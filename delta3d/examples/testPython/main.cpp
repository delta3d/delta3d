#include "dt.h"
#include "dtabc.h"

#include <OpenThreads/Thread>

#include <boost/python/detail/wrap_python.hpp>

#include "application.h"
#include "camera.h"
#include "globals.h"
#include "notify.h"
#include "system.h"

using namespace dtCore;
using namespace dtABC;


/**
 * The Python test application.
 */
class TestPythonApplication : public Application,
                              public OpenThreads::Thread
{
   DECLARE_MANAGEMENT_LAYER(TestPythonApplication)
   
   public:
   
      /**
       * Constructor.
       */
      TestPythonApplication()
         : Application("config.xml")
      {
         SetDataFilePathList("../../data;" + GetDeltaDataPathList());
         
         Object* obj = new Object("cessna");
         
         obj->LoadFile("cessna.osg");
         
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

IMPLEMENT_MANAGEMENT_LAYER(TestPythonApplication)

int main( int argc, char **argv )
{
   TestPythonApplication* testPythonApp = new TestPythonApplication;
   
   testPythonApp->Config();
   testPythonApp->Run();
   
   return 0;
}
