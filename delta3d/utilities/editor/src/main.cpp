/*
 * Delta3D Open Source Game and Simulation Engine 
 * Simulation, Training, and Game Editor (STAGE)
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
#include "dtDAL/librarymanager.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/editoractions.h"
#include "dtEditQt/uiresources.h"
#include <dtUtil/log.h>

int main(int argc, char *argv[])
{
    int result;
    QApplication app(argc, argv);

    dtEditQt::UIResources::staticInitialize();
    QPixmap pixmap(dtEditQt::UIResources::SPLASH_SCREEN.c_str());
    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash->show();

    try
    {
        dtUtil::Log::GetInstance().SetLogLevel(dtUtil::Log::LOG_DEBUG);
        dtCore::SetDataFilePathList(".;" + dtCore::GetDeltaDataPathList());

        //Now that everything is initialized, show the main window.
        //Construct the application...
        dtEditQt::MainWindow mainWindow;
        mainWindow.show();
        
        splash->finish(&mainWindow);
        delete splash;
        splash = NULL;

        dtEditQt::EditorEvents::getInstance().emitEditorInitiationEvent();
        mainWindow.setWindowMenuTabsChecked();

        dtCore::System::Instance()->Start();
        result = app.exec();
        dtCore::System::Instance()->Stop();
    }
    catch (dtUtil::Exception &e)
    {
        std::ostringstream ss;
        e.Print();
        ss << "Exception (" << e.TypeEnum() << "): " << e.What()
            << "\n\tLine: " << e.Line() << " File: " << e.File();

        // hide the splash screen if it's up or you can't see the error!
        if (splash != NULL) {
            delete splash;
            splash = NULL;
        }

        QMessageBox::critical(NULL,"Exception",ss.str().c_str(),
            QMessageBox::Ok,QMessageBox::NoButton);
        return 1;
    }

    return result;
}

