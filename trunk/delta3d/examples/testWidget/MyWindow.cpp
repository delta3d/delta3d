#include <string>

#include "MyWindow.h"

IMPLEMENT_MANAGEMENT_LAYER( MyWindow )

const char* MyWindow::PATH = "../../data;";
const char* MyWindow::FILE = "tacoma.ive";



MyWindow::MyWindow()
:  MyParent()
{
   ctor();
}



MyWindow::MyWindow( int w, int h, const char* l /*= 0L*/ )
:  MyParent(w, h, l)
{
   ctor();
}



MyWindow::MyWindow( int x, int y, int w, int h, const char* l /*= 0L*/ )
:  MyParent(x, y, w, h, l)
{
   ctor();
}



MyWindow::~MyWindow()
{
}



void
MyWindow::show( void )
{
   MyParent::show();

   std::string path(PATH + dtCore::GetDeltaDataPathList() );
   std::string file(FILE);

   SendMessage( "setpath", &path );
   SendMessage( "loadfile", &file );
}



void
MyWindow::ctor( void )
{
   SetEvent( FL_PUSH );
   SetEvent( FL_RELEASE );
   SetEvent( FL_ENTER );
   SetEvent( FL_LEAVE );
   SetEvent( FL_DRAG );
   SetEvent( FL_MOVE );
   SetEvent( FL_MOUSEWHEEL );
   SetEvent( FL_FOCUS );
   SetEvent( FL_UNFOCUS );
   SetEvent( FL_KEYDOWN );
   SetEvent( FL_KEYUP );
}
