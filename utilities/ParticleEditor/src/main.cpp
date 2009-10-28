#include <mainwindow.h>
#include <dtCore/system.h>
#include <QtGui/QApplication>
#include <dtQt/deltastepper.h>
#include <dtQt/qtguiwindowsystemwrapper.h>

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
   QApplication qapp(argc, argv);  

   //Create special QGLWidget's when we create DeltaWin instances
   dtQt::QtGuiWindowSystemWrapper::EnableQtGUIWrapper();

   //The main UI window
   psEditor::MainWindow win;

   //The particle editor Application
   dtCore::RefPtr<ParticleViewer> particleEditorApplication = new ParticleViewer();
   particleEditorApplication->Config();

   win.SetParticleViewer(particleEditorApplication.get());
   win.SetupUI();
   win.show(); //show the UI

   //create a little class to ensure Delta3D performs Window "steps"
   dtCore::System::GetInstance().Start();
   dtQt::DeltaStepper stepper;
   stepper.Start();

   if (argc >= 2)
   {
      win.LoadFile(QCoreApplication::arguments().at(1));
   }

   qapp.exec();
   stepper.Stop();

   return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
