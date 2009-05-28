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
 *
 * Curtiss Murphy
 * R. Erik Johnson
 */
#include <prefix/dtstageprefix-src.h>
#include <dtEditQt/editoractions.h>

#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtGui/QActionGroup>
#include <QtGui/QMainWindow>
#include <QtGui/QMessageBox>
#include <QtGui/QTextEdit>
#include <QtCore/QTimer>

#include <osgDB/FileNameUtils>

#include <dtEditQt/uiresources.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/viewportoverlay.h>
#include <dtEditQt/editoraboutbox.h>
#include <dtEditQt/libraryeditor.h>
#include <dtEditQt/gameeventsdialog.h>
#include <dtEditQt/stagecamera.h>
#include <dtEditQt/projectcontextdialog.h>
#include <dtEditQt/mapdialog.h>
#include <dtEditQt/dialogmapproperties.h>
#include <dtEditQt/mapsaveasdialog.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/preferencesdialog.h>
#include <dtEditQt/propertyeditor.h>
#include <dtEditQt/undomanager.h>
#include <dtEditQt/taskeditor.h>
#include <dtEditQt/externaltooldialog.h>
#include <dtEditQt/externaltool.h>
#include <dtEditQt/externaltoolargparsers.h>
#include "ui_positiondialog.h"

#include <dtQt/dialoglistselection.h>
#include <dtQt/librarypathseditor.h>

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
#include <dtDAL/librarymanager.h>
#include <dtCore/globals.h>

#include <sstream>

namespace dtEditQt
{

   // Singleton global variable for the library manager.
   dtCore::RefPtr<EditorActions> EditorActions::sInstance(NULL);

   ///////////////////////////////////////////////////////////////////////////////
   EditorActions::EditorActions()
      : mExternalToolActionGroup(new QActionGroup(NULL))
      , mIsector(new dtCore::Isector)
   {
      LOG_INFO("Initializing Editor Actions.");
      setupFileActions();
      setupEditActions();
      setupWindowActions();
      SetupToolsActions();
      setupHelpActions();
      setupRecentItems();

      mSaveMilliSeconds = 300000;
      mWasCancelled = false;

      connect(&EditorEvents::GetInstance(), SIGNAL(projectChanged()),    this, SLOT(slotPauseAutosave()));
      connect(&EditorEvents::GetInstance(), SIGNAL(currentMapChanged()), this, SLOT(slotRestartAutosave()));

      connect(&EditorEvents::GetInstance(),
         SIGNAL(selectedActors(ActorProxyRefPtrVector&)), this,
         SLOT(slotSelectedActors(ActorProxyRefPtrVector&)));

      mTimer = new QTimer((QWidget*)EditorData::GetInstance().getMainWindow());
      mTimer->setInterval(mSaveMilliSeconds);
      connect(mTimer, SIGNAL(timeout()), this, SLOT(slotAutosave()));

      connect(&EditorEvents::GetInstance(),
         SIGNAL(actorProxyCreated(ActorProxyRefPtr, bool)), this,
         SLOT(slotOnActorCreated(ActorProxyRefPtr, bool)));
   }

   ///////////////////////////////////////////////////////////////////////////////
   EditorActions::~EditorActions()
   {
      mTimer->stop();
      delete mTimer;

      while (mTools.size() > 0)
      {
         ExternalTool* tool = mTools.takeFirst();
         delete tool;
      }

      delete mExternalToolActionGroup;

      while (mExternalToolArgParsers.size() > 0)
      {
         const ExternalToolArgParser* parser = mExternalToolArgParsers.takeFirst();
         delete parser;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   EditorActions& EditorActions::GetInstance()
   {
      if (EditorActions::sInstance.get() == NULL)
      {
         EditorActions::sInstance = new EditorActions();
      }
      return *(EditorActions::sInstance.get());
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotSelectedActors(std::vector< dtCore::RefPtr<dtDAL::ActorProxy> >& newActors)
   {
      mActors.clear();
      mActors.reserve(newActors.size());
      for (unsigned int i = 0; i < newActors.size(); ++i)
      {
         mActors.push_back(newActors[i]);
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::setupFileActions()
   {
      // File - New Map...
      mActionFileNewMap = new QAction(QIcon(UIResources::ICON_FILE_NEW_MAP.c_str()),
         tr("&New Map..."), this);
      mActionFileNewMap->setShortcut(tr("Ctrl+N"));
      mActionFileNewMap->setStatusTip(tr("Create a new map."));
      connect(mActionFileNewMap, SIGNAL(triggered()), this, SLOT(slotFileNewMap()));

      // File - Open Map...
      mActionFileOpenMap = new QAction(QIcon(UIResources::ICON_FILE_OPEN_MAP.c_str()),
         tr("&Open Map..."), this);
      mActionFileOpenMap->setShortcut(tr("Ctrl+O"));
      mActionFileOpenMap->setStatusTip(tr("Open an existing map in this project."));
      connect(mActionFileOpenMap, SIGNAL(triggered()), this, SLOT(slotFileOpenMap()));

      // File - Close Map
      mActionFileCloseMap = new QAction(tr("Close Map"), this);
      mActionFileCloseMap->setStatusTip(tr("Close the currently opened map"));
      connect(mActionFileCloseMap, SIGNAL(triggered()), this, SLOT(slotFileCloseMap()));

      // File - Save Map...
      mActionFileSaveMap = new QAction(QIcon(UIResources::ICON_FILE_SAVE.c_str()),
         tr("&Save Map"), this);
      mActionFileSaveMap->setShortcut(tr("Ctrl+S"));
      mActionFileSaveMap->setStatusTip(tr("Save the current map."));
      connect(mActionFileSaveMap, SIGNAL(triggered()), this, SLOT(slotFileSaveMap()));

      // File - Save Map As...
      mActionFileSaveMapAs = new QAction(tr("Save Map &As..."),this);
      mActionFileSaveMapAs->setStatusTip(tr("Save the current map under a different file."));
      connect(mActionFileSaveMapAs, SIGNAL(triggered()), this, SLOT(slotFileSaveMapAs()));

      // File - Change Project
      mActionFileChangeProject = new QAction(tr("&Change Project..."), this);
      mActionFileChangeProject->setStatusTip(tr("Change the current project context."));
      connect(mActionFileChangeProject, SIGNAL(triggered()), this, SLOT(slotProjectChangeContext()));

      // File - Map Properties Editor...
      mActionEditMapProperties = new QAction(tr("Map &Properties..."), this);
      mActionEditMapProperties->setStatusTip(tr("Edit the properties of the current map."));
      connect(mActionEditMapProperties, SIGNAL(triggered()), this, SLOT(slotEditMapProperties()));

      // File - Map Libraries Editor...
      mActionEditMapLibraries = new QAction(tr("Map &Libraries..."), this);
      mActionEditMapLibraries->setStatusTip(tr("Add and Remove actor libraries from the current map."));
      connect(mActionEditMapLibraries, SIGNAL(triggered()), this, SLOT(slotEditMapLibraries()));

      // File - Map Events Editor...
      mActionEditMapEvents = new QAction(tr("Map &Events..."), this);
      mActionEditMapEvents->setStatusTip(tr("Add and Remove Game Events from the current map."));
      connect(mActionEditMapEvents, SIGNAL(triggered()), this, SLOT(slotEditMapEvents()));

      // File - Edit Library Paths...
      mActionFileEditLibraryPaths = new QAction(tr("Edit Library Pat&hs..."), this);
      mActionFileEditLibraryPaths->setStatusTip(tr("Add or Remove paths to actor libraries."));
      connect(mActionFileEditLibraryPaths, SIGNAL(triggered()), this, SLOT(slotFileEditLibraryPaths()));

      mActionFileEditPreferences = new QAction(tr("Preferences..."), this);
      mActionFileEditPreferences->setStatusTip(tr("Edit editor preferences"));
      connect(mActionFileEditPreferences, SIGNAL(triggered()), this, SLOT(slotFileEditPreferences()));

      // File - Exit...
      mActionFileExit = new QAction(tr("E&xit"), this);
      mActionFileExit->setShortcut(tr("Alt+F4"));
      mActionFileExit->setStatusTip(tr("Exit the level editor."));
      connect(mActionFileExit, SIGNAL(triggered()), this, SLOT(slotFileExit()));
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::setupEditActions()
   {
      // Edit - Local Space...
      mActionLocalSpace = new QAction(QIcon(UIResources::ICON_EDIT_LOCAL_SPACE.c_str()), "Local Space", this);
      mActionLocalSpace->setCheckable(true);
      mActionLocalSpace->setChecked(true);
      mActionLocalSpace->setStatusTip(tr("Sets the selection gizmos to local space."));
      connect(mActionLocalSpace, SIGNAL(triggered()), this, SLOT(slotEditLocalSpace()));

      // Edit - Duplicate Actors...
      mActionEditDuplicateActor = new QAction(QIcon(UIResources::ICON_EDIT_DUPLICATE.c_str()),
         tr("Du&plicate Selection"), this);
      mActionEditDuplicateActor->setShortcut(tr("Ctrl+D"));
      mActionEditDuplicateActor->setStatusTip(tr("Duplicates the current actor selection."));
      connect(mActionEditDuplicateActor, SIGNAL(triggered()), this, SLOT(slotEditDuplicateActors()));

      // Edit - Delete Actors...
      mActionEditDeleteActor = new QAction(QIcon(UIResources::ICON_EDIT_DELETE.c_str()),
         tr("&Delete Selection"), this);
      //mActionEditDeleteActor->setShortcut(tr("delete"));
      mActionEditDeleteActor->setStatusTip(tr("Deletes the current actor selection."));
      connect(mActionEditDeleteActor, SIGNAL(triggered()), this, SLOT(slotEditDeleteActors()));

      // Edit - Ground Clamp Actors.
      mActionEditGroundClampActors = new QAction(QIcon(UIResources::ICON_GROUND_CLAMP.c_str()),
         tr("&Ground Clamp"), this);
      mActionEditGroundClampActors->setShortcut(tr("Ctrl+G"));
      mActionEditGroundClampActors->setStatusTip(tr("Moves the currently selected actors' Z value to be in line with whatever is below them."));
      connect(mActionEditGroundClampActors, SIGNAL(triggered()), this, SLOT(slotEditGroundClampActors()));

      // Edit - Task Editor
      mActionEditTaskEditor = new QAction(QIcon(UIResources::ICON_GROUND_CLAMP.c_str()),
         tr("Tas&k Editor"), this);
      connect(mActionEditTaskEditor, SIGNAL(triggered()), this, SLOT(slotTaskEditor()));

      // Edit - Goto Actor
      mActionEditGotoActor = new QAction(QIcon(UIResources::LARGE_ICON_EDIT_GOTO.c_str()),
         tr("Goto Actor"), this);
      mActionEditGotoActor->setStatusTip(tr("Places the camera at the selected actor."));
      connect(mActionEditGotoActor, SIGNAL(triggered()), this, SLOT(slotEditGotoActor()));

      mActionGetGotoPosition = new QAction(tr("Go&to Position..."), this);
      mActionGetGotoPosition->setStatusTip(tr("Move all cameras to desired position."));
      connect(mActionGetGotoPosition, SIGNAL(triggered()), this, SLOT(slotGetGotoPosition()));

      // Edit - Group Actors...
      mActionGroupActors = new QAction(QIcon(UIResources::ICON_EDIT_GROUP.c_str()), "Group Actors Together", this);
      mActionGroupActors->setStatusTip(tr("Groups or Ungroups selected actors."));
      connect(mActionGroupActors, SIGNAL(triggered()), this, SLOT(slotEditGroupActors()));

      // Edit - Group Actors...
      mActionUngroupActors = new QAction(QIcon(UIResources::ICON_EDIT_UNGROUP.c_str()), "Ungroup Actors", this);
      mActionUngroupActors->setStatusTip(tr("Groups or Ungroups selected actors."));
      connect(mActionUngroupActors, SIGNAL(triggered()), this, SLOT(slotEditUngroupActors()));

      // Edit - Undo
      mActionEditUndo = new QAction(QIcon(UIResources::ICON_EDIT_UNDO.c_str()), tr("&Undo"), this);
      mActionEditUndo->setShortcut(tr("Ctrl+Z"));
      mActionEditUndo->setStatusTip(tr("Undoes the last property edit, actor delete, or actor creation."));
      connect(mActionEditUndo, SIGNAL(triggered()), this, SLOT(slotEditUndo()));

      // Edit - Redo
      mActionEditRedo = new QAction(QIcon(UIResources::ICON_EDIT_REDO.c_str()), tr("&Redo"), this);
      mActionEditRedo->setShortcut(tr("Ctrl+Y"));
      mActionEditRedo->setStatusTip(tr("Redoes the previous property edit, actor delete, or actor creation undo command."));
      connect(mActionEditRedo, SIGNAL(triggered()), this, SLOT(slotEditRedo()));
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::setupWindowActions()
   {
      mActionWindowsPropertyEditor = new QAction(tr("Property Editor"), this);
      mActionWindowsPropertyEditor->setShortcut(tr("Alt+1"));
      mActionWindowsPropertyEditor->setStatusTip(tr("Hides and retrieves the actor property editor"));
      mActionWindowsPropertyEditor->setCheckable(true);
      mActionWindowsPropertyEditor->setChecked(true);

      mActionWindowsActorSearch = new QAction(tr("Actor Search"), this);
      mActionWindowsActorSearch->setShortcut(tr("Alt+2"));
      mActionWindowsActorSearch->setStatusTip(tr("Hides and retrieves the actor search window"));
      mActionWindowsActorSearch->setCheckable(true);
      mActionWindowsActorSearch->setChecked(true);

      mActionWindowsResourceBrowser = new QAction(tr("Resource Browser"), this);
      mActionWindowsResourceBrowser->setShortcut(tr("Alt+3"));
      mActionWindowsResourceBrowser->setStatusTip(tr("Hides and retrieves the resource browser"));
      mActionWindowsResourceBrowser->setCheckable(true);
      mActionWindowsResourceBrowser->setChecked(true);

      mActionWindowsResetWindows = new QAction(tr("Reset Windows"), this);
      mActionWindowsResetWindows->setShortcut(tr("Ctrl+R"));
      mActionWindowsResetWindows->setStatusTip(tr("Restores the windows to a default state"));
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorActions::SetupToolsActions()
   {
      mActionAddTool = new QAction(tr("&External Tools..."), this);
      mActionAddTool->setStatusTip(tr("Add/edit external tools"));
      connect(mActionAddTool, SIGNAL(triggered()), this, SLOT(SlotNewExternalToolEditor()));

      mExternalToolArgParsers.push_back(new CurrentContextArgParser());
      mExternalToolArgParsers.push_back(new CurrentMapNameArgParser());
      mExternalToolArgParsers.push_back(new CurrentMeshArgParser());
      mExternalToolArgParsers.push_back(new CurrentParticleSystemArgParser());
      mExternalToolArgParsers.push_back(new CurrentSkeletonArgParser());

      // create a finite number of ExternalTool's which can be used and add them
      // to an QActionGroup for reference
      for (int i = 0; i < 10; ++i)
      {
         ExternalTool* tool = new ExternalTool();
         tool->GetAction()->setVisible(false);
         tool->SetArgParsers(mExternalToolArgParsers);
         mExternalToolActionGroup->addAction(tool->GetAction());
         mTools.push_back(tool);
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::setupHelpActions()
   {
      // Help - About Editor
      mActionHelpAboutEditor = new QAction(tr("&About STAGE..."), this);
      mActionHelpAboutEditor->setStatusTip(tr("About STAGE"));
      connect(mActionHelpAboutEditor, SIGNAL(triggered()), this, SLOT(slotHelpAboutEditor()));

      // Help - About QT
      mActionHelpAboutQT = new QAction(tr("A&bout Qt..."), this);
      mActionHelpAboutQT->setStatusTip(tr("About Qt"));
      connect(mActionHelpAboutQT, SIGNAL(triggered()), this, SLOT(slotHelpAboutQT()));
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::setupRecentItems()
   {
      //std::vector<std::string> projs = EditorData::GetInstance().getMainWindow()->findRecentProjects();
      std::list<std::string>::iterator itor;

      mActionFileRecentProject0 = new QAction(tr("(Recent Project 1)"), this);
      mActionFileRecentProject0->setEnabled(false);
      connect(mActionFileRecentProject0, SIGNAL(triggered()), this, SLOT(slotFileRecentProject0()));

      itor = EditorData::GetInstance().getRecentProjects().begin();
      if (itor == EditorData::GetInstance().getRecentProjects().end())
      {
         return;
      }

      if (EditorData::GetInstance().getRecentProjects().size() > 0)
      {
         std::string str = (*itor);
         mActionFileRecentProject0->setText((*itor).c_str());
         mActionFileRecentProject0->setEnabled(true);
         connect(mActionFileRecentProject0, SIGNAL(triggered()), this, SLOT(slotFileRecentProject0()));
         ++itor;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorActions::refreshRecentProjects()
   {
      // do we have any recent projects?
      if (EditorData::GetInstance().getRecentProjects().size() == 0)
      {
         return;
      }
      // set the text on the actions now
      std::list<std::string>::iterator itor = EditorData::GetInstance().getRecentProjects().begin();
      if (itor == EditorData::GetInstance().getRecentProjects().end())
      {
         return;
      }

      //unsigned int i = EditorData::GetInstance().getRecentProjects().size();
      QString curText = mActionFileRecentProject0->text();
      mActionFileRecentProject0->setText(tr((*itor).c_str()));
      ++itor;
      mActionFileRecentProject0->setEnabled(true);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotFileEditPreferences()
   {
      slotPauseAutosave();
      PreferencesDialog dlg((QWidget*)EditorData::GetInstance().getMainWindow());
      if (dlg.exec()== QDialog::Accepted)
      {
         EditorEvents::GetInstance().emitEditorPreferencesChanged();
      }
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
      {
         return;
      }

      slotPauseAutosave();
      MapDialog mapDialog((QWidget*)EditorData::GetInstance().getMainWindow());
      if (mapDialog.exec() == QDialog::Accepted)
      {
         changeMaps(EditorData::GetInstance().getCurrentMap(), mapDialog.getFinalizedMap());
         EditorData::GetInstance().addRecentMap(mapDialog.getFinalizedMap()->GetName());
      }

      slotRestartAutosave();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotFileOpenMap()
   {
      // Make sure we have a valid project.  The project "should" always be
      // valid since this action is only enabled when there is a valid project.
      if (!dtDAL::Project::GetInstance().IsContextValid())
      {
         slotPauseAutosave();
         QMessageBox::critical(EditorData::GetInstance().getMainWindow(), tr("Map Open Error"),
            tr("The current project is not valid."), tr("OK"));
         slotRestartAutosave();
         return;
      }

      // Check the current map for changes and save them...
      int saveResult = saveCurrentMapChanges(true);
      if (saveResult == QMessageBox::Cancel || saveResult == QMessageBox::Abort)
      {
         return;
      }

      slotPauseAutosave();

      dtQt::DialogListSelection openMapDialog(EditorData::GetInstance().getMainWindow(), tr("Open Existing Map"), tr("Available Maps"));

      QStringList listItems;
      const std::set<std::string>& mapNames = dtDAL::Project::GetInstance().GetMapNames();
      for (std::set<std::string>::const_iterator i = mapNames.begin(); i != mapNames.end(); ++i)
      {
         listItems << i->c_str();
      }

      openMapDialog.SetListItems(listItems);
      if (openMapDialog.exec() == QDialog::Accepted)
      {
         dtCore::RefPtr<dtDAL::Map> newMap;

         // Attempt to open the specified map..
         try
         {
            EditorData::GetInstance().getMainWindow()->startWaitCursor();

            const QString& mapName = openMapDialog.GetSelectedItem();
            newMap = &dtDAL::Project::GetInstance().GetMap(mapName.toStdString());

            EditorData::GetInstance().getMainWindow()->endWaitCursor();
         }
         catch (dtUtil::Exception& e)
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

         // Finally, change to the requested map.
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
      {
         return;
      }

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
      // Save the current map without asking the user for permission.
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

      dtDAL::Map* myMap = EditorData::GetInstance().getCurrentMap();
      if (myMap == NULL)
      {
         slotRestartAutosave();
         return;
      }

      try
      {
         myMap->SetDescription(dlg.getMapDescription());
         EditorData::GetInstance().getMainWindow()->startWaitCursor();
         dtDAL::Project::GetInstance().SaveMapAs(*myMap, name, strippedName);
         EditorData::GetInstance().getMainWindow()->endWaitCursor();
      }
      catch (const dtUtil::Exception& e)
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

      dtDAL::Map* currMap = dtEditQt::EditorData::GetInstance().getCurrentMap();
      if (currMap == NULL)
      {
         EditorEvents::GetInstance().emitEditorCloseEvent();
         qApp->quit();
         return;
      }

      int result = saveCurrentMapChanges(true);
      if (result == QMessageBox::Abort)
      {
         // An error occurred during saving.
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
         mWasCancelled = true;
         slotRestartAutosave();
         return;
      }

      EditorEvents::GetInstance().emitEditorCloseEvent();
      // close the map because the actor libraries will be closed before the DAL, so a crash could
      // happen when the project tries to close the open maps in the destructor.
      changeMaps(currMap, NULL);

      qApp->quit();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotEditMapProperties()
   {
      DialogMapProperties mapPropsDialog((QWidget*)EditorData::GetInstance().getMainWindow());
      dtDAL::Map* map = EditorData::GetInstance().getCurrentMap();

      // If the current map is invalid, issue an error, else populate the dialog
      // box with the values from the current map.  The map "should" always be
      // valid since this action is only enabled when there is a valid map.
      if (map == NULL)
      {
         QMessageBox::critical((QWidget *)EditorData::GetInstance().getMainWindow(), tr("Error"),
            tr("Current map is not valid or no map is open."), tr("OK"));
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

      // If the user pressed the ok button, set the new map values.
      if (mapPropsDialog.exec()== QDialog::Accepted)
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
      if (!EditorData::GetInstance().getCurrentMap())
      {
         QMessageBox::critical(NULL, tr("Failure"),
            tr("A map must be open in order to edit libraries"),
            tr("OK"));
         return;
      }

      LibraryEditor libEdit((QWidget*)EditorData::GetInstance().getMainWindow());
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
      dtQt::LibraryPathsEditor editor(EditorData::GetInstance().getMainWindow(),
               EditorData::GetInstance().getCurrentLibraryDirectory());
      editor.exec();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotEditLocalSpace()
   {
      EditorData::GetInstance().SetUseGlobalOrientationForViewportWidget(!mActionLocalSpace->isChecked());
      EditorEvents::GetInstance().emitEditorPreferencesChanged();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotEditDuplicateActors()
   {
      LOG_INFO("Duplicating current actor selection.");

      // This commits any changes in the property editor.
      PropertyEditor& propEditor = EditorData::GetInstance().getMainWindow()->GetPropertyEditor();
      propEditor.CommitCurrentEdits();

      ViewportOverlay::ActorProxyList& selection =ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();
      dtCore::Scene* scene = ViewportManager::GetInstance().getMasterScene();
      dtCore::RefPtr<dtDAL::Map> currMap = EditorData::GetInstance().getCurrentMap();
      StageCamera* worldCam = ViewportManager::GetInstance().getWorldViewCamera();

      // Make sure we have valid data.
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

      // Create our offset vector which is used to jitter the cloned
      // proxies providing better feedback to the user.
      float actorCreationOffset = EditorData::GetInstance().GetActorCreationOffset();
      osg::Vec3 offset;
      if (worldCam != NULL)
      {
         offset = worldCam->getRightDir() * actorCreationOffset;
      }
      else
      {
         offset = osg::Vec3(actorCreationOffset, 0, 0);
      }

      // We're about to do a LOT of work, especially if lots of things are select
      // so, start a change transaction.
      EditorData::GetInstance().getMainWindow()->startWaitCursor();
      EditorEvents::GetInstance().emitBeginChangeTransaction();

      // Once we have a reference to the current selection and the scene,
      // clone each proxy, add it to the scene, make the newly cloned
      // proxy(s) the current selection.
      ViewportOverlay::ActorProxyList::iterator itor, itorEnd;
      itor    = selection.begin();
      itorEnd = selection.end();

      std::vector< dtCore::RefPtr<dtDAL::ActorProxy> > newSelection;
      for (; itor != itorEnd; ++itor)
      {
         dtDAL::ActorProxy* proxy = const_cast<dtDAL::ActorProxy*>(itor->get());
         dtCore::RefPtr<dtDAL::ActorProxy> copy = proxy->Clone();
         if (!copy.valid())
         {
            LOG_ERROR("Error duplicating proxy: " + proxy->GetName());
            continue;
         }

         newSelection.push_back(copy);
      }

      std::vector< dtCore::RefPtr<dtDAL::ActorProxy> >::iterator i, iend;

      i    = newSelection.begin();
      iend = newSelection.end();
      for (; i != iend; ++i)
      {
         dtDAL::ActorProxy* proxy = i->get();

         // Store the original location of the proxy so we can position after
         // it has been added to the scene.
         osg::Vec3 oldPosition;
         dtDAL::TransformableActorProxy* tProxy =
            dynamic_cast<dtDAL::TransformableActorProxy*>(proxy);
         if (tProxy != NULL)
         {
            oldPosition = tProxy->GetTranslation();
         }

         // Add the new proxy to the map and send out a create event.
         currMap->AddProxy(*proxy);

         EditorEvents::GetInstance().emitActorProxyCreated(proxy, false);

         // Move the newly duplicated actor to where it is supposed to go.
         if (tProxy != NULL)
         {
            tProxy->SetTranslation(oldPosition + offset);
         }
      }

      // Finally set the newly cloned proxies to be the current selection.
      ViewportManager::GetInstance().getViewportOverlay()->setMultiSelectMode(false);
      EditorEvents::GetInstance().emitActorsSelected(newSelection);
      EditorEvents::GetInstance().emitEndChangeTransaction();

      EditorData::GetInstance().getMainWindow()->endWaitCursor();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotEditDeleteActors()
   {
      LOG_INFO("Deleting current actor selection. ");

      ViewportOverlay::ActorProxyList selection = ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();
      dtCore::RefPtr<dtDAL::Map> currMap = EditorData::GetInstance().getCurrentMap();

      // Make sure we have valid data.
      if (!currMap.valid())
      {
         LOG_ERROR("Current map is not valid.");
         return;
      }

      // We're about to do a LOT of work, especially if lots of things are select
      // so, start a change transaction.
      EditorData::GetInstance().getMainWindow()->startWaitCursor();
      EditorEvents::GetInstance().emitBeginChangeTransaction();

      // Once we have a reference to the current selection and the scene,
      // remove each proxy's actor from the scene then remove the proxy from
      // the map.
      ViewportOverlay::ActorProxyList::iterator itor;
      for (itor = selection.begin(); itor != selection.end(); ++itor)
      {
         // \TODO: Find out why this const_cast is necessary. It compiles without
         // it on MSVC 7.1, but not on gcc4.0.2 -osb
         dtDAL::ActorProxy* proxy = const_cast<dtDAL::ActorProxy*>(itor->get());
         deleteProxy(proxy, currMap);
      }

      // Now that we have removed the selected objects, clear the current selection.
      std::vector< dtCore::RefPtr<dtDAL::ActorProxy> > emptySelection;
      EditorEvents::GetInstance().emitActorsSelected(emptySelection);
      EditorEvents::GetInstance().emitEndChangeTransaction();

      EditorData::GetInstance().getMainWindow()->endWaitCursor();
   }

   //////////////////////////////////////////////////////////////////////////////
   bool EditorActions::deleteProxy(dtDAL::ActorProxy* proxy,
      dtCore::RefPtr<dtDAL::Map> currMap)
   {
      bool               result   = false;
      dtCore::Scene*     scene    = ViewportManager::GetInstance().getMasterScene();
      dtDAL::ActorProxy* envProxy = currMap->GetEnvironmentActor();
      if (envProxy != NULL)
      {
         if (envProxy == proxy)
         {
            dtDAL::IEnvironmentActor* envActor =
               dynamic_cast<dtDAL::IEnvironmentActor*>(envProxy->GetActor());
            std::vector<dtCore::DeltaDrawable*> drawables;
            envActor->GetAllActors(drawables);
            envActor->RemoveAllActors();
            for (unsigned int i = 0; i < drawables.size(); ++i)
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
         {
            scene->RemoveDrawable(proxy->GetBillBoardIcon()->GetDrawable());
         }

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
   void EditorActions::slotOnActorCreated(ActorProxyRefPtr actor, bool forceNoAdjustments)
   {
      dtDAL::IEnvironmentActor* envActor =
         dynamic_cast<dtDAL::IEnvironmentActor*>(actor->GetActor());
      if (envActor == NULL)
      {
         return;
      }

      dtDAL::Map* map = EditorData::GetInstance().getCurrentMap();
      if (map == NULL)
      {
         return;
      }

      dtDAL::ActorProxy* envProxy = map->GetEnvironmentActor();
      QWidget* window = static_cast<QWidget*>(EditorData::GetInstance().getMainWindow());
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
            dtCore::Scene* scene = ViewportManager::GetInstance().getMasterScene();
            dtDAL::IEnvironmentActor* env =
               dynamic_cast<dtDAL::IEnvironmentActor*>(envProxy->GetActor());
            if (env != NULL)
            {
               env->RemoveAllActors();
            }
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
            dtCore::Scene* scene = ViewportManager::GetInstance().getMasterScene();
            scene->RemoveAllDrawables();
            map->SetEnvironmentActor(actor.get());
            dtDAL::Project::GetInstance().LoadMapIntoScene(*map, *scene);
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

      ViewportOverlay::ActorProxyList& selection =ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();
      dtCore::Scene* scene = ViewportManager::GetInstance().getMasterScene();

      if (scene == NULL)
      {
         LOG_ERROR("Current scene is not valid.");
         return;
      }

      // We're about to do a LOT of work, especially if lots of things are select
      // so, start a change transaction.
      EditorData::GetInstance().getMainWindow()->startWaitCursor();
      EditorEvents::GetInstance().emitBeginChangeTransaction();

      // Iterate through the current selection, trace a ray directly below it.  If there is
      // an intersection, move the current proxy to that point.
      ViewportOverlay::ActorProxyList::iterator itor;

      mIsector->Reset();
      mIsector->SetScene(scene);

      for (itor = selection.begin(); itor != selection.end(); ++itor)
      {
         dtDAL::ActorProxy* proxy = const_cast<dtDAL::ActorProxy*>(itor->get());
         dtDAL::TransformableActorProxy* tProxy =
            dynamic_cast<dtDAL::TransformableActorProxy*>(proxy);

         if (tProxy != NULL)
         {
            osg::Vec3 pos = tProxy->GetTranslation();

            mIsector->SetStartPosition(pos);
            mIsector->SetDirection(osg::Vec3(0, 0, -1));
            mIsector->Reset();

            // Find a possible intersection point.  If we find an intersection
            // point, move the actor to that location.
            if (mIsector->Update())
            {
               osgUtil::IntersectVisitor& iv = mIsector->GetIntersectVisitor();
               osg::Vec3 p = iv.getHitList(mIsector->GetLineSegment())[0].getWorldIntersectPoint();
               tProxy->SetTranslation(p);
            }
         }
      }

      EditorEvents::GetInstance().emitEndChangeTransaction();
      EditorData::GetInstance().getMainWindow()->endWaitCursor();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotEditGotoActor()
   {
      if (mActors.size()> 0)
      {
         EditorEvents::GetInstance().emitGotoActor(mActors[0]);
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotEditGroupActors()
   {
      EditorData::GetInstance().getCurrentMap()->SetModified(true);
      EditorEvents::GetInstance().emitProjectChanged();

      ViewportOverlay* overlay = ViewportManager::GetInstance().getViewportOverlay();
      ViewportOverlay::ActorProxyList& selection = overlay->getCurrentActorSelection();

      // Add all the selected actions into a new group.
      dtDAL::Map* map = EditorData::GetInstance().getCurrentMap();
      if (map)
      {
         // Remove the current actors from any groups they are currently in.
         for (int index = 0; index < (int)selection.size(); index++)
         {
            dtDAL::ActorProxy* proxy = selection[index].get();
            map->RemoveActorFromGroups(proxy);
         }

         int groupIndex = map->GetGroupCount();

         for (int index = 0; index < (int)selection.size(); index++)
         {
            dtDAL::ActorProxy* proxy = selection[index].get();
            map->AddActorToGroup(groupIndex, proxy);
         }
      }

      mActionGroupActors->setEnabled(false);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotEditUngroupActors()
   {
      EditorData::GetInstance().getCurrentMap()->SetModified(true);
      EditorEvents::GetInstance().emitProjectChanged();

      ViewportOverlay* overlay = ViewportManager::GetInstance().getViewportOverlay();
      ViewportOverlay::ActorProxyList& selection = overlay->getCurrentActorSelection();

      // Remove all the selected actions from their current groups.
      dtDAL::Map* map = EditorData::GetInstance().getCurrentMap();
      if (map)
      {
         for (int index = 0; index < (int)selection.size(); index++)
         {
            dtDAL::ActorProxy* proxy = selection[index].get();
            map->RemoveActorFromGroups(proxy);
         }
      }

      mActionUngroupActors->setEnabled(false);
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotProjectChangeContext()
   {
      int result = saveCurrentMapChanges(true);
      if (result == QMessageBox::Cancel || result == QMessageBox::Abort)
      {
         return;
      }

      slotPauseAutosave();
      ProjectContextDialog dialog((QWidget*)EditorData::GetInstance().getMainWindow());
      if (dialog.exec() == QDialog::Accepted)
      {
         std::string contextName = dialog.getProjectPath().toStdString();

         // First try to set the new project context.
         try
         {
            changeMaps(EditorData::GetInstance().getCurrentMap(), NULL);
            dtDAL::Project::GetInstance().CreateContext(contextName);
            dtDAL::Project::GetInstance().SetContext(contextName);
         }
         catch (dtUtil::Exception& e)
         {
            QMessageBox::critical((QWidget*)EditorData::GetInstance().getMainWindow(),
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
      EditorAboutBox box((QWidget*)EditorData::GetInstance().getMainWindow());
      box.exec();
      slotRestartAutosave();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotHelpAboutQT()
   {
      slotPauseAutosave();
      QMessageBox::aboutQt((QWidget*)EditorData::GetInstance().getMainWindow());
      slotRestartAutosave();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotAutosave()
   {
      try
      {
         if (EditorData::GetInstance().getCurrentMap())
         {
            dtDAL::Project::GetInstance().SaveMapBackup(*EditorData::GetInstance().getCurrentMap());
         }
      }
      catch(const dtUtil::Exception& e)
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
      {
         return name;
      }
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
      mTimer->stop();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotRestartAutosave()
   {
      mTimer->start();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotFileRecentProject0()
   {
      if (saveCurrentMapChanges(true) == QMessageBox::Cancel)
      {
         return;
      }

      changeMaps(EditorData::GetInstance().getCurrentMap(), NULL);

      EditorData::GetInstance().setCurrentProjectContext(mActionFileRecentProject0->text().toStdString());
      try
      {
         dtDAL::Project::GetInstance().SetContext(mActionFileRecentProject0->text().toStdString());
      }
      catch (const dtUtil::Exception& ex)
      {
         QMessageBox::warning((QWidget*)EditorData::GetInstance().getMainWindow(),
            tr("Project Context Open Error"), ex.What().c_str(), tr("OK"));
         return;
      }
      EditorEvents::GetInstance().emitProjectChanged();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorActions::slotFileRecentMap0()
   {
      if (saveCurrentMapChanges(true) == QMessageBox::Cancel)
      {
         return;
      }

      const std::string& newMapName = mActionFileRecentMap0->text().toStdString();
      dtDAL::Map* newMap;
      try
      {
         newMap = &dtDAL::Project::GetInstance().GetMap(newMapName);
      }
      catch (dtUtil::Exception& e)
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
   void EditorActions::changeMaps(dtDAL::Map* oldMap, dtDAL::Map* newMap)
   {
      // Make sure the two maps are different!  If they are the same, then
      // we need to close the map but make sure to re-open it.
      bool areSameMap = (oldMap == newMap);
      std::string oldMapName;

      // Make sure to catch this goofy state...
      if (oldMap == NULL && newMap == NULL)
      {
         return;
      }

      // Remove all the old map drawables from the current scene.
      if (oldMap != NULL)
      {
         EditorData::GetInstance().getMainWindow()->startWaitCursor();
         ViewportManager::GetInstance().clearMasterScene(oldMap->GetAllProxies());
         oldMapName = oldMap->GetName();

         // Close the old map...
         try
         {
            EditorEvents::GetInstance().emitLibraryAboutToBeRemoved();
            dtDAL::Project::GetInstance().CloseMap(*oldMap,true);
            EditorEvents::GetInstance().emitMapLibraryRemoved();

            EditorData::GetInstance().getMainWindow()->endWaitCursor();
         }
         catch(const dtUtil::Exception& e)
         {
            EditorData::GetInstance().getMainWindow()->endWaitCursor();
            QMessageBox::critical((QWidget *)EditorData::GetInstance().getMainWindow(),
               tr("Error"), e.What().c_str(), tr("OK"));
         }
      }

      EditorData::GetInstance().getMainWindow()->startWaitCursor();
      // If we tried to load the same map, make sure to re-open it since we just closed
      // it.
      if (areSameMap)
      {
         try
         {
            newMap = &dtDAL::Project::GetInstance().GetMap(oldMapName);
         }
         catch (dtUtil::Exception& e)
         {
            EditorData::GetInstance().getMainWindow()->endWaitCursor();
            QMessageBox::critical((QWidget *)EditorData::GetInstance().getMainWindow(),
               tr("Map Open Error"), e.What().c_str(), tr("OK"));
            return;
         }
      }

      // Load the new map into the current scene.
      if (newMap != NULL)
      {
         if (newMap->HasLoadingErrors())
         {
            HandleMissingLibraries(*newMap);
            HandleMissingActorsTypes(*newMap);
         }

         try
         {
            dtDAL::Project::GetInstance().LoadMapIntoScene(*newMap,
               *(ViewportManager::GetInstance().getMasterScene()), true);

         }
         catch (const dtUtil::Exception& e)
         {
            QMessageBox::critical((QWidget *)EditorData::GetInstance().getMainWindow(),
               tr("Error"), e.What().c_str(), tr("OK"));
         }
      }

      EditorData::GetInstance().getMainWindow()->endWaitCursor();
      // Update the editor state to reflect the changes.
      EditorData::GetInstance().setCurrentMap(newMap);
      EditorEvents::GetInstance().emitCurrentMapChanged();

      // Now that we have changed maps, clear the current selection.
      std::vector< dtCore::RefPtr<dtDAL::ActorProxy> > emptySelection;
      EditorEvents::GetInstance().emitActorsSelected(emptySelection);
   }

   //////////////////////////////////////////////////////////////////////////////
   int EditorActions::saveCurrentMapChanges(bool askPermission)
   {
      // This commits any changes in the property editor.
      PropertyEditor& propEditor = EditorData::GetInstance().getMainWindow()->GetPropertyEditor();
      propEditor.CommitCurrentEdits();

      dtDAL::Map* currMap = EditorData::GetInstance().getCurrentMap();
      int result = QMessageBox::NoButton;

      if (currMap == NULL || !currMap->IsModified())
      {
         return QMessageBox::Ignore;
      }

      slotPauseAutosave();

      if (askPermission)
      {
         result = QMessageBox::information(
            (QWidget*)EditorData::GetInstance().getMainWindow(),
            tr("Save Map?"),
            tr("The current map has been modified, would you like to save it?"),
            QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
      }

      if (result == QMessageBox::Yes || !askPermission)
      {
         try
         {
            EditorData::GetInstance().getMainWindow()->startWaitCursor();
            dtDAL::Project::GetInstance().SaveMap(*currMap);
            ((QMainWindow*)EditorData::GetInstance().getMainWindow())->setWindowTitle(
               getWindowName().c_str());
            EditorData::GetInstance().getMainWindow()->endWaitCursor();
         }
         catch (dtUtil::Exception& e)
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
         {
            dtDAL::Project::GetInstance().ClearBackup(*currMap);
         }
      }

      if (!askPermission)
      {
         slotRestartAutosave();
         return QMessageBox::Ignore;
      }
      else
      {
         slotRestartAutosave();
         return result; // Return the users response.
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorActions::SlotNewExternalToolEditor()
   {
      // launch ext tool editor
      ExternalToolDialog dialog(mTools, EditorData::GetInstance().getMainWindow());
      int retCode = dialog.exec();

      if (retCode == QDialog::Accepted)
      {
         //notify the world
         emit ExternalToolsModified(GetExternalToolActions());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   const QList<ExternalTool*>& EditorActions::GetExternalTools() const
   {
      return mTools;
   }

   //////////////////////////////////////////////////////////////////////////
   QList<ExternalTool*>& EditorActions::GetExternalTools()
   {
      return mTools;
   }

   //////////////////////////////////////////////////////////////////////////
   const QList<QAction*> EditorActions::GetExternalToolActions() const
   {
      QList<QAction*> actions;
      for (int i = 0; i < mTools.size(); ++i)
      {
         actions.push_back(mTools.at(i)->GetAction());
      }

      return actions;
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorActions::slotGetGotoPosition()
   {
      // display dialog
      QDialog* dialog = new QDialog(EditorData::GetInstance().getMainWindow());
      Ui::PositionDialog ui;
      ui.setupUi(dialog);
      int retCode = dialog->exec();

      // if OK
      if (retCode == QDialog::Accepted)
      {
         // emit new position to move cameras to
         EditorEvents::GetInstance().emitGotoPosition(ui.xSpinBox->value(), ui.ySpinBox->value(), ui.zSpinBox->value());
      }

      delete dialog;
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorActions::HandleMissingLibraries(const dtDAL::Map& newMap) const
   {
      const std::vector<std::string>& missingLibs = newMap.GetMissingLibraries();

      if (!missingLibs.empty())
      {
         QString errors(tr("The following libraries listed in the map could not be loaded:\n\n"));

         for (size_t i = 0; i < missingLibs.size(); ++i)
         {
            std::string nativeName = dtUtil::LibrarySharingManager::GetPlatformSpecificLibraryName(missingLibs[i]);
            errors.append(nativeName.c_str());
            errors.append("\n");
         }

         errors.append("\nThis could happen for a number of reasons. Please ensure that the name is correct, ");
         errors.append("the library is in the path (or the working directory), the library can load correctly, and dependent libraries are available.");
         errors.append("If you save this map, the library and any actors referenced by the library will be lost.");

         QMessageBox::warning(EditorData::GetInstance().getMainWindow(), tr("Missing Libraries"), errors, tr("OK"));
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorActions::HandleMissingActorsTypes(const dtDAL::Map& newMap) const
   {
      const std::set<std::string>& missingActorTypes = newMap.GetMissingActorTypes();

      if (!missingActorTypes.empty())
      {
         QString errors;
         errors = tr("The following ActorTypes listed in the map could not be created.  ");
         errors += tr("This could happen if the ActorType has changed names, or");
         errors += tr(" if the Actor has been removed from the actor library or registry.");
         errors += tr("\nIf you save this map, any actors of these ActorType will be lost:\n\n");

         std::set<std::string>::const_iterator itr = missingActorTypes.begin();
         while (itr != missingActorTypes.end())
         {
            errors += QString::fromStdString((*itr));

            std::string replacement = dtDAL::LibraryManager::GetInstance().FindActorTypeReplacement((*itr));
            if (!replacement.empty())
            {
               errors += tr(" (replace with ") + QString::fromStdString(replacement) + ")";
            }

            errors.append("\n");
            ++itr;
         }

         QMessageBox::warning(EditorData::GetInstance().getMainWindow(), tr("Missing ActorTypes"), errors, tr("OK"));
      }
   }

} // namespace dtEditQt
