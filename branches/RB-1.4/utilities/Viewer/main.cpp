#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#pragma  warning( disable : 4311 )  // casting void*->long warning
#pragma  warning( disable : 4312 )  // casting long->void* warning
#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#include "dtCore/dt.h"
#include "dtABC/dtabc.h"
#include <cassert>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4244) // for warning C4244: '=' : conversion from 'short' to 'uchar', possible loss of data
#endif

#include "UserInterface.h"

#ifdef _MSC_VER
#	pragma warning(pop)
#endif

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
