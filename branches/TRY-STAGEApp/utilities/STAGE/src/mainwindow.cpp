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
 * Matthew W. Campbell
 */
#include <prefix/dtstageprefix-src.h>
#include <QtGui/QApplication>
#include <QtGui/QIcon>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QDockWidget>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QMessageBox>
#include <QtGui/QCloseEvent>
#include <QtCore/QDir>
#include <QtCore/QTimer>

#include <dtActors/volumeeditactor.h>
#include <dtCore/uniqueid.h>
#include <dtCore/globals.h>
#include <dtCore/deltawin.h>
#include <dtCore/transform.h>
#include <dtUtil/macros.h>
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
#include <dtEditQt/editorviewportcontainer.h>
#include <dtEditQt/pluginmanager.h>
#include <dtEditQt/propertyeditor.h>
#include <dtEditQt/actortab.h>
#include <dtEditQt/resourcebrowser.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/projectcontextdialog.h>
#include <dtEditQt/uiresources.h>
#include <dtEditQt/externaltool.h>
#include <dtEditQt/stageapplication.h>
#include <dtQt/osggraphicswindowqt.h>

#include <osgDB/FileNameUtils>
#include <osgDB/Registry>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   MainWindow::MainWindow(const std::string& stagePath)
      : mCfgMgr()
      , mPluginManager(new PluginManager(this))
      , mSTAGEFullPath(stagePath)
      , mFileMenu(NULL)
      , mEditMenu(NULL)
      , mProjectMenu(NULL)
      , mWindowMenu(NULL)
      , mHelpMenu(NULL)
      , mRecentProjs(NULL)
      , mRecentMaps(NULL)
      , mToolsMenu(NULL)
      , mPropertyWindow(NULL)
      , mActorTab(NULL)
      , mResourceBrowser(NULL)
   {
      // Ensure that the global singletons are lazily instantiated now
      dtDAL::LibraryManager::GetInstance();
      EditorActions::GetInstance();
      EditorEvents::GetInstance();
      EditorData::GetInstance();

      // alert the project instance that we are working within STAGE
      // changed on 7/10/2006 banderegg
      dtDAL::Project::GetInstance().SetEditMode(true);

      ViewportManager::GetInstance();
      
      //Read STAGE configuration file      
      mCfgMgr.ReadXML(dtCore::GetDeltaRootPath() + "/utilities/STAGE/STAGEConfig.xml");

      connectSlots();
      setupDockWindows();
      setupStatusBar();
      setupMenus();
      setupToolbar();

      setupVolumeEditActor();

      //EditorData::GetInstance().setUndoManager(new UndoManager());

      // Make sure some default UI states are correctly initialized.
      setWindowTitle(tr("STAGE"));
      EditorData::GetInstance().setMainWindow(this);

      // add the application icon
      QIcon icon;
      icon.addPixmap(QPixmap(UIResources::ICON_APPLICATION.c_str()));
      setWindowIcon(icon);      
      
      // setup plugins
      SetupPlugins();
   }

   ///////////////////////////////////////////////////////////////////////////////
   MainWindow::~MainWindow()
   {
      if(mCfgMgr.GetVariable(ConfigurationManager::GENERAL,
                              "SaveConfigurationOnClose") == "true")
      {
         //Save configuration
         

         //Sample of how to save some STAGE config variables that we might care about:
         //QSplitter* hSplit = mSplitters.at(0);
         //QSize hSize = hSplit->frameSize();  
         //mCfgMgr.SetVariable(ConfigurationManager::LAYOUT, "ShowTopView", hSize.height());
         //mCfgMgr.SetVariable(ConfigurationManager::LAYOUT, "HorizontalViewFrameWidth", hSize.width());

         mCfgMgr.WriteXML(dtCore::GetDeltaRootPath() + "/utilities/STAGE/STAGEConfig.xml");
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::setupMenus()
   {
      EditorActions& editorActions = EditorActions::GetInstance();

      mRecentProjs = new QMenu(tr("Recent Projects"), this);
      mRecentMaps  = new QMenu(tr("Recent Maps"),     this);

      mRecentProjs->addAction(editorActions.mActionFileRecentProject0);

      if (mCfgMgr.GetVariable(ConfigurationManager::MENU, "MenuType") == "Basic")
      {
         mFileMenu = menuBar()->addMenu(tr("&Edit Map"));         
      }
      else
      {
         mFileMenu = menuBar()->addMenu(tr("&File"));
         mFileMenu->addAction(editorActions.mActionFileNewMap);
         mFileMenu->addAction(editorActions.mActionFileOpenMap);
         mFileMenu->addAction(editorActions.mActionFileCloseMap);
         mFileMenu->addSeparator();
         mFileMenu->addAction(editorActions.mActionFileSaveMap);
         mFileMenu->addAction(editorActions.mActionFileSaveMapAs);
         mFileMenu->addSeparator();
         mFileMenu->addAction(editorActions.mActionFileExportPrefab);
         mFileMenu->addSeparator();
         mFileMenu->addAction(editorActions.mActionFileChangeProject);
         mFileMenu->addSeparator();
         mFileMenu->addMenu(mRecentProjs);
         mFileMenu->addSeparator();
         mFileMenu->addAction(editorActions.mActionFileEditLibraryPaths);
         mFileMenu->addSeparator();
         mFileMenu->addAction(editorActions.mActionFileExit);

         mEditMenu = menuBar()->addMenu(tr("&Edit"));
         mEditMenu->addAction(editorActions.mActionEditUndo);
         mEditMenu->addAction(editorActions.mActionEditRedo);
         mEditMenu->addSeparator();
         mEditMenu->addAction(editorActions.mActionLocalSpace);
         mEditMenu->addSeparator();
         mEditMenu->addAction(editorActions.mActionGroupActors);
         mEditMenu->addAction(editorActions.mActionUngroupActors);
         mEditMenu->addAction(editorActions.mActionEditDuplicateActor);
         mEditMenu->addAction(editorActions.mActionEditDeleteActor);
         mEditMenu->addAction(editorActions.mActionEditGroundClampActors);
         mEditMenu->addAction(editorActions.mActionEditGotoActor);
         mEditMenu->addSeparator();
         mEditMenu->addAction(editorActions.mActionGetGotoPosition);
         mEditMenu->addSeparator();
         mEditMenu->addAction(editorActions.mActionEditMapProperties);
         mEditMenu->addAction(editorActions.mActionEditMapLibraries);
         mEditMenu->addAction(editorActions.mActionEditMapEvents);
         mEditMenu->addSeparator();
         mEditMenu->addAction(editorActions.mActionFileEditPreferences);

         mWindowMenu = menuBar()->addMenu(tr("&Window"));
         mWindowMenu->addAction(editorActions.mActionWindowsPropertyEditor);
         mWindowMenu->addAction(editorActions.mActionWindowsActorSearch);
         mWindowMenu->addAction(editorActions.mActionWindowsResourceBrowser);
         mWindowMenu->addSeparator();
         mWindowMenu->addAction(editorActions.mActionWindowsResetWindows);

         mToolsMenu = menuBar()->addMenu(tr("&Tools"));

         mHelpMenu = menuBar()->addMenu(tr("&Help"));
         mHelpMenu->addAction(editorActions.mActionHelpAboutEditor);
         mHelpMenu->addAction(editorActions.mActionHelpAboutQT);
      } //end else 
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::setupToolbar()
   {
      mFileToolBar = new QToolBar(this);
      mFileToolBar->setObjectName("FileToolBar");
      mFileToolBar->setWindowTitle(tr("File Toolbar"));
      mFileToolBar->setMinimumWidth(10);
      mFileToolBar->addAction(EditorActions::GetInstance().mActionFileNewMap);
      mFileToolBar->addAction(EditorActions::GetInstance().mActionFileOpenMap);
      mFileToolBar->addAction(EditorActions::GetInstance().mActionFileSaveMap);
      addToolBar(mFileToolBar);

      mEditToolBar = new QToolBar(this);
      mEditToolBar->setObjectName("EditToolBar");
      mEditToolBar->setWindowTitle(tr("Edit Toolbar"));
      mEditToolBar->setMinimumWidth(10);
      mEditToolBar->addAction(EditorActions::GetInstance().mActionLocalSpace);
      mEditToolBar->addSeparator();
      mEditToolBar->addAction(EditorActions::GetInstance().mActionGroupActors);
      mEditToolBar->addAction(EditorActions::GetInstance().mActionUngroupActors);
      mEditToolBar->addAction(EditorActions::GetInstance().mActionEditDuplicateActor);
      mEditToolBar->addAction(EditorActions::GetInstance().mActionEditDeleteActor);
      mEditToolBar->addAction(EditorActions::GetInstance().mActionEditGotoActor);
      mEditToolBar->addAction(EditorActions::GetInstance().mActionEditGroundClampActors);
      //mEditToolBar->addAction(EditorActions::GetInstance().mActionEditTaskEditor);
      addToolBar(mEditToolBar);

      mUndoToolBar = new QToolBar(this);
      mUndoToolBar->setObjectName("UndoToolBar");
      mUndoToolBar->setWindowTitle(tr("Undo Toolbar"));
      mUndoToolBar->setMinimumWidth(10);
      mUndoToolBar->addAction(EditorActions::GetInstance().mActionEditUndo);
      mUndoToolBar->addAction(EditorActions::GetInstance().mActionEditRedo);
      addToolBar(mUndoToolBar);

      mExternalToolsToolBar = new QToolBar(this);
      mExternalToolsToolBar->setObjectName("ExternalToolsToolBar");
      mExternalToolsToolBar->setWindowTitle(tr("External Tools ToolBar"));
      addToolBar(mExternalToolsToolBar);
      RebuildToolsMenu(EditorActions::GetInstance().GetExternalToolActions());
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


      if(mCfgMgr.GetVariable(ConfigurationManager::LAYOUT, "ShowPropertyEditor") != "false")
      {      
         // create the main left dock window
         mPropertyWindow = new PropertyEditor(this);
         mPropertyWindow->setObjectName("PropertyWindow");

         // listen for selection changed event
         connect(&EditorEvents::GetInstance(), SIGNAL(selectedActors(ActorProxyRefPtrVector&)),
                  mPropertyWindow, SLOT(handleActorsSelected(ActorProxyRefPtrVector&)));

         // listen for property change events and update the tree.  These can be generated
         // by the viewports, or the tree itself.
         connect(&EditorEvents::GetInstance(), SIGNAL(actorPropertyChanged(ActorProxyRefPtr, ActorPropertyRefPtr)),
                  mPropertyWindow, SLOT(actorPropertyChanged(ActorProxyRefPtr, ActorPropertyRefPtr)));

         // listen for name changes so we can update our group box label or handle undo changes
         connect(&EditorEvents::GetInstance(), SIGNAL(proxyNameChanged(ActorProxyRefPtr, std::string)),
                  mPropertyWindow, SLOT(proxyNameChanged(ActorProxyRefPtr, std::string)));
         
         mPropertyWindow->setFeatures(QDockWidget::AllDockWidgetFeatures);
         addDockWidget(Qt::LeftDockWidgetArea,  mPropertyWindow);
      }

      if(mCfgMgr.GetVariable(ConfigurationManager::LAYOUT, "ShowActorTab") != "false")
      {
         mActorTab = new ActorTab(this);
         mActorTab->setObjectName("ActorTab");
         mActorTab->setFeatures(QDockWidget::AllDockWidgetFeatures);
         addDockWidget(Qt::LeftDockWidgetArea, mActorTab);
      }

      if(mCfgMgr.GetVariable(ConfigurationManager::LAYOUT, "ShowResourceBrowser") != "false")
      {
         mResourceBrowser = new ResourceBrowser(this);
         mResourceBrowser->setObjectName("ResourceBrowser");
         mResourceBrowser->setFeatures(QDockWidget::AllDockWidgetFeatures);      
         addDockWidget(Qt::RightDockWidgetArea, mResourceBrowser);
      }

      // Create the viewports, assign them to splitters, and embed the splitters
      // in the central widget of the main window.
      mMainViewportParent = setupViewports();
      setCentralWidget(mMainViewportParent);
   }

   ///////////////////////////////////////////////////////////////////////////////
   QWidget* MainWindow::setupViewports()
   {
      ViewportManager& vpMgr = ViewportManager::GetInstance();

      // Create our splitters which will house the newly created
      // viewports.
      QSplitter* hSplit  = new QSplitter(Qt::Vertical); // Split top-bottom
      QSplitter* vSplit1 = new QSplitter(hSplit); // Split for top and front views
      QSplitter* vSplit2 = new QSplitter(hSplit); // Split for 3d and side views
      mSplitters.push_back(hSplit);
      mSplitters.push_back(vSplit1);
      mSplitters.push_back(vSplit2);

      //Create the actual viewport objects..
      mPerspView = (PerspectiveViewport*)vpMgr.createViewport("Perspective View",
         ViewportManager::ViewportType::PERSPECTIVE);
      mPerspView->setAutoInteractionMode(true);
      mPerspView->getCamera()->getDeltaCamera()->GetOSGCamera()->setCullMask(0x0F00000F);  //magic number, based on ObjectMotionModel's mask
      mPerspView->GetRootNode()->setNodeMask(0x0000000F);

      mTopView = (OrthoViewport*)vpMgr.createViewport("Top View (XY)",
         ViewportManager::ViewportType::ORTHOGRAPHIC);
      mTopView->setAutoInteractionMode(true);
      mTopView->getCamera()->getDeltaCamera()->GetOSGCamera()->setCullMask(0x0F0000F0);
      mTopView->GetRootNode()->setNodeMask(0x000000F0);

      mSideView = (OrthoViewport*)vpMgr.createViewport("Side View (YZ)",
         ViewportManager::ViewportType::ORTHOGRAPHIC);
      mSideView->setViewType(OrthoViewport::OrthoViewType::SIDE,false);
      mSideView->setAutoInteractionMode(true);
      mSideView->getCamera()->getDeltaCamera()->GetOSGCamera()->setCullMask(0x0F000F00);
      mSideView->GetRootNode()->setNodeMask(0x00000F00);

      mFrontView = (OrthoViewport*)vpMgr.createViewport("Front View (XZ)",
         ViewportManager::ViewportType::ORTHOGRAPHIC);
      mFrontView->setViewType(OrthoViewport::OrthoViewType::FRONT,false);
      mFrontView->setAutoInteractionMode(true);
      mFrontView->getCamera()->getDeltaCamera()->GetOSGCamera()->setCullMask(0x0F00F000);
      mFrontView->GetRootNode()->setNodeMask(0x0000F000);


      // We now wrap each viewport in a viewport container to provide the
      // toolbar and right click menu add-ons which are needed by the editor
      // for each viewport.
      ViewportContainer* container = NULL;

      if(mCfgMgr.GetVariable(ConfigurationManager::LAYOUT, "ShowTopView") != "false")
      {
         container = new ViewportContainer(mSideView, vSplit2);
      }
      if(mCfgMgr.GetVariable(ConfigurationManager::LAYOUT, "ShowPerspView") != "false")
      {
         container = new ViewportContainer(mPerspView, vSplit2);
      }
      if(mCfgMgr.GetVariable(ConfigurationManager::LAYOUT, "ShowTopView") != "false")
      {
         container = new ViewportContainer(mTopView, vSplit1);
      }
      if(mCfgMgr.GetVariable(ConfigurationManager::LAYOUT, "ShowFrontView") != "false")
      {
         container = new ViewportContainer(mFrontView, vSplit1);
      }

      // Create our editor container for all of our views.
      EditorViewportContainer* editorContainer = new EditorViewportContainer(hSplit);
      editorContainer->addViewport(mPerspView);
      editorContainer->addViewport(mSideView);
      editorContainer->addViewport(mTopView);
      editorContainer->addViewport(mFrontView);

      // Returns the root of the viewport widget hierarchy.
      return editorContainer;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::setupVolumeEditActor()
   {
      //The persistent pseudo-actor that is used for special-purpose editing
      dtCore::RefPtr<dtDAL::ActorProxy> proxy =         
         dtDAL::LibraryManager::GetInstance().CreateActorProxy("dtutil", "Volume Edit").get();      
      ViewportManager::GetInstance().getMasterScene()->AddDrawable(proxy->GetActor());

      //move the VolumeEditActor away from the Perspective camera so we can see it.
      dtCore::Transformable* volEditAct = dynamic_cast<dtCore::Transformable*>(proxy->GetActor());
      if(volEditAct != NULL)
      {
         dtCore::Transform xForm;
         volEditAct->GetTransform(xForm);

         osg::Vec3 xyz = xForm.GetTranslation();
         xyz[1] += 50.0f;
         xForm.SetTranslation(xyz);

         volEditAct->SetTransform(xForm);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::enableActions()
   {
      const bool hasProject    = dtDAL::Project::GetInstance().IsContextValid();
      const bool hasCurrentMap = (EditorData::GetInstance().getCurrentMap() != NULL);
      const bool hasBoth       = hasProject && hasCurrentMap;

      EditorActions::GetInstance().mActionFileNewMap->setEnabled(hasProject);
      EditorActions::GetInstance().mActionFileOpenMap->setEnabled(hasProject);
      EditorActions::GetInstance().mActionFileCloseMap->setEnabled(hasBoth);
      EditorActions::GetInstance().mActionFileSaveMap->setEnabled(hasBoth);
      EditorActions::GetInstance().mActionFileSaveMapAs->setEnabled(hasBoth);
      EditorActions::GetInstance().mActionFileExportPrefab->setEnabled(false);
      EditorActions::GetInstance().mActionFileExit->setEnabled(true);

      EditorActions::GetInstance().mActionGroupActors->setEnabled(false);
      EditorActions::GetInstance().mActionUngroupActors->setEnabled(false);
      EditorActions::GetInstance().mActionEditDuplicateActor->setEnabled(false);
      EditorActions::GetInstance().mActionEditDeleteActor->setEnabled(false);
      EditorActions::GetInstance().mActionEditGotoActor->setEnabled(false);
      EditorActions::GetInstance().mActionEditGroundClampActors->setEnabled(false);
      EditorActions::GetInstance().mActionEditMapProperties->setEnabled(hasBoth);
      EditorActions::GetInstance().mActionEditMapLibraries->setEnabled(hasBoth);
      EditorActions::GetInstance().mActionEditTaskEditor->setEnabled(hasBoth);
      EditorActions::GetInstance().mActionEditMapEvents->setEnabled(hasBoth);

      EditorActions::GetInstance().mActionWindowsActorSearch->setEnabled(hasBoth);
      EditorActions::GetInstance().mActionWindowsPropertyEditor->setEnabled(hasBoth);
      EditorActions::GetInstance().mActionWindowsResourceBrowser->setEnabled(hasBoth);
      EditorActions::GetInstance().mActionWindowsResetWindows->setEnabled(true);

      EditorActions::GetInstance().mActionHelpAboutEditor->setEnabled(true);
      EditorActions::GetInstance().mActionHelpAboutQT->setEnabled(true);

      // enable main window areas
      if(mPropertyWindow != NULL)
      {
         mPropertyWindow->setEnabled(hasBoth);
      }
      if(mActorTab != NULL)
      {
         mActorTab->setEnabled(hasBoth);
      }
      if(mResourceBrowser != NULL)
      {
         mResourceBrowser->setEnabled(hasProject);
      }

      mMainViewportParent->setEnabled(true);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::onResetWindows()
   {      
      if (mPropertyWindow != NULL)
      {
         // If detached, reattach
         mPropertyWindow->setFloating(false);
         // This should always default back to visible, like the app was restarted and the .ini
         // was deleted      
         mPropertyWindow->setVisible(true);
         EditorActions::GetInstance().mActionWindowsPropertyEditor->setChecked(true);
         addDockWidget(Qt::LeftDockWidgetArea,  mPropertyWindow);
      }
      
      if (mActorTab != NULL)
      {
         mActorTab->setFloating(false);
         mActorTab->setVisible(true);
         EditorActions::GetInstance().mActionWindowsActorSearch->setChecked(true);
         addDockWidget(Qt::LeftDockWidgetArea,  mActorTab);
      }

      if (mResourceBrowser != NULL)
      {
         mResourceBrowser->setFloating(false);
         mResourceBrowser->setVisible(true);      
         EditorActions::GetInstance().mActionWindowsResourceBrowser->setChecked(true);      
         addDockWidget(Qt::RightDockWidgetArea, mResourceBrowser);
      }

      ResetSplitters();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::onEditorInitiated()
   {
      setUpdatesEnabled(false);

      try
      {
         enableActions();

         // Load the custom library paths if they exist
         loadLibraryPaths();
         findAndLoadPreferences();
         mPerspView->onEditorPreferencesChanged();

         if (!EditorData::GetInstance().getLoadLastProject())//FindRecentProjects().empty())
         {
            ProjectContextDialog dialog(this);
            if (dialog.exec() == QDialog::Accepted)
            {
               std::string contextName = dialog.getProjectPath().toStdString();

               // First try to set the new project context.
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
               catch(const dtUtil::Exception& e)
               {
                  endWaitCursor();
                  QMessageBox::critical((QWidget*)this,
                     tr("Error"), tr(e.What().c_str()), tr("OK"));
               }
            }
         }
         else
         {
            std::list<std::string>& projects = EditorData::GetInstance().getRecentProjects();
            if (!projects.empty())
            {
               startWaitCursor();

               std::string contextName = projects.front();
               dtDAL::Project::GetInstance().SetContext(contextName);
               EditorData::GetInstance().setCurrentProjectContext(contextName);
               EditorData::GetInstance().addRecentProject(contextName);
               EditorEvents::GetInstance().emitProjectChanged();
               EditorActions::GetInstance().refreshRecentProjects();

               EditorEvents::GetInstance().emitProjectChanged();

               endWaitCursor();
            }
         }

         startWaitCursor();
         EditorActions::GetInstance().refreshRecentProjects();
         endWaitCursor();

         if (EditorData::GetInstance().getLoadLastMap())
         {
            std::list<std::string>& maps = EditorData::GetInstance().getRecentMaps();
            if (!maps.empty())
            {
               checkAndLoadBackup(maps.front());
            }
         }

         EditorActions::GetInstance().getTimer()->start();

         updateWindowTitle();
         //findAndLoadPreferences();

         setUpdatesEnabled(true);
         if (mPropertyWindow != NULL)
         {
            mPropertyWindow->setUpdatesEnabled(true);
         }
         if (mActorTab != NULL)
         {
            mActorTab->setUpdatesEnabled(true);
         }
         if (mResourceBrowser != NULL)
         {
            mResourceBrowser->setUpdatesEnabled(true);
         }
      }
      catch(const dtUtil::Exception& ex)
      {
         setUpdatesEnabled(true);
         if(mPropertyWindow != NULL)
         {
            mPropertyWindow->setUpdatesEnabled(true);
         }
         if (mActorTab != NULL)
         {
            mActorTab->setUpdatesEnabled(true);
         }
         if (mResourceBrowser != NULL)
         {
            mResourceBrowser->setUpdatesEnabled(true);
         }

         throw ex;
      }
      catch(const std::exception& ex)
      {
         setUpdatesEnabled(true);
         if (mPropertyWindow != NULL)
         {
            mPropertyWindow->setUpdatesEnabled(true);
         }
         if (mActorTab != NULL)
         {
            mActorTab->setUpdatesEnabled(true);
         }
         if (mResourceBrowser != NULL)
         {
            mResourceBrowser->setUpdatesEnabled(true);
         }

         throw ex;
      }

      update();
      repaint();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::onEditorShutDown()
   {
      if(mPropertyWindow != NULL)
      {
         // listen for selection changed event
         disconnect(&EditorEvents::GetInstance(), SIGNAL(selectedActors(ActorProxyRefPtrVector&)),
            mPropertyWindow, SLOT(handleActorsSelected(ActorProxyRefPtrVector&)));

         // listen for property change events and update the tree.  These can be generated
         // by the viewports, or the tree itself.
         disconnect(&EditorEvents::GetInstance(), SIGNAL(actorPropertyChanged(ActorProxyRefPtr,
            ActorPropertyRefPtr)),
            mPropertyWindow, SLOT(actorPropertyChanged(ActorProxyRefPtr,
            ActorPropertyRefPtr)));

         // listen for name changes so we can update our group box label or handle undo changes
         disconnect(&EditorEvents::GetInstance(), SIGNAL(proxyNameChanged(ActorProxyRefPtr, std::string)),
                  mPropertyWindow, SLOT(proxyNameChanged(ActorProxyRefPtr, std::string)));
      }

      EditorData& editorData = EditorData::GetInstance();
      EditorSettings settings;

      //Save the main window state...
      settings.beginGroup(EditorSettings::MAINWIN_GROUP);
      settings.setValue(EditorSettings::MAINWIN_POSITION, pos());
      settings.setValue(EditorSettings::MAINWIN_SIZE, size());
      settings.setValue(EditorSettings::MAINWIN_DOCK_STATE, saveState(EditorSettings::MAINWIN_DOCK_STATE_ID));
      settings.setValue(EditorSettings::MAINWIN_GEOMETRY, saveGeometry());
      settings.endGroup();

      //splitter data
      settings.remove(EditorSettings::SPLITTER_GROUP);
      settings.beginWriteArray(EditorSettings::SPLITTER_GROUP);
      for (int s = 0; s < mSplitters.size(); ++s)
      {
         settings.setArrayIndex(s);
         settings.setValue(EditorSettings::SPLITTER_SIZE, mSplitters.at(s)->saveState());
      }
      settings.endArray();

      // Save the general preferences...
      settings.beginGroup(EditorSettings::PREFERENCES_GROUP);
      settings.setValue(EditorSettings::LOAD_RECENT_PROJECTS, editorData.getLoadLastProject());
      settings.setValue(EditorSettings::LOAD_RECENT_MAPS, editorData.getLoadLastMap());
      settings.setValue(EditorSettings::RIGID_CAMERA, editorData.getRigidCamera());
      settings.setValue(EditorSettings::ACTOR_CREATION_OFFSET, editorData.GetActorCreationOffset());
      settings.setValue(EditorSettings::SAVE_MILLISECONDS, EditorActions::GetInstance().mSaveMilliSeconds);
      settings.setValue(EditorSettings::SELECTION_COLOR, editorData.getSelectionColor());
      settings.endGroup();


      // Save any custom library paths
      // Save the current project state...
      settings.remove(EditorSettings::LIBRARY_PATHS);
      settings.beginGroup(EditorSettings::LIBRARY_PATHS);

      std::vector<std::string> pathList;
      dtUtil::LibrarySharingManager::GetInstance().GetSearchPath(pathList);
      if (!pathList.empty())
      {
         int pos = 0;
         for (std::vector<std::string>::iterator iter = pathList.begin();
            iter != pathList.end();
            ++iter)
         {
            QString item = EditorSettings::LIBRARY_PATH_N;

            item += QString::number(pos++);

            settings.setValue(item, tr((*iter).c_str()));
         }
      }
      settings.endGroup();

      //save the external tools
      const QList<ExternalTool*> extTools = EditorActions::GetInstance().GetExternalTools();

      settings.remove(EditorSettings::EXTERNAL_TOOLS); //remove old ones
      settings.beginWriteArray(EditorSettings::EXTERNAL_TOOLS);
      int savedIdx = 0;///<used to keep the external tools sorted correctly
      for (int toolIdx = 0; toolIdx < extTools.size(); ++toolIdx)
      {
         if (extTools.at(toolIdx)->GetAction()->isVisible())
         {
            settings.setArrayIndex(savedIdx);
            savedIdx++;
            settings.setValue(EditorSettings::EXTERNAL_TOOL_TITLE, extTools.at(toolIdx)->GetTitle());
            settings.setValue(EditorSettings::EXTERNAL_TOOL_COMMAND, extTools.at(toolIdx)->GetCmd());
            settings.setValue(EditorSettings::EXTERNAL_TOOL_ARGS, extTools.at(toolIdx)->GetArgs());
            settings.setValue(EditorSettings::EXTERNAL_TOOL_WORKING_DIR, extTools.at(toolIdx)->GetWorkingDir());
            settings.setValue(EditorSettings::EXTERNAL_TOOL_ICON, extTools.at(toolIdx)->GetIcon());
         }
      }
      settings.endArray();

      // Save the recent projects unless the user does not wish to do so.
      if (!EditorData::GetInstance().getLoadLastProject())
      {
         return;
      }

      // Save the current project state...
      settings.beginGroup(EditorSettings::RECENT_PROJECTS);
      if (!EditorData::GetInstance().getRecentProjects().empty())
      {
         settings.setValue(EditorSettings::RECENT_PROJECT0,
            QVariant(QString(EditorData::GetInstance().getRecentProjects().front().c_str())));
         EditorData::GetInstance().getRecentProjects().pop_front();
      }
      settings.endGroup();

      //Check to see if the user wants the app to remember the recently loaded map.
      if (!EditorData::GetInstance().getLoadLastMap() ||
         !EditorData::GetInstance().getCurrentMap())
      {
         // Error check, if they have a previous settings file with a recent map in it, it
         // needs to be deleted as to not load it next time
         settings.remove(EditorSettings::RECENT_MAPS);
         return;
      }

      //Save the current map state...
      settings.beginGroup(EditorSettings::RECENT_MAPS);
      if (!EditorData::GetInstance().getRecentMaps().empty())
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
      {
         return;
      }

      EditorData::GetInstance().getCurrentMap()->SetModified(true);
      updateWindowTitle();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::closeEvent(QCloseEvent* e)
   {
      EditorActions::GetInstance().mWasCancelled = false;

      dtDAL::Map* curMap = dtEditQt::EditorData::GetInstance().getCurrentMap();
      if (curMap == NULL)
      {
         EditorEvents::GetInstance().emitEditorCloseEvent();
         e->accept();
         return;
      }

      dtEditQt::EditorActions::GetInstance().slotFileExit();
      EditorActions::GetInstance().mWasCancelled ? e->ignore() : e->accept();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MainWindow::keyPressEvent(QKeyEvent* e)
   {
      QMainWindow::keyPressEvent(e);

      if (e->key() == Qt::Key_Home) ///<Go to currently selected item
      {
         EditorActions::GetInstance().slotEditGotoActor();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::onPropertyEditorSelection()
   {
      if(mPropertyWindow != NULL)
      {
         mPropertyWindow->setVisible(EditorActions::GetInstance().mActionWindowsPropertyEditor->isChecked());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::onActorSearchSelection()
   {
      if(mActorTab != NULL)
      {
         mActorTab->setVisible(EditorActions::GetInstance().mActionWindowsActorSearch->isChecked());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::onResourceBrowserSelection()
   {
      if(mResourceBrowser != NULL)
      {
         mResourceBrowser->setVisible(EditorActions::GetInstance().mActionWindowsResourceBrowser->isChecked());
      }
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

      // Remove this actor from any groups it may have been.
      dtDAL::Map* map = EditorData::GetInstance().getCurrentMap();
      if (map)
      {
         map->RemoveActorFromGroups(proxy.get());
      }
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
      if(mPropertyWindow != NULL)
      {
         EditorActions::GetInstance().mActionWindowsPropertyEditor->setChecked(mPropertyWindow->isVisible());
      }
      if(mActorTab != NULL)
      {
         EditorActions::GetInstance().mActionWindowsActorSearch->setChecked(mActorTab->isVisible());
      }
      if(mResourceBrowser != NULL)
      {
         EditorActions::GetInstance().mActionWindowsResourceBrowser->setChecked(mResourceBrowser->isVisible());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::connectSlots()
   {
      EditorActions& editorActions = EditorActions::GetInstance();

      connect(editorActions.mActionWindowsPropertyEditor, SIGNAL(triggered()),
         this, SLOT(onPropertyEditorSelection()));
      connect(editorActions.mActionWindowsActorSearch,    SIGNAL(triggered()),
         this, SLOT(onActorSearchSelection()));
      connect(editorActions.mActionWindowsResourceBrowser,SIGNAL(triggered()),
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
      connect(editorActions.mActionWindowsResetWindows, SIGNAL(triggered()),
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
      connect(&EditorEvents::GetInstance(), SIGNAL(showStatusBarMessage(const QString, int)),
         this, SLOT(showStatusBarMessage(const QString, int)));

      connect(&editorActions, SIGNAL(ExternalToolsModified(const QList<QAction*>&)),
         this, SLOT(RebuildToolsMenu(const QList<QAction*>&)));
   }

   ///////////////////////////////////////////////////////////////////////////////
   PropertyEditor* MainWindow::GetPropertyEditor()
   {
      return mPropertyWindow;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string& MainWindow::GetSTAGEPath()
   {
      return mSTAGEFullPath;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::findAndLoadPreferences()
   {
      EditorSettings settings;

      // Load settings from last session.  Reasonable defaults are specified,
      // if the settings are not present.

      // Main window settings...
      settings.beginGroup(EditorSettings::MAINWIN_GROUP);
      resize(settings.value(EditorSettings::MAINWIN_SIZE, QSize(800, 600)).toSize());
      move(settings.value(EditorSettings::MAINWIN_POSITION, QPoint(100, 100)).toPoint());

      // When restoring the window state, first see if the key exists.
      if (settings.contains(EditorSettings::MAINWIN_DOCK_STATE))
      {
         QByteArray state = settings.value(EditorSettings::MAINWIN_DOCK_STATE).toByteArray();
         restoreState(state,EditorSettings::MAINWIN_DOCK_STATE_ID);
      }

      // When restoring the window state, first see if the key exists.
      if (settings.contains(EditorSettings::MAINWIN_GEOMETRY))
      {
         QByteArray state = settings.value(EditorSettings::MAINWIN_GEOMETRY).toByteArray();
         restoreGeometry(state);
      }
      settings.endGroup();

      // restore splitter positions
      settings.beginReadArray(EditorSettings::SPLITTER_GROUP);
      for (int s = 0; s < mSplitters.size(); ++s)
      {
         settings.setArrayIndex(s);
         mSplitters.at(s)->restoreState(settings.value(EditorSettings::SPLITTER_SIZE).toByteArray());
      }
      settings.endArray();

      // Now check for the general preferences...
      settings.beginGroup(EditorSettings::PREFERENCES_GROUP);
      if (settings.contains(EditorSettings::LOAD_RECENT_PROJECTS))
      {
         bool loadProjs = settings.value(EditorSettings::LOAD_RECENT_PROJECTS).toBool();
         EditorData::GetInstance().setLoadLastProject(loadProjs);

         if (!FindRecentProjects().empty())
         {
            // no point in trying to find any maps if no previous context were found
            FindRecentMaps();
         }

         if (loadProjs)
         {
            if (settings.contains(EditorSettings::LOAD_RECENT_MAPS))
            {
               bool loadMaps = settings.value(EditorSettings::LOAD_RECENT_MAPS).toBool();
               EditorData::GetInstance().setLoadLastMap(loadMaps);
            }
         }
         else
         {
            EditorData::GetInstance().setLoadLastMap(false);
         }
      }
      else
      {
         EditorData::GetInstance().setLoadLastProject(true);
         EditorData::GetInstance().setLoadLastMap(true);
      }

      if (settings.contains(EditorSettings::RIGID_CAMERA))
      {
         bool rigidCamera = settings.value(EditorSettings::RIGID_CAMERA).toBool();
         EditorData::GetInstance().setRigidCamera(rigidCamera);
         mPerspView->onEditorPreferencesChanged();
      }

      if (settings.contains(EditorSettings::ACTOR_CREATION_OFFSET))
      {
         bool success;
         float actorCreationOffset = (float) settings.value(EditorSettings::ACTOR_CREATION_OFFSET).toDouble(&success);
         if (success)
         {
            EditorData::GetInstance().SetActorCreationOffset(actorCreationOffset);
         }
      }

      if (settings.contains(EditorSettings::SAVE_MILLISECONDS))
      {
         int ms = settings.value(EditorSettings::SAVE_MILLISECONDS).toInt();
         EditorActions::GetInstance().mSaveMilliSeconds = ms;
         EditorActions::GetInstance().getTimer()->setInterval(ms);
      }

      if (settings.contains(EditorSettings::SELECTION_COLOR))
      {
         QColor color = qvariant_cast<QColor>(settings.value(EditorSettings::SELECTION_COLOR));
         EditorData::GetInstance().setSelectionColor(color);
      }
      settings.endGroup();

      //external tools
      const int numExtTools = settings.beginReadArray(EditorSettings::EXTERNAL_TOOLS);
      QList<ExternalTool*>& extTools = EditorActions::GetInstance().GetExternalTools();
      for (int toolIdx = 0; toolIdx < numExtTools; ++toolIdx)
      {
         settings.setArrayIndex(toolIdx);
         ExternalTool* tool = extTools.at(toolIdx);
         tool->SetTitle(settings.value(EditorSettings::EXTERNAL_TOOL_TITLE).toString());
         tool->SetCmd(settings.value(EditorSettings::EXTERNAL_TOOL_COMMAND).toString());
         tool->SetArgs(settings.value(EditorSettings::EXTERNAL_TOOL_ARGS).toString());
         tool->SetWorkingDir(settings.value(EditorSettings::EXTERNAL_TOOL_WORKING_DIR).toString());
         tool->SetIcon(settings.value(EditorSettings::EXTERNAL_TOOL_ICON).toString());
         tool->GetAction()->setVisible(true);
      }
      settings.endArray();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::loadLibraryPaths()
   {
      EditorSettings settings;

      settings.beginGroup(EditorSettings::LIBRARY_PATHS);

      if (settings.contains(EditorSettings::LIBRARY_PATH_0))
      {
         int pos = 0;

         std::string path;

         do
         {
            QString item = EditorSettings::LIBRARY_PATH_N;

            item += QString::number(pos++);

            path = settings.value(item).toString().toStdString();

            if (!path.empty())
            {
               dtUtil::LibrarySharingManager::GetInstance().AddToSearchPath(path);
            }
         }
         while (!path.empty());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> MainWindow::FindRecentProjects()
   {
      EditorSettings settings;
      std::vector<std::string> projects;
      bool failedToLoadContext = false;

      settings.beginGroup(EditorSettings::RECENT_PROJECTS);
      if (settings.contains(EditorSettings::RECENT_PROJECT0))
      {
         std::string project = settings.value(EditorSettings::RECENT_PROJECT0).toString().toStdString();

         if (dtUtil::FileUtils::GetInstance().DirExists(project))
         {
            EditorData::GetInstance().addRecentProject(project);
            if (EditorData::GetInstance().getLoadLastProject())
            {
               EditorData::GetInstance().setCurrentProjectContext(project);
               try 
               {
                  dtDAL::Project::GetInstance().SetContext(project);
               }               
               catch (dtUtil::Exception&)
               {
                   failedToLoadContext = true;                   
               }               
            }
            projects.push_back(project);
         }         
      }

      if(failedToLoadContext)
      {
         QMessageBox::critical(this, tr("Failed to load previous context"),
            tr("Failed to load the previous project context.\n") +
            tr("This can happen if the last project context\n has been moved, renamed, or deleted.\n") +
            tr("You may need to restart STAGE."),
            tr("OK"));

         // Remove the recent projects entry from the settings object since it
         // has become somehow corrupted.
         settings.remove(EditorSettings::RECENT_PROJECT0);
      }

      settings.endGroup();
      return projects;
   }

   ///////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> MainWindow::FindRecentMaps()
   {
      std::vector<std::string> maps;
      EditorSettings settings;

      settings.beginGroup(EditorSettings::RECENT_MAPS);
      if (settings.contains(EditorSettings::RECENT_MAP0))
      {
         const std::string& mapName = settings.value(EditorSettings::RECENT_MAP0).toString().toStdString();
         if (!mapName.empty())
         {
            if (dtDAL::Project::GetInstance().IsContextValid())
            {
               std::set<std::string>::const_iterator itor = dtDAL::Project::GetInstance().GetMapNames().find(mapName);
               if (itor != dtDAL::Project::GetInstance().GetMapNames().end())
               {
                  maps.push_back(mapName);
                  EditorData::GetInstance().addRecentMap(mapName);
               }
               else
               {
                  settings.setValue(EditorSettings::RECENT_MAP0, QVariant(""));
                  QMessageBox::critical(this, tr("Failed to load previous map"),
                     tr("Failed to load the previous map.\n") +
                     tr("This can happen if the last map has been moved, renamed, \ndeleted, or no longer in the previous project context."),
                     QMessageBox::Ok);
               }
            }
         }
      }
      settings.endGroup();
      return maps;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::checkAndLoadBackup(const std::string& str)
   {
      if (dtDAL::Project::GetInstance().HasBackup(str))
      {
         int result = QMessageBox::information(this, tr("Backup file found"),
            tr("A backup save file has been detected. Would you like to open it?"),
            tr("Yes"), tr("No"), tr("Cancel"));

         if (result == 0)
         {
            startWaitCursor();
            dtDAL::Map& backupMap =
               dtDAL::Project::GetInstance().OpenMapBackup(str);

            EditorActions::GetInstance().changeMaps(
               EditorData::GetInstance().getCurrentMap(), &backupMap);
            EditorData::GetInstance().addRecentMap(backupMap.GetName());
            endWaitCursor();
         }
         else if (result == 1)
         {
            /*
            if (!dtUtil::FileUtils::GetInstance().FileExists(str))
            {
               QMessageBox::critical(this, tr("Error loading map"),
               tr("The map ") + str.c_str() + tr(" does not exist or has become corrupted, backup file still retained."));
               return;
            }
            else
            {
               dtDAL::Project::GetInstance().ClearBackup(str);
            }
            //*/

            startWaitCursor();
            dtDAL::Project::GetInstance().ClearBackup(str);
            EditorActions::GetInstance().changeMaps(EditorData::GetInstance().getCurrentMap(),
               &dtDAL::Project::GetInstance().GetMap(str));
            EditorData::GetInstance().addRecentMap(str);
            endWaitCursor();
         }
         else
         {
            return;
         }
      }
      else
      {
         startWaitCursor();
         dtDAL::Map& m = dtDAL::Project::GetInstance().GetMap(str);
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
      QApplication::setOverrideCursor(Qt::WaitCursor);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::endWaitCursor()
   {
      QApplication::restoreOverrideCursor();
   }


   //////////////////////////////////////////////////////////////////////////
   void MainWindow::RebuildToolsMenu(const QList<QAction*>& actions)
   {
      if(mToolsMenu == NULL)
      {
         return;
      }
      mToolsMenu->clear();
      mExternalToolsToolBar->clear();
      for (int toolIdx = 0; toolIdx < actions.size(); ++toolIdx)
      {
         mToolsMenu->addAction(actions[toolIdx]);
         mExternalToolsToolBar->addAction(actions[toolIdx]);
      }

      mToolsMenu->addSeparator();
      mToolsMenu->addAction(EditorActions::GetInstance().mActionAddTool);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::showStatusBarMessage(const QString message, int timeout)
   {
      statusBar()->showMessage(message, timeout);
   }

   //////////////////////////////////////////////////////////////////////////
   void MainWindow::ResetSplitters()
   {
      // funny little logic to set the splitter's children's sizes so that they
      // are equally distributed across the window
      for (int splitterIdx = 0; splitterIdx < mSplitters.size(); ++splitterIdx)
      {
         QList<int> sizes = mSplitters.at(splitterIdx)->sizes();
         int total = 0;
         for (int i = 0; i < sizes.size(); ++i)
         {
            total += sizes.at(i);
         }

         QList<int> newSizes;
         for (int i = 0; i < sizes.size(); ++i)
         {
            newSizes.push_back(total/sizes.size());
         }

         mSplitters.at(splitterIdx)->setSizes(newSizes);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MainWindow::SetupPlugins()
   {
      std::string pluginPath;
      if (dtCore::IsEnvironment("STAGE_PLUGIN_PATH"))
      {
         pluginPath = dtCore::GetEnvironment("STAGE_PLUGIN_PATH");;
      }

      if (pluginPath.empty())
      {
         pluginPath = QCoreApplication::applicationDirPath().toStdString() + "/stplugins";
      }

      #ifdef DELTA_WIN32
         #ifdef _DEBUG
           pluginPath += "/Debug";
         #else
           pluginPath += "/Release";
         #endif
      #endif

      if (!dtUtil::FileUtils::GetInstance().DirExists(pluginPath))
      {
         //no plugin path found...lets not try to load any plugins
         LOG_INFO("No plugin path was found. No plugins will be loaded.");
         return;
      }     

      LOG_INFO("Trying to load plugins from directory " + pluginPath);

      // instantiate all plugin factories and immediately start system plugins
      mPluginManager->LoadPluginsInDir(pluginPath);

      // start plugins that were set in config file
      mPluginManager->StartPluginsInConfigFile();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MainWindow::SetupViewer(STAGEApplication* viewer)
   {
      //dtQt::OSGGraphicsWindowQt* osgGraphWindow = dynamic_cast<dtQt::OSGGraphicsWindowQt*>(viewer->GetWindow()->GetOsgViewerGraphicsWindow());
      //if (osgGraphWindow == NULL)
      //{
      //   LOG_ERROR("Unable to initialize. The deltawin could not be created with a QGLWidget.");
      //   return;
      //}

      ////stuff the QGLWidget into it's parent widget placeholder and ensure it gets
      ////resized to fit the parent
      //QWidget* widget = osgGraphWindow->GetQGLWidget();
      //if (widget != NULL)
      //{
      //   widget->setGeometry(mMainViewportParent->geometry());
      //   widget->setParent(mMainViewportParent);         
      //}


      Viewport* masterViewport = ViewportManager::GetInstance().GetMasterViewport();
      if (masterViewport)
      {
         viewer->AddView(*masterViewport->GetView());
      }

      viewer->AddView(*mSideView->GetView());
      viewer->AddView(*mTopView->GetView());
      viewer->AddView(*mFrontView->GetView());
      viewer->AddView(*mPerspView->GetView());

      //ViewportManager::GetInstance().getWorldViewCamera()->getDeltaCamera()->SetWindow(viewer->GetWindow());
      
   }
} // namespace dtEditQt
