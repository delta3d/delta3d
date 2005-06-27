#include <QApplication>
#include <QMessageBox>
#include <QSplashScreen>
#include <QPixmap>
#include <sstream>
#include <dtCore/globals.h>
#include <dtCore/system.h>
#include "dtEditQt/mainwindow.h"
#include "dtEditQt/viewportmanager.h"
#include "dtDAL/log.h"
#include "dtDAL/librarymanager.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/editoractions.h"
#include "dtEditQt/uiresources.h"

int main(int argc, char *argv[])
{
    int result;
    QApplication app(argc, argv);

    QPixmap pixmap(dtEditQt::UIResources::SPLASH_SCREEN.c_str());
    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash->show();

    try
    {
       //std::cout << dtEditQt::UIResources::mPrefix << std::endl;
        std::cout << dtEditQt::UIResources::ICON_FILE_NEW_MAP << std::endl;
   
        dtDAL::Log::GetInstance().SetLogLevel(dtDAL::Log::LOG_WARNING);
        dtCore::SetDataFilePathList(".;" + dtCore::GetDeltaDataPathList());

        //Construct the application...
        dtEditQt::MainWindow mainWindow;
        dtEditQt::EditorEvents::getInstance().emitEditorInitiationEvent();

        //Now that everything is initialized, show the main window.
        mainWindow.show();
        splash->finish(&mainWindow);
        delete splash;
        mainWindow.setWindowMenuTabsChecked();

        dtCore::System::GetSystem()->Start();
        result = app.exec();
        dtCore::System::GetSystem()->Stop();
    }
    catch (dtDAL::Exception &e)
    {
        std::ostringstream ss;
        e.Print();
        ss << "Exception (" << e.TypeEnum() << "): " << e.What()
            << "\n\tLine: " << e.Line() << " File: " << e.File();
        QMessageBox::critical(NULL,"Exception",ss.str().c_str(),
            QMessageBox::Ok,QMessageBox::NoButton);
        return 1;
    }

    return result;
}

