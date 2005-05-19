#include "testSoarx.h"

int main( int argc, char **argv )
{
   dtCore::SetDataFilePathList("..;" + dtCore::GetDeltaDataPathList());
   dtCore::SetNotifyLevel(dtCore::INFO);
   {
      dtCore::RefPtr<TestTerrainApp> app = new TestTerrainApp( "config.xml" );

      app->Config();
      app->Run();
   }

   return 0;
}