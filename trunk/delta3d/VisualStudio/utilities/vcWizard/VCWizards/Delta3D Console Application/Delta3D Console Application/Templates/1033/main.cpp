// main.cpp : defines the entry point for the console application.

#include "[!output PROJECT_NAME].h"

using namespace dtCore;

int main()
{
   //set data search path to parent directory and delta3d/data
   SetDataFilePathList( "..;" + GetDeltaDataPathList() ); 
	
   //Instantiate the application and look for the config file
   RefPtr<[!output PROJECT_NAME]> app = new [!output PROJECT_NAME]();
	
   app->Config(); //configuring the application
   app->Run(); // running the simulation loop
   
   return 0;

}

