#include <assert.h>

#include "testAudioApp.h"



int
main( int argc, const char* argv[] )
{
   dtCore::SetDataFilePathList( "..;../../data;../../../data/;" + dtCore::GetDeltaDataPathList() );
   
   testAudioApp*  app   = new testAudioApp("config.xml");
   assert( app );
   

   app->Run();

   delete   app;
   app   = NULL;

   return   0L;
}
