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
#include <dtCore/globals.h>

int main(unsigned int argc, char* argv[])
{
   dtCore::SetDataFilePathList( dtCore::GetDeltaRootPath() + "/examples/testRecorder/" + dtCore::GetDeltaDataPathList() );
   dtCore::RefPtr<TestRecorder> app = new TestRecorder( "config.xml" );

   app->Config();
   app->Run();

   return 1;
}
