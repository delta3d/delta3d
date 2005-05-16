// main.cpp : defines the entry point for the console application.

#include "testCEG.h"
#include "dtCore/dt.h"
#include "dtABC/dtabc.h"

using namespace dtCore;

int main( int argc, const char* argv[] )
{
   //set data search path to parent directory and delta3d/data
   SetDataFilePathList( "..;" + GetDeltaDataPathList() ); 

   std::string filename = "";
   if (argc > 1)
   {
      Notify(ALWAYS,"Using GUI file %s...",argv[1]);
      filename = argv[1];
   }

   //Instantiate the application and look for the config file
   RefPtr<TestCEGApp> app = new TestCEGApp();
	app->SetLayoutFilename(filename);

   app->Config(); //configuring the application
   app->Run(); // running the simulation loop
   
   return 0;
}
