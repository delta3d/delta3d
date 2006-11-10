/*
* Delta3D Open Source Game and Simulation Engine 
* Simulation, Training, and Game Editor (STAGE)
* Copyright (C) 2005, BMH Associates, Inc.
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2 of the License, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* Matthew W. Campbell
*/
#include <prefix/dtstageprefix-src.h>
#include <QtGui/QApplication>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QDockWidget>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QMessageBox>
#include <QtGui/QCloseEvent>
#include <QtCore/QTimer>
#include <dtCore/uniqueid.h>
#include <QtGui/QIcon>
#include <dtDAL/project.h>
#include <dtDAL/librarymanager.h>
#include <dtUtil/fileutils.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/editorsettings.h>
#include <dtEditQt/perspectiveviewport.h>
#include <dtEditQt/orthoviewport.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/viewportcontainer.h>
#include <dtEditQt/propertyeditor.h>
#include <dtEditQt/actortab.h>
#include <dtEditQt/resourcebrowser.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/projectcontextdialog.h>
#include <dtEditQt/uiresources.h>
#include <osgDB/FileNameUtils>


namespace dtEditQt
{

    ///////////////////////////////////////////////////////////////////////////////
    MainWindow::MainWindow()
    {
        //Ensure that the global singletons are lazily instantiated now
        dtDAL::LibraryManager::GetInstance();
        EditorActions::GetInstance();
        EditorEvents::GetInstance();
        EditorData::GetInstance();
        
        //alert the project instance that we are working within STAGE
        //changed on 7/10/2006 banderegg
        dtDAL::Project::GetInstance().SetEditMode(true);

        ViewportManager::GetInstance();

        connectSlots();
        setupDockWindows();
        setupStatusBar();
        setupMenus();
        setupToolbar();

        //EditorData::GetInstance().setUndoManager(new UndoManager());

        //Make sure some default UI states are correctly initialized.
        EditorActions::GetInstance().actionSelectionCamera->setChecked(true);
        setWindowTitle(tr("STAGE"));
        EditorData::GetInstance().setMainWindow(this);
        
        // add the application icon
        QIcon *icon = new QIcon();
        icon->addPixmap(QPixmap(UIResources::ICON_APPLICATION.c_str()));
        setWindowIcon(*icon);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::setupMenus()
    {
        EditorActions &editorActions = EditorActions::GetInstance();

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
        fileMenu->addSeparator();
        fileMenu->addAction(editorActions.actionFileExit);

        editMenu = menuBar()->addMenu(tr("&Edit"));
        editMenu->addAction(editorActions.actionEditUndo);
        editMenu->addAction(editorActions.actionEditRedo);
        editMenu->addSeparator();
        editMenu->addAction(editorActions.actionEditDuplicateActor);
        editMenu->addAction(editorActions.actionEditDeleteActor);
        editMenu->addAction(editorActions.actionEditGroundClampActors);
        editMenu->addAction(editorActions.actionEditGotoActor);
        editMenu->addSeparator();
        editMenu->addAction(editorActions.actionEditMapProperties);
        editMenu->addAction(editorActions.actionEditMapLibraries);
        editMenu->addAction(editorActions.actionEditMapEvents);
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
        fileToolBar->addAction(EditorActions::GetInstance().actionFileNewMap);
        fileToolBar->addAction(EditorActions::GetInstance().actionFileOpenMap);
        fileToolBar->addAction(EditorActions::GetInstance().actionFileSaveMap);
        addToolBar(fileToolBar);

        editToolBar = new QToolBar(this);
        editToolBar->setObjectName("EditToolBar");
        editToolBar->setWindowTitle(tr("Edit Toolbar"));
        editToolBar->setMinimumWidth(4);
        editToolBar->addAction(EditorActions::GetInstance().actionEditDuplicateActor);
        editToolBar->addAction(EditorActions::GetInstance().actionEditDeleteActor);
        editToolBar->addAction(EditorActions::GetInstance().actionEditGotoActor);
        editToolBar->addAction(EditorActions::GetInstance().actionEditGroundClampActors);
        editToolBar->addAction(EditorActions::GetInstance().actionToggleTerrainPaging);
        //editToolBar->addAction(EditorActions::GetInstance().actionEditTaskEditor);
        addToolBar(editToolBar);

        undoToolBar = new QToolBar(this);
        undoToolBar->setObjectName("UndoToolBar");
        undoToolBar->setWindowTitle(tr("Undo Toolbar"));
        undoToolBar->addAction(EditorActions::GetInstance().actionEditUndo);
        undoToolBar->addAction(EditorActions::GetInstance().actionEditRedo);
        addToolBar(undoToolBar);

        selectionToolBar = new QToolBar(this);
        selectionToolBar->setObjectName("SelectionToolBar");
        selectionToolBar->setWindowTitle(tr("Selection Toolbar"));
        selectionToolBar->addAction(EditorActions::GetInstance().actionSelectionCamera);
        selectionToolBar->addAction(EditorActions::GetInstance().actionSelectionSelectActor);
        selectionToolBar->addAction(EditorActions::GetInstance().actionSelectionTranslateActor);
        selectionToolBar->addAction(EditorActions::GetInstance().actionSelectionRotateActor);
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
        ViewportManager &vpMgr = ViewportManager::GetInstance();

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
        bool hasCurrentMap = (EditorData::GetInstance().getCurrentMap() != NULL);
        bool hasBoth = hasProject && hasCurrentMap;

        EditorActions::GetInstance().actionFileNewMap->setEnabled(hasProject);
        EditorActions::GetInstance().actionFileOpenMap->setEnabled(hasProject);
        EditorActions::GetInstance().actionFileCloseMap->setEnabled(hasBoth);
        EditorActions::GetInstance().actionFileSaveMap->setEnabled(hasBoth);
        EditorActions::GetInstance().actionFileSaveMapAs->setEnabled(hasBoth);
        EditorActions::GetInstance().actionFileExit->setEnabled(true);

        EditorActions::GetInstance().actionEditDuplicateActor->setEnabled(false);
        EditorActions::GetInstance().actionEditDeleteActor->setEnabled(false);
        EditorActions::GetInstance().actionEditGotoActor->setEnabled(false);
        EditorActions::GetInstance().actionEditGroundClampActors->setEnabled(false);
        EditorActions::GetInstance().actionEditMapProperties->setEnabled(hasBoth);
        EditorActions::GetInstance().actionEditMapLibraries->setEnabled(hasBoth);
        EditorActions::GetInstance().actionEditTaskEditor->setEnabled(hasBoth);

        EditorActions::GetInstance().actionSelectionCamera->setEnabled(hasBoth);
        EditorActions::GetInstance().actionSelectionSelectActor->setEnabled(hasBoth);
        EditorActions::GetInstance().actionSelectionTranslateActor->setEnabled(hasBoth);
        EditorActions::GetInstance().actionSelectionRotateActor->setEnabled(hasBoth);

        EditorActions::GetInstance().actionWindowsActorSearch->setEnabled(hasBoth);
        EditorActions::GetInstance().actionWindowsPropertyEditor->setEnabled(hasBoth);
        EditorActions::GetInstance().actionWindowsResourceBrowser->setEnabled(hasBoth);
        EditorActions::GetInstance().actionWindowsResetWindows->setEnabled(hasBoth);

        EditorActions::GetInstance().actionHelpAboutEditor->setEnabled(true);
        EditorActions::GetInstance().actionHelpAboutQT->setEnabled(true);

        // enable main window areas
        propertyWindow->setEnabled(hasBoth);
        actorTab->setEnabled(hasBoth);
        resourceBrowser->setEnabled(hasProject);
        mainViewportParent->setEnabled(hasBoth);
        editMenu->setEnabled(hasBoth);
        windowMenu->setEnabled(hasBoth);
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

        EditorActions::GetInstance().actionWindowsPropertyEditor->setChecked(true);
        EditorActions::GetInstance().actionWindowsActorSearch->setChecked(true);
        EditorActions::GetInstance().actionWindowsResourceBrowser->setChecked(true);

        addDockWidget(Qt::LeftDockWidgetArea,  propertyWindow);
        addDockWidget(Qt::LeftDockWidgetArea,  actorTab);
        addDockWidget(Qt::RightDockWidgetArea, resourceBrowser);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onEditorInitiated()
    {
        setUpdatesEnabled(false);
        try
        {
           enableActions();
   
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
                       startWaitCursor();
                       dtDAL::Project::GetInstance().SetContext(contextName);
                       EditorData::GetInstance().setCurrentProjectContext(contextName);
                       EditorData::GetInstance().addRecentProject(contextName);
                       EditorEvents::GetInstance().emitProjectChanged();
                       EditorActions::GetInstance().refreshRecentProjects();
                       endWaitCursor();
                   }
                   catch (dtUtil::Exception &e)
                   {
                       endWaitCursor();
                       QMessageBox::critical((QWidget *)this,
                           tr("Error"), tr(e.What().c_str()), tr("OK"));
                   }
               }
           }
           else
           {
               projectsExist = true;
   
               startWaitCursor();
               EditorEvents::GetInstance().emitProjectChanged();
               endWaitCursor();
           }
   
           startWaitCursor();
           EditorActions::GetInstance().refreshRecentProjects();
           endWaitCursor();
   
           if(!EditorData::GetInstance().getLoadLastMap())
           {
               return;
           }
   
           if(projectsExist)
           {
               std::vector<std::string> maps = findRecentMaps();
   
               for(unsigned int i = 0; i < maps.size(); i++)
                   checkAndLoadBackup(maps[i]);
           }
   
           EditorActions::GetInstance().getTimer()->start();
   
           updateWindowTitle();
           perspView->onEditorPreferencesChanged();
           findAndLoadPreferences();

           setUpdatesEnabled(true);
        }
        catch (dtUtil::Exception& ex)
        {
           setUpdatesEnabled(true);
           throw ex;
        }   
        catch (std::exception& ex)
        {
           setUpdatesEnabled(true);
           throw ex;
        }

        update();
        repaint();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onEditorShutDown()
    {
        EditorData &editorData = EditorData::GetInstance();
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
            settings.setValue(EditorSettings::SAVE_MILLISECONDS,EditorActions::GetInstance().saveMilliSeconds);
            settings.setValue(EditorSettings::SELECTION_COLOR,editorData.getSelectionColor());
        settings.endGroup();

        //Save the recent projects unless the user does not wish to do so.
        if(!EditorData::GetInstance().getLoadLastProject())
            return;

        //Save the current project state...
        settings.beginGroup(EditorSettings::RECENT_PROJECTS);
            if(!EditorData::GetInstance().getRecentProjects().empty() &&
                EditorData::GetInstance().getRecentProjects().front() != "")
            {
                settings.setValue(EditorSettings::RECENT_PROJECT0,
                    QVariant(QString(EditorData::GetInstance().getRecentProjects().front().c_str())));
                EditorData::GetInstance().getRecentProjects().pop_front();
            }
        settings.endGroup();

        //Check to see if the user wants the app to remember the recently loaded map.
        if(!EditorData::GetInstance().getLoadLastMap() ||
           !EditorData::GetInstance().getCurrentMap())
        {
            // Error check, if they have a previous settings file with a recent map in it, it
            // needs to be deleted as to not load it next time
            settings.remove(EditorSettings::RECENT_MAPS);
            return;
        }

        //Save the current map state...
        settings.beginGroup(EditorSettings::RECENT_MAPS);
            if(!EditorData::GetInstance().getRecentMaps().empty() &&
                EditorData::GetInstance().getRecentMaps().front() != "")
            {
                settings.setValue(EditorSettings::RECENT_MAP0,
                    QVariant(QString(EditorData::GetInstance().getRecentMaps().front().c_str())));
                EditorData::GetInstance().getRecentMaps().pop_front();
            }
        settings.endGroup();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onActorPropertyChanged(dtCore::RefPtr<dtDAL::ActorProxy> proxy,
            dtCore::RefPtr<dtDAL::ActorProperty> property)
    {
        if (!dtDAL::Project::GetInstance().IsContextValid())
            return;

        EditorData::GetInstance().getCurrentMap()->SetModified(true);
        updateWindowTitle();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::closeEvent(QCloseEvent *e)
    {
        EditorActions::GetInstance().wasCancelled = false;

        dtDAL::Map *curMap = dtEditQt::EditorData::GetInstance().getCurrentMap();
        if(curMap == NULL)
        {
            EditorEvents::GetInstance().emitEditorCloseEvent();
            e->accept();
            return;
        }

        dtEditQt::EditorActions::GetInstance().slotFileExit();
        EditorActions::GetInstance().wasCancelled ? e->ignore() : e->accept();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onPropertyEditorSelection()
    {
        propertyWindow->setVisible(EditorActions::GetInstance().actionWindowsPropertyEditor->isChecked());
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onActorSearchSelection()
    {
        actorTab->setVisible(EditorActions::GetInstance().actionWindowsActorSearch->isChecked());
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onResourceBrowserSelection()
    {
        resourceBrowser->setVisible(EditorActions::GetInstance().actionWindowsResourceBrowser->isChecked());
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::updateWindowTitle()
    {
        setWindowTitle(EditorActions::GetInstance().getWindowName().c_str());
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onActorProxyDestroyed(dtCore::RefPtr<dtDAL::ActorProxy> proxy)
    {
        EditorData::GetInstance().getCurrentMap()->SetModified(true);
        updateWindowTitle();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onActorProxyNameChanged(dtCore::RefPtr<dtDAL::ActorProxy> proxy, std::string oldName)
    {
        EditorData::GetInstance().getCurrentMap()->SetModified(true);
        updateWindowTitle();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onActorProxyCreated(dtCore::RefPtr<dtDAL::ActorProxy> proxy, bool forceNoAdjustments)
    {
        EditorData::GetInstance().getCurrentMap()->SetModified(true);
        updateWindowTitle();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::onMapPropertyChanged()
    {
        EditorData::GetInstance().getCurrentMap()->SetModified(true);
        updateWindowTitle();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::setWindowMenuTabsChecked()
    {
        EditorActions::GetInstance().actionWindowsPropertyEditor->setChecked(propertyWindow->isVisible());
        EditorActions::GetInstance().actionWindowsActorSearch->setChecked(actorTab->isVisible());
        EditorActions::GetInstance().actionWindowsResourceBrowser->setChecked(resourceBrowser->isVisible());
    }

    ///////////////////////////////////////////////////////////////////////////////
    void MainWindow::connectSlots()
    {
        EditorActions &editorActions = EditorActions::GetInstance();

        connect(editorActions.actionWindowsPropertyEditor, SIGNAL(triggered()),
            this, SLOT(onPropertyEditorSelection()));
        connect(editorActions.actionWindowsActorSearch,    SIGNAL(triggered()),
            this, SLOT(onActorSearchSelection()));
        connect(editorActions.actionWindowsResourceBrowser,SIGNAL(triggered()),
            this, SLOT(onResourceBrowserSelection()));
        connect(&EditorEvents::GetInstance(), SIGNAL(editorInitiationEvent()),
            this, SLOT(onEditorInitiated()));
        connect(&EditorEvents::GetInstance(), SIGNAL(editorCloseEvent()),
            this, SLOT(onEditorShutDown()));
        connect(&EditorEvents::GetInstance(), SIGNAL(projectChanged()),
            this, SLOT(enableActions()));
        connect(&EditorEvents::GetInstance(), SIGNAL(currentMapChanged()),
            this, SLOT(enableActions()));
        connect(&EditorEvents::GetInstance(), SIGNAL(projectChanged()),
            this, SLOT(updateWindowTitle()));
        connect(&EditorEvents::GetInstance(), SIGNAL(currentMapChanged()),
            this, SLOT(updateWindowTitle()));
        connect(&EditorEvents::GetInstance(), SIGNAL(mapLibraryImported()),
            this, SLOT(updateWindowTitle()));
        connect(&EditorEvents::GetInstance(), SIGNAL(mapLibraryRemoved()),
            this, SLOT(updateWindowTitle()));
        connect(&EditorEvents::GetInstance(), SIGNAL(mapPropertyChanged()),
            this, SLOT(onMapPropertyChanged()));
        connect(editorActions.actionWindowsResetWindows, SIGNAL(triggered()),
            this, SLOT(onResetWindows()));
        connect(&EditorEvents::GetInstance(), SIGNAL(actorProxyDestroyed(ActorProxyRefPtr)),
            this, SLOT(onActorProxyDestroyed(ActorProxyRefPtr)));
        connect(&EditorEvents::GetInstance(), SIGNAL(actorProxyCreated(ActorProxyRefPtr, bool)),
            this, SLOT(onActorProxyCreated(ActorProxyRefPtr, bool)));
        connect(&EditorEvents::GetInstance(),
            SIGNAL(actorPropertyChanged(ActorProxyRefPtr, ActorPropertyRefPtr)),
            this, SLOT(onActorPropertyChanged(ActorProxyRefPtr, ActorPropertyRefPtr)));
        connect(&EditorEvents::GetInstance(), SIGNAL(proxyNameChanged(ActorProxyRefPtr, std::string)), 
           this, SLOT(onActorProxyNameChanged(ActorProxyRefPtr, std::string)));
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    PropertyEditor& MainWindow::GetPropertyEditor()
    {
      return *propertyWindow;
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
            EditorData::GetInstance().setLoadLastProject(loadProjs);

            if(loadProjs)
            {
                if(settings.contains(EditorSettings::LOAD_RECENT_MAPS))
                {
                    bool loadMaps = settings.value(EditorSettings::LOAD_RECENT_MAPS,
                        QVariant(true)).toBool();
                    EditorData::GetInstance().setLoadLastMap(loadMaps);
                }
            }
            else
                EditorData::GetInstance().setLoadLastMap(false);
        }
        else
        {
            EditorData::GetInstance().setLoadLastProject(true);
            EditorData::GetInstance().setLoadLastMap(true);
        }

        if(settings.contains(EditorSettings::RIGID_CAMERA))
        {
            bool rigidCamera = settings.value(EditorSettings::RIGID_CAMERA, QVariant(true)).toBool();
            EditorData::GetInstance().setRigidCamera(rigidCamera);
        }

        if(settings.contains(EditorSettings::SAVE_MILLISECONDS))
        {
            int ms = settings.value(EditorSettings::SAVE_MILLISECONDS, QVariant(300000)).toInt();
            EditorActions::GetInstance().saveMilliSeconds = ms;
            EditorActions::GetInstance().getTimer()->setInterval(ms);
        }

        if (settings.contains(EditorSettings::SELECTION_COLOR))
        {
            QColor color = qvariant_cast<QColor>(settings.value(EditorSettings::SELECTION_COLOR));
            EditorData::GetInstance().setSelectionColor(color);
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

                if(dtUtil::FileUtils::GetInstance().DirExists(project))
                {
                    EditorData::GetInstance().addRecentProject(project);
                    if(EditorData::GetInstance().getLoadLastProject())
                    {
                        EditorData::GetInstance().setCurrentProjectContext(project);
                        dtDAL::Project::GetInstance().SetContext(project);
                    }
                    projects.push_back(project);
                }
                else
                {
                    QMessageBox::critical(this, tr("Failed to load previous context"),
                        tr("Failed to load the previous project context.\n") +
                        tr("This can happen if the last project context\n has been moved, renamed, or deleted."),
                        tr("OK"));

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
                startWaitCursor();
                dtDAL::Map &backupMap =
                    dtDAL::Project::GetInstance().OpenMapBackup(str);

                EditorActions::GetInstance().changeMaps(
                    EditorData::GetInstance().getCurrentMap(), &backupMap);
                EditorData::GetInstance().addRecentMap(backupMap.GetName());
                endWaitCursor();
            }
            else if(result == 1)
            {
                /*if(!dtUtil::FileUtils::GetInstance().FileExists(str))
                {
                    QMessageBox::critical(this, tr("Error loading map"),
                        tr("The map ") + str.c_str() + tr(" does not exist or has become corrupted, backup file still retained."));
                    return;
                }
                else
                    dtDAL::Project::GetInstance().ClearBackup(str);*/

                startWaitCursor();
                dtDAL::Project::GetInstance().ClearBackup(str);
                EditorActions::GetInstance().changeMaps(EditorData::GetInstance().getCurrentMap(),
                   &dtDAL::Project::GetInstance().GetMap(str));
                EditorData::GetInstance().addRecentMap(str);
                endWaitCursor();
            }
            else
                return;
        }
        else
        {
            startWaitCursor();
            dtDAL::Map &m = dtDAL::Project::GetInstance().GetMap(str);
            EditorActions::GetInstance().changeMaps(
                EditorData::GetInstance().getCurrentMap(), &m);
            EditorData::GetInstance().addRecentMap(m.GetName());
            endWaitCursor();
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
