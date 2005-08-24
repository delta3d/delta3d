#include "testSoarx.h"

int main( int argc, char **argv )
{
   dtCore::SetDataFilePathList( dtCore::GetDeltaRootPath() + "/examples/testSoarx/;" +
                                dtCore::GetDeltaDataPathList()  );
   
   {
      dtCore::RefPtr<TestTerrainApp> app = new TestTerrainApp( "config.xml" );

      app->Config();
      app->Run();
   }

   return 0;
}