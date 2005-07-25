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
* @author Curtiss Murphy
*/
#include "dtEditQt/editoractions.h"

#include <QApplication>
#include <QAction>
#include <QIcon>
#include <QActionGroup>
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QSettings>
#include <QTimer>

#include <osgDB/FileNameUtils>

#include "dtEditQt/global.h"
#include "dtEditQt/uiresources.h"
#include "dtEditQt/editordata.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/viewportmanager.h"
#include "dtEditQt/viewportoverlay.h"
#include "dtEditQt/editoraboutbox.h"
#include "dtEditQt/libraryeditor.h"
#include "dtEditQt/camera.h"
#include "dtEditQt/projectcontextdialog.h"
#include "dtEditQt/mapdialog.h"
#include "dtEditQt/dialogmapproperties.h"
#include "dtEditQt/dialoglistselection.h"
#include "dtEditQt/mapsaveasdialog.h"
#include "dtEditQt/mainwindow.h"
#include "dtEditQt/preferencesdialog.h"
#include "dtEditQt/undomanager.h"

#include "dtDAL/log.h"
#include "dtDAL/project.h"
#include "dtDAL/map.h"
#include "dtDAL/exception.h"
#include "dtDAL/transformableactorproxy.h"
#include "dtDAL/fileutils.h"
#include "dtDAL/intersectionquery.h"
#include "dtDAL/actorproxy.h"
#include "dtDAL/actorproxyicon.h"

#include <sstream>

namespace dtEditQt
{
    //Singleton global variable for the library manager.
    osg::ref_ptr<EditorActions> EditorActions::instance(NULL);

    ///////////////////////////////////////////////////////////////////////////////
    EditorActions::EditorActions()
    {
        LOG_INFO("Initializing Editor Actions.");
        setupFileActions();
        setupEditActions();
        setupSelectionActions();
        setupWindowActions();
        setupHelpActions();
        setupRecentItems();

        saveMilliSeconds = 300000;
        wasCancelled = false;

        connect(&EditorEvents::getInstance(), SIGNAL(projectChanged()),
                 this, SLOT(slotPauseAutosave()));
        connect(&EditorEvents::getInstance(), SIGNAL(currentMapChanged()),
                 this, SLOT(slotRestartAutosave()));

        connect(&EditorEvents::getInstance(),
            SIGNAL(selectedActors(std::vector< osg::ref_ptr<dtDAL::ActorProxy> >&)),
            this,
            SLOT(slotSelectedActors(std::vector< osg::ref_ptr<dtDAL::ActorProxy> >&)));

        timer = new QTimer((QWidget*)EditorData::getInstance().getMainWindow());
        timer->setInterval(saveMilliSeconds);
        connect(timer, SIGNAL(timeout()), this, SLOT(slotAutosave()));
    }

    ///////////////////////////////////////////////////////////////////////////////
    EditorActions::~EditorActions()
    {

    }

    ///////////////////////////////////////////////////////////////////////////////
    EditorActions &EditorActions::getInstance()
    {
        if (EditorActions::instance.get() == NULL)
            EditorActions::instance = new EditorActions();
        return *(EditorActions::instance.get());
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotSelectedActors(std::vector< osg::ref_ptr<dtDAL::ActorProxy> > &newActors)
    {
        actors.clear();
        actors.reserve(newActors.size());
        for(unsigned int i = 0; i < newActors.size(); i++)
            actors.push_back(newActors[i]);
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::setupFileActions()
    {
        //File - New Map...
        actionFileNewMap = new QAction(QIcon(UIResources::ICON_FILE_NEW_MAP.c_str()),
            tr("&New Map..."),this);
        actionFileNewMap->setShortcut(tr("Ctrl+N"));
        actionFileNewMap->setStatusTip(tr("Create a new map."));
        connect(actionFileNewMap, SIGNAL(triggered()), this, SLOT(slotFileNewMap()));

        //File - Open Map...
        actionFileOpenMap = new QAction(QIcon(UIResources::ICON_FILE_OPEN_MAP.c_str()),
            tr("&Open Map..."),this);
        actionFileOpenMap->setShortcut(tr("Ctrl+O"));
        actionFileOpenMap->setStatusTip(tr("Open an existing map in this project."));
        connect(actionFileOpenMap, SIGNAL(triggered()), this, SLOT(slotFileOpenMap()));

        //File - Close Map
        actionFileCloseMap = new QAction(tr("Close Map"), this);
        actionFileCloseMap->setStatusTip(tr("Close the currently opened map"));
        connect(actionFileCloseMap, SIGNAL(triggered()), this, SLOT(slotFileCloseMap()));

        //File - Save Map...
        actionFileSaveMap = new QAction(QIcon(UIResources::ICON_FILE_SAVE.c_str()),
            tr("&Save Map"),this);
        actionFileSaveMap->setShortcut(tr("Ctrl+S"));
        actionFileSaveMap->setStatusTip(tr("Save the current map."));
        connect(actionFileSaveMap, SIGNAL(triggered()), this, SLOT(slotFileSaveMap()));

        //File - Save Map As...
        actionFileSaveMapAs = new QAction(tr("Save Map &As..."),this);
        actionFileSaveMapAs->setStatusTip(tr("Save the current map under a different file."));
        connect(actionFileSaveMapAs, SIGNAL(triggered()), this, SLOT(slotFileSaveMapAs()));

        // File - Change Project
        actionFileChangeProject = new QAction(tr("&Change Project..."), this);
        actionFileChangeProject->setStatusTip(tr("Change the current project context."));
        connect(actionFileChangeProject, SIGNAL(triggered()), this, SLOT(slotProjectChangeContext()));

        //File - Map Properties Editor...
        actionEditMapProperties = new QAction(tr("Map &Properties..."), this);
        actionEditMapProperties->setStatusTip(tr("Edit the properties of the current map."));
        connect(actionEditMapProperties, SIGNAL(triggered()), this, SLOT(slotEditMapProperties()));

        //File - Map Libraries Editor...
        actionEditMapLibraries = new QAction(tr("Map &Libraries..."), this);
        actionEditMapLibraries->setStatusTip(tr("Add and remove actor libraries from the current map."));
        connect(actionEditMapLibraries, SIGNAL(triggered()), this, SLOT(slotEditMapLibraries()));

        actionFileEditPreferences = new QAction(tr("Preferences..."), this);
        actionFileEditPreferences->setStatusTip(tr("Edit editor preferences"));
        connect(actionFileEditPreferences, SIGNAL(triggered()), this, SLOT(slotFileEditPreferences()));

        //File - Exit...
        actionFileExit = new QAction(tr("E&xit"), this);
        actionFileExit->setShortcut(tr("Alt+F4"));
        actionFileExit->setStatusTip(tr("Exit the level editor."));
        connect(actionFileExit, SIGNAL(triggered()), this, SLOT(slotFileExit()));
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::setupEditActions()
    {
        // Edit - Duplicate Actors...
        actionEditDuplicateActor = new QAction(QIcon(UIResources::ICON_EDIT_DUPLICATE.c_str()),
            tr("Du&plicate Selection"), this);
        actionEditDuplicateActor->setShortcut(tr("Ctrl+D"));
        actionEditDuplicateActor->setStatusTip(tr("Duplicates the current actor selection."));
        connect(actionEditDuplicateActor, SIGNAL(triggered()),
                this, SLOT(slotEditDuplicateActors()));

        // Edit - Delete Actors...
        actionEditDeleteActor = new QAction(QIcon(UIResources::ICON_EDIT_DELETE.c_str()),
            tr("&Delete Selection"), this);
        //actionEditDeleteActor->setShortcut(tr("delete"));
        actionEditDeleteActor->setStatusTip(tr("Deletes the current actor selection."));
        connect(actionEditDeleteActor, SIGNAL(triggered()),
                this, SLOT(slotEditDeleteActors()));

        // Edit - Ground Clamp Actors.
        actionEditGroundClampActors = new QAction(QIcon(UIResources::ICON_GROUND_CLAMP.c_str()),
            tr("&Ground Clamp"),this);
        actionEditGroundClampActors->setShortcut(tr("Ctrl+G"));
        actionEditGroundClampActors->setStatusTip(
            tr("Moves the currently selected actors' Z value to be in line with whatever is below them."));
        connect(actionEditGroundClampActors,SIGNAL(triggered()),
                this,SLOT(slotEditGroundClampActors()));

        // Edit - Goto Actor
        actionEditGotoActor = new QAction(QIcon(UIResources::LARGE_ICON_EDIT_GOTO.c_str()),
            tr("Goto Actor"), this);
        actionEditGotoActor->setStatusTip(tr("Places the camera at the selected actor."));
        connect(actionEditGotoActor, SIGNAL(triggered()), this, SLOT(slotEditGotoActor()));

        // Edit - Undo
        actionEditUndo = new QAction(QIcon(UIResources::ICON_EDIT_UNDO.c_str()), tr("&Undo"),this);
        actionEditUndo->setShortcut(tr("Ctrl+Z"));
        actionEditUndo->setStatusTip(tr("Undoes the last property edit, actor delete, or actor creation."));
        connect(actionEditUndo,SIGNAL(triggered()),this,SLOT(slotEditUndo()));

        // Edit - Redo
        actionEditRedo = new QAction(QIcon(UIResources::ICON_EDIT_REDO.c_str()), tr("&Redo"),this);
        actionEditRedo->setShortcut(tr("Ctrl+Y"));
        actionEditRedo->setStatusTip(tr("Redoes the previous property edit, actor delete, or actor creation undo command."));
        connect(actionEditRedo,SIGNAL(triggered()),this,SLOT(slotEditRedo()));
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::setupSelectionActions()
    {
        //The actor selection tools need to be in an action group since they are
        //mutually exclusive.
        modeToolsGroup = new QActionGroup(this);

        actionSelectionCamera =
            new QAction(QIcon(UIResources::ICON_TOOLMODE_CAMERA.c_str()),tr("&Camera"), modeToolsGroup);
        actionSelectionCamera->setCheckable(true);
        actionSelectionCamera->setActionGroup(modeToolsGroup);
        actionSelectionCamera->setStatusTip(tr("Use this tool to navigate through the world."));

        actionSelectionSelectActor =
            new QAction(QIcon(UIResources::ICON_TOOLMODE_SELECT.c_str()),tr("&Select Actor(s)"), modeToolsGroup);
        actionSelectionSelectActor->setCheckable(true);
        actionSelectionSelectActor->setActionGroup(modeToolsGroup);
        actionSelectionSelectActor->setStatusTip(tr("Use this tool to pick actors in the scene."));

        actionSelectionTranslateActor =
            new QAction(QIcon(UIResources::ICON_TOOLMODE_TRANSLATE.c_str()),tr("&Translate Actor(s)"), modeToolsGroup);
        actionSelectionTranslateActor->setCheckable(true);
        actionSelectionTranslateActor->setActionGroup(modeToolsGroup);
        actionSelectionTranslateActor->setStatusTip(tr("Use this tool to move the current actor selection."));

        actionSelectionRotateActor =
            new QAction(QIcon(UIResources::ICON_TOOLMODE_ROTATE.c_str()),tr("&Rotate Actor(s)"), modeToolsGroup);
        actionSelectionRotateActor->setCheckable(true);
        actionSelectionRotateActor->setActionGroup(modeToolsGroup);
        actionSelectionRotateActor->setStatusTip(tr("Use this tool to rotate the current actor selection."));
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::setupWindowActions()
    {
        actionWindowsPropertyEditor = new QAction(tr("Property Editor"), this);
        actionWindowsPropertyEditor->setShortcut(tr("Alt+1"));
        actionWindowsPropertyEditor->setStatusTip(tr("Hides and retrieves the actor property editor"));
        actionWindowsPropertyEditor->setCheckable(true);
        actionWindowsPropertyEditor->setChecked(true);

        actionWindowsActorSearch = new QAction(tr("Actor Search"), this);
        actionWindowsActorSearch->setShortcut(tr("Alt+2"));
        actionWindowsActorSearch->setStatusTip(tr("Hides and retrieves the actor search window"));
        actionWindowsActorSearch->setCheckable(true);
        actionWindowsActorSearch->setChecked(true);

        actionWindowsResourceBrowser = new QAction(tr("Resource Broswer"), this);
        actionWindowsResourceBrowser->setShortcut(tr("Alt+3"));
        actionWindowsResourceBrowser->setStatusTip(tr("Hides and retrieves the resource browser"));
        actionWindowsResourceBrowser->setCheckable(true);
        actionWindowsResourceBrowser->setChecked(true);

        actionWindowsResetWindows = new QAction(tr("Reset Docking Windows"), this);
        actionWindowsResetWindows->setShortcut(tr("Ctrl+R"));
        actionWindowsResetWindows->setStatusTip(tr("Restores the docking windows to a default state"));
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::setupHelpActions()
    {
        // Help - About Editor
        actionHelpAboutEditor = new QAction(tr("&About Delta3D Editor..."), this);
        actionHelpAboutEditor->setStatusTip(tr("About Delta3D Editor"));
        connect(actionHelpAboutEditor, SIGNAL(triggered()), this, SLOT(slotHelpAboutEditor()));

        // Help - About QT
        actionHelpAboutQT = new QAction(tr("A&bout QT..."), this);
        actionHelpAboutQT->setStatusTip(tr("About QT"));
        connect(actionHelpAboutQT, SIGNAL(triggered()), this, SLOT(slotHelpAboutQT()));
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::setupRecentItems()
    {
        //std::vector<std::string> projs = EditorData::getInstance().getMainWindow()->findRecentProjects();
        std::list<std::string>::iterator itor;

        actionFileRecentProject0 = new QAction(tr("(Recent Project 1)"), this);
        actionFileRecentProject0->setEnabled(false);
        connect(actionFileRecentProject0, SIGNAL(triggered()), this, SLOT(slotFileRecentProject0()));

        itor = EditorData::getInstance().getRecentProjects().begin();
        if(itor == EditorData::getInstance().getRecentProjects().end())
            return;

        if(EditorData::getInstance().getRecentProjects().size() > 0)
        {
            std::string str = (*itor);
            actionFileRecentProject0->setText((*itor).c_str());
            actionFileRecentProject0->setEnabled(true);
            connect(actionFileRecentProject0, SIGNAL(triggered()), this, SLOT(slotFileRecentProject0()));
            ++itor;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorActions::refreshRecentProjects()
    {
        // do we have any recent projects?
        if(EditorData::getInstance().getRecentProjects().size() == 0)
            return;
        // set the text on the actions now
        std::list<std::string>::iterator itor = EditorData::getInstance().getRecentProjects().begin();
        if(itor == EditorData::getInstance().getRecentProjects().end())
            return;

        //unsigned int i = EditorData::getInstance().getRecentProjects().size();
        QString curText = actionFileRecentProject0->text();
        actionFileRecentProject0->setText(tr((*itor).c_str()));
        ++itor;
        actionFileRecentProject0->setEnabled(true);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotFileEditPreferences()
    {
        slotPauseAutosave();
        PreferencesDialog dlg((QWidget*)EditorData::getInstance().getMainWindow());
        if (dlg.exec() == QDialog::Accepted)
            EditorEvents::getInstance().emitEditorPreferencesChanged();
        slotRestartAutosave();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorActions::refreshRecentMaps()
    {

    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotFileNewMap()
    {
        int saveResult = saveCurrentMapChanges(true);
        if (saveResult == QMessageBox::Cancel || saveResult == QMessageBox::Abort)
            return;

        slotPauseAutosave();
        MapDialog mapDialog((QWidget*)EditorData::getInstance().getMainWindow());
        if(mapDialog.exec() == QDialog::Accepted) {
            changeMaps(EditorData::getInstance().getCurrentMap().get(),
                mapDialog.getFinalizedMap());
            EditorData::getInstance().addRecentMap(mapDialog.getFinalizedMap()->GetName());
        }

        slotRestartAutosave();
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotFileOpenMap()
    {
        //Make sure we have a valid project.  The project "should" always be
        //valid since this action is only enabled when there is a valid project.
        if(!dtDAL::Project::GetInstance().IsContextValid())
        {
            slotPauseAutosave();
            QMessageBox::critical((QWidget *)EditorData::getInstance().getMainWindow(),
                    tr("Map Open Error"),tr("The current project is not valid."),tr("OK"));
            slotRestartAutosave();
            return;
        }

        //Check the current map for changes and save them...
        int saveResult = saveCurrentMapChanges(true);
        if (saveResult == QMessageBox::Cancel || saveResult == QMessageBox::Abort)
            return;

        slotPauseAutosave();

        DialogListSelection openMapDialog((QWidget *)EditorData::getInstance().getMainWindow(),
            tr("Open Existing Map"),tr("Available Maps"));

        QStringList listItems;
        std::set<std::string> mapNames = dtDAL::Project::GetInstance().GetMapNames();
        for(std::set<std::string>::iterator i=mapNames.begin(); i!=mapNames.end(); ++i)
            listItems << i->c_str();

        openMapDialog.setListItems(listItems);
        if(openMapDialog.exec() == QDialog::Accepted)
        {
            dtDAL::Map *newMap;

            //Attempt to open the specified map..
            try
            {
                EditorData::getInstance().getMainWindow()->startWaitCursor();

                const QString &mapName = openMapDialog.getSelectedItem();
                newMap = &dtDAL::Project::GetInstance().GetMap(mapName.toStdString());

                EditorData::getInstance().getMainWindow()->endWaitCursor();
            }
            catch(dtDAL::Exception &e)
            {
                EditorData::getInstance().getMainWindow()->endWaitCursor();

                QString error = "An error occured while opening the map. ";
                error += e.What().c_str();
                LOG_ERROR(error.toStdString());
                QMessageBox::critical((QWidget *)EditorData::getInstance().getMainWindow(),
                    tr("Map Open Error"),error,tr("OK"));

                slotRestartAutosave();
                return;
            }

            //Finally, change to the requested map.
            osg::ref_ptr<dtDAL::Map> mapRef = newMap;
            EditorData::getInstance().getMainWindow()->checkAndLoadBackup(newMap->GetName());
            newMap->SetModified(false);
        }

        slotRestartAutosave();
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotFileCloseMap()
    {
        // no map open? peace out
        if(!EditorData::getInstance().getCurrentMap().valid())
            return;

        saveCurrentMapChanges(EditorData::getInstance().getCurrentMap()->IsModified());

        changeMaps(EditorData::getInstance().getCurrentMap().get(), NULL);
        EditorData::getInstance().getMainWindow()->enableActions();
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotFileSaveMap()
    {
        LOG_INFO("Saving current map.");

        //Save the current map without asking the user for permission.
        saveCurrentMapChanges(false);
        slotRestartAutosave();
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotFileSaveMapAs()
    {
        slotPauseAutosave();

        MapSaveAsDialog dlg;
        if(dlg.exec() == QDialog::Rejected)
        {
            slotRestartAutosave();
            return;
        }

        std::string strippedName = osgDB::getSimpleFileName(dlg.getMapFileName());
        std::string name         = osgDB::getStrippedName(strippedName);

        dtDAL::Map *myMap = EditorData::getInstance().getCurrentMap().get();
        if(myMap == NULL)
        {
            slotRestartAutosave();
            return;
        }

        try
        {
            myMap->SetDescription(dlg.getMapDescription());
            EditorData::getInstance().getMainWindow()->startWaitCursor();
            dtDAL::Project::GetInstance().SaveMapAs(*myMap, name, strippedName);
            EditorData::getInstance().getMainWindow()->endWaitCursor();
        }
        catch(const dtDAL::Exception &e)
        {
            EditorData::getInstance().getMainWindow()->endWaitCursor();
            LOG_ERROR(e.What());
            QMessageBox::critical((QWidget *)EditorData::getInstance().getMainWindow(),
                tr("Error"), QString(e.What().c_str()), tr("OK"));

            slotRestartAutosave();
            return;
        }

        EditorEvents::getInstance().emitCurrentMapChanged();
        slotRestartAutosave();
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotFileExit()
    {
        slotPauseAutosave();

        dtDAL::Map *currMap = dtEditQt::EditorData::getInstance().getCurrentMap().get();
        if(currMap == NULL)
        {
            EditorEvents::getInstance().emitEditorCloseEvent();
            qApp->quit();
            return;
        }

        int result = saveCurrentMapChanges(true);
        if (result == QMessageBox::Abort)
        {
            //An error occurred during saving.
            if(QMessageBox::critical((QWidget*)EditorData::getInstance().getMainWindow(),
                tr("Error"), tr("Continue exiting?"), tr("Yes"), tr("No")) == QMessageBox::Yes)
            {
                EditorEvents::getInstance().emitEditorCloseEvent();
                qApp->quit();
            }

            slotRestartAutosave();
            return;
        }
        else if (result == QMessageBox::Cancel)
        {
            wasCancelled = true;
            slotRestartAutosave();
            return;
        }

        EditorEvents::getInstance().emitEditorCloseEvent();
        qApp->quit();
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotEditMapProperties()
    {
        DialogMapProperties mapPropsDialog((QWidget *)EditorData::getInstance().getMainWindow());
        dtDAL::Map *map = EditorData::getInstance().getCurrentMap().get();

        //If the current map is invalid, issue an error, else populate the dialog
        //box with the values from the current map.  The map "should" always be
        //valid since this action is only enabled when there is a valid map.
        if (map == NULL) {
            QMessageBox::critical((QWidget *)EditorData::getInstance().getMainWindow(),tr("Error"),
                tr("Current map is not valid or no map is open."),tr("OK"));
            return;
        }
        else
        {
            mapPropsDialog.getMapName()->setText(map->GetName().c_str());
            mapPropsDialog.getMapDescription()->setText(map->GetDescription().c_str());
            mapPropsDialog.getMapAuthor()->setText(map->GetAuthor().c_str());
            mapPropsDialog.getMapCopyright()->setText(map->GetCopyright().c_str());
            mapPropsDialog.getMapComments()->setPlainText(map->GetComment().c_str());
        }

        //If the user pressed the ok button, set the new map values.
        if (mapPropsDialog.exec() == QDialog::Accepted)
        {
            map->SetName(mapPropsDialog.getMapName()->text().toStdString());
            map->SetDescription(mapPropsDialog.getMapDescription()->text().toStdString());
            map->SetAuthor(mapPropsDialog.getMapAuthor()->text().toStdString());
            map->SetCopyright(mapPropsDialog.getMapCopyright()->text().toStdString());
            map->SetComment(mapPropsDialog.getMapComments()->toPlainText().toStdString());
        }
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotEditMapLibraries()
    {
        // we need a current map to edit libraries
        if(!EditorData::getInstance().getCurrentMap().valid())
        {
            QMessageBox::critical(NULL, tr("Failure"),
                tr("A map must be open in order to edit libraries"),
                tr("OK"));
            return;
        }

        LibraryEditor libEdit((QWidget *)EditorData::getInstance().getMainWindow());
        libEdit.exec();
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotEditDuplicateActors()
    {
        LOG_INFO("Duplicating current actor selection.");

        ViewportOverlay::ActorProxyList &selection =
                ViewportManager::getInstance().getViewportOverlay()->getCurrentActorSelection();
        dtCore::Scene *scene = ViewportManager::getInstance().getMasterScene();
        osg::ref_ptr<dtDAL::Map> currMap = EditorData::getInstance().getCurrentMap();
        Camera *worldCam = ViewportManager::getInstance().getWorldViewCamera();

        //Make sure we have valid data.
        if (!currMap.valid()) {
            LOG_ERROR("Current map is not valid.");
            return;
        }

        if (scene == NULL) {
            LOG_ERROR("Current scene is not valid.");
            return;
        }

        //Create our offset vector which is used to jitter the cloned
        //proxies providing better feedback to the user.
        osg::Vec3 offset;
        if (worldCam != NULL)
            offset = worldCam->getRightDir() * 5;
        else
            offset = osg::Vec3(5,0,0);

        // We're about to do a LOT of work, especially if lots of things are select
        // so, start a change transaction.
        EditorData::getInstance().getMainWindow()->startWaitCursor();
        EditorEvents::getInstance().emitBeginChangeTransaction();

        //Once we have a reference to the current selection and the scene,
        //clone each proxy, add it to the scene, make the newly cloned
        //proxy(s) the current selection.
        ViewportOverlay::ActorProxyList::iterator itor;
        std::vector<osg::ref_ptr<dtDAL::ActorProxy> > newSelection;
        for (itor=selection.begin(); itor!=selection.end(); ++itor)
        {
            dtDAL::ActorProxy *proxy = const_cast<dtDAL::ActorProxy *>(itor->get());
            osg::ref_ptr<dtDAL::ActorProxy> copy = proxy->Clone();
            if (!copy.valid())
            {
                LOG_ERROR("Error duplicating proxy: " + proxy->GetName());
                continue;
            }

            //Store the original location of the proxy so we can position after
            //it has been added to the scene.
            osg::Vec3 oldPosition;
            dtDAL::TransformableActorProxy *tProxy =
                dynamic_cast<dtDAL::TransformableActorProxy *>(copy.get());
            if (tProxy != NULL)
                oldPosition = tProxy->GetTranslation();

            //Add the new proxy to the map and send out a create event.
            currMap->AddProxy(*copy.get());

            EditorEvents::getInstance().emitActorProxyCreated(copy, false);

            //Move the newly duplicated actor to where it is supposed to go.
            if (tProxy != NULL)
                tProxy->SetTranslation(oldPosition+offset);


            newSelection.push_back(copy);
        }

        //Finally set the newly cloned proxies to be the current selection.
        ViewportManager::getInstance().getViewportOverlay()->setMultiSelectMode(false);
        EditorEvents::getInstance().emitActorsSelected(newSelection);
        EditorEvents::getInstance().emitEndChangeTransaction();

        EditorData::getInstance().getMainWindow()->endWaitCursor();
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotEditDeleteActors()
    {
        LOG_INFO("Deleting current actor selection. ");

        ViewportOverlay::ActorProxyList &selection =
                ViewportManager::getInstance().getViewportOverlay()->getCurrentActorSelection();
        osg::ref_ptr<dtDAL::Map> currMap = EditorData::getInstance().getCurrentMap();

        //Make sure we have valid data.
        if (!currMap.valid()) {
            LOG_ERROR("Current map is not valid.");
            return;
        }

        // We're about to do a LOT of work, especially if lots of things are select
        // so, start a change transaction.
        EditorData::getInstance().getMainWindow()->startWaitCursor();
        EditorEvents::getInstance().emitBeginChangeTransaction();

        //Once we have a reference to the current selection and the scene,
        //remove each proxy's actor from the scene then remove the proxy from
        //the map.
        ViewportOverlay::ActorProxyList::iterator itor;
        for (itor=selection.begin(); itor!=selection.end(); ++itor)
        {
            dtDAL::ActorProxy *proxy = const_cast<dtDAL::ActorProxy *>(itor->get());
            deleteProxy(proxy, currMap);
        }

        //Now that we have removed the selected objects, clear the current selection.
        std::vector<osg::ref_ptr<dtDAL::ActorProxy> > emptySelection;
        EditorEvents::getInstance().emitActorsSelected(emptySelection);
        EditorEvents::getInstance().emitEndChangeTransaction();

        EditorData::getInstance().getMainWindow()->endWaitCursor();
    }


    //////////////////////////////////////////////////////////////////////////////
    bool EditorActions::deleteProxy(dtDAL::ActorProxy *proxy, osg::ref_ptr<dtDAL::Map> currMap)
    {
        dtCore::Scene *scene = ViewportManager::getInstance().getMasterScene();
        bool result = false;

        if (proxy != NULL && scene != NULL)
        {
            osg::ref_ptr<dtDAL::ActorProxy> tempRef = proxy;
            scene->RemoveDrawable(proxy->GetActor());
            if (proxy->GetBillBoardIcon() != NULL)
                scene->RemoveDrawable(proxy->GetBillBoardIcon()->GetDrawable());

            EditorEvents::getInstance().emitActorProxyAboutToBeDestroyed(tempRef);

            if (!currMap->RemoveProxy(*proxy))
            {
                LOG_ERROR("Unable to remove actor proxy: " + proxy->GetName());
            }
            else
            {
                EditorEvents::getInstance().emitActorProxyDestroyed(tempRef);
                result = true;
            }
        }

        return result;
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotEditUndo()
    {
        EditorData::getInstance().getUndoManager().doUndo();
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotEditRedo()
    {
        EditorData::getInstance().getUndoManager().doRedo();
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotEditGroundClampActors()
    {
        LOG_INFO("Ground clamping actors.");

        ViewportOverlay::ActorProxyList &selection =
            ViewportManager::getInstance().getViewportOverlay()->getCurrentActorSelection();
        dtCore::Scene *scene = ViewportManager::getInstance().getMasterScene();

        if (scene == NULL) {
            LOG_ERROR("Current scene is not valid.");
            return;
        }

        // We're about to do a LOT of work, especially if lots of things are select
        // so, start a change transaction.
        EditorData::getInstance().getMainWindow()->startWaitCursor();
        EditorEvents::getInstance().emitBeginChangeTransaction();

        //Iterate through the current selection, trace a ray directly below it.  If there is
        //an intersection, move the current proxy to that point.
        ViewportOverlay::ActorProxyList::iterator itor;
        dtDAL::IntersectionQuery query(scene);

        for (itor=selection.begin(); itor!=selection.end(); ++itor)
        {
            dtDAL::ActorProxy *proxy = const_cast<dtDAL::ActorProxy *>(itor->get());
            dtDAL::TransformableActorProxy *tProxy =
                dynamic_cast<dtDAL::TransformableActorProxy *>(proxy);

            if (tProxy != NULL)
            {
                osg::Vec3 pos = tProxy->GetTranslation();

                query.SetStartPos(pos);
                query.SetDirection(osg::Vec3(0,0,-1));
                query.Reset();

                //Find a possible intersection point.  If we find an intersection
                //point, move the actor to that location.
                if (query.Exec()) {
                    osgUtil::IntersectVisitor &iv = query.GetIntersectVisitor();
                    osg::Vec3 p = iv.getHitList(query.GetLineSegment())[0].getWorldIntersectPoint();
                    tProxy->SetTranslation(p);
                }
            }
        }

        EditorEvents::getInstance().emitEndChangeTransaction();
        EditorData::getInstance().getMainWindow()->endWaitCursor();
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotEditGotoActor()
    {
        if(actors.size() > 0)
            EditorEvents::getInstance().emitGotoActor(actors[0]);
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotProjectChangeContext()
    {
        int result = saveCurrentMapChanges(true);
        if (result == QMessageBox::Cancel || result == QMessageBox::Abort)
            return;

        slotPauseAutosave();
        ProjectContextDialog dialog((QWidget *)EditorData::getInstance().getMainWindow());
        if (dialog.exec() == QDialog::Accepted) {
            std::string contextName = dialog.getProjectPath().toStdString();

            //First try to set the new project context.
            try
            {
                changeMaps(EditorData::getInstance().getCurrentMap().get(),NULL);
                dtDAL::Project::GetInstance().SetContext(contextName);
            }
            catch (dtDAL::Exception &e)
            {
                QMessageBox::critical((QWidget *)EditorData::getInstance().getMainWindow(),
                    tr("Error"), tr(e.What().c_str()), tr("OK"));

                slotRestartAutosave();
                return;
            }

            EditorData::getInstance().setCurrentProjectContext(contextName);
            EditorData::getInstance().addRecentProject(contextName);
            EditorEvents::getInstance().emitProjectChanged();
            refreshRecentProjects();
        }

        slotRestartAutosave();
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotHelpAboutEditor()
    {
        slotPauseAutosave();
        EditorAboutBox box((QWidget *)EditorData::getInstance().getMainWindow());
        box.exec();
        slotRestartAutosave();
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotHelpAboutQT()
    {
        slotPauseAutosave();
        QMessageBox::aboutQt((QWidget *)EditorData::getInstance().getMainWindow());
        slotRestartAutosave();
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotAutosave()
    {
        try
        {
            if(EditorData::getInstance().getCurrentMap().valid())
            {
                dtDAL::Project::GetInstance().SaveMapBackup(*EditorData::getInstance().getCurrentMap());
            }
        }
        catch(const dtDAL::Exception &e)
        {
            QMessageBox::critical(NULL, tr("Error"), e.What().c_str(), tr("OK"));
        }
    }

    //////////////////////////////////////////////////////////////////////////////
    std::string EditorActions::getWindowName()
    {
        ((QMainWindow*)EditorData::getInstance().getMainWindow())->windowTitle().clear();
        std::string name = "Delta3D Editor";
        std::string projDir;
        std::string temp = dtDAL::Project::GetInstance().GetContext();
        if(temp.empty())
            return name;
        unsigned int index = temp.find_last_of(dtDAL::FileUtils::PATH_SEPARATOR);
        projDir = temp.substr(index+1);
        name += " - ";
        name += projDir;

        // if we have a map, append the name
        osg::ref_ptr<dtDAL::Map> map = EditorData::getInstance().getCurrentMap();
        if (map.valid())
        {
            name += " - " + map->GetName();

            // if modified, append the '*'
            if (map->IsModified())
            {
                name += "*";
            }
        }

        return name;
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotPauseAutosave()
    {
        timer->stop();
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotRestartAutosave()
    {
        timer->start();
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotFileRecentProject0()
    {
        if (saveCurrentMapChanges(true) == QMessageBox::Cancel)
            return;

        changeMaps(EditorData::getInstance().getCurrentMap().get(),NULL);

        EditorData::getInstance().setCurrentProjectContext(actionFileRecentProject0->text().toStdString());
        dtDAL::Project::GetInstance().SetContext(actionFileRecentProject0->text().toStdString());
        EditorEvents::getInstance().emitProjectChanged();
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotFileRecentMap0()
    {
        if (saveCurrentMapChanges(true) == QMessageBox::Cancel)
            return;

        const std::string &newMapName = actionFileRecentMap0->text().toStdString();
        dtDAL::Map *newMap;
        try
        {
            newMap = &dtDAL::Project::GetInstance().GetMap(newMapName);
        }
        catch (dtDAL::Exception &e)
        {
            QMessageBox::critical((QWidget *)EditorData::getInstance().getMainWindow(),
                    tr("Map Open Error"), e.What().c_str(), tr("OK"));
            return;
        }

        changeMaps(EditorData::getInstance().getCurrentMap().get(),newMap);
    }


    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::changeMaps(dtDAL::Map *oldMap, dtDAL::Map *newMap)
    {
        //Make sure the two maps are different!  If they are the same, then
        //we need to close the map but make sure to re-open it.
        bool areSameMap = (oldMap == newMap);
        std::string oldMapName;

        //Make sure to catch this goofy state...
        if (oldMap == NULL && newMap == NULL)
            return;

        //Remove all the old map drawables from the current scene.
        if (oldMap != NULL)
        {
            EditorData::getInstance().getMainWindow()->startWaitCursor();
            ViewportManager::getInstance().clearMasterScene(oldMap->GetAllProxies());
            oldMapName = oldMap->GetName();

            //Close the old map...
            try
            {
                EditorEvents::getInstance().emitLibraryAboutToBeRemoved();
                dtDAL::Project::GetInstance().CloseMap(*oldMap,true);
                EditorEvents::getInstance().emitMapLibraryRemoved();

                EditorData::getInstance().getMainWindow()->endWaitCursor();
            }
            catch(const dtDAL::Exception &e)
            {
                EditorData::getInstance().getMainWindow()->endWaitCursor();
                QMessageBox::critical((QWidget *)EditorData::getInstance().getMainWindow(),
                    tr("Error"), e.What().c_str(), tr("OK"));
            }
        }

        EditorData::getInstance().getMainWindow()->startWaitCursor();
        //If we tried to load the same map, make sure to re-open it since we just closed
        //it.
        if (areSameMap)
        {
            try
            {
                newMap = &dtDAL::Project::GetInstance().GetMap(oldMapName);
            }
            catch (dtDAL::Exception &e)
            {
                EditorData::getInstance().getMainWindow()->endWaitCursor();
                QMessageBox::critical((QWidget *)EditorData::getInstance().getMainWindow(),
                        tr("Map Open Error"), e.What().c_str(), tr("OK"));
                return;
            }
        }

        //Load the new map into the current scene.
        if (newMap != NULL)
        {
            try
            {
                dtDAL::Project::GetInstance().LoadMapIntoScene(*newMap,
                    *(ViewportManager::getInstance().getMasterScene()), true);
            }
            catch (const dtDAL::Exception &e)
            {
                QMessageBox::critical((QWidget *)EditorData::getInstance().getMainWindow(),
                    tr("Error"), e.What().c_str(), tr("OK"));
            }
        }

        EditorData::getInstance().getMainWindow()->endWaitCursor();

        //Update the editor state to reflect the changes.
        EditorData::getInstance().setCurrentMap(newMap);
        EditorEvents::getInstance().emitCurrentMapChanged();

        //Now that we have changed maps, clear the current selection.
        std::vector<osg::ref_ptr<dtDAL::ActorProxy> > emptySelection;
        EditorEvents::getInstance().emitActorsSelected(emptySelection);
    }

    //////////////////////////////////////////////////////////////////////////////
    int EditorActions::saveCurrentMapChanges(bool askPermission)
    {
        dtDAL::Map *currMap = EditorData::getInstance().getCurrentMap().get();
        int result = QMessageBox::NoButton;

        if (currMap == NULL || !currMap->IsModified())
        {
            return QMessageBox::Ignore;
        }

        slotPauseAutosave();

        if (askPermission)
        {
            result = QMessageBox::information((QWidget *)EditorData::getInstance().getMainWindow(),
                tr("Save Map?"), tr("The current map has been modified, would you like to save it?"),
                QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
        }

        if (result == QMessageBox::Yes || !askPermission)
        {
            try
            {
                EditorData::getInstance().getMainWindow()->startWaitCursor();
                dtDAL::Project::GetInstance().SaveMap(*currMap);
                ((QMainWindow*)EditorData::getInstance().getMainWindow())->setWindowTitle(
                    getWindowName().c_str());
                EditorData::getInstance().getMainWindow()->endWaitCursor();
            }
            catch (dtDAL::Exception &e)
            {
                EditorData::getInstance().getMainWindow()->endWaitCursor();
                QString error = "An error occured while saving the map. ";
                error += e.What().c_str();
                LOG_ERROR(error.toStdString());
                QMessageBox::critical((QWidget *)EditorData::getInstance().getMainWindow(),
                    tr("Map Save Error"),error,tr("OK"));

                slotRestartAutosave();
                return QMessageBox::Abort;
            }
        }

        if (result == QMessageBox::No)
        {
            if (dtDAL::Project::GetInstance().HasBackup(*currMap))
                dtDAL::Project::GetInstance().ClearBackup(*currMap);
        }

        if (!askPermission)
        {
            slotRestartAutosave();
            return QMessageBox::Ignore;
        }
        else
        {
            slotRestartAutosave();
            return result; //Return the users response.
        }
    }

}

