#include <assert.h>
#include "dt.h"
#include "fltkwindow.h"

int
main( int argc, const char** argv )
{
   FLTKWindow* app  = new FLTKWindow;
   assert( app );

   Fl::visual( FL_DOUBLE | FL_RGB8 );
   app->make_window( argc, argv );
   Fl::run();

   delete   app;
   app   = NULL;

   return   0;
}
