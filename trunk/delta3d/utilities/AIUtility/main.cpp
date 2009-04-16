#include <cstdio>
#include <cstdlib>

#include <QtGui/QApplication>

#include "mainwindow.h"
#include "aiutilityapp.h"

#include <dtUtil/macros.h>

#include <dtCore/deltawin.h>
#include <dtCore/system.h>

#include <dtQt/qtguiwindowsystemwrapper.h>
#include <dtQt/osggraphicswindowqt.h>
#include <dtQt/osgadapterwidget.h>

#include <dtUtil/log.h>

int main(int argc, char *argv[])
{
   QApplication qapp(argc, argv);

   osg::GraphicsContext::WindowingSystemInterface* winSys = osg::GraphicsContext::getWindowingSystemInterface();

   if (winSys != NULL)
   {
      osg::GraphicsContext::setWindowingSystemInterface(new dtQt::QtGuiWindowSystemWrapper(*winSys));
   }
   else
   {
      LOG_ERROR("Unable to initialize. OSG reported not having any windowing system interface.");
      return EXIT_FAILURE;
   }


   dtCore::RefPtr<AIUtilityApp> app = new AIUtilityApp;

   app->Config();

   dtQt::OSGGraphicsWindowQt* osgGraphWindow = dynamic_cast<dtQt::OSGGraphicsWindowQt*>(app->GetWindow()->GetOsgViewerGraphicsWindow());

   if (osgGraphWindow == NULL)
   {
      LOG_ERROR("Unable to initialize. The deltawin could not be created with a QGLWidget.");
      return EXIT_FAILURE;
   }

   MainWindow win(*osgGraphWindow->GetQGLWidget());

   win.show();

   QObject::connect(QApplication::instance(), SIGNAL(lastWindowClosed()), app, SLOT(DoQuit()));

   dtCore::System::GetInstance().Start();
   qapp.exec();
   dtCore::System::GetInstance().Stop();

   return EXIT_SUCCESS;
}
