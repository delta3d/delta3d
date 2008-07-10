#include <cstdio>
#include <QtGui/QApplication>

#include "ObjectWorkspace.h"
#include "Delta3DThread.h"
#include <dtUtil/macros.h>

#ifdef DELTA_WIN32
   #include <Windows.h>
   #define SLEEP(milliseconds) Sleep((milliseconds))
#else
   #include <unistd.h>
   #define SLEEP(milliseconds) usleep(((milliseconds) * 1000))
#endif

#include <iostream>

int main(int argc, char *argv[]) 
{
   QApplication qapp(argc, argv);

   ObjectWorkspace win;
   win.show();

   Delta3DThread *thread = new Delta3DThread(&qapp);

   QObject::connect(QApplication::instance(), SIGNAL(lastWindowClosed()), thread, SLOT(quit()));

   thread->SetObjectWorkspace(&win);   
   thread->run();
   
   qapp.exec();
  
   delete thread;

   return EXIT_SUCCESS;
}
