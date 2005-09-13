// main.cpp : defines the entry point for the console application.

#include "testNetwork.h"

using namespace dtCore;

int main()
{
   //set data search path to parent directory and delta3d/data
   SetDataFilePathList( GetDeltaRootPath() + "/examples/testNetwork/;" +
                        GetDeltaDataPathList()  );

   //Instantiate the application and look for the config file
   dtCore::RefPtr<testNetwork> app = new testNetwork( "testnetworkconfig.xml" );
	
   app->Config(); //configuring the application
   app->Run(); // running the simulation loop
   
   return 0;

}

