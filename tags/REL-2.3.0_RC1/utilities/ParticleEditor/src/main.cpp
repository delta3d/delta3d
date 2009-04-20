#include <delta3dthread.h>
#include <mainwindow.h>

#include <dtCore/camera.h>
#include <dtCore/globals.h>
#include <dtCore/scene.h>
#include <dtCore/compass.h>
#include <dtCore/system.h>
#include <dtUtil/fileutils.h>

#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

#include <sstream>
#include <string.h>
#include <math.h>

////////////////////////////////////////////////////////////////////////////////

#ifdef DELTA_WIN32
#include <Windows.h>
#define SLEEP(milliseconds) Sleep((milliseconds))
#else
#include <unistd.h>
#define SLEEP(milliseconds) usleep(((milliseconds) * 1000))
#endif

/////The Editor preferences - gets saved automatically when app exits

// Previous loaded file history
char absoluteHistory[5][1024];
char relativeHistory[5][1024];

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
   QApplication qapp(argc, argv);  

   Delta3DThread *thread = new Delta3DThread(&qapp);

   QObject::connect(QApplication::instance(), SIGNAL(lastWindowClosed()), thread, SLOT(quit()));

   psEditor::MainWindow win;
   win.show();

   thread->SetMainWindow(&win);
   thread->run();

   if (argc >= 2)
   {
      win.LoadFile(QCoreApplication::arguments().at(1));
   }

   qapp.exec();

   delete thread;

   return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
