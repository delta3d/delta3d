#pragma  warning( disable : 4311 )  // casting void*->long warning
#pragma  warning( disable : 4312 )  // casting long->void* warning

#include "dt.h"
#include "dtabc.h"
#include <assert.h>

#include "UserInterface.h"



int
main( int argc, char** argv )
{
   UserInterface* app   = new UserInterface( argc, argv );
   assert( app );

   Fl::visual( FL_DOUBLE | FL_RGB8 );
   app->Show();
   Fl::run();

   delete   app;
   app   = NULL;

   return   0;
}
