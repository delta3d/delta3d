#include <stdio.h>
#include <QApplication>

#include "MainWindow.h"
#include "Delta3DThread.h"

#include <Windows.h>
#include <WinBase.h>

int main(int argc, char *argv[]) 
{

   QApplication qapp(argc, argv);

   MainWindow win;

   win.show();

   Delta3DThread *thread = new Delta3DThread(&qapp);

   QObject::connect(QApplication::instance(), SIGNAL(lastWindowClosed()), thread, SLOT(quit()));

   thread->SetMainWindow(&win);
   thread->start();

   qapp.exec();

   //thread->quit();
   thread->exit();

   while (thread->isRunning())
   {
      Sleep(100);
   }

   delete thread;

	return EXIT_SUCCESS;
}
