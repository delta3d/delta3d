// main.cpp : defines the entry point for the console application.

#include "testNetwork.h"
#include <gnelib.h>


using namespace dtCore;

///Supplying a host name as the first argument on the command line will create
///a client and try to connect to that server.
///No parameters on the command line will create a server.
int main(int argc, char *argv[] )
{
   //set data search path to parent directory and delta3d/data
   SetDataFilePathList( GetDeltaRootPath() + "/examples/testNetwork/;" +
                        GetDeltaDataPathList()  );

   dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ALWAYS, "",
      "Usage: testNetwork.exe [hostname]");

   std::string hostName;

   if (argc>1)
   {
      hostName = std::string(argv[1]);
   }

   dtCore::RefPtr<TestNetwork> app = new TestNetwork( hostName, "testnetworkconfig.xml" );

   app->Config(); //configuring the application
   app->Run(); // running the simulation loop

   return 0;
}

