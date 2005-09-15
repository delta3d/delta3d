// main.cpp : defines the entry point for the console application.

#include "testNetwork.h"
#include <gnelib.h>


using namespace dtCore;

int main(int argc, char *argv[] )
{
   //set data search path to parent directory and delta3d/data
   SetDataFilePathList( GetDeltaRootPath() + "/examples/testNetwork/;" +
                        GetDeltaDataPathList()  );


   dtUtil::Log::GetInstance().SetLogLevel(dtUtil::Log::LOG_DEBUG);

   std::string hostName;

   if (argc>1)
   {
      hostName = std::string(argv[1]);
   }

   dtCore::RefPtr<TestNetwork> app = new TestNetwork( hostName, "config.xml" );

   app->Config(); //configuring the application
   app->Run(); // running the simulation loop

   return 0;
}

