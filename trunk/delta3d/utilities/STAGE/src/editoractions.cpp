/* -*-c++-*-
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
 * Curtiss Murphy
 */
#include <prefix/dtstageprefix-src.h>
#include <dtEditQt/editoractions.h>

#include <QtGui/QApplication>
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtGui/QActionGroup>
#include <QtGui/QMainWindow>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QVBoxLayout>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QTextEdit>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtCore/QProcess>

#include <osgDB/FileNameUtils>

#include <dtEditQt/uiresources.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/viewportoverlay.h>
#include <dtEditQt/editoraboutbox.h>
#include <dtEditQt/libraryeditor.h>
#include <dtEditQt/librarypathseditor.h>
#include <dtEditQt/gameeventsdialog.h>
#include <dtEditQt/camera.h>
#include <dtEditQt/projectcontextdialog.h>
#include <dtEditQt/mapdialog.h>
#include <dtEditQt/dialogmapproperties.h>
#include <dtEditQt/dialoglistselection.h>
#include <dtEditQt/mapsaveasdialog.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/preferencesdialog.h>
#include <dtEditQt/propertyeditor.h>
#include <dtEditQt/undomanager.h>
#include <dtEditQt/taskeditor.h>

#include <dtUtil/log.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/librarysharingmanager.h>
#include <dtCore/isector.h>
#include <dtDAL/project.h>
#include <dtDAL/map.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproxyicon.h>
#include <dtDAL/environmentactor.h>
#include <dtCore/globals.h>

#include <sstream>

namespace dtEditQt
{
   //Singleton global variable for the library manager.
   dtCore::RefPtr<EditorActions> EditorActions::instance(NULL);

   ///////////////////////////////////////////////////////////////////////////////
   EditorActions::EditorActions() 
      : mIsector(new dtCore::Isector)
      , mSkeletalEditorProcess(NULL)
      , mParticleEditorProcess(NULL)
      , mViewerProcess(NULL)
   {
      LOG_INFO("Initializing Editor Actions.");
      setupFileActions();
      setupEditActions();
      setupSelectionActions();
      setupWindowActions();
      setupHelpActions();
      setupRecentItems();
      setupSubeditorActions();

      saveMilliSeconds = 300000;
      wasCancelled = false;

      connect(&EditorEvents::GetInstance(), SIGNAL(projectChanged()), this, SLOT(slotPauseAutosave()));
      connect(&EditorEvents::GetInstance(), SIGNAL(currentMapChanged()), this, SLOT(slotRestartAutosave()));

      connect(&EditorEvents::GetInstance(),
      SIGNAL(selectedActors(ActorProxyRefPtrVector&)), this,
      SLOT(slotSelectedActors(ActorProxyRefPtrVector&)));

      timer = new QTimer((QWidget*)EditorData::GetInstance().getMainWindow());
      timer->setInterval(saveMilliSeconds);
      connect(timer, SIGNAL(timeout()), this, SLOT(slotAutosave()));

      connect(&EditorEvents::GetInstance(),
      SIGNAL(actorProxyCreated(ActorProxyRefPtr, bool)), this,
      SLOT(slotOnActorCreated(ActorProxyRefPtr, bool)));
   }

   ///////////////////////////////////////////////////////////////////////////////
   EditorActions::~EditorActions()
   {

   }

   ///////////////////////////////////////////////////////////////////////////////
   EditorActions &EditorActions::GetInstance()
   {
      if (EditorActions::instance.get() == NULL)
         EditorActions::instance = new EditorActions();
      return *(EditorActions::instance.get());
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotSelectedActors(std::vector< dtCore::RefPtr<dtDAL::ActorProxy> > &newActors)
   {
      actors.clear();
      actors.reserve(newActors.size());
      for (unsigned int i = 0; i < newActors.size(); i++)
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
      actionEditMapLibraries->setStatusTip(tr("Add and Remove actor libraries from the current map."));
      connect(actionEditMapLibraries, SIGNAL(triggered()), this, SLOT(slotEditMapLibraries()));

      //File - Map Events Editor...
      actionEditMapEvents = new QAction(tr("Map &Events..."), this);
      actionEditMapEvents->setStatusTip(tr("Add and Remove Game Events from the current map."));
      connect(actionEditMapEvents, SIGNAL(triggered()), this, SLOT(slotEditMapEvents()));

      //File - Edit Library Paths...
      actionFileEditLibraryPaths = new QAction(tr("Edit Library Pat&hs..."), this);
      actionFileEditLibraryPaths->setStatusTip(tr("Add or Remove paths to actor libraries."));
      connect(actionFileEditLibraryPaths, SIGNAL(triggered()), this, SLOT(slotFileEditLibraryPaths()));

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
      connect(actionEditDuplicateActor, SIGNAL(triggered()), this, SLOT(slotEditDuplicateActors()));

      // Edit - Delete Actors...
      actionEditDeleteActor = new QAction(QIcon(UIResources::ICON_EDIT_DELETE.c_str()),
            tr("&Delete Selection"), this);
      //actionEditDeleteActor->setShortcut(tr("delete"));
      actionEditDeleteActor->setStatusTip(tr("Deletes the current actor selection."));
      connect(actionEditDeleteActor, SIGNAL(triggered()), this, SLOT(slotEditDeleteActors()));

      // Edit - Ground Clamp Actors.
      actionEditGroundClampActors = new QAction(QIcon(UIResources::ICON_GROUND_CLAMP.c_str()),
            tr("&Ground Clamp"),this);
      actionEditGroundClampActors->setShortcut(tr("Ctrl+G"));
      actionEditGroundClampActors->setStatusTip(tr("Moves the currently selected actors' Z value to be in line with whatever is below them."));
      connect(actionEditGroundClampActors, SIGNAL(triggered()), this, SLOT(slotEditGroundClampActors()));

      // Edit - Toggle terrain paging.
      actionToggleTerrainPaging = new QAction(QIcon(UIResources::ICON_GROUND_CLAMP.c_str()),
            tr("&Toggle Paging"),this);
      actionToggleTerrainPaging->setShortcut(tr("Ctrl+T"));
      actionToggleTerrainPaging->setStatusTip(tr("Turns off or on terrain paging."));
      connect(actionToggleTerrainPaging, SIGNAL(triggered()), this, SLOT(slotToggleTerrainPaging()));

      // Edit - Task Editor
      actionEditTaskEditor = new QAction(QIcon(UIResources::ICON_GROUND_CLAMP.c_str()),
            tr("Tas&k Editor"),this);
      connect(actionEditTaskEditor, SIGNAL(triggered()), this, SLOT(slotTaskEditor()));

      // Edit - Goto Actor
      actionEditGotoActor = new QAction(QIcon(UIResources::LARGE_ICON_EDIT_GOTO.c_str()),
            tr("Goto Actor"), this);
      actionEditGotoActor->setStatusTip(tr("Places the camera at the selected actor."));
      connect(actionEditGotoActor, SIGNAL(triggered()), this, SLOT(slotEditGotoActor()));

      // Edit - Undo
      actionEditUndo = new QAction(QIcon(UIResources::ICON_EDIT_UNDO.c_str()), tr("&Undo"),this);
      actionEditUndo->setShortcut(tr("Ctrl+Z"));
      actionEditUndo->setStatusTip(tr("Undoes the last property edit, actor delete, or actor creation."));
      connect(actionEditUndo, SIGNAL(triggered()), this, SLOT(slotEditUndo()));

      // Edit - Redo
      actionEditRedo = new QAction(QIcon(UIResources::ICON_EDIT_REDO.c_str()), tr("&Redo"),this);
      actionEditRedo->setShortcut(tr("Ctrl+Y"));
      actionEditRedo->setStatusTip(tr("Redoes the previous property edit, actor delete, or actor creation undo command."));
      connect(actionEditRedo, SIGNAL(triggered()), this, SLOT(slotEditRedo()));
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::setupSelectionActions()
   {
      //The actor selection tools need to be in an action group since they are
      //mutually exclusive.
      modeToolsGroup = new QActionGroup(this);

      actionSelectionCamera =new QAction(QIcon(UIResources::ICON_TOOLMODE_CAMERA.c_str()),tr("&Camera"), modeToolsGroup);
      actionSelectionCamera->setShortcut(tr("Ctrl+Shift+C"));
      actionSelectionCamera->setCheckable(true);
      actionSelectionCamera->setActionGroup(modeToolsGroup);
      actionSelectionCamera->setStatusTip(tr("Use this tool to navigate through the world."));

      actionSelectionSelectActor =new QAction(QIcon(UIResources::ICON_TOOLMODE_SELECT.c_str()),tr("&Select Actor(s)"), modeToolsGroup);
      actionSelectionSelectActor->setShortcut(tr("Ctrl+Shift+S"));
      actionSelectionSelectActor->setCheckable(true);
      actionSelectionSelectActor->setActionGroup(modeToolsGroup);
      actionSelectionSelectActor->setStatusTip(tr("Use this tool to pick actors in the scene."));

      actionSelectionTranslateActor =new QAction(QIcon(UIResources::ICON_TOOLMODE_TRANSLATE.c_str()),tr("&Translate Actor(s)"), modeToolsGroup);
      actionSelectionTranslateActor->setShortcut(tr("Ctrl+Shift+T"));
      actionSelectionTranslateActor->setCheckable(true);
      actionSelectionTranslateActor->setActionGroup(modeToolsGroup);
      actionSelectionTranslateActor->setStatusTip(tr("Use this tool to move the current actor selection."));

      actionSelectionRotateActor =new QAction(QIcon(UIResources::ICON_TOOLMODE_ROTATE.c_str()),tr("&Rotate Actor(s)"), modeToolsGroup);
      actionSelectionRotateActor->setShortcut(tr("Ctrl+Shift+R"));
      actionSelectionRotateActor->setCheckable(true);
      actionSelectionRotateActor->setActionGroup(modeToolsGroup);
      actionSelectionRotateActor->setStatusTip(tr("Use this tool to rotate the current actor selection."));
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::setupSubeditorActions()
   {
      actionEditSkeletalMesh = new QAction(QIcon(UIResources::ICON_EDITOR_SKELETAL_MESH.c_str()),tr("&Launch Skeletal Mesh Editor"), modeToolsGroup);           
      actionEditSkeletalMesh->setStatusTip(tr("Launches the skeletal mesh editor."));
      //actionEditSkeletalMesh->setDisabled(true);
      connect(actionEditSkeletalMesh, SIGNAL(triggered()), this, SLOT(slotLaunchSkeletalMeshEditor()));

      actionEditParticleSystem = new QAction(QIcon(UIResources::ICON_EDITOR_PARTICLE_SYSTEM.c_str()),tr("&Launch Particle System Editor"), modeToolsGroup);       
      actionEditParticleSystem->setStatusTip(tr("Launches the particle system editor."));
      connect(actionEditParticleSystem, SIGNAL(triggered()), this, SLOT(slotLaunchParticleEditor()));

      actionLaunchViewer = new QAction(QIcon(UIResources::ICON_EDITOR_VIEWER.c_str()),tr("&Launch Delta3D Model Viewer"), modeToolsGroup);       
      actionLaunchViewer->setStatusTip(tr("Launches the model viewer."));
      connect(actionLaunchViewer, SIGNAL(triggered()), this, SLOT(slotLaunchDeltaViewer()));
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

      actionWindowsResourceBrowser = new QAction(tr("Resource Browser"), this);
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
      actionHelpAboutEditor = new QAction(tr("&About STAGE..."), this);
      actionHelpAboutEditor->setStatusTip(tr("About STAGE"));
      connect(actionHelpAboutEditor, SIGNAL(triggered()), this, SLOT(slotHelpAboutEditor()));

      // Help - About QT
      actionHelpAboutQT = new QAction(tr("A&bout Qt..."), this);
      actionHelpAboutQT->setStatusTip(tr("About Qt"));
      connect(actionHelpAboutQT, SIGNAL(triggered()), this, SLOT(slotHelpAboutQT()));
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::setupRecentItems()
   {
      //std::vector<std::string> projs = EditorData::GetInstance().getMainWindow()->findRecentProjects();
      std::list<std::string>::iterator itor;

      actionFileRecentProject0 = new QAction(tr("(Recent Project 1)"), this);
      actionFileRecentProject0->setEnabled(false);
      connect(actionFileRecentProject0, SIGNAL(triggered()), this, SLOT(slotFileRecentProject0()));

      itor = EditorData::GetInstance().getRecentProjects().begin();
      if (itor == EditorData::GetInstance().getRecentProjects().end())
         return;

      if (EditorData::GetInstance().getRecentProjects().size()> 0)
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
      if (EditorData::GetInstance().getRecentProjects().size()== 0)
         return;
      // set the text on the actions now
      std::list<std::string>::iterator itor = EditorData::GetInstance().getRecentProjects().begin();
      if (itor == EditorData::GetInstance().getRecentProjects().end())
         return;

      //unsigned int i = EditorData::GetInstance().getRecentProjects().size();
      QString curText = actionFileRecentProject0->text();
      actionFileRecentProject0->setText(tr((*itor).c_str()));
      ++itor;
      actionFileRecentProject0->setEnabled(true);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotFileEditPreferences()
   {
      slotPauseAutosave();
      PreferencesDialog dlg((QWidget*)EditorData::GetInstance().getMainWindow());
      if (dlg.exec()== QDialog::Accepted)
         EditorEvents::GetInstance().emitEditorPreferencesChanged();
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
      MapDialog mapDialog((QWidget*)EditorData::GetInstance().getMainWindow());
      if (mapDialog.exec()== QDialog::Accepted)
      {
         changeMaps(EditorData::GetInstance().getCurrentMap(), mapDialog.getFinalizedMap());
         EditorData::GetInstance().addRecentMap(mapDialog.getFinalizedMap()->GetName());
      }

      slotRestartAutosave();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotFileOpenMap()
   {
      //Make sure we have a valid project.  The project "should" always be
      //valid since this action is only enabled when there is a valid project.
      if (!dtDAL::Project::GetInstance().IsContextValid())
      {
         slotPauseAutosave();
         QMessageBox::critical(EditorData::GetInstance().getMainWindow(), tr("Map Open Error"),
               tr("The current project is not valid."), tr("OK"));
         slotRestartAutosave();
         return;
      }

      //Check the current map for changes and save them...
      int saveResult = saveCurrentMapChanges(true);
      if (saveResult == QMessageBox::Cancel || saveResult == QMessageBox::Abort)
         return;

      slotPauseAutosave();

      DialogListSelection openMapDialog(EditorData::GetInstance().getMainWindow(), tr("Open Existing Map"), tr("Available Maps"));

      QStringList listItems;
      std::set<std::string> mapNames = dtDAL::Project::GetInstance().GetMapNames();
      for (std::set<std::string>::iterator i=mapNames.begin(); i!=mapNames.end(); ++i)
         listItems << i->c_str();

      openMapDialog.setListItems(listItems);
      if (openMapDialog.exec()== QDialog::Accepted)
      {
         dtCore::RefPtr<dtDAL::Map> newMap;

         //Attempt to open the specified map..
         try
         {
            EditorData::GetInstance().getMainWindow()->startWaitCursor();

            const QString &mapName = openMapDialog.getSelectedItem();
            newMap = &dtDAL::Project::GetInstance().GetMap(mapName.toStdString());

            EditorData::GetInstance().getMainWindow()->endWaitCursor();
         }
         catch(dtUtil::Exception &e)
         {
            EditorData::GetInstance().getMainWindow()->endWaitCursor();

            QString error = "An error occured while opening the map. ";
            error += e.What().c_str();
            LOG_ERROR(error.toStdString());
            QMessageBox::critical((QWidget *)EditorData::GetInstance().getMainWindow(),
                  tr("Map Open Error"),error,tr("OK"));

            slotRestartAutosave();
            return;
         }

         //Finally, change to the requested map.
         dtCore::RefPtr<dtDAL::Map> mapRef = newMap;
         EditorData::GetInstance().getMainWindow()->checkAndLoadBackup(newMap->GetName());
         newMap->SetModified(false);
      }

      slotRestartAutosave();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotFileCloseMap()
   {
      // no map open? peace out
      if (!EditorData::GetInstance().getCurrentMap())
         return;

      saveCurrentMapChanges(EditorData::GetInstance().getCurrentMap()->IsModified());

      changeMaps(EditorData::GetInstance().getCurrentMap(), NULL);
      EditorData::GetInstance().getMainWindow()->enableActions();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotFileSaveMap()
   {
      LOG_INFO("Saving current map.");
      //if (EditorData::GetInstance().getMainWindow()->isActiveWindow())
      //   EditorData::GetInstance().getMainWindow()->
      //Save the current map without asking the user for permission.
      saveCurrentMapChanges(false);
      slotRestartAutosave();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotFileSaveMapAs()
   {
      slotPauseAutosave();

      MapSaveAsDialog dlg;
      if (dlg.exec()== QDialog::Rejected)
      {
         slotRestartAutosave();
         return;
      }

      std::string strippedName = osgDB::getSimpleFileName(dlg.getMapFileName());
      std::string name = osgDB::getStrippedName(strippedName);

      dtDAL::Map *myMap = EditorData::GetInstance().getCurrentMap();
      if (myMap == NULL)
      {
         slotRestartAutosave();
         return;
      }

      try
      {
         myMap->SetDescription(dlg.getMapDescription());
         EditorData::GetInstance().getMainWindow()->startWaitCursor();
         dtDAL::Project::GetInstance().SaveMapAs(*myMap, name, strippedName, ViewportManager::GetInstance().getMasterScene());
         EditorData::GetInstance().getMainWindow()->endWaitCursor();
      }
      catch(const dtUtil::Exception &e)
      {
         EditorData::GetInstance().getMainWindow()->endWaitCursor();
         LOG_ERROR(e.What());
         QMessageBox::critical((QWidget *)EditorData::GetInstance().getMainWindow(),
               tr("Error"), QString(e.What().c_str()), tr("OK"));

         slotRestartAutosave();
         return;
      }

      EditorEvents::GetInstance().emitCurrentMapChanged();
      slotRestartAutosave();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotFileExit()
   {
      slotPauseAutosave();

      dtDAL::Map *currMap = dtEditQt::EditorData::GetInstance().getCurrentMap();
      if (currMap == NULL)
      {
         EditorEvents::GetInstance().emitEditorCloseEvent();
         qApp->quit();
         return;
      }

      int result = saveCurrentMapChanges(true);
      if (result == QMessageBox::Abort)
      {
         //An error occurred during saving.
         if (QMessageBox::critical((QWidget*)EditorData::GetInstance().getMainWindow(), tr("Error"), tr("Continue exiting?"), tr("Yes"), tr("No"))== QMessageBox::Yes)
         {
            EditorEvents::GetInstance().emitEditorCloseEvent();
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

      EditorEvents::GetInstance().emitEditorCloseEvent();
      //close the map because the actor libraries will be closed before the DAL, so a crash could
      //happen when the project tries to close the open maps in the destructor.
      changeMaps(currMap, NULL);

      qApp->quit();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotEditMapProperties()
   {
      DialogMapProperties mapPropsDialog((QWidget *)EditorData::GetInstance().getMainWindow());
      dtDAL::Map *map = EditorData::GetInstance().getCurrentMap();

      //If the current map is invalid, issue an error, else populate the dialog
      //box with the values from the current map.  The map "should" always be
      //valid since this action is only enabled when there is a valid map.
      if (map == NULL)
      {
         QMessageBox::critical((QWidget *)EditorData::GetInstance().getMainWindow(), tr("Error"),
               tr("Current map is not valid or no map is open."), tr("OK"));
         return;
      }
      else
      {
         mapPropsDialog.getMapName()->setText(map->GetName().c_str());
         mapPropsDialog.getPathNodeFileName()->setText(map->GetPathNodeFileName().c_str());
         mapPropsDialog.getCreateNavMesh()->setChecked(map->GetCreateNavMesh());
         mapPropsDialog.getMapDescription()->setText(map->GetDescription().c_str());
         mapPropsDialog.getMapAuthor()->setText(map->GetAuthor().c_str());
         mapPropsDialog.getMapCopyright()->setText(map->GetCopyright().c_str());
         mapPropsDialog.getMapComments()->setPlainText(map->GetComment().c_str());
      }

      //If the user pressed the ok button, set the new map values.
      if (mapPropsDialog.exec()== QDialog::Accepted)
      {
         map->SetName(mapPropsDialog.getMapName()->text().toStdString());
         map->SetPathNodeFileName(mapPropsDialog.getPathNodeFileName()->text().toStdString());
         map->SetCreateNavMesh(mapPropsDialog.getCreateNavMesh()->isChecked());
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
      if (!EditorData::GetInstance().getCurrentMap())
      {
         QMessageBox::critical(NULL, tr("Failure"),
         tr("A map must be open in order to edit libraries"),
         tr("OK"));
         return;
      }

      LibraryEditor libEdit((QWidget *)EditorData::GetInstance().getMainWindow());
      libEdit.exec();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotEditMapEvents()
   {
      // we need a current map to edit libraries
      if (!EditorData::GetInstance().getCurrentMap())
      {
         QMessageBox::critical(NULL, tr("Failure"),
         tr("A map must be open in order to edit events"),
         tr("OK"));
         return;
      }

      GameEventsDialog editor(static_cast<QWidget*>(EditorData::GetInstance().getMainWindow()));
      editor.exec();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotFileEditLibraryPaths()
   {
      // Bring up the library paths editor
      LibraryPathsEditor editor(static_cast<QWidget*>(EditorData::GetInstance().getMainWindow()));
      editor.exec();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotEditDuplicateActors()
   {
      LOG_INFO("Duplicating current actor selection.");

      //This commits any changes in the property editor.
      PropertyEditor& propEditor = EditorData::GetInstance().getMainWindow()->GetPropertyEditor();
      propEditor.CommitCurrentEdits();

      ViewportOverlay::ActorProxyList &selection =ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();
      dtCore::Scene *scene = ViewportManager::GetInstance().getMasterScene();
      dtCore::RefPtr<dtDAL::Map> currMap = EditorData::GetInstance().getCurrentMap();
      Camera *worldCam = ViewportManager::GetInstance().getWorldViewCamera();

      //Make sure we have valid data.
      if (!currMap.valid())
      {
         LOG_ERROR("Current map is not valid.");
         return;
      }

      if (scene == NULL)
      {
         LOG_ERROR("Current scene is not valid.");
         return;
      }

      //Create our offset vector which is used to jitter the cloned
      //proxies providing better feedback to the user.
      osg::Vec3 offset;
      if (worldCam != NULL)
         offset = worldCam->getRightDir()* 5;
      else
         offset = osg::Vec3(5, 0, 0);

      // We're about to do a LOT of work, especially if lots of things are select
      // so, start a change transaction.
      EditorData::GetInstance().getMainWindow()->startWaitCursor();
      EditorEvents::GetInstance().emitBeginChangeTransaction();

      //Once we have a reference to the current selection and the scene,
      //clone each proxy, add it to the scene, make the newly cloned
      //proxy(s) the current selection.
      ViewportOverlay::ActorProxyList::iterator itor, itorEnd;
      itor = selection.begin();
      itorEnd = selection.end();
      
      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > newSelection;
      for (; itor!=itorEnd; ++itor)
      {
         dtDAL::ActorProxy *proxy = const_cast<dtDAL::ActorProxy *>(itor->get());
         dtCore::RefPtr<dtDAL::ActorProxy> copy = proxy->Clone();
         if (!copy.valid())
         {
            LOG_ERROR("Error duplicating proxy: " + proxy->GetName());
            continue;
         }

         newSelection.push_back(copy);
      }

      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> >::iterator i, iend;

      i = newSelection.begin();
      iend = newSelection.end();
      for (; i != iend; ++i)
      {
         dtDAL::ActorProxy* proxy = i->get();

         //Store the original location of the proxy so we can position after
         //it has been added to the scene.
         osg::Vec3 oldPosition;
         dtDAL::TransformableActorProxy
               *tProxy =dynamic_cast<dtDAL::TransformableActorProxy *>(proxy);
         if (tProxy != NULL)
            oldPosition = tProxy->GetTranslation();
   
         //Add the new proxy to the map and send out a create event.
         currMap->AddProxy(*proxy);
   
         EditorEvents::GetInstance().emitActorProxyCreated(proxy, false);
   
         //Move the newly duplicated actor to where it is supposed to go.
         if (tProxy != NULL)
            tProxy->SetTranslation(oldPosition+offset);
      }

      //Finally set the newly cloned proxies to be the current selection.
      ViewportManager::GetInstance().getViewportOverlay()->setMultiSelectMode(false);
      EditorEvents::GetInstance().emitActorsSelected(newSelection);
      EditorEvents::GetInstance().emitEndChangeTransaction();

      EditorData::GetInstance().getMainWindow()->endWaitCursor();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotEditDeleteActors()
   {
      LOG_INFO("Deleting current actor selection. ");

      ViewportOverlay::ActorProxyList selection =ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();
      dtCore::RefPtr<dtDAL::Map> currMap = EditorData::GetInstance().getCurrentMap();

      //Make sure we have valid data.
      if (!currMap.valid())
      {
         LOG_ERROR("Current map is not valid.");
         return;
      }

      // We're about to do a LOT of work, especially if lots of things are select
      // so, start a change transaction.
      EditorData::GetInstance().getMainWindow()->startWaitCursor();
      EditorEvents::GetInstance().emitBeginChangeTransaction();

      //Once we have a reference to the current selection and the scene,
      //remove each proxy's actor from the scene then remove the proxy from
      //the map.
      ViewportOverlay::ActorProxyList::iterator itor;
      for (itor=selection.begin(); itor!=selection.end(); ++itor)
      {
         // \TODO: Find out why this const_cast is necessary. It compiles without
         // it on MSVC 7.1, but not on gcc4.0.2 -osb
         dtDAL::ActorProxy *proxy = const_cast<dtDAL::ActorProxy*>(itor->get());
         deleteProxy(proxy, currMap);
      }

      //Now that we have removed the selected objects, clear the current selection.
      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > emptySelection;
      EditorEvents::GetInstance().emitActorsSelected(emptySelection);
      EditorEvents::GetInstance().emitEndChangeTransaction();

      EditorData::GetInstance().getMainWindow()->endWaitCursor();
   }

   //////////////////////////////////////////////////////////////////////////////
   bool EditorActions::deleteProxy(dtDAL::ActorProxy *proxy,
         dtCore::RefPtr<dtDAL::Map> currMap)
   {
      bool result = false;
      dtCore::Scene *scene = ViewportManager::GetInstance().getMasterScene();
      dtDAL::ActorProxy *envProxy = currMap->GetEnvironmentActor();
      if (envProxy != NULL)
      {
         if (envProxy == proxy)
         {
            dtDAL::IEnvironmentActor
                  *envActor = dynamic_cast<dtDAL::IEnvironmentActor*>(envProxy->GetActor());
            std::vector<dtCore::DeltaDrawable*> drawables;
            envActor->GetAllActors(drawables);
            envActor->RemoveAllActors();
            for (unsigned int i = 0; i < drawables.size(); i++)
            {
               scene->AddDrawable(drawables[i]);
            }

            currMap->SetEnvironmentActor(NULL);

            if (!currMap->RemoveProxy(*proxy))
            {
               LOG_ERROR("Unable to remove actor proxy: " + proxy->GetName());
            }
            else
            {
               EditorEvents::GetInstance().emitActorProxyDestroyed(proxy);
               result = true;
            }
            return result;
         }
      }

      if (proxy != NULL && scene != NULL)
      {
         dtCore::RefPtr<dtDAL::ActorProxy> tempRef = proxy;
         scene->RemoveDrawable(proxy->GetActor());
         if (proxy->GetBillBoardIcon()!= NULL)
            scene->RemoveDrawable(proxy->GetBillBoardIcon()->GetDrawable());

         EditorEvents::GetInstance().emitActorProxyAboutToBeDestroyed(tempRef);

         if (!currMap->RemoveProxy(*proxy))
         {
            LOG_ERROR("Unable to remove actor proxy: " + proxy->GetName());
         }
         else
         {
            EditorEvents::GetInstance().emitActorProxyDestroyed(tempRef);
            result = true;
         }
      }
      return result;
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotOnActorCreated(ActorProxyRefPtr actor,
         bool forceNoAdjustments)
   {
      dtDAL::IEnvironmentActor
            *envActor = dynamic_cast<dtDAL::IEnvironmentActor*>(actor->GetActor());
      if (envActor == NULL)
         return;

      dtDAL::Map *map = EditorData::GetInstance().getCurrentMap();
      if (map == NULL)
         return;

      dtDAL::ActorProxy *envProxy = map->GetEnvironmentActor();
      QWidget *window = static_cast<QWidget*>(EditorData::GetInstance().getMainWindow());
      if (envProxy != NULL)
      {
         QMessageBox::Button
               button = QMessageBox::information(
                     window,
                     tr("Set Environment Actor"),
                     tr("Would you like to set this actor as the scene's environment, overwriting the current environment actor?"),
                     QMessageBox::Yes, QMessageBox::No);

         if (button == QMessageBox::Yes)
         {
            dtCore::Scene *scene = ViewportManager::GetInstance().getMasterScene();
            dtDAL::IEnvironmentActor
                  *env = dynamic_cast<dtDAL::IEnvironmentActor*>(envProxy->GetActor());
            if (env != NULL)
               env->RemoveAllActors();
            map->SetEnvironmentActor(actor.get());
            dtDAL::Project::GetInstance().LoadMapIntoScene(*map, *scene);
         }
      }
      else
      {
         QMessageBox::Button
               button = QMessageBox::information(
                     window,
                     tr("Set Environment Actor"),
                     tr("Would you like to set this actor as the scene's environment?"),
                     QMessageBox::Yes, QMessageBox::No);

         if (button == QMessageBox::Yes)
         {
            dtCore::Scene *scene = ViewportManager::GetInstance().getMasterScene();
            scene->RemoveAllDrawables();
            map->SetEnvironmentActor(actor.get());
            dtDAL::Project::GetInstance().LoadMapIntoScene(*map, *scene);
         }
      }
   }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotLaunchSkeletalMeshEditor()
    {       
       // Create a new process if we don't already have one
       if (!mSkeletalEditorProcess)
       {
          mSkeletalEditorProcess = new QProcess(this);           
       }     

       // Don't launch more than one copy of the editor
       if (mSkeletalEditorProcess->state() != QProcess::Running)
       {           
          QString program = "AnimationViewer.exe";
          QStringList arguments;

          mSkeletalEditorProcess->start(program, arguments);     

          QProcess::ProcessState state = mSkeletalEditorProcess->state();

          // Our process should have started
          if (state == QProcess::NotRunning)
          {
             QMessageBox::information(NULL, tr("Process Error"), 
                tr("Unable to launch AnimationViewer.exe.  Make sure application exists."),
                QMessageBox::Ok);

          }
       }
      
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotLaunchParticleEditor()
    {
       // Create a new process if we don't already have one
       if (!mParticleEditorProcess)
       {
          mParticleEditorProcess = new QProcess(this);           
       }     

       // Don't launch more than one copy of the editor
       if (mParticleEditorProcess->state() != QProcess::Running)
       {           
          QString program = "psEditor.exe";
          QStringList arguments;

          mParticleEditorProcess->start(program, arguments);    

          QProcess::ProcessState state = mParticleEditorProcess->state();

          // Our process should have started
          if (state == QProcess::NotRunning)
          {
             QMessageBox::information(NULL, tr("Process Error"), 
                tr("Unable to launch psEditor.exe.  Make sure application exists."),
                QMessageBox::Ok);

          }
       }
    }

    //////////////////////////////////////////////////////////////////////////////
    void EditorActions::slotLaunchDeltaViewer()
    {
       // Create a new process if we don't already have one
       if (!mViewerProcess)
       {
          mViewerProcess = new QProcess(this);           
       }     

       // Don't launch more than one copy of the editor
       if (mViewerProcess->state() != QProcess::Running)
       {           
          QString program = "viewer.exe";
          QStringList arguments;

          mViewerProcess->start(program, arguments); 

          QProcess::ProcessState state = mViewerProcess->state();

          // Our process should have started
          if (state == QProcess::NotRunning)
          {
             QMessageBox::information(NULL, tr("Process Error"), 
                tr("Unable to launch Viewer.exe.  Make sure application exists."),
                QMessageBox::Ok);
           
          }
       }
    }    

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotEditUndo()
   {
      EditorData::GetInstance().getUndoManager().doUndo();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotEditRedo()
   {
      EditorData::GetInstance().getUndoManager().doRedo();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotTaskEditor()
   {
      TaskEditor taskEditor(static_cast<QWidget*>(EditorData::GetInstance().getMainWindow()));
      taskEditor.exec();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotEditGroundClampActors()
   {
      LOG_INFO("Ground clamping actors.");

      ViewportOverlay::ActorProxyList &selection =ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();
      dtCore::Scene *scene = ViewportManager::GetInstance().getMasterScene();

      if (scene == NULL)
      {
         LOG_ERROR("Current scene is not valid.");
         return;
      }

      // We're about to do a LOT of work, especially if lots of things are select
      // so, start a change transaction.
      EditorData::GetInstance().getMainWindow()->startWaitCursor();
      EditorEvents::GetInstance().emitBeginChangeTransaction();

      //Iterate through the current selection, trace a ray directly below it.  If there is
      //an intersection, move the current proxy to that point.
      ViewportOverlay::ActorProxyList::iterator itor;

      mIsector->Reset();
      mIsector->SetScene(scene);

      for (itor=selection.begin(); itor!=selection.end(); ++itor)
      {
         dtDAL::ActorProxy *proxy = const_cast<dtDAL::ActorProxy *>(itor->get());
         dtDAL::TransformableActorProxy
               *tProxy =dynamic_cast<dtDAL::TransformableActorProxy *>(proxy);

         if (tProxy != NULL)
         {
            osg::Vec3 pos = tProxy->GetTranslation();

            mIsector->SetStartPosition(pos);
            mIsector->SetDirection(osg::Vec3(0, 0, -1));
            mIsector->Reset();

            //Find a possible intersection point.  If we find an intersection
            //point, move the actor to that location.
            if (mIsector->Update())
            {
               osgUtil::IntersectVisitor &iv = mIsector->GetIntersectVisitor();
               osg::Vec3 p = iv.getHitList(mIsector->GetLineSegment())[0].getWorldIntersectPoint();
               tProxy->SetTranslation(p);
            }
         }
      }

      EditorEvents::GetInstance().emitEndChangeTransaction();
      EditorData::GetInstance().getMainWindow()->endWaitCursor();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotToggleTerrainPaging()
   {
      if (ViewportManager::GetInstance().IsPagingEnabled())
         ViewportManager::GetInstance().EnablePaging(false);
      else
         ViewportManager::GetInstance().EnablePaging(true);
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotEditGotoActor()
   {
      if (actors.size()> 0)
         EditorEvents::GetInstance().emitGotoActor(actors[0]);
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotProjectChangeContext()
   {
      int result = saveCurrentMapChanges(true);
      if (result == QMessageBox::Cancel || result == QMessageBox::Abort)
         return;

      slotPauseAutosave();
      ProjectContextDialog dialog((QWidget *)EditorData::GetInstance().getMainWindow());
      if (dialog.exec()== QDialog::Accepted)
      {
         std::string contextName = dialog.getProjectPath().toStdString();

         //First try to set the new project context.
         try
         {
            changeMaps(EditorData::GetInstance().getCurrentMap(), NULL);
            dtDAL::Project::GetInstance().CreateContext(contextName);
            dtDAL::Project::GetInstance().SetContext(contextName);
         }
         catch (dtUtil::Exception &e)
         {
            QMessageBox::critical((QWidget *)EditorData::GetInstance().getMainWindow(),
                  tr("Error"), tr(e.What().c_str()), tr("OK"));

            slotRestartAutosave();
            return;
         }

         EditorData::GetInstance().setCurrentProjectContext(contextName);
         EditorData::GetInstance().addRecentProject(contextName);
         EditorEvents::GetInstance().emitProjectChanged();
         refreshRecentProjects();
      }

      slotRestartAutosave();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotHelpAboutEditor()
   {
      slotPauseAutosave();
      EditorAboutBox box((QWidget *)EditorData::GetInstance().getMainWindow());
      box.exec();
      slotRestartAutosave();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotHelpAboutQT()
   {
      slotPauseAutosave();
      QMessageBox::aboutQt((QWidget *)EditorData::GetInstance().getMainWindow());
      slotRestartAutosave();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotAutosave()
   {
      try
      {
         if(EditorData::GetInstance().getCurrentMap())
         {
            dtDAL::Project::GetInstance().SaveMapBackup(*EditorData::GetInstance().getCurrentMap());
         }
      }
      catch(const dtUtil::Exception &e)
      {
         QMessageBox::critical(NULL, tr("Error"), e.What().c_str(), tr("OK"));
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   const std::string EditorActions::getWindowName() const
   {
      ((QMainWindow*)EditorData::GetInstance().getMainWindow())->windowTitle().clear();
      std::string name("STAGE");
      std::string projDir;
      std::string temp = dtDAL::Project::GetInstance().GetContext();
      if (temp.empty())
         return name;
      unsigned int index = temp.find_last_of(dtUtil::FileUtils::PATH_SEPARATOR);
      projDir = temp.substr(index+1);
      name += " - ";
      name += projDir;

      // if we have a map, append the name
      dtCore::RefPtr<dtDAL::Map> map = EditorData::GetInstance().getCurrentMap();
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
      if (saveCurrentMapChanges(true)== QMessageBox::Cancel)
         return;

      changeMaps(EditorData::GetInstance().getCurrentMap(), NULL);

      EditorData::GetInstance().setCurrentProjectContext(actionFileRecentProject0->text().toStdString());
      try
      {
         dtDAL::Project::GetInstance().SetContext(actionFileRecentProject0->text().toStdString());
      }
      catch (const dtUtil::Exception& ex)
      {
         QMessageBox::warning((QWidget *)EditorData::GetInstance().getMainWindow(),
               tr("Project Context Open Error"), ex.What().c_str(), tr("OK"));
         return;
      }
      EditorEvents::GetInstance().emitProjectChanged();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotFileRecentMap0()
   {
      if (saveCurrentMapChanges(true)== QMessageBox::Cancel)
         return;

      const std::string &newMapName = actionFileRecentMap0->text().toStdString();
      dtDAL::Map *newMap;
      try
      {
         newMap = &dtDAL::Project::GetInstance().GetMap(newMapName);
      }
      catch (dtUtil::Exception &e)
      {
         QMessageBox::critical((QWidget *)EditorData::GetInstance().getMainWindow(),
               tr("Map Open Error"), e.What().c_str(), tr("OK"));
         return;
      }

      changeMaps(EditorData::GetInstance().getCurrentMap(), newMap);
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
         EditorData::GetInstance().getMainWindow()->startWaitCursor();
         ViewportManager::GetInstance().clearMasterScene(oldMap->GetAllProxies());
         oldMapName = oldMap->GetName();

         //Close the old map...
         try
         {
            EditorEvents::GetInstance().emitLibraryAboutToBeRemoved();
            dtDAL::Project::GetInstance().CloseMap(*oldMap,true);
            if (ViewportManager::GetInstance().IsPagingEnabled())
            ViewportManager::GetInstance().EnablePaging(false);
            EditorEvents::GetInstance().emitMapLibraryRemoved();

            EditorData::GetInstance().getMainWindow()->endWaitCursor();
         }
         catch(const dtUtil::Exception &e)
         {
            EditorData::GetInstance().getMainWindow()->endWaitCursor();
            QMessageBox::critical((QWidget *)EditorData::GetInstance().getMainWindow(),
                  tr("Error"), e.What().c_str(), tr("OK"));
         }
      }

      EditorData::GetInstance().getMainWindow()->startWaitCursor();
      //If we tried to load the same map, make sure to re-open it since we just closed
      //it.
      if (areSameMap)
      {
         try
         {
            newMap = &dtDAL::Project::GetInstance().GetMap(oldMapName);
         }
         catch (dtUtil::Exception &e)
         {
            EditorData::GetInstance().getMainWindow()->endWaitCursor();
            QMessageBox::critical((QWidget *)EditorData::GetInstance().getMainWindow(),
                  tr("Map Open Error"), e.What().c_str(), tr("OK"));
            return;
         }
      }

      //Load the new map into the current scene.
      if (newMap != NULL)
      {
         const std::vector<std::string>
               & missingLibs = newMap->GetMissingLibraries();

         if (!missingLibs.empty())
         {
            QString
                  errors(tr("The following libraries listed in the map could not be loaded:\n\n"));
            for (unsigned i = 0; i < missingLibs.size(); ++i)
            {
               std::string
                     nativeName = dtUtil::LibrarySharingManager::GetPlatformSpecificLibraryName(missingLibs[i]);
               errors.append(nativeName.c_str());
               errors.append("\n");
            }

            errors.append("\nThis could happen for a number of reasons. Please ensure that the name is correct, ");
            errors.append("the library is in the path (or the working directory), the library can load correctly, and dependent libraries are available.");
            errors.append("If you save this map, the library and any actors referenced by the library will be lost.");

            QMessageBox::warning((QWidget *)EditorData::GetInstance().getMainWindow(), tr("Missing Libraries"), errors, tr("OK"));
         }

         try
         {
            dtDAL::Project::GetInstance().LoadMapIntoScene(*newMap,
                  *(ViewportManager::GetInstance().getMasterScene()), true, false);

         }
         catch (const dtUtil::Exception& e)
         {
            QMessageBox::critical((QWidget *)EditorData::GetInstance().getMainWindow(),
                  tr("Error"), e.What().c_str(), tr("OK"));
         }
      }

      EditorData::GetInstance().getMainWindow()->endWaitCursor();
      //Update the editor state to reflect the changes.
      EditorData::GetInstance().setCurrentMap(newMap);
      EditorEvents::GetInstance().emitCurrentMapChanged();

      if (ViewportManager::GetInstance().IsPagingEnabled())
         ViewportManager::GetInstance().EnablePaging(false);
      ViewportManager::GetInstance().EnablePaging(true);

      //Now that we have changed maps, clear the current selection.
      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > emptySelection;
      EditorEvents::GetInstance().emitActorsSelected(emptySelection);
   }

   //////////////////////////////////////////////////////////////////////////////
   int EditorActions::saveCurrentMapChanges(bool askPermission)
   {
      //This commits any changes in the property editor.
      PropertyEditor& propEditor = EditorData::GetInstance().getMainWindow()->GetPropertyEditor();
      propEditor.CommitCurrentEdits();

      dtDAL::Map *currMap = EditorData::GetInstance().getCurrentMap();
      int result = QMessageBox::NoButton;

      if (currMap == NULL || !currMap->IsModified())
      {
         return QMessageBox::Ignore;
      }

      slotPauseAutosave();

      if (askPermission)
      {
         result = QMessageBox::information(
               (QWidget *)EditorData::GetInstance().getMainWindow(),
               tr("Save Map?"),
               tr("The current map has been modified, would you like to save it?"),
               QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
      }

      if (result == QMessageBox::Yes || !askPermission)
      {
         try
         {
            EditorData::GetInstance().getMainWindow()->startWaitCursor();
            dtDAL::Project::GetInstance().SaveMap(*currMap, ViewportManager::GetInstance().getMasterScene());
            ((QMainWindow*)EditorData::GetInstance().getMainWindow())->setWindowTitle(
                  getWindowName().c_str());
            EditorData::GetInstance().getMainWindow()->endWaitCursor();
         }
         catch (dtUtil::Exception &e)
         {
            EditorData::GetInstance().getMainWindow()->endWaitCursor();
            QString error = "An error occured while saving the map. ";
            error += e.What().c_str();
            LOG_ERROR(error.toStdString());
            QMessageBox::critical((QWidget *)EditorData::GetInstance().getMainWindow(),
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

