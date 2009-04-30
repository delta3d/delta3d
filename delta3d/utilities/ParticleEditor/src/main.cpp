#include <delta3dthread.h>
#include <mainwindow.h>

#include <QtGui/QApplication>

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
