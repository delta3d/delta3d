/*
 * Delta3D Open Source Game and Simulation Engine Level Editor
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
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
        dtDAL::Log::GetInstance().SetLogLevel(dtDAL::Log::LOG_WARNING);
        dtCore::SetDataFilePathList(".;" + dtCore::GetDeltaDataPathList());

        //Construct the application...
        dtEditQt::MainWindow mainWindow;

        //Now that everything is initialized, show the main window.
        mainWindow.show();
        
        splash->finish(&mainWindow);
        delete splash;

        dtEditQt::EditorEvents::getInstance().emitEditorInitiationEvent();
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

