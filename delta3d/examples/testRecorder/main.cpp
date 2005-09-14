/** \author John K. Grant
  * \date August 26, 2005
  * \file main.cpp
  * \brief An example for using dtCore::Recorder
  * This example shows how to create a Recorder,
  * dump an XML file with results of your recording,
  * and also read back the recorded information for a replay.
  */

#include "testrecorder.h"
#include <dtUtil/log.h>

int main(unsigned int argc, char* argv[])
{
   dtUtil::Log::GetInstance().SetLogLevel( dtUtil::Log::LOG_DEBUG );

   dtCore::RefPtr<TestRecorder> app(0);
   if( argc > 1 )
      app = new TestRecorder( argv[1] );
   else
      app = new TestRecorder( dtABC::Application::GenerateDefaultConfigFile() );

   app->Config();
   app->Run();

   return 1;
}
