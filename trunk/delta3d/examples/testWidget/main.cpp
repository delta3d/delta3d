#include "dt.h"
#include "dtabc.h"

#include "TestWidget.h"



int
main( int argc, const char* argv[] )
{
   TestWidget* app   = new TestWidget;
   assert( app );

   Fl::visual( FL_DOUBLE | FL_RGB8 );
   app->make_window();
   Fl::run();

   delete   app;
   app   = NULL;

   return   0;
}
