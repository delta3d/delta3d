#include "testsoundapp.h"
#include "globals.h"


/// A simple application that demonstrates the most basic methods
/// for managing sounds.  This application just loads sounds and
/// plays them.  It doesn't demonstrate more advanced functions.
/// See the testSoundApp class for more information.
int
main( int argc, const char* argv[] )
{
   // set the directory to find the sound files & config.xml
   dtCore::SetDataFilePathList("..;../../data;" + dtCore::GetDeltaDataPathList());

   testSoundApp   app;
   app.Run();

   return   0L;
}
