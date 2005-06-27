/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* @author Matthew W. Campbell
*/
#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QDockWidget>
#include <QSplitter>
#include <QStatusBar>
#include <QMessageBox>
#include <QCloseEvent>
#include <QTimer>
#include <dtCore/uniqueid.h>

#include "dtDAL/log.h"
#include "dtDAL/project.h"
#include "dtDAL/librarymanager.h"
#include "dtDAL/fileutils.h"
#include "dtEditQt/global.h"
#include "dtEditQt/mainwindow.h"
#include "dtEditQt/editoractions.h"
#include "dtEditQt/editorsettings.h"
#include "dtEditQt/perspectiveviewport.h"
#include "dtEditQt/orthoviewport.h"
#include "dtEditQt/viewportmanager.h"
#include "dtEditQt/viewportcontainer.h"
#include "dtEditQt/propertyeditor.h"
#include "dtEditQt/actortab.h"
#include "dtEditQt/resourcebrowser.h"
#include "dtEditQt/editordata.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/viewportmanager.h"
#include "dtEditQt/projectcontextdialog.h"
//#include "dtEditQt/undomanager.h"

#include <osgDB/FileNameUtils>


namespace dtEditQt
{

    ///////////////////////////////////////////////////////////////////////////////
    MainWindow::MainWindow()
    {
        //Ensure that the global singletons are lazily instantiated now
        dtDAL::LibraryManager::GetInstance();
        EditorActions::getInstance();
        EditorEvents::getInstance();
        EditorData::getInstance();
        dtDAL::Project::GetInstance();
        ViewportManager::getInstance();

        connectSlots();
        setupDockWindows();
        setupStatusBar();
        setupMenus();
        setupToolbar();

        //EditorData::getInstance().setUndoManager(new UndoManager());

        //Make sure some default UI states are correctly initialized.
        EditorActions::getInstance().actionSelectionCamera->setChecked(true);
        setWindowTitle(tr("Delta3D Level Editor"));
        EditorData::getInstance().setMainWindow(this);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::setupMenus()
    {
        EditorActions &editorActions = EditorActions::getInstance();

        recentProjs = new QMenu(tr("Recent Projects"));
        recentMaps  = new QMenu(tr("Recent Maps"));

        recentProjs->addAction(editorActions.actionFileRecentProject0);

        fileMenu = menuBar()->addMenu(tr("&File"));
        fileMenu->addAction(editorActions.actionFileNewMap);
        fileMenu->addAction(editorActions.actionFileOpenMap);
        fileMenu->addAction(editorActions.actionFileCloseMap);
        fileMenu->addSeparator();
        fileMenu->addAction(editorActions.actionFileSaveMap);
        fileMenu->addAction(editorActions.actionFileSaveMapAs);
        fileMenu->addSeparator();
        fileMenu->addAction(editorActions.actionFileChangeProject);
        fileMenu->addSeparator();
        fileMenu->addMenu(recentProjs);
        // h4x0r
        //fileMenu->addMenu(recentMaps);
        fileMenu->addSeparator();
        fileMenu->addAction(editorActions.actionFileExit);

        editMenu = menuBar()->addMenu(tr("&Edit"));
        editMenu->addAction(editorActions.actionEditUndo);
        editMenu->addAction(editorActions.actionEditRedo);
        editMenu->addSeparator();
        editMenu->addAction(editorActions.actionEditDuplicateActor);
        editMenu->addAction(editorActions.actionEditDeleteActor);
        editMenu->addAction(editorActions.actionEditGroundClampActors);
        editMenu->addSeparator();
        editMenu->addAction(editorActions.actionEditMapProperties);
        editMenu->addAction(editorActions.actionEditMapLibraries);
        editMenu->addSeparator();
        editMenu->addAction(editorActions.actionFileEditPreferences);

        selectionMenu = menuBar()->addMenu(tr("&Selection"));
        selectionMenu->addAction(editorActions.actionSelectionCamera);
        selectionMenu->addAction(editorActions.actionSelectionSelectActor);
        selectionMenu->addAction(editorActions.actionSelectionTranslateActor);
        selectionMenu->addAction(editorActions.actionSelectionRotateActor);

        windowMenu = menuBar()->addMenu(tr("&Window"));
        windowMenu->addAction(editorActions.actionWindowsPropertyEditor);
        windowMenu->addAction(editorActions.actionWindowsActorSearch);
        windowMenu->addAction(editorActions.actionWindowsResourceBrowser);
        windowMenu->addSeparator();
        windowMenu->addAction(editorActions.actionWindowsResetWindows);

        helpMenu = menuBar()->addMenu(tr("&Help"));
        helpMenu->addAction(editorActions.actionHelpAboutEditor);
        helpMenu->addAction(editorActions.actionHelpAboutQT);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::setupToolbar()
    {
        fileToolBar = new QToolBar(this);
        fileToolBar->setObjectName("FileToolBar");
        fileToolBar->setWindowTitle(tr("File Toolbar"));
        fileToolBar->addAction(EditorActions::getInstance().actionFileNewMap);
        fileToolBar->addAction(EditorActions::getInstance().actionFileOpenMap);
        fileToolBar->addAction(EditorActions::getInstance().actionFileSaveMap);
        addToolBar(fileToolBar);

        editToolBar = new QToolBar(this);
        editToolBar->setObjectName("EditToolBar");
        editToolBar->setWindowTitle(tr("Edit Toolbar"));
        editToolBar->addAction(EditorActions::getInstance().actionEditDuplicateActor);
        editToolBar->addAction(EditorActions::getInstance().actionEditDeleteActor);
        editToolBar->addAction(EditorActions::getInstance().actionEditGroundClampActors);
        addToolBar(editToolBar);

        undoToolBar = new QToolBar(this);
        undoToolBar->setObjectName("UndoToolBar");
        undoToolBar->setWindowTitle(tr("Undo Toolbar"));
        undoToolBar->addAction(EditorActions::getInstance().actionEditUndo);
        undoToolBar->addAction(EditorActions::getInstance().actionEditRedo);
        addToolBar(undoToolBar);

        selectionToolBar = new QToolBar(this);
        selectionToolBar->setObjectName("SelectionToolBar");
        selectionToolBar->setWindowTitle(tr("Selection Toolbar"));
        selectionToolBar->addAction(EditorActions::getInstance().actionSelectionCamera);
        selectionToolBar->addAction(EditorActions::getInstance().actionSelectionSelectActor);
        selectionToolBar->addAction(EditorActions::getInstance().actionSelectionTranslateActor);
        selectionToolBar->addAction(EditorActions::getInstance().actionSelectionRotateActor);
        addToolBar(selectionToolBar);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::setupStatusBar()
    {
        statusBar();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::setupDockWindows()
    {
        // setup the layout of the dock windows
        setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea/*Qt::DockWindowAreaLeft*/);

        // create the main left dock window
        propertyWindow = new PropertyEditor(this);
        propertyWindow->setObjectName("PropertyWindow");

        actorTab = new ActorTab(this);
        actorTab->setObjectName("ActorTab");

        resourceBrowser = new ResourceBrowser(this);
        resourceBrowser->setObjectName("ResourceBrowser");

        propertyWindow->setFeatures(QDockWidget::AllDockWidgetFeatures);
        actorTab->setFeatures(QDockWidget::AllDockWidgetFeatures);
        resourceBrowser->setFeatures(QDockWidget::AllDockWidgetFeatures);

        addDockWidget(Qt::LeftDockWidgetArea, propertyWindow);
        addDockWidget(Qt::LeftDockWidgetArea, actorTab);
        addDockWidget(Qt::RightDockWidgetArea, resourceBrowser);

        //Create the viewports, assign them to splitters, and embed the splitters
        //in the central widget of the main window.
        mainViewportParent = setupViewports();
        setCentralWidget(mainViewportParent);
    }

    ///////////////////////////////////////////////////////////////////////////////
    QWidget *MainWindow::setupViewports()
    {
        ViewportManager &vpMgr = ViewportManager::getInstance();

        //Create our splitters which will house the newly created
        //viewports.
        QSplitter *hSplit  = new QSplitter(Qt::Vertical); //Split top-bottom
        QSplitter *vSplit1 = new QSplitter(hSplit); //Split for top and front views
        QSplitter *vSplit2 = new QSplitter(hSplit); //Split for 3d and side views

        //Create the actual viewport objects..
        this->perspView = (PerspectiveViewport *)vpMgr.createViewport("Perspective View",
            ViewportManager::ViewportType::PERSPECTIVE);
        this->perspView->setAutoInteractionMode(true);

        this->topView = (OrthoViewport *)vpMgr.createViewport("Top View (XY)",
            ViewportManager::ViewportType::ORTHOGRAPHIC);
        this->topView->setAutoInteractionMode(true);

        this->sideView = (OrthoViewport *)vpMgr.createViewport("Side View (YZ)",
            ViewportManager::ViewportType::ORTHOGRAPHIC);
        this->sideView->setViewType(OrthoViewport::OrthoViewType::SIDE,false);
        this->sideView->setAutoInteractionMode(true);

        this->frontView = (OrthoViewport *)vpMgr.createViewport("Front View (XZ)",
            ViewportManager::ViewportType::ORTHOGRAPHIC);
        this->frontView->setViewType(OrthoViewport::OrthoViewType::FRONT,false);
        this->frontView->setAutoInteractionMode(true);


        //We now wrap each viewport in a viewport container to provide the
        //toolbar and right click menu add-ons which are needed by the editor
        //for each viewport.
        ViewportContainer *container;

        container = new ViewportContainer(this->sideView,vSplit2);
        container = new ViewportContainer(this->perspView,vSplit2);
        container = new ViewportContainer(this->topView,vSplit1);
        container = new ViewportContainer(this->frontView,vSplit1);

        //Returns the root of the viewport widget hierarchy.
        return hSplit;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::enableActions()
    {
        bool hasProject = dtDAL::Project::GetInstance().IsContextValid();
        bool hasCurrentMap = (EditorData::getInstance().getCurrentMap() != NULL);
        bool hasBoth = hasProject && hasCurrentMap;

        EditorActions::getInstance().actionFileNewMap->setEnabled(hasProject);
        EditorActions::getInstance().actionFileOpenMap->setEnabled(hasProject);
        EditorActions::getInstance().actionFileCloseMap->setEnabled(hasBoth);
        EditorActions::getInstance().actionFileSaveMap->setEnabled(hasBoth);
        EditorActions::getInstance().actionFileSaveMapAs->setEnabled(hasBoth);
        EditorActions::getInstance().actionFileExit->setEnabled(true);

        EditorActions::getInstance().actionEditDuplicateActor->setEnabled(false);
        EditorActions::getInstance().actionEditDeleteActor->setEnabled(false);
        EditorActions::getInstance().actionEditGroundClampActors->setEnabled(false);
        EditorActions::getInstance().actionEditMapProperties->setEnabled(hasBoth);
        EditorActions::getInstance().actionEditMapLibraries->setEnabled(hasBoth);

        EditorActions::getInstance().actionSelectionCamera->setEnabled(hasBoth);
        EditorActions::getInstance().actionSelectionSelectActor->setEnabled(hasBoth);
        EditorActions::getInstance().actionSelectionTranslateActor->setEnabled(hasBoth);
        EditorActions::getInstance().actionSelectionRotateActor->setEnabled(hasBoth);

        EditorActions::getInstance().actionWindowsActorSearch->setEnabled(hasBoth);
        EditorActions::getInstance().actionWindowsPropertyEditor->setEnabled(hasBoth);
        EditorActions::getInstance().actionWindowsResourceBrowser->setEnabled(hasBoth);
        EditorActions::getInstance().actionWindowsResetWindows->setEnabled(hasBoth);

        EditorActions::getInstance().actionHelpAboutEditor->setEnabled(true);
        EditorActions::getInstance().actionHelpAboutQT->setEnabled(true);

        // enable main window areas
        propertyWindow->setEnabled(hasBoth);
        actorTab->setEnabled(hasBoth);
        resourceBrowser->setEnabled(hasProject);
        mainViewportParent->setEnabled(hasBoth);
        editMenu->setEnabled(hasBoth);
        selectionMenu->setEnabled(hasBoth);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onResetWindows()
    {
        // If they're detached, reattach
        propertyWindow->setFloating(false);
        actorTab->setFloating(false);
        resourceBrowser->setFloating(false);

        // This should always default back to visible, like the app was restarted and the .ini
        // was deleted
        propertyWindow->setVisible(true);
        actorTab->setVisible(true);
        resourceBrowser->setVisible(true);

        EditorActions::getInstance().actionWindowsPropertyEditor->setChecked(true);
        EditorActions::getInstance().actionWindowsActorSearch->setChecked(true);
        EditorActions::getInstance().actionWindowsResourceBrowser->setChecked(true);

        addDockWidget(Qt::LeftDockWidgetArea,  propertyWindow);
        addDockWidget(Qt::LeftDockWidgetArea,  actorTab);
        addDockWidget(Qt::RightDockWidgetArea, resourceBrowser);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onEditorInitiated()
    {
        enableActions();
        findAndLoadPreferences();

        bool projectsExist = false;
        if(findRecentProjects().empty())
        {
            ProjectContextDialog dialog(this);
            if (dialog.exec() == QDialog::Accepted)
            {
                std::string contextName = dialog.getProjectPath().toStdString();

                //First try to set the new project context.
                try
                {
                    EditorData::getInstance().getMainWindow()->startWaitCursor();
                    dtDAL::Project::GetInstance().SetContext(contextName);
                    EditorData::getInstance().setCurrentProjectContext(contextName);
                    EditorData::getInstance().addRecentProject(contextName);
                    EditorEvents::getInstance().emitProjectChanged();
                    EditorActions::getInstance().refreshRecentProjects();
                    EditorData::getInstance().getMainWindow()->endWaitCursor();
                }
                catch (dtDAL::Exception &e)
                {
                    EditorData::getInstance().getMainWindow()->endWaitCursor();
                    QMessageBox::critical((QWidget *)this,
                        tr("Error"), tr(e.What().c_str()), tr("OK"));
                }
            }
        }
        else
        {
            projectsExist = true;

            EditorData::getInstance().getMainWindow()->startWaitCursor();
            EditorEvents::getInstance().emitProjectChanged();
            EditorData::getInstance().getMainWindow()->endWaitCursor();
        }

        EditorData::getInstance().getMainWindow()->startWaitCursor();
        EditorActions::getInstance().refreshRecentProjects();
        EditorData::getInstance().getMainWindow()->endWaitCursor();

        if(!EditorData::getInstance().getLoadLastMap())
        {
            return;
        }

        if(projectsExist)
        {
            std::vector<std::string> maps = findRecentMaps();

            for(unsigned int i = 0; i < maps.size(); i++)
                checkAndLoadBackup(maps[i]);
        }

        EditorActions::getInstance().getTimer()->start();

        updateWindowTitle();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onEditorShutDown()
    {
        EditorData &editorData = EditorData::getInstance();
        EditorSettings settings;

        //Save the main window state...
        settings.beginGroup(EditorSettings::MAINWIN_GROUP);
        settings.setValue(EditorSettings::MAINWIN_POSITION,pos());
        settings.setValue(EditorSettings::MAINWIN_SIZE,size());
        settings.setValue(EditorSettings::MAINWIN_DOCK_STATE,saveState(EditorSettings::MAINWIN_DOCK_STATE_ID));
        settings.endGroup();

        //Save the general preferences...
        settings.beginGroup(EditorSettings::PREFERENCES_GROUP);
            settings.setValue(EditorSettings::LOAD_RECENT_PROJECTS,editorData.getLoadLastProject());
            settings.setValue(EditorSettings::LOAD_RECENT_MAPS,editorData.getLoadLastMap());
            settings.setValue(EditorSettings::RIGID_CAMERA, editorData.getRigidCamera());
            settings.setValue(EditorSettings::SAVE_MILLISECONDS,EditorActions::getInstance().saveMilliSeconds);
            settings.setValue(EditorSettings::SELECTION_COLOR,editorData.getSelectionColor());
        settings.endGroup();

        //Save the recent projects unless the user does not wish to do so.
        if(!EditorData::getInstance().getLoadLastProject())
            return;

        //Save the current project state...
        settings.beginGroup(EditorSettings::RECENT_PROJECTS);
            if(!EditorData::getInstance().getRecentProjects().empty() &&
                EditorData::getInstance().getRecentProjects().front() != "")
            {
                settings.setValue(EditorSettings::RECENT_PROJECT0,
                    QVariant(QString(EditorData::getInstance().getRecentProjects().front().c_str())));
                EditorData::getInstance().getRecentProjects().pop_front();
            }
        settings.endGroup();

        //Check to see if the user wants the app to remember the recently loaded map.
        if(!EditorData::getInstance().getLoadLastMap() ||
           !EditorData::getInstance().getCurrentMap().valid())
        {
            // Error check, if they have a previous settings file with a recent map in it, it
            // needs to be deleted as to not load it next time
            settings.remove(EditorSettings::RECENT_MAPS);
            return;
        }

        //Save the current map state...
        settings.beginGroup(EditorSettings::RECENT_MAPS);
            if(!EditorData::getInstance().getRecentMaps().empty() &&
                EditorData::getInstance().getRecentMaps().front() != "")
            {
                settings.setValue(EditorSettings::RECENT_MAP0,
                    QVariant(QString(EditorData::getInstance().getRecentMaps().front().c_str())));
                EditorData::getInstance().getRecentMaps().pop_front();
            }
        settings.endGroup();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onActorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy,
            osg::ref_ptr<dtDAL::ActorProperty> property)
    {
        if (!dtDAL::Project::GetInstance().IsContextValid())
            return;

        EditorData::getInstance().getCurrentMap()->SetModified(true);
        updateWindowTitle();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::closeEvent(QCloseEvent *e)
    {
        std::cout << "CLOSE EVENT." << std::endl;
        EditorActions::getInstance().wasCancelled = false;

        dtDAL::Map *curMap = dtEditQt::EditorData::getInstance().getCurrentMap().get();
        if(curMap == NULL)
        {
            EditorEvents::getInstance().emitEditorCloseEvent();
            e->accept();
            return;
        }

        std::cout << "Calling slot file exit." << std::endl;
        dtEditQt::EditorActions::getInstance().slotFileExit();

        EditorActions::getInstance().wasCancelled ? e->ignore() : e->accept();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onPropertyEditorSelection()
    {
         propertyWindow->setVisible(EditorActions::getInstance().actionWindowsPropertyEditor->isChecked());
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onActorSearchSelection()
    {
         actorTab->setVisible(EditorActions::getInstance().actionWindowsActorSearch->isChecked());
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onResourceBrowserSelection()
    {
         resourceBrowser->setVisible(EditorActions::getInstance().actionWindowsResourceBrowser->isChecked());
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::updateWindowTitle()
    {
        setWindowTitle(EditorActions::getInstance().getWindowName().c_str());
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onActorProxyDestroyed(osg::ref_ptr<dtDAL::ActorProxy> proxy)
    {
        EditorData::getInstance().getCurrentMap()->SetModified(true);
        updateWindowTitle();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onActorProxyCreated(osg::ref_ptr<dtDAL::ActorProxy> proxy)
    {
        EditorData::getInstance().getCurrentMap()->SetModified(true);
        updateWindowTitle();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onMapPropertyChanged()
    {
        EditorData::getInstance().getCurrentMap()->SetModified(true);
        updateWindowTitle();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::setWindowMenuTabsChecked()
    {
        EditorActions::getInstance().actionWindowsPropertyEditor->setChecked(propertyWindow->isVisible());
        EditorActions::getInstance().actionWindowsActorSearch->setChecked(actorTab->isVisible());
        EditorActions::getInstance().actionWindowsResourceBrowser->setChecked(resourceBrowser->isVisible());
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::connectSlots()
    {
        EditorActions &editorActions = EditorActions::getInstance();

        connect(editorActions.actionWindowsPropertyEditor, SIGNAL(triggered()),
            this, SLOT(onPropertyEditorSelection()));
        connect(editorActions.actionWindowsActorSearch,    SIGNAL(triggered()),
            this, SLOT(onActorSearchSelection()));
        connect(editorActions.actionWindowsResourceBrowser,SIGNAL(triggered()),
            this, SLOT(onResourceBrowserSelection()));
        connect(&EditorEvents::getInstance(), SIGNAL(editorInitiationEvent()),
            this, SLOT(onEditorInitiated()));
        connect(&EditorEvents::getInstance(), SIGNAL(editorCloseEvent()),
            this, SLOT(onEditorShutDown()));
        connect(&EditorEvents::getInstance(), SIGNAL(projectChanged()),
            this, SLOT(enableActions()));
        connect(&EditorEvents::getInstance(), SIGNAL(currentMapChanged()),
            this, SLOT(enableActions()));
        connect(&EditorEvents::getInstance(), SIGNAL(projectChanged()),
            this, SLOT(updateWindowTitle()));
        connect(&EditorEvents::getInstance(), SIGNAL(currentMapChanged()),
            this, SLOT(updateWindowTitle()));
        connect(&EditorEvents::getInstance(), SIGNAL(mapLibraryImported()),
            this, SLOT(updateWindowTitle()));
        connect(&EditorEvents::getInstance(), SIGNAL(mapLibraryRemoved()),
            this, SLOT(updateWindowTitle()));
        connect(&EditorEvents::getInstance(), SIGNAL(mapPropertyChanged()),
            this, SLOT(onMapPropertyChanged()));
        connect(editorActions.actionWindowsResetWindows, SIGNAL(triggered()),
            this, SLOT(onResetWindows()));
        connect(&EditorEvents::getInstance(), SIGNAL(actorProxyDestroyed(osg::ref_ptr<dtDAL::ActorProxy>)),
            this, SLOT(onActorProxyDestroyed(osg::ref_ptr<dtDAL::ActorProxy>)));
        connect(&EditorEvents::getInstance(), SIGNAL(actorProxyCreated(osg::ref_ptr<dtDAL::ActorProxy>)),
            this, SLOT(onActorProxyCreated(osg::ref_ptr<dtDAL::ActorProxy>)));
        connect(&EditorEvents::getInstance(),
            SIGNAL(actorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy>, osg::ref_ptr<dtDAL::ActorProperty>)),
            this, SLOT(onActorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy>, osg::ref_ptr<dtDAL::ActorProperty>)));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::findAndLoadPreferences()
    {
        EditorSettings settings;

        //Load settings from last session.  Reasonable defaults are specified,
        //if the settings are not present.

        //Main window settings...
        settings.beginGroup(EditorSettings::MAINWIN_GROUP);
        resize(settings.value(EditorSettings::MAINWIN_SIZE,QSize(800,600)).toSize());
        move(settings.value(EditorSettings::MAINWIN_POSITION,QPoint(100,100)).toPoint());

        //When restoring the window state, first see if the key exists.
        if (settings.contains(EditorSettings::MAINWIN_DOCK_STATE))
        {
            QByteArray state = settings.value(EditorSettings::MAINWIN_DOCK_STATE).toByteArray();
            restoreState(state,EditorSettings::MAINWIN_DOCK_STATE_ID);
        }
        settings.endGroup();

        //Now check for the general preferences...
        settings.beginGroup(EditorSettings::PREFERENCES_GROUP);
        if(settings.contains(EditorSettings::LOAD_RECENT_PROJECTS))
        {
            bool loadProjs = settings.value(EditorSettings::LOAD_RECENT_PROJECTS,
                QVariant(true)).toBool();
            EditorData::getInstance().setLoadLastProject(loadProjs);

            if(loadProjs)
            {
                if(settings.contains(EditorSettings::LOAD_RECENT_MAPS))
                {
                    bool loadMaps = settings.value(EditorSettings::LOAD_RECENT_MAPS,
                        QVariant(true)).toBool();
                    EditorData::getInstance().setLoadLastMap(loadMaps);
                }
            }
            else
                EditorData::getInstance().setLoadLastMap(false);
        }
        else
        {
            EditorData::getInstance().setLoadLastProject(true);
            EditorData::getInstance().setLoadLastMap(true);
        }

        if(settings.contains(EditorSettings::RIGID_CAMERA))
        {
            bool rigidCamera = settings.value(EditorSettings::RIGID_CAMERA, QVariant(true)).toBool();
            EditorData::getInstance().setRigidCamera(rigidCamera);
        }

        if(settings.contains(EditorSettings::SAVE_MILLISECONDS))
        {
            int ms = settings.value(EditorSettings::SAVE_MILLISECONDS, QVariant(300000)).toInt();
            EditorActions::getInstance().saveMilliSeconds = ms;
            EditorActions::getInstance().getTimer()->setInterval(ms);
        }

        if (settings.contains(EditorSettings::SELECTION_COLOR))
        {
            QColor color = qvariant_cast<QColor>(settings.value(EditorSettings::SELECTION_COLOR));
            EditorData::getInstance().setSelectionColor(color);
        }
        settings.endGroup();
    }

    ///////////////////////////////////////////////////////////////////////////////
    std::vector<std::string> MainWindow::findRecentProjects()
    {
        EditorSettings settings;
        std::vector<std::string> projects;

        settings.beginGroup(EditorSettings::RECENT_PROJECTS);
            if(settings.contains(EditorSettings::RECENT_PROJECT0))
            {
                std::string project = settings.value(EditorSettings::RECENT_PROJECT0,
                    QString("")).toString().toStdString();

                if(dtDAL::FileUtils::GetInstance().DirExists(project))
                {
                    EditorData::getInstance().addRecentProject(project);
                    if(EditorData::getInstance().getLoadLastProject())
                    {
                        EditorData::getInstance().setCurrentProjectContext(project);
                        dtDAL::Project::GetInstance().SetContext(project);
                    }
                    projects.push_back(project);
                }
                else
                {
                    QMessageBox::critical(this, tr("Failed to load previous context"),
                        tr("Failed to load the previous project context.\n") +
                        tr("This can happen if the last project context\n has been moved, renamed, or deleted."),
                        tr("Ok"));

                    //Remove the recent projects entry from the settings object since it
                    //has become somehow corrupted.
                    settings.remove(EditorSettings::RECENT_PROJECT0);
                }
            }
        settings.endGroup();
        return projects;
    }

    ///////////////////////////////////////////////////////////////////////////////
    std::vector<std::string> MainWindow::findRecentMaps()
    {
        std::vector<std::string> maps;
        EditorSettings settings;

        settings.beginGroup(EditorSettings::RECENT_MAPS);
            if(settings.contains(EditorSettings::RECENT_MAP0))
            {
                const std::string& mapName = settings.value(EditorSettings::RECENT_MAP0,
                                            QString("")).toString().toStdString();

                std::set<std::string>::const_iterator itor =
                    dtDAL::Project::GetInstance().GetMapNames().find(mapName);
                if(itor != dtDAL::Project::GetInstance().GetMapNames().end())
                {
                    maps.push_back(mapName);
                }
                else
                    settings.setValue(EditorSettings::RECENT_MAP0, QVariant(""));
            }
        settings.endGroup();
        return maps;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::checkAndLoadBackup(const std::string &str)
    {
        if(dtDAL::Project::GetInstance().HasBackup(str))
        {
            int result = QMessageBox::information(this, tr("Backup file found"),
                tr("A backup save file has been detected. Would you like to open it?"),
                tr("Yes"), tr("No"), tr("Cancel"));

            if(result == 0)
            {
                EditorData::getInstance().getMainWindow()->startWaitCursor();
                dtDAL::Map &backupMap =
                    dtDAL::Project::GetInstance().OpenMapBackup(str);

                EditorActions::getInstance().changeMaps(
                    EditorData::getInstance().getCurrentMap().get(), &backupMap);
                EditorData::getInstance().addRecentMap(backupMap.GetName());
                EditorData::getInstance().getMainWindow()->endWaitCursor();
            }
            else if(result == 1)
            {
                /*if(!dtDAL::FileUtils::GetInstance().FileExists(str))
                {
                    QMessageBox::critical(this, tr("Error loading map"), 
                        tr("The map ") + str.c_str() + tr(" does not exist or has become corrupted, backup file still retained."));
                    return;
                }
                else
                    dtDAL::Project::GetInstance().ClearBackup(str);*/

                EditorData::getInstance().getMainWindow()->startWaitCursor();
                dtDAL::Project::GetInstance().ClearBackup(str);
                EditorActions::getInstance().changeMaps(EditorData::getInstance().getCurrentMap().get(), 
                    &dtDAL::Project::GetInstance().GetMap(str));
                EditorData::getInstance().addRecentMap(str);
                EditorData::getInstance().getMainWindow()->endWaitCursor();
            }
            else
                return;
        }
        else
        {
            EditorData::getInstance().getMainWindow()->startWaitCursor();
            dtDAL::Map &m = dtDAL::Project::GetInstance().GetMap(str);
            EditorActions::getInstance().changeMaps(
                EditorData::getInstance().getCurrentMap().get(), &m);
            EditorData::getInstance().addRecentMap(m.GetName());
            EditorData::getInstance().getMainWindow()->endWaitCursor();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::startWaitCursor()
    {
        // Note - calling this method multiple times will nest hte wait cursors,
        // so remember to call endWaitCursor() for each one.
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::endWaitCursor()
    {
        QApplication::restoreOverrideCursor();
    }

}
