/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 */
#include <prefix/stageprefix.h>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QSplashScreen>
#include <QtGui/QPixmap>
#include <sstream>
#include <dtCore/system.h>
#include <dtCore/deltawin.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/uiresources.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/exception.h>
#include <dtAudio/audiomanager.h>
#include <dtQt/qtguiwindowsystemwrapper.h>
#include <dtQt/deltastepper.h>
#include <dtEditQt/stageapplication.h>
#include <dtEditQt/stageglwidgetfactory.h>
#include <QtGui/QPlastiqueStyle>
#include <QtCore/QTextStream>

#include <iostream>

int main(int argc, char* argv[])
{
   dtAudio::AudioManager::Instantiate();

   // set proper log file name 
#if (_DEBUG)
   dtUtil::Log::GetInstance().SetLogLevel(dtUtil::Log::LOG_INFO);
#else
   dtUtil::Log::GetInstance().SetLogLevel(dtUtil::Log::LOG_WARNING);
#endif

   dtUtil::LogFile::SetTitle("STAGE Log File");
   dtUtil::LogFile::SetFileName("STAGE_log.html");

   dtUtil::SetDataFilePathList(".;" +
      dtUtil::GetDeltaDataPathList() + ";" +
      dtUtil::GetDeltaRootPath() + "/utilities/STAGE");

   int result;
   //QApplication::setStyle(new QPlastiqueStyle);
   QApplication app(argc, argv);
//   QString style(":style/darkorange.qss");
//   QFile data(style);
//   if (data.open(QFile::ReadOnly)) {
//      QTextStream textStream(&data);
//      QString styleStr = textStream.readAll();
//      std::cout << styleStr.toStdString() << std::endl;
//      app.setStyleSheet(styleStr);
//   }

   dtEditQt::UIResources::staticInitialize();
   QPixmap pixmap(dtEditQt::UIResources::SPLASH_SCREEN.c_str());
   QSplashScreen* splash = new QSplashScreen(pixmap);
   splash->show();


   // Now that everything is initialized, show the main window.
   // Construct the application...

   //1st arg is the STAGEConfig.xml full path
   std::string configFile = "";
   if (argc > 1)
   {
      configFile = argv[1];
   }

   //Create special QGLWidget's when we create DeltaWin instances
   dtQt::QtGuiWindowSystemWrapper::EnableQtGUIWrapper();

   osg::GraphicsContext::WindowingSystemInterface* winSys = osg::GraphicsContext::getWindowingSystemInterface();
   dtQt::QtGuiWindowSystemWrapper* qtgui = dynamic_cast<dtQt::QtGuiWindowSystemWrapper*>(winSys);
   if (qtgui)
   {
      qtgui->SetGLWidgetFactory(new dtEditQt::STAGEGLWidgetFactory());
   }
   

   try
   {
      // Now that everything is initialized, show the main window.
      // Construct the application...
      dtEditQt::MainWindow mainWindow(configFile);

      //create a dummy window that won't get realized, then delete it.  This is
      //just to get around the automatic creation of a DeltaWin by Application.
      dtCore::DeltaWin::DeltaWinTraits traits;
      traits.realizeUponCreate = false;
      traits.name = "dummy";
      dtCore::RefPtr<dtCore::DeltaWin> dummyWin = new dtCore::DeltaWin(traits);

      dtCore::RefPtr<dtEditQt::STAGEApplication> viewer = new dtEditQt::STAGEApplication(dummyWin.get());
      viewer->Config();
      
      dummyWin = NULL;

      dtEditQt::ViewportManager::GetInstance().SetApplication(viewer.get());
      mainWindow.show();

      //create a little class to ensure Delta3D performs Window "steps"
      dtCore::System::GetInstance().Start();
      dtQt::DeltaStepper stepper;
      stepper.connect(&stepper, SIGNAL(PostTick()),
            &dtEditQt::ViewportManager::GetInstance(), SLOT(onPostTick()));
      stepper.Start();

      splash->finish(&mainWindow);
      delete splash;
      splash = NULL;

      dtEditQt::EditorEvents::GetInstance().emitEditorInitiationEvent();
      mainWindow.setWindowMenuTabsChecked();

      result = app.exec();
      stepper.Stop();

      dtEditQt::ViewportManager::GetInstance().SetApplication(NULL);
   }
   catch (const dtUtil::Exception& e)
   {
      e.LogException(dtUtil::Log::LOG_ERROR);

      // hide the splash screen if it's up or you can't see the error!
      if (splash != NULL)
      {
         delete splash;
         splash = NULL;
      }

      QMessageBox::critical(NULL,"Exception", e.ToString().c_str(),
         QMessageBox::Ok,QMessageBox::NoButton);

      result = 1;
   }

   dtAudio::AudioManager::Destroy();

   return result;
}
