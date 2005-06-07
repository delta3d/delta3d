#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#pragma  warning( disable : 4311 )  // casting void*->long warning
#pragma  warning( disable : 4312 )  // casting long->void* warning
#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#include "dtCore/dt.h"
#include "dtABC/dtabc.h"
#include <cassert>

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
