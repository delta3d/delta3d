#include <fstream>
#include <iostream>
#include "testai.h"
#include <dtCore/globals.h>


//we attempt to take a map name from the command line argument
//if none is present we call the default constructor which has a default map name
int main ( int argc, char *argv[] )
{

   //set data search path to parent delta3d/data
   dtCore::SetDataFilePathList( dtCore::GetDeltaRootPath()+"/data" );
   dtCore::RefPtr<TestAI> app = 0;

   if ( argc != 2 ) 
   {
      app = new TestAI();
   }
   else 
   {
      std::string mapName(argv[1]);
      app = new TestAI(mapName);
   }


   app->Config(); //configuring the application
   app->Run(); // running the simulation loop
}
