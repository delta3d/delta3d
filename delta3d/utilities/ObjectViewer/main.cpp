#include <QtGui/QApplication>
#include <QtCore/QFileInfo>
#include <dtQt/qtguiwindowsystemwrapper.h>
#include <dtQt/deltastepper.h>

#include "ObjectWorkspace.h"
#include "ObjectViewer.h"


int main(int argc, char* argv[]) 
{
   QApplication qapp(argc, argv);

   //Create special QGLWidget's when we create DeltaWin instances
   dtQt::QtGuiWindowSystemWrapper::EnableQtGUIWrapper();

   //The main UI window
   ObjectWorkspace win;

   //The object viewer Application
   dtCore::RefPtr<ObjectViewer> mViewer = new ObjectViewer();
   mViewer->Config();

   win.SetViewer(mViewer.get());
   win.OnInitialization();
   win.show(); //show the UI

   //create a little class to ensure Delta3D performs Window "steps"
   dtCore::System::GetInstance().Start();
   dtQt::DeltaStepper stepper;
   stepper.Start();

   // If we have an argument(possibly from drag and drop)
   if (argc >= 2)
   {
      QFileInfo fileInfo(QCoreApplication::arguments().at(1));

      if (fileInfo.fileName().endsWith(QString("xml"), Qt::CaseInsensitive))
      {
         win.OnLoadMap(fileInfo.baseName().toStdString());
      }
      else
      {
         win.OnLoadGeometry(QCoreApplication::arguments().at(1).toStdString());
      }
   }
   
   qapp.exec();
   stepper.Stop();

   return EXIT_SUCCESS;
}
