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
#include <prefix/stageprefix.h>
#include <dtEditQt/mainwindow.h>

#include <dtUtil/warningdisable.h>
DT_DISABLE_WARNING_ALL_START
#include <QtCore/QFile>
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
#include <QtGui/QActionGroup>
#include <QtCore/QTimer>
DT_DISABLE_WARNING_END

#include <dtActors/volumeeditactor.h>
#include <dtCore/deltawin.h>
#include <dtCore/transform.h>
#include <dtCore/project.h>
#include <dtCore/actorfactory.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/map.h>
#include <dtGame/actorcomponent.h>
#include <dtGame/gameactor.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/mswinmacros.h>
#include <dtEditQt/configurationmanager.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/editorsettings.h>
#include <dtEditQt/perspectiveviewport.h>
#include <dtEditQt/orthoviewport.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/viewportcontainer.h>
#include <dtEditQt/editorviewportcontainer.h>
#include <dtEditQt/pluginmanager.h>
#include <dtEditQt/propertyeditor.h>
#include <dtEditQt/actordockwidget.h>
#include <dtEditQt/actorsearchdockwidget.h>
#include <dtEditQt/resourcebrowser.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/projectcontextdialog.h>
#include <dtEditQt/uiresources.h>
#include <dtEditQt/externaltool.h>
#include <dtQt/actortreepanel.h>
#include <dtQt/objecttypelistpanel.h>
#include <dtQt/objecttypeselectdialog.h>

#include <osgDB/FileNameUtils>
   


/////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS
/////////////////////////////////////////////////////////////////////////////
typedef dtQt::ObjectTypeListPanel::ObjectTypeList ObjectTypeList;
typedef std::set<const dtCore::ObjectType*> ObjectTypeSet;

typedef dtCore::ActorPtrVector ActorArray;
typedef std::vector<dtGame::ActorComponent*> ActorCompArray;


namespace dtEditQt
{
   ///////////////////////////////////////////////////////////////////////////////
   MainWindow::MainWindow(const std::string& stageConfigFile)
      : mPluginManager(new PluginManager(this))
      , mSTAGEConfigFullPath(stageConfigFile)
      , mVolEditActorProxy()
      , mFileMenu(NULL)
      , mEditMenu(NULL)
      , mProjectMenu(NULL)
      , mWindowMenu(NULL)
      , mHelpMenu(NULL)
      , mRecentProjs(NULL)
      , mRecentMaps(NULL)
      , mToolsMenu(NULL)
      , mToolModeActionGroup(NULL)
      , mNormalToolMode(NULL)
      , mAddActorComponent(NULL)
      , mRemoveActorComponent(NULL)
      , mChangeActorType(NULL)
      , mPerspView(NULL)
      , mTopView(NULL)
      , mSideView(NULL)
      , mFrontView(NULL)
      , mEditorContainer(NULL)
      , mPropertyWindow(NULL)
      , mActorDockWidg(NULL)
      , mActorSearchDockWidg(NULL)
      , mResourceBrowser(NULL)
      , mActorTreeDock(NULL)
      , mActorTreePanel(NULL)
   {
      //Read STAGE configuration file
      if (stageConfigFile != "")
      {
         // If the file doesn't exist, assume its' a relative path instead.
         if (!dtUtil::FileUtils::GetInstance().FileExists(mSTAGEConfigFullPath))
         {
            mSTAGEConfigFullPath = dtUtil::FileUtils::GetInstance().CurrentDirectory() + "\\" + mSTAGEConfigFullPath;
         }

         ConfigurationManager::GetInstance().ReadXML(mSTAGEConfigFullPath);
      }

      //Setup stylesheet based on what's in the configuration file.
      //Note that stylesheet has to be setup for qApp before ANYTHING in the
      //qApp starts up or the stylesheet does not get applied.
      std::string styleSheetFile =
         ConfigurationManager::GetInstance().GetVariable(ConfigurationManager::GENERAL,
                                                         CONF_MGR_STYLESHEET);
      if (styleSheetFile != "")
      {
         dtUtil::FileInfo fileInfo = dtUtil::FileUtils::GetInstance().GetFileInfo(mSTAGEConfigFullPath);
         std::string styleSheetURL = fileInfo.path + "/" + styleSheetFile;

         //check to see if the stylesheet is "just found" (we were given an absolute path)
         if (!dtUtil::FileUtils::GetInstance().FileExists(styleSheetURL))
         {
            //file not found, assume it's in STAGE's stylesheets directory
            styleSheetURL = dtUtil::GetDeltaRootPath() + "/utilities/STAGE/style/" + styleSheetFile;
         }

         QFile file(styleSheetURL.c_str());
         file.open(QFile::ReadOnly);
         QString ss = QString(file.readAll());

         qApp->setStyleSheet(ss);
      }

      // Ensure that the global singletons are lazily instantiated now
      dtCore::ActorFactory::GetInstance();
      EditorActions::GetInstance();
      EditorEvents::GetInstance();
      EditorData::GetInstance();

      // alert the project instance that we are working within STAGE
      // changed on 7/10/2006 banderegg
      dtCore::Project::GetInstance().SetEditMode(true);

      ViewportManager::GetInstance();

      mAddActorComponent = new QAction("Add Actor Components...", this);
      mRemoveActorComponent = new QAction("Remove Actor Components...", this);
      mChangeActorType = new QAction("Change Actor Type...", this); 

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
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::setupMenus()
   {
      EditorActions& editorActions = EditorActions::GetInstance();

      mRecentProjs = new QMenu(tr("Recent Projects"), this);
      mRecentMaps  = new QMenu(tr("Recent Maps"),     this);

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
      mEditMenu->addAction(mAddActorComponent);
      mEditMenu->addAction(mRemoveActorComponent);
      mEditMenu->addSeparator();
      mEditMenu->addAction(mChangeActorType);
      mEditMenu->addSeparator();
      mEditMenu->addAction(editorActions.mActionLocalSpace);
      mEditMenu->addSeparator();
      mEditMenu->addAction(editorActions.mActionGroupActors);
      mEditMenu->addAction(editorActions.mActionUngroupActors);
      mEditMenu->addSeparator();
      mEditMenu->addAction(editorActions.mActionEditDuplicateActor);
      mEditMenu->addAction(editorActions.mActionEditDeleteActor);
      mEditMenu->addAction(editorActions.mActionEditGroundClampActors);
      mEditMenu->addAction(editorActions.mActionEditGotoActor);
      mEditMenu->addSeparator();
      mEditMenu->addAction(editorActions.mActionEditResetTranslation);
      mEditMenu->addAction(editorActions.mActionEditResetRotation);
      mEditMenu->addAction(editorActions.mActionEditResetScale);
      mEditMenu->addSeparator();
      mEditMenu->addAction(editorActions.mActionGetGotoPosition);
      mEditMenu->addSeparator();
      mEditMenu->addAction(editorActions.mActionEditMapProperties);
      mEditMenu->addAction(editorActions.mActionEditMapLibraries);
      mEditMenu->addAction(editorActions.mActionEditMapEvents);
      mEditMenu->addSeparator();
      mEditMenu->addAction(editorActions.mActionFileEditPreferences);

      mWindowMenu = menuBar()->addMenu(tr("&Window"));

      ConfigurationManager& config = ConfigurationManager::GetInstance();
      ConfigurationManager::SectionType layoutSection = ConfigurationManager::LAYOUT;

      if(config.GetVariable(layoutSection, CONF_MGR_SHOW_PROPERTY_EDITOR) != "false")
      {
         mWindowMenu->addAction(editorActions.mActionWindowsPropertyEditor);
      }

      if(config.GetVariable(layoutSection, CONF_MGR_SHOW_ACTOR_BROWSER) != "false"
         || config.GetVariable(layoutSection, CONF_MGR_SHOW_PREFAB_BROWSER) != "false")
      {
         mWindowMenu->addAction(editorActions.mActionWindowsActor);
      }

      if(config.GetVariable(layoutSection, CONF_MGR_SHOW_ACTOR_SEARCH) != "false")
      {
         mWindowMenu->addAction(editorActions.mActionWindowsActorSearch);
      }

      if(config.GetVariable(layoutSection, CONF_MGR_SHOW_ACTOR_TREE_DOCK) != "false")
      {
         mWindowMenu->addAction(editorActions.mActionWindowsActorTreePanel);
      }

      if(config.GetVariable(layoutSection, CONF_MGR_SHOW_RESOURCE_BROWSER) != "false")
      {
         mWindowMenu->addAction(editorActions.mActionWindowsResourceBrowser);
      }
      mWindowMenu->addSeparator();
      mWindowMenu->addAction(editorActions.mActionWindowsResetWindows);

      mToolsMenu = menuBar()->addMenu(tr("&Tools"));

      mHelpMenu = menuBar()->addMenu(tr("&Help"));
      mHelpMenu->addAction(editorActions.mActionHelpEditorHelp);
      mHelpMenu->addAction(editorActions.mActionHelpAboutEditor);
      mHelpMenu->addAction(editorActions.mActionHelpAboutQT);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::setupToolbar()
   {
      //NOTE: Modify the onResetWindows() method when adding new Toolbars!

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
      mEditToolBar->addSeparator();
      mEditToolBar->addAction(EditorActions::GetInstance().mActionEditDuplicateActor);
      mEditToolBar->addAction(EditorActions::GetInstance().mActionEditDeleteActor);
      mEditToolBar->addAction(EditorActions::GetInstance().mActionEditGotoActor);
      mEditToolBar->addAction(EditorActions::GetInstance().mActionEditGroundClampActors);
      //mEditToolBar->addAction(EditorActions::GetInstance().mActionEditTaskEditor);
      mEditToolBar->addSeparator();
      mEditToolBar->addAction(EditorActions::GetInstance().mActionEditResetTranslation);
      mEditToolBar->addAction(EditorActions::GetInstance().mActionEditResetRotation);
      mEditToolBar->addAction(EditorActions::GetInstance().mActionEditResetScale);
      addToolBar(mEditToolBar);

      mUndoToolBar = new QToolBar(this);
      mUndoToolBar->setObjectName("UndoToolBar");
      mUndoToolBar->setWindowTitle(tr("Undo Toolbar"));
      mUndoToolBar->setMinimumWidth(10);
      mUndoToolBar->addAction(EditorActions::GetInstance().mActionEditUndo);
      mUndoToolBar->addAction(EditorActions::GetInstance().mActionEditRedo);
      addToolBar(mUndoToolBar);

      mBrushToolBar = new QToolBar(this);
      mBrushToolBar->setObjectName("BrushToolBar");
      mBrushToolBar->setWindowTitle(tr("Brush Toolbar"));
      mBrushToolBar->setMinimumWidth(10);
      mBrushToolBar->addAction(EditorActions::GetInstance().mActionBrushShape);
      mBrushToolBar->addAction(EditorActions::GetInstance().mActionBrushReset);
      mBrushToolBar->addAction(EditorActions::GetInstance().mActionHideShowBrush);
      addToolBar(mBrushToolBar);

      mToolsToolBar = new QToolBar(this);
      mToolsToolBar->setObjectName("ToolsToolBar");
      mToolsToolBar->setWindowTitle(tr("Tools Toolbar"));
      mToolsToolBar->setMinimumWidth(10);
      mToolsToolBar->addAction(EditorActions::GetInstance().mActionHideShowTriggers);
      addToolBar(mToolsToolBar);

      mToolModeActionGroup = new QActionGroup(this);
      mToolModeActionGroup->setExclusive(true);

      mNormalToolMode = new QAction(QIcon(UIResources::ICON_TOOLMODE_NORMAL.c_str()), "Normal Tools", this);
      AddExclusiveToolMode(mNormalToolMode);
      mNormalToolMode->setChecked(true);

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

      ConfigurationManager& config = ConfigurationManager::GetInstance();
      ConfigurationManager::SectionType layoutSection = ConfigurationManager::LAYOUT;

      if(config.GetVariable(layoutSection, CONF_MGR_SHOW_PROPERTY_EDITOR) != "false")
      {
         // create the main left dock window
         mPropertyWindow = new PropertyEditor(this);
         mPropertyWindow->setObjectName("PropertyWindow");

         // listen for selection changed event
         connect(&EditorEvents::GetInstance(), SIGNAL(selectedActors(ActorRefPtrVector&)),
                  mPropertyWindow, SLOT(HandleActorsSelected(ActorRefPtrVector&)));

         // listen for property change events and update the tree.  These can be generated
         // by the viewports, or the tree itself.
         connect(&EditorEvents::GetInstance(), SIGNAL(actorPropertyChanged(dtCore::ActorPtr, ActorPropertyRefPtr)),
                  this, SLOT(MarkEdited()));

         // listen for name changes so we can update our group box label or handle undo changes
         connect(&EditorEvents::GetInstance(), SIGNAL(ProxyNameChanged(dtCore::BaseActorObject&, std::string)),
                  mPropertyWindow, SLOT(ProxyNameChanged(dtCore::BaseActorObject&, std::string)));

         mPropertyWindow->setFeatures(QDockWidget::AllDockWidgetFeatures);
         addDockWidget(Qt::LeftDockWidgetArea,  mPropertyWindow);
      }

      if(config.GetVariable(layoutSection, CONF_MGR_SHOW_ACTOR_DOCKWIDGET) != "false")
      {
         mActorDockWidg = new ActorDockWidget(this);
         mActorDockWidg->setObjectName("ActorDockWidget");
         mActorDockWidg->setFeatures(QDockWidget::AllDockWidgetFeatures);
         addDockWidget(Qt::LeftDockWidgetArea, mActorDockWidg);
      }

      if(config.GetVariable(layoutSection, CONF_MGR_SHOW_SEARCH_DOCKWIDGET) != "false")
      {
         mActorSearchDockWidg = new ActorSearchDockWidget(this);
         mActorSearchDockWidg->setObjectName("ActorSearchDockWidget");
         mActorDockWidg->setFeatures(QDockWidget::AllDockWidgetFeatures);
         addDockWidget(Qt::LeftDockWidgetArea, mActorSearchDockWidg);
      }

      if(config.GetVariable(layoutSection, CONF_MGR_SHOW_ACTOR_TREE_DOCK) != "false")
      {
         mActorTreeDock = new QDockWidget(this);
         mActorTreeDock->setObjectName("ActorTreeDock");
         mActorTreeDock->setWindowTitle(tr("Actor Tree"));
         mActorTreeDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);

         mActorTreePanel = new dtQt::ActorTreePanel();
         mActorTreeDock->setWidget(mActorTreePanel);

         EditorActions& editorActions = EditorActions::GetInstance();
         connect(&EditorEvents::GetInstance(), SIGNAL(selectedActors(ActorRefPtrVector&)),
                  mActorTreePanel, SLOT(OnActorsSelected(ActorRefPtrVector&)));
         connect(&mActorTreePanel->GetTreeWidget(), SIGNAL(SignalActorAttach(dtCore::ActorPtr,dtCore::ActorPtr,dtCore::ActorPtr)),
                  &editorActions, SLOT(slotChangeActorParent(dtCore::ActorPtr,dtCore::ActorPtr,dtCore::ActorPtr)));
         connect(&mActorTreePanel->GetTreeWidget(), SIGNAL(SignalActorDetach(dtCore::ActorPtr,dtCore::ActorPtr)),
                  &editorActions, SLOT(slotDetachActorParent(dtCore::ActorPtr,dtCore::ActorPtr)));
         
         connect(editorActions.mActionWindowsActorTreePanel, SIGNAL(triggered(bool)),
            mActorTreeDock, SLOT(setVisible(bool)));
         connect(mActorTreeDock, SIGNAL(visibilityChanged(bool)),
            editorActions.mActionWindowsActorTreePanel, SLOT(setChecked(bool)));
      }

      // Create the viewports, assign them to splitters, and embed the splitters
      // in the central widget of the main window.
      mMainViewportParent = setupViewports();
      setCentralWidget(mMainViewportParent);

      if(config.GetVariable(layoutSection, CONF_MGR_SHOW_RESOURCE_BROWSER) != "false")
      {
         mResourceBrowser = new ResourceBrowser(this);
         mResourceBrowser->setObjectName("ResourceBrowser");
         mResourceBrowser->setFeatures(QDockWidget::AllDockWidgetFeatures);
         addDockWidget(Qt::RightDockWidgetArea, mResourceBrowser);
      }
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
      mPerspView->getCamera()->getDeltaCamera()->GetOSGCamera()->setCullMask(0x0F000001);  //magic number, based on ObjectMotionModel's mask
      mPerspView->GetRootNode()->setNodeMask(0x00000001);
      mPerspView->ShowCompass();

      mTopView = (OrthoViewport*)vpMgr.createViewport("Top View (XY)",
         ViewportManager::ViewportType::ORTHOGRAPHIC);
      mTopView->setViewType(OrthoViewport::OrthoViewType::TOP,false);
      mTopView->setAutoInteractionMode(true);
      mTopView->getCamera()->getDeltaCamera()->GetOSGCamera()->setCullMask(0x0F000002);
      mTopView->GetRootNode()->setNodeMask(0x00000002);

      mSideView = (OrthoViewport*)vpMgr.createViewport("Side View (YZ)",
         ViewportManager::ViewportType::ORTHOGRAPHIC);
      mSideView->setViewType(OrthoViewport::OrthoViewType::SIDE,false);
      mSideView->setAutoInteractionMode(true);
      mSideView->getCamera()->getDeltaCamera()->GetOSGCamera()->setCullMask(0x0F000004);
      mSideView->GetRootNode()->setNodeMask(0x00000004);

      mFrontView = (OrthoViewport*)vpMgr.createViewport("Front View (XZ)",
         ViewportManager::ViewportType::ORTHOGRAPHIC);
      mFrontView->setViewType(OrthoViewport::OrthoViewType::FRONT,false);
      mFrontView->setAutoInteractionMode(true);
      mFrontView->getCamera()->getDeltaCamera()->GetOSGCamera()->setCullMask(0x0F000008);
      mFrontView->GetRootNode()->setNodeMask(0x00000008);


      // We now wrap each viewport in a viewport container to provide the
      // toolbar and right click menu add-ons which are needed by the editor
      // for each viewport.
      ViewportContainer* topContainer = new ViewportContainer(mTopView, vSplit1);
      ViewportContainer* frontContainer = new ViewportContainer(mFrontView, vSplit1);
      ViewportContainer* sideContainer = new ViewportContainer(mSideView, vSplit2);
      ViewportContainer* perspContainer = new ViewportContainer(mPerspView, vSplit2);

      mViewportContainers[QString(mTopView->getName().c_str())] = topContainer;
      mViewportContainers[QString(mFrontView->getName().c_str())] = frontContainer;
      mViewportContainers[QString(mSideView->getName().c_str())] = sideContainer;
      mViewportContainers[QString(mPerspView->getName().c_str())] = perspContainer;

      bool bTopHidden = false;
      if (ConfigurationManager::GetInstance().GetVariable(ConfigurationManager::LAYOUT, CONF_MGR_SHOW_TOP_VIEW) == "false")
      {
         mTopView->GetQGLWidget()->hide();
         mTopView->RemoveView();
         topContainer->hide();
         bTopHidden = true;
      }

      if (ConfigurationManager::GetInstance().GetVariable(ConfigurationManager::LAYOUT, CONF_MGR_SHOW_FRONT_VIEW) == "false")
      {
         mFrontView->GetQGLWidget()->hide();
         mFrontView->RemoveView();
         frontContainer->hide();
         if (bTopHidden)
         {
            vSplit1->hide();
         }
      }

      bool bBottomHidden = false;
      if (ConfigurationManager::GetInstance().GetVariable(ConfigurationManager::LAYOUT, CONF_MGR_SHOW_SIDE_VIEW) == "false")
      {
         mSideView->GetQGLWidget()->hide();
         mSideView->RemoveView();
         sideContainer->hide();
         bBottomHidden = true;
      }

      if (ConfigurationManager::GetInstance().GetVariable(ConfigurationManager::LAYOUT, CONF_MGR_SHOW_PERSP_VIEW) == "false")
      {
         mPerspView->GetQGLWidget()->hide();
         mPerspView->RemoveView();
         perspContainer->hide();
         if (bBottomHidden)
         {
            vSplit2->hide();
         }
      }

      // Create our editor container for all of our views.
      mEditorContainer = new EditorViewportContainer(hSplit);
      mEditorContainer->updateSnaps();

      // Returns the root of the viewport widget hierarchy.
      return mEditorContainer;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::setupVolumeEditActor()
   {
      //The persistent pseudo-actor that is used for special-purpose editing
      mVolEditActorProxy =
         dynamic_cast<dtActors::VolumeEditActorProxy*>(dtCore::ActorFactory::GetInstance().CreateActor("dtutil", "Volume Edit").get());
      ViewportManager::GetInstance().getMasterScene()->AddChild(mVolEditActorProxy->GetDrawable());

      //move the VolumeEditActor away from the Perspective camera so we can see it.
      dtActors::VolumeEditActor* volEditAct =
            dynamic_cast<dtActors::VolumeEditActor*>(mVolEditActorProxy->GetDrawable());
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
      const bool hasProject    = dtCore::Project::GetInstance().IsContextValid();
      const bool hasCurrentMap = (EditorData::GetInstance().getCurrentMap() != NULL);
      const bool hasBoth       = hasProject && hasCurrentMap;

      mAddActorComponent->setEnabled(hasBoth);
      mRemoveActorComponent->setEnabled(hasBoth);
      mChangeActorType->setEnabled(hasBoth);

      EditorActions& ea = EditorActions::GetInstance();
      ea.mActionFileNewMap->setEnabled(hasProject);
      ea.mActionFileOpenMap->setEnabled(hasProject);
      ea.mActionFileCloseMap->setEnabled(hasBoth);
      ea.mActionFileSaveMap->setEnabled(hasBoth);
      ea.mActionFileSaveMapAs->setEnabled(hasBoth);
      ea.mActionFileExportPrefab->setEnabled(false);
      ea.mActionFileExit->setEnabled(true);

      ea.mActionGroupActors->setEnabled(false);
      ea.mActionUngroupActors->setEnabled(false);
      ea.mActionEditDuplicateActor->setEnabled(false);
      ea.mActionEditDeleteActor->setEnabled(false);
      ea.mActionEditGotoActor->setEnabled(false);
      ea.mActionEditGroundClampActors->setEnabled(false);
      ea.mActionEditMapProperties->setEnabled(hasBoth);
      ea.mActionEditMapLibraries->setEnabled(hasBoth);
      ea.mActionEditTaskEditor->setEnabled(hasBoth);
      ea.mActionEditMapEvents->setEnabled(hasBoth);
      ea.mActionEditResetTranslation->setEnabled(false);
      ea.mActionEditResetRotation->setEnabled(false);
      ea.mActionEditResetScale->setEnabled(false);

      ea.mActionWindowsActor->setEnabled(hasBoth);
      ea.mActionWindowsActorSearch->setEnabled(hasBoth);
      ea.mActionWindowsActorTreePanel->setEnabled(hasBoth);
      ea.mActionWindowsPropertyEditor->setEnabled(hasBoth);
      ea.mActionWindowsResourceBrowser->setEnabled(hasBoth);
      ea.mActionWindowsResetWindows->setEnabled(true);

      ea.mActionHelpEditorHelp->setEnabled(true);
      ea.mActionHelpAboutEditor->setEnabled(true);
      ea.mActionHelpAboutQT->setEnabled(true);

      // enable main window areas
      if(mPropertyWindow != NULL)
      {
         mPropertyWindow->setEnabled(hasBoth);
      }

      if(mActorDockWidg != NULL)
      {
         mActorDockWidg->setEnabled(hasBoth);
      }

      if (mActorSearchDockWidg != NULL)
      {
         mActorSearchDockWidg->setEnabled(hasBoth);
      }

      if (mActorTreeDock != NULL)
      {
         mActorTreeDock->setEnabled(hasBoth);
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

      if (mActorDockWidg != NULL)
      {
         mActorDockWidg->setFloating(false);
         mActorDockWidg->setVisible(true);
         EditorActions::GetInstance().mActionWindowsActor->setChecked(true);
         addDockWidget(Qt::LeftDockWidgetArea,  mActorDockWidg);
      }

      if (mActorSearchDockWidg != NULL)
      {
         mActorSearchDockWidg->setFloating(false);
         mActorSearchDockWidg->setVisible(true);
         EditorActions::GetInstance().mActionWindowsActorSearch->setChecked(true);
         addDockWidget(Qt::LeftDockWidgetArea,  mActorSearchDockWidg);
      }

      if (mActorTreeDock != NULL)
      {
         mActorTreeDock->setFloating(false);
         mActorTreeDock->setVisible(true);
         addDockWidget(Qt::LeftDockWidgetArea,  mActorTreeDock);
      }

      if (mResourceBrowser != NULL)
      {
         mResourceBrowser->setFloating(false);
         mResourceBrowser->setVisible(true);
         EditorActions::GetInstance().mActionWindowsResourceBrowser->setChecked(true);
         addDockWidget(Qt::RightDockWidgetArea, mResourceBrowser);
      }

      ResetSplitters();

      //delete and rebuild our toolbars
      delete mFileToolBar;
      delete mEditToolBar;
      delete mUndoToolBar;
      delete mBrushToolBar;
      delete mToolsToolBar;
      delete mExternalToolsToolBar;

      setupToolbar();

      if (mPerspView && mPerspView->getCamera())
         mPerspView->getCamera()->resetRotation();
      //ViewportManager::GetInstance().LoadPresetCamera(1);

      EditorEvents::GetInstance().emitResetWindows();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::onEditorInitiated()
   {
      enableActions();

      // Load the custom library paths if they exist
      loadLibraryPaths();
      findAndLoadPreferences();
      mPerspView->onEditorPreferencesChanged();

      if (EditorData::GetInstance().getLoadLastProject() == false)
      {
         //Display the Project Change dialog to prompt the user
         EditorActions::GetInstance().slotProjectChangeContext();
      }
      else
      {
         std::list<std::string>& projects = EditorData::GetInstance().getRecentProjects();
         if (!projects.empty())
         {
            const std::string path = projects.front();
            EditorActions::GetInstance().SlotChangeProjectContext(path);
         }
      }

      //startWaitCursor();
      //EditorActions::GetInstance().refreshRecentProjects();
      //endWaitCursor();

      if (dtCore::Project::GetInstance().IsContextValid() && EditorData::GetInstance().getLoadLastMap())
      {
         QTimer::singleShot(1000, this, SLOT(onAutoLoadMap()));
      }

      EditorActions::GetInstance().getTimer()->start();

      updateWindowTitle();
      //findAndLoadPreferences();

      update();
      repaint();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool MainWindow::MapDoesNotExist( const std::string& mapToLoad )
   {
      const std::set<std::string>& mapNames = dtCore::Project::GetInstance().GetMapNames();
      return mapNames.find(mapToLoad) != mapNames.end();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MainWindow::onAutoLoadMap()
   {
      // Turn off map reload here so that if it crashes in a user's actor library, next time you load stage, it won't try again.
      bool oldReloadMaps = EditorData::GetInstance().getLoadLastMap();
      EditorData::GetInstance().setLoadLastMap(false);
      WritePreferences();

      std::string mapToLoad = "";
      std::list<std::string>& maps = EditorData::GetInstance().getRecentMaps();
      if (!maps.empty())
      {
         mapToLoad = maps.front();
      }

      // this allows to start STAGE in batch mode with a new empty map ready to be
      // edited. The new empty map must be pushed as "recent" map.
      if (MapDoesNotExist(mapToLoad))
      {
         EditorActions::GetInstance().createNewEmptyMap(mapToLoad);
      }

      checkAndLoadBackup(mapToLoad);
      EditorData::GetInstance().setLoadLastMap(oldReloadMaps);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::onEditorShutDown()
   {
      WritePreferences();

      if(mPropertyWindow != NULL)
      {
         // listen for selection changed event
         disconnect(&EditorEvents::GetInstance(), SIGNAL(selectedActors(ActorRefPtrVector&)),
            mPropertyWindow, SLOT(HandleActorsSelected(ActorRefPtrVector&)));

         // listen for property change events and update the tree.  These can be generated
         // by the viewports, or the tree itself.
         disconnect(&EditorEvents::GetInstance(), SIGNAL(actorPropertyChanged(dtCore::ActorPtr,
            ActorPropertyRefPtr)),
            mPropertyWindow, SLOT(ActorPropertyChanged(dtCore::ActorPtr,
            ActorPropertyRefPtr)));

         // listen for name changes so we can update our group box label or handle undo changes
         disconnect(&EditorEvents::GetInstance(), SIGNAL(ProxyNameChanged(dtCore::BaseActorObject&, std::string)),
                  mPropertyWindow, SLOT(ProxyNameChanged(dtCore::BaseActorObject&, std::string)));

         mPropertyWindow->hide();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::MarkEdited()
   {
      if (!dtCore::Project::GetInstance().IsContextValid())
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

      dtCore::Map* curMap = dtEditQt::EditorData::GetInstance().getCurrentMap();
      if (curMap == NULL)
      {
         EditorEvents::GetInstance().emitEditorCloseEvent();
         e->accept();
         return;
      }

      dtEditQt::EditorActions::GetInstance().slotFileExit();
      if (EditorActions::GetInstance().mWasCancelled)
      {
         e->ignore();
      }
      else
      {
         e->accept();
      }
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
   bool MainWindow::IsActorComponentType(const dtCore::ObjectType& objType) const
   {
      return objType.InstanceOf(*dtGame::ActorComponent::BaseActorComponentType);
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool MainWindow::IsActorType(const dtCore::ObjectType& objType) const
   {
      // Actor types should not be actor component types.
      return ! objType.InstanceOf(*dtGame::ActorComponent::BaseActorComponentType);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::onAddActorComponents()
   {
      dtQt::ObjectTypeSelectDialog dialog(this);
      dialog.setWindowTitle("Add Actor Component");

      dtQt::ObjectTypeListPanel& objTypePanel = dialog.GetObjectTypeListPanel();
      objTypePanel.SetFilterFunc(dtUtil::MakeFunctor(&MainWindow::IsActorComponentType, this));

      // Ensure the list updates using the filter function to only show
      // actor component types.
      dialog.UpdateUI();
      
      if (dialog.GetItemCount() > 0 && dialog.exec() == QDialog::Accepted)
      {
         int results = 0;

         typedef dtQt::ObjectTypeListPanel::ObjectTypeList ObjectTypeList;

         ObjectTypeList compTypes;
         if (0 < objTypePanel.GetSelection(compTypes))
         {
            ActorArray actors;
            dtEditQt::EditorData::GetInstance().GetSelectedActors(actors);

            const dtCore::ActorType* actType = NULL;
            ObjectTypeList::const_iterator curIter = compTypes.begin();
            ObjectTypeList::const_iterator endIter = compTypes.end();
            for (; curIter != endIter; ++curIter)
            {
               actType = dynamic_cast<const dtCore::ActorType*>(*curIter);

               dtGame::ActorComponentBase* curActor = NULL;
               ActorArray::iterator curActorIter = actors.begin();
               ActorArray::iterator endActorIter = actors.end();
               for (; curActorIter != endActorIter; ++curActorIter)
               {
                  curActor = dynamic_cast<dtGame::ActorComponentBase*>(*curActorIter);

                  if (curActor == NULL)
                  {
                     LOG_ERROR("Could not convert \"" + (*curActorIter)->GetName()
                        + "\" to a GameActor to add an ActorComponent of type \""
                        + actType->GetName() + "\".");
                  }
                  // Actor is valid but is the type an actor component.
                  else if (actType->InstanceOf(*dtGame::ActorComponent::BaseActorComponentType))
                  {
                     dtCore::RefPtr<dtCore::BaseActorObject> newComp
                        = dtCore::ActorFactory::GetInstance().CreateActor(*actType);

                     dtGame::ActorComponent* actComp = dynamic_cast<dtGame::ActorComponent*>(newComp.get());

                     if (actComp == NULL)
                     {
                        std::string name;
                        dtGame::GameActorProxy* gameActor = dynamic_cast<dtGame::GameActorProxy*>(curActor);
                        if (gameActor != NULL)
                        {
                           name = gameActor->GetName();
                        }

                        LOG_ERROR("Could not add actor type \"" + actType->GetName()
                           + "\" to actor \"" + name + "\" since the type is not an actor component derived type.");
                     }
                     else
                     {
                        curActor->AddComponent(*actComp);

                        ++results;
                     }
                  }
               }
            }
         }

         if (results > 0)
         {
            mPropertyWindow->UpdateUI();
            MarkEdited();
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::onRemoveActorComponents()
   {
      int results = 0;

      ObjectTypeSet compTypesFound;
      ObjectTypeList compTypes;

      ActorArray actors;
      dtEditQt::EditorData::GetInstance().GetSelectedActors(actors);

      // Get all unique actor component types from the selected actors.
      dtGame::ActorComponentBase* curActor = NULL;
      ActorArray::iterator curActorIter = actors.begin();
      ActorArray::iterator endActorIter = actors.end();
      for (; curActorIter != endActorIter; ++curActorIter)
      {
         curActor = dynamic_cast<dtGame::ActorComponentBase*>(*curActorIter);

         if (curActor != NULL)
         {
            ActorCompArray actComps;
            curActor->GetAllComponents(actComps);

            if ( ! actComps.empty())
            {
               // Capture all the actor component types.
               ActorCompArray::iterator curCompIter = actComps.begin();
               ActorCompArray::iterator endCompIter = actComps.end();
               for (; curCompIter != endCompIter; ++curCompIter)
               {
                  compTypesFound.insert(&(*curCompIter)->GetActorType());
               }
            }
         }
      }

      if ( ! compTypesFound.empty())
      {
         compTypes.insert(compTypes.end(), compTypesFound.begin(), compTypesFound.end());
      }

      // Avoid further processing if there is nothing to work with.
      if (compTypes.empty())
      {
         // TODO: Show message
         return;
      }

      dtQt::ObjectTypeSelectDialog dialog(this);
      dialog.setWindowTitle("Remove Actor Components");

      dtQt::ObjectTypeListPanel& objTypePanel = dialog.GetObjectTypeListPanel();
      objTypePanel.SetFilterFunc(dtUtil::MakeFunctor(&MainWindow::IsActorComponentType, this));
      objTypePanel.SetList(compTypes);
      
      if (dialog.GetItemCount() > 0 && dialog.exec() == QDialog::Accepted)
      {
         compTypes.clear();
         if (0 < objTypePanel.GetSelection(compTypes))
         {
            mPropertyWindow->ClearUI();

            // For each actor...
            curActorIter = actors.begin();
            endActorIter = actors.end();
            for (; curActorIter != endActorIter; ++curActorIter)
            {
               curActor = dynamic_cast<dtGame::ActorComponentBase*>(*curActorIter);

               if (curActor != NULL)
               {
                  // For each actor component type...
                  const dtCore::ActorType* actType = NULL;
                  ObjectTypeList::const_iterator curIter = compTypes.begin();
                  ObjectTypeList::const_iterator endIter = compTypes.end();
                  for (; curIter != endIter; ++curIter)
                  {
                     actType = dynamic_cast<const dtCore::ActorType*>(*curIter);

                     if (actType != NULL)
                     {
                        ActorCompArray comps = curActor->GetComponents(actType);

                        // Remove all actor components of the specified type.
                        ActorCompArray::iterator curCompIter = comps.begin();
                        ActorCompArray::iterator endCompIter = comps.end();
                        for (; curCompIter != endCompIter; ++curCompIter)
                        {
                           curActor->RemoveComponent(*(*curCompIter));

                           ++results;
                        }
                     }
                  }

               }
            }
         }
      }

      if (results > 0)
      {
         mPropertyWindow->UpdateUI();
         MarkEdited();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::onChangeActorType()
   {
      int results = 0;

      dtQt::ObjectTypeSelectDialog dialog(this);
      dialog.setWindowTitle("Change Actor Type");
      dialog.SetSingleSelectMode(true);

      dtQt::ObjectTypeListPanel& objTypePanel = dialog.GetObjectTypeListPanel();
      objTypePanel.SetFilterFunc(dtUtil::MakeFunctor(&MainWindow::IsActorType, this));

      // Ensure the list updates using the filter function to only show
      // actor component types.
      dialog.UpdateUI();
      
      if (dialog.GetItemCount() > 0 && dialog.exec() == QDialog::Accepted)
      {
         const dtCore::ObjectType* targetType = dialog.GetSelectedType();
         if (targetType == NULL)
         {
            LOG_WARNING("No actor type was selected to which to convert the selected actors.");
            return;
         }

         ActorArray actors;
         dtEditQt::EditorData::GetInstance().GetSelectedActors(actors);

         // Convert all the selected actors to the target type.
         dtCore::BaseActorObject* curActor = NULL;
         ActorArray::iterator curActorIter = actors.begin();
         ActorArray::iterator endActorIter = actors.end();
         for (; curActorIter != endActorIter; ++curActorIter)
         {
            curActor = dynamic_cast<dtCore::BaseActorObject*>(*curActorIter);

            if (curActor != NULL)
            {
               // Determine if the actor's type is the same as the target type.
               if (targetType == &curActor->GetActorType())
               {
                  LOG_INFO("Not converting actor \"" + curActor->GetName() + "\" to type \""
                     + targetType->GetName() + "\" since its current type matches.");
               }
               else // The type is different...
               {
                  // TODO: Convert selected actors to the target type.
               }
            }
         }
      }

      if (results > 0)
      {
         mPropertyWindow->UpdateUI();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::onActorSelection()
   {
      if(mActorDockWidg != NULL)
      {
         mActorDockWidg->setVisible(EditorActions::GetInstance().mActionWindowsActor->isChecked());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::onActorSearchSelection()
   {
      if (mActorSearchDockWidg != NULL)
      {
         mActorSearchDockWidg->setVisible(EditorActions::GetInstance().mActionWindowsActorSearch->isChecked());
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
   void MainWindow::onActorProxyNameChanged(dtCore::BaseActorObject& proxy, std::string oldName)
   {
      EditorData::GetInstance().getCurrentMap()->OnProxyRenamed(proxy);
      MarkEdited();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::setWindowMenuTabsChecked()
   {
      if(mPropertyWindow != NULL && EditorActions::GetInstance().mActionWindowsPropertyEditor != NULL)
      {
         EditorActions::GetInstance().mActionWindowsPropertyEditor->setChecked(mPropertyWindow->isVisible());
      }
      if(mActorDockWidg != NULL && EditorActions::GetInstance().mActionWindowsActor != NULL)
      {
         EditorActions::GetInstance().mActionWindowsActor->setChecked(mActorDockWidg->isVisible());
      }
      if(mActorSearchDockWidg != NULL && EditorActions::GetInstance().mActionWindowsActorSearch != NULL)
      {
         EditorActions::GetInstance().mActionWindowsActorSearch->setChecked(mActorSearchDockWidg->isVisible());
      }
      if(mResourceBrowser != NULL && EditorActions::GetInstance().mActionWindowsResourceBrowser != NULL)
      {
         EditorActions::GetInstance().mActionWindowsResourceBrowser->setChecked(mResourceBrowser->isVisible());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::connectSlots()
   {
      EditorActions& editorActions = EditorActions::GetInstance();

      connect(mAddActorComponent, SIGNAL(triggered()),
         this, SLOT(onAddActorComponents()));
      connect(mRemoveActorComponent, SIGNAL(triggered()),
         this, SLOT(onRemoveActorComponents()));
      connect(mChangeActorType, SIGNAL(triggered()),
         this, SLOT(onChangeActorType()));

      connect(editorActions.mActionWindowsPropertyEditor, SIGNAL(triggered()),
         this, SLOT(onPropertyEditorSelection()));
      connect(editorActions.mActionWindowsActor,          SIGNAL(triggered()),
         this, SLOT(onActorSelection()));
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
      connect(&EditorEvents::GetInstance(), SIGNAL(projectChanged()),
         this, SLOT(RefreshRecentProjectsMenu()));
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
         this, SLOT(MarkEdited()));
      connect(editorActions.mActionWindowsResetWindows, SIGNAL(triggered()),
         this, SLOT(onResetWindows()));
      connect(&EditorEvents::GetInstance(), SIGNAL(actorProxyDestroyed(dtCore::ActorPtr)),
         this, SLOT(MarkEdited()));
      connect(&EditorEvents::GetInstance(), SIGNAL(actorProxyCreated(dtCore::ActorPtr, bool)),
         this, SLOT(MarkEdited()));
      connect(&EditorEvents::GetInstance(),
         SIGNAL(actorPropertyChanged(dtCore::ActorPtr, ActorPropertyRefPtr)),
         this, SLOT(MarkEdited()));
      connect(&EditorEvents::GetInstance(),
         SIGNAL(actorHierarchyUpdated(dtCore::ActorPtr, dtCore::ActorPtr)),
         this, SLOT(MarkEdited()));
      connect(&EditorEvents::GetInstance(), SIGNAL(ProxyNameChanged(dtCore::BaseActorObject&, std::string)),
         this, SLOT(onActorProxyNameChanged(dtCore::BaseActorObject&, std::string)));
      connect(&EditorEvents::GetInstance(), SIGNAL(showStatusBarMessage(const QString, int)),
         this, SLOT(showStatusBarMessage(const QString, int)));

      connect(&editorActions, SIGNAL(ExternalToolsModified(const QList<QAction*>&)),
         this, SLOT(RebuildToolsMenu(const QList<QAction*>&)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MainWindow::AddExclusiveToolMode(QAction* action)
   {
      if (!action)
      {
         return;
      }

      mToolsToolBar->addAction(action);
      action->setActionGroup(mToolModeActionGroup);
      action->setCheckable(true);
   }

   ////////////////////////////////////////////////////////////////////////////////
   QAction* MainWindow::FindExclusiveToolMode(std::string name)
   {
      QList<QAction*> actions = mToolsToolBar->actions();
      for (int actionIndex = 0; actionIndex < (int)actions.size(); actionIndex++)
      {
         if (name == actions[actionIndex]->text().toStdString())
         {
            return actions[actionIndex];
         }
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MainWindow::RemoveExclusiveToolMode(QAction* action)
   {
      if (!action)
      {
         return;
      }

      action->setActionGroup(NULL);
      mToolsToolBar->removeAction(action);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MainWindow::SetNormalToolMode()
   {
      //Only go to normal mode if something else isn't "checked"
      //This is because we may have switched between "Linked Tool" mode
      //to "Backdrop" mode and don't want to change to Normal mode.
      //
      //However, if all we have done is turn off one of those modes, then we
      //do want to "go normal."
      QList<QAction*> actions = mToolsToolBar->actions();
      for (int actionIndex = 0; actionIndex < (int)actions.size(); actionIndex++)
      {
         if(actions[actionIndex]->isChecked())
         {
            return;  //something is already checked.
         }
      }

      mNormalToolMode->setChecked(true);
   }

   ///////////////////////////////////////////////////////////////////////////////
   PropertyEditor* MainWindow::GetPropertyEditor()
   {
      return mPropertyWindow;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string& MainWindow::GetSTAGEConfigFile()
   {
      return mSTAGEConfigFullPath;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtActors::VolumeEditActor* MainWindow::GetVolumeEditActor()
   {
      return dynamic_cast<dtActors::VolumeEditActor*>(mVolEditActorProxy.get()->GetDrawable());
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtActors::VolumeEditActorProxy* MainWindow::GetVolumeEditActorProxy()
   {
      return mVolEditActorProxy.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::findAndLoadPreferences()
   {
      EditorSettings testSettings;

      //If there are no user preferences .ini file (must be the first time they
      //have ever loaded STAGE) ... see if we can't find a default.ini file
      testSettings.beginGroup(EditorSettings::PREFERENCES_GROUP);
      QVariant val = testSettings.value(EditorSettings::SAVE_MILLISECONDS, -1);
      if (val == -1)
      {
         //attempt to locate a "default" ini file and copy it to the
         //user's directory
         std::string src = dtUtil::FileUtils::GetInstance().CurrentDirectory()
                            + "/default.ini";
         if (dtUtil::FileUtils::GetInstance().FileExists(src))
         {
            std::string path = testSettings.fileName().toStdString();

            std::string chopThisOff = "/" + EditorSettings::APPLICATION.toStdString()
                                        + ".ini";

            path = path.substr(0, path.rfind(chopThisOff));

            std::string dest = path + "/"
               + EditorSettings::APPLICATION.toStdString() + ".ini";

            //make sure folder exists before attempting to copy into it
            bool doFileCopy = true;
            if (! dtUtil::FileUtils::GetInstance().DirExists(path))
            {
               try
               {
                  dtUtil::FileUtils::GetInstance().MakeDirectory(path);
               }
               catch (dtUtil::Exception&)
               {
                  doFileCopy = false;
                  LOG_ERROR("Unable to create directory for default.ini");
               }
            }
            if (doFileCopy)
            {
               try
               {
                  dtUtil::FileUtils::GetInstance().FileCopy(src, dest, false);
               }
               catch (dtUtil::Exception&)
               {
                  LOG_ERROR("Unable to copy default.ini to user preferences folder.");
               }
            }
         }
      }
      testSettings.endGroup();

      EditorSettings settings;

      // Load settings from last session.  Reasonable defaults are specified,
      // if the settings are not present.

      // Main window settings...
      settings.beginGroup(EditorSettings::MAINWIN_GROUP);
      resize(settings.value(EditorSettings::MAINWIN_SIZE, QSize(800, 600)).toSize());
      move(settings.value(EditorSettings::MAINWIN_POSITION, QPoint(100, 100)).toPoint());

      // When restoring the window state, first see if the key exists.
      if (settings.contains(EditorSettings::MAINWIN_GEOMETRY))
      {
         QByteArray state = settings.value(EditorSettings::MAINWIN_GEOMETRY).toByteArray();
         restoreGeometry(state);
      }

      // When restoring the window state, first see if the key exists.
      if (settings.contains(EditorSettings::MAINWIN_DOCK_STATE))
      {
         QByteArray state = settings.value(EditorSettings::MAINWIN_DOCK_STATE).toByteArray();
         restoreState(state,EditorSettings::MAINWIN_DOCK_STATE_ID);
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

      //recent project context list
      SetupRecentProjects();

      // Now check for the general preferences...
      settings.beginGroup(EditorSettings::PREFERENCES_GROUP);

      bool loadProjs = settings.value(EditorSettings::LOAD_RECENT_PROJECTS, true).toBool();
      EditorData::GetInstance().setLoadLastProject(loadProjs);

      bool loadMaps = settings.value(EditorSettings::LOAD_RECENT_MAPS, true).toBool();
      EditorData::GetInstance().setLoadLastMap(loadMaps);

      FindRecentMaps();

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

      if (settings.contains(EditorSettings::NUM_RECENT_PROJECTS))
      {
         bool success;
         unsigned int numRecentProjects = settings.value(EditorSettings::NUM_RECENT_PROJECTS).toUInt(&success);
         if (success)
         {
            EditorData::GetInstance().SetNumRecentProjects(numRecentProjects);
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

      //volume edit show/hide
      if (settings.contains(EditorSettings::VOLUME_EDIT_VISIBLE))
      {
         //By default, the volume edit brush is visible, so we only need to toggle it
         //if we should hide it.
         bool visibile = settings.value(EditorSettings::VOLUME_EDIT_VISIBLE).toBool();
         if (visibile == false)
         {
            EditorActions::GetInstance().slotShowHideBrush();
         }
      }

      //volume edit shape
      if (settings.contains(EditorSettings::VOLUME_EDIT_SHAPE))
      {
         const std::string shapeName = settings.value(EditorSettings::VOLUME_EDIT_SHAPE).toString().toStdString();
         dtActors::VolumeEditActor::VolumeShapeType* shapeType = dtActors::VolumeEditActor::VolumeShapeType::GetValueForName(shapeName);
         if (shapeType)
         {
            EditorActions::GetInstance().setBrushShape(*shapeType);
         }
      }

      settings.endGroup();

      //Camera speed settings
      settings.beginGroup(EditorSettings::CAMERA_SPEED_GROUP);
      ReadCameraSpeed(settings, QString::fromStdString(mPerspView->getName()));
      ReadCameraSpeed(settings, QString::fromStdString(mTopView->getName()));
      ReadCameraSpeed(settings, QString::fromStdString(mFrontView->getName()));
      ReadCameraSpeed(settings, QString::fromStdString(mSideView->getName()));
      settings.endGroup();

      // Clip distance settings.
      settings.beginGroup(EditorSettings::CLIP_DISTANCE_GROUP);
      ReadClipDistance(settings, QString::fromStdString(mPerspView->getName()));
      ReadClipDistance(settings, QString::fromStdString(mTopView->getName()));
      ReadClipDistance(settings, QString::fromStdString(mFrontView->getName()));
      ReadClipDistance(settings, QString::fromStdString(mSideView->getName()));
      settings.endGroup();

      // Snap settings.
      settings.beginGroup(EditorSettings::SNAP_GROUP);

      bool snapTranslationEnabled = false;
      if (settings.contains(EditorSettings::SNAP_TRANSLATION_ENABLED))
      {
         snapTranslationEnabled = settings.value(EditorSettings::SNAP_TRANSLATION_ENABLED).toBool();
      }

      bool snapRotationEnabled = false;
      if (settings.contains(EditorSettings::SNAP_ROTATION_ENABLED))
      {
         snapRotationEnabled = settings.value(EditorSettings::SNAP_ROTATION_ENABLED).toBool();
      }

      bool snapScaleEnabled = false;
      if (settings.contains(EditorSettings::SNAP_SCALE_ENABLED))
      {
         snapScaleEnabled = settings.value(EditorSettings::SNAP_SCALE_ENABLED).toBool();
      }

      ViewportManager::GetInstance().emitSetSnapEnabled(snapTranslationEnabled, snapRotationEnabled, snapScaleEnabled);

      if (settings.contains(EditorSettings::SNAP_TRANSLATION_VALUE))
      {
         bool success;
         float value = (float)settings.value(EditorSettings::SNAP_TRANSLATION_VALUE).toDouble(&success);
         if (success)
         {
            ViewportManager::GetInstance().emitSetSnapTranslation(value);
         }
      }

      if (settings.contains(EditorSettings::SNAP_ROTATION_VALUE))
      {
         bool success;
         float value = (float)settings.value(EditorSettings::SNAP_ROTATION_VALUE).toDouble(&success);
         if (success)
         {
            ViewportManager::GetInstance().emitSetSnapRotation(value);
         }
      }

      if (settings.contains(EditorSettings::SNAP_SCALE_VALUE))
      {
         bool success;
         float value = (float)settings.value(EditorSettings::SNAP_SCALE_VALUE).toDouble(&success);
         if (success)
         {
            ViewportManager::GetInstance().emitSetSnapScale(value);
         }
      }
      settings.endGroup();

      //plugins
      mPluginManager->StartPluginsInConfigFile();

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

   void MainWindow::WritePreferences()
   {
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
      settings.setValue(EditorSettings::NUM_RECENT_PROJECTS, editorData.GetNumRecentProjects());
      settings.setValue(EditorSettings::SAVE_MILLISECONDS, EditorActions::GetInstance().mSaveMilliSeconds);
      settings.setValue(EditorSettings::SELECTION_COLOR, editorData.getSelectionColor());

      //Volume edit brush shape, visibility
      settings.setValue(EditorSettings::VOLUME_EDIT_VISIBLE, GetVolumeEditActor()->GetOSGNode()->getNodeMask() ? "true" : "false");
      settings.setValue(EditorSettings::VOLUME_EDIT_SHAPE, QString::fromStdString(GetVolumeEditActor()->GetShape().GetName()));

      settings.endGroup();

      //camera speed settings
      settings.beginGroup(EditorSettings::CAMERA_SPEED_GROUP);
      QString perspName(mPerspView->getName().c_str());
      QString topName(mTopView->getName().c_str());
      QString frontName(mFrontView->getName().c_str());
      QString sideName(mSideView->getName().c_str());
      settings.setValue(perspName, mViewportContainers.value(perspName)->GetCameraSpeed());
      settings.setValue(topName, mViewportContainers.value(topName)->GetCameraSpeed());
      settings.setValue(frontName, mViewportContainers.value(frontName)->GetCameraSpeed());
      settings.setValue(sideName, mViewportContainers.value(sideName)->GetCameraSpeed());
      settings.endGroup();

      // Clip distance settings.
      settings.beginGroup(EditorSettings::CLIP_DISTANCE_GROUP);
      settings.setValue(perspName, mViewportContainers.value(perspName)->GetClipDistance());
      settings.setValue(topName, mViewportContainers.value(topName)->GetClipDistance());
      settings.setValue(frontName, mViewportContainers.value(frontName)->GetClipDistance());
      settings.setValue(sideName, mViewportContainers.value(sideName)->GetClipDistance());
      settings.endGroup();

      // Save our current snap settings.
      settings.beginGroup(EditorSettings::SNAP_GROUP);
      settings.setValue(EditorSettings::SNAP_TRANSLATION_ENABLED, ViewportManager::GetInstance().GetSnapTranslationEnabled());
      settings.setValue(EditorSettings::SNAP_ROTATION_ENABLED, ViewportManager::GetInstance().GetSnapRotationEnabled());
      settings.setValue(EditorSettings::SNAP_SCALE_ENABLED, ViewportManager::GetInstance().GetSnapScaleEnabled());
      settings.setValue(EditorSettings::SNAP_TRANSLATION_VALUE, ViewportManager::GetInstance().GetSnapTranslation());
      settings.setValue(EditorSettings::SNAP_ROTATION_VALUE, ViewportManager::GetInstance().GetSnapRotation());
      settings.setValue(EditorSettings::SNAP_SCALE_VALUE, ViewportManager::GetInstance().GetSnapScale());
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

      // Save the current project state...
      QStringList projectStringList;
      std::list<std::string>::iterator itr = EditorData::GetInstance().getRecentProjects().begin();
      while (itr != EditorData::GetInstance().getRecentProjects().end())
      {
         projectStringList << QString::fromStdString(*itr);
         ++itr;
      }
      settings.setValue(EditorSettings::RECENT_PROJECTS, projectStringList);

      //Check to see if the user wants the app to remember the recently loaded map.
      if (EditorData::GetInstance().getLoadLastMap() == false ||
          EditorData::GetInstance().getCurrentMap() == NULL)
      {
         // Error check, if they have a previous settings file with a recent map in it, it
         // needs to be deleted as to not load it next time
         settings.remove(EditorSettings::RECENT_MAPS);
      }
      else
      {
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

      //Save the Plugin state
      mPluginManager->StoreActivePluginsToConfigFile();
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
   bool MainWindow::LoadLastProject()
   {
      bool contextLoaded = false;
      if (EditorData::GetInstance().getLoadLastProject() == false)
      {
         return false;
      }

      //try to set the last used Project Context
      const std::string lastProject = EditorData::GetInstance().getRecentProjects().front();
      if (dtUtil::FileUtils::GetInstance().DirExists(lastProject))
      {
         EditorActions::GetInstance().SlotChangeProjectContext(lastProject);
         contextLoaded = true; //did it really load?
      }

      return contextLoaded;
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
            maps.push_back(mapName);
            EditorData::GetInstance().addRecentMap(mapName);
         }
      }
      settings.endGroup();
      return maps;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MainWindow::checkAndLoadBackup(const std::string& str)
   {
      if (str.empty())
      {
         return;
      }

      bool hasBackup;

      try
      {
         hasBackup = dtCore::Project::GetInstance().HasBackup(str);
      }
      catch (dtUtil::Exception&)
      {
         //must not have a valid backup
         hasBackup = false;
      }

      if (hasBackup)
      {
         int result = QMessageBox::information(this, tr("Backup file found"),
            tr("A backup save file has been detected. Would you like to open it?"),
            tr("Yes"), tr("No"), tr("Cancel"));

         if (result == 0)
         {
            startWaitCursor();
            dtCore::Map& backupMap =
               dtCore::Project::GetInstance().OpenMapBackup(str);

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
               dtCore::Project::GetInstance().ClearBackup(str);
            }
            //*/

            startWaitCursor();
            dtCore::Project::GetInstance().ClearBackup(str);
            EditorActions::GetInstance().changeMaps(EditorData::GetInstance().getCurrentMap(),
               &dtCore::Project::GetInstance().GetMap(str));
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
         try
         {
            dtCore::ObserverPtr<dtCore::Map> newMap = &dtCore::Project::GetInstance().GetMap(str);
            dtCore::ObserverPtr<dtCore::Map> currentMap = EditorData::GetInstance().getCurrentMap();
            EditorActions::GetInstance().changeMaps(currentMap.get(), newMap.get());
            
            // add current map instead of "newMap" to avoid bad pointer issues when the newMap was 
            // the same as "earlier" currentMap
            currentMap = EditorData::GetInstance().getCurrentMap();
            EditorData::GetInstance().addRecentMap(currentMap->GetName());
         }
         catch (dtUtil::Exception&)
         {
            QMessageBox::critical(this, tr("Failed to load map"),
               tr("Failed to load previous map at: \n") +
               tr(str.c_str()),
               QMessageBox::Ok);
         }
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

   ////////////////////////////////////////////////////////////////////////////////
   void MainWindow::RefreshRecentProjectsMenu()
   {
      mRecentProjs->clear();
      for (size_t i = 0; i < EditorActions::GetInstance().mActionRecentProjects.size(); ++i)
      {
         mRecentProjs->addAction(EditorActions::GetInstance().mActionRecentProjects[i]);
      }
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
      if (dtUtil::IsEnvironment("STAGE_PLUGIN_PATH"))
      {
         pluginPath = dtUtil::GetEnvironment("STAGE_PLUGIN_PATH");
      }

      if (pluginPath.empty())
      {
#ifdef DELTA_WIN32
         pluginPath = QCoreApplication::applicationDirPath().toStdString() + "/stplugins";
#else
         pluginPath = QCoreApplication::applicationDirPath().toStdString() + "/../lib/stplugins";
         if (!dtUtil::FileUtils::GetInstance().DirExists(pluginPath))
         {
            pluginPath = QCoreApplication::applicationDirPath().toStdString() + "/../PlugIns/stplugins";
         }
#endif
      }

      #ifdef DELTA_WIN32
         #ifdef _DEBUG
           pluginPath += "/Debug";
         //#else
         //  pluginPath += "/Release";
         #endif
      #endif

      if (!dtUtil::FileUtils::GetInstance().DirExists(pluginPath))
      {
         //no plugin path found...lets not try to load any plugins
         LOG_WARNING("Plugin path \"" + pluginPath + "\" was found. No plugins will be loaded.");
         return;
      }

      LOG_INFO("Trying to load plugins from directory " + pluginPath);

      // instantiate all plugin factories and immediately start system plugins
      mPluginManager->LoadPluginsInDir(pluginPath);

      // start plugins that were set in config file
      mPluginManager->StartPluginsInConfigFile();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MainWindow::SetupRecentProjects() const
   {
      EditorSettings settings;
      QStringList recentProjectList = settings.value(EditorSettings::RECENT_PROJECTS).toStringList();

      //Projects are stored with the most recently used listed first, but EditorData
      //will store the last project added as the first.  So we have to add them
      //with the most recently used getting added last.
      QStringListIterator itr(recentProjectList);
      itr.toBack();
      while (itr.hasPrevious())
      {
         EditorData::GetInstance().addRecentProject(itr.previous().toStdString());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void MainWindow::ReadCameraSpeed(const EditorSettings &settings, const QString& viewName) const
   {
      if (settings.contains(viewName))
      {
         int value = settings.value(viewName).toInt();
         if (mViewportContainers.contains(viewName))
         {
            mViewportContainers.value(viewName)->SetCameraSpeed(value);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MainWindow::ReadClipDistance(const EditorSettings& settings, const QString& viewName) const
   {
      if (settings.contains(viewName))
      {
         int value = settings.value(viewName).toInt();
         if (mViewportContainers.contains(viewName))
         {
            mViewportContainers.value(viewName)->SetClipDistance(value);
         }
      }
   }

} // namespace dtEditQt
