#include <cstdio>
#include <cstdlib>

#include <QtGui/QApplication>

#include "mainwindow.h"
#include "aiutilityapp.h"

#include <dtUtil/macros.h>

#include <dtCore/deltawin.h>

#include <dtQt/qtguiwindowsystemwrapper.h>
#include <dtQt/osggraphicswindowqt.h>
#include <dtQt/osgadapterwidget.h>

#include <dtUtil/log.h>

int main(int argc, char *argv[])
{
   QApplication qapp(argc, argv);

   dtQt::QtGuiWindowSystemWrapper::EnableQtGUIWrapper();

   dtCore::RefPtr<AIUtilityApp> app = new AIUtilityApp;

   dtQt::OSGGraphicsWindowQt* osgGraphWindow = dynamic_cast<dtQt::OSGGraphicsWindowQt*>(app->GetWindow()->GetOsgViewerGraphicsWindow());

   if (osgGraphWindow == NULL)
   {
      LOG_ERROR("Unable to initialize. The deltawin could not be created with a QGLWidget.");
      return EXIT_FAILURE;
   }

   MainWindow win(*osgGraphWindow->GetQGLWidget());

   win.show();

   QObject::connect(QApplication::instance(), SIGNAL(lastWindowClosed()), app.get(), SLOT(DoQuit()));

   app->Config();
   qapp.exec();

   return EXIT_SUCCESS;
}
