#include "testtrigger.h"
#include <dtCore/globals.h>

int main()
{
   //set data search path to parent directory and delta3d/data
   dtCore::SetDataFilePathList(  dtCore::GetDeltaRootPath() + "/examples/testTrigger/;" +
                                 dtCore::GetDeltaDataPathList()  );
	
   //Instantiate the application and look for the config file
   dtCore::RefPtr<TestTrigger> app = new TestTrigger();
	
   app->Config(); //configuring the application
   app->Run(); // running the simulation loop
   
   return 0;
}
