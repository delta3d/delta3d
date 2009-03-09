// main.cpp : defines the entry point for the console application.

#include "[!output PROJECT_NAME].h"
#include <dtCore/globals.h>

int main()
{
   //set data search path to parent directory and delta3d/data
   dtCore::SetDataFilePathList( "..;" + dtCore::GetDeltaDataPathList() ); 
	
   //Instantiate the application and look for the config file
   dtCore::RefPtr<[!output PROJECT_NAME]> app = new [!output PROJECT_NAME]();
	
   app->Config(); //configuring the application
   app->Run(); // running the simulation loop
   
   return 0;

}

