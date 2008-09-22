#include <cstdio>
#include <QtGui/QApplication>

#include "MainWindow.h"
#include "Delta3DThread.h"
#include <dtUtil/macros.h>

#ifdef DELTA_WIN32
   #include <Windows.h>
   #define SLEEP(milliseconds) Sleep((milliseconds))
#else
   #include <unistd.h>
   #define SLEEP(milliseconds) usleep(((milliseconds) * 1000))
#endif

int main(int argc, char *argv[]) 
{
   QApplication qapp(argc, argv);  

   Delta3DThread *thread = new Delta3DThread(&qapp);

   QObject::connect(QApplication::instance(), SIGNAL(lastWindowClosed()), thread, SLOT(quit()));

   MainWindow win;
   win.show();

   thread->SetMainWindow(&win);
   thread->run();

   if (argc >= 2)
   {
      win.LoadCharFile(QCoreApplication::arguments().at(1));
   }

   qapp.exec();

   delete thread;

   return EXIT_SUCCESS;
}
