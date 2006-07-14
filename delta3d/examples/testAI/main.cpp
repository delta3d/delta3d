#include "TestAI.h"
#include <dtCore/globals.h>


int main()
{
   //set data search path to parent delta3d/data
   dtCore::SetDataFilePathList( dtCore::GetDeltaRootPath() + "\\Data");
   std::string pData = dtCore::GetDataFilePathList();
   
   //Instantiate the application and look for the config file
   dtCore::RefPtr<TestAI> app = new TestAI();
   
   app->Config(); //configuring the application
   app->Run(); // running the simulation loop
  

   return 0;

}

