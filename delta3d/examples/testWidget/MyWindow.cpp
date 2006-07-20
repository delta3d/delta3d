#include "MyWindow.h"

#include <dtCore/globals.h>

std::string MyWindow::PATH = "/examples/testWidget/;";
std::string MyWindow::FILE = "models/brdm.ive";

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

void MyWindow::show()
{
   MyParent::show();

   std::string path((dtCore::GetDeltaRootPath() + PATH) + dtCore::GetDeltaDataPathList() );

   SendMessage( "setpath", &path );
   SendMessage( "loadfile", &FILE );
}

void MyWindow::ctor()
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
