#include "p51.h"

#include <OpenThreads/Thread>

#include <boost/python/detail/wrap_python.hpp>

#include "camera.h"
#include "globals.h"
#include "notify.h"
#include "system.h"
#include "pythonbindings.h"

using namespace P51;

/**
 * A system thread class.
 */
class SystemThread : public OpenThreads::Thread
{
   public:

      /**
       * Constructor.
       *
       * @param system the system object
       */
      SystemThread(System* system)
      {
         mSystem = system;
      }

      /**
       * The run thread.
       */
      virtual void run()
      {
         mSystem->Config();

         mSystem->Run();
      }


   private:

      /**
       * The system object.
       */
      System* mSystem;
};

int main( int argc, char **argv )
{
   Window win;
   Scene scene;
   System *sys = System::GetSystem();

   Camera cam;
   cam.SetWindow( &win );
   cam.SetScene( &scene );
   Transform position;
   position.Set(0.f, -50.f, 5.f, 0.f, -10.f, 0.f);
   cam.SetTransform( &position );
   
   //This is where we'll find our files to load
   SetDataFilePathList("../../data/");

   Object obj("cessna");
   obj.LoadFile("cessna.osg");

   position.SetTranslation( 0.f, 0.f, -2.f );
   obj.SetTransform( &position );

   //Add the Objects to the Scene to be rendered
   scene.AddDrawable( &obj );


   // Run in another thread

   SystemThread systemThread(sys);

   systemThread.start();

   
   Py_Initialize();

   initP51();

   // Evaluation loop

   PyRun_InteractiveLoop(stdin, "???");


   sys->Stop();

   systemThread.join();

   return 0;
}