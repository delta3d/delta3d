#include "ObjectWorkspace.h"
#include "AnimationControlDock.h"
#include "ResourceDock.h"
#include "ObjectViewer.h"

#include <dtQt/nodetreepanel.h>
#include <dtQt/osggraphicswindowqt.h>
#include <dtQt/projectcontextdialog.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/xercesparser.h>
#include <dtCore/project.h>
#include <dtCore/map.h>
#include <dtCore/datatype.h>
#include <dtAnim/cal3ddatabase.h>
#include <dtCore/deltawin.h>

#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QtGui/QFileDialog>
#include <QtGui/QTableWidget>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QDockWidget>
#include <QtGui/QMessageBox>
#include <QtGui/QHBoxLayout>

#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtGui/QDragEnterEvent>
#include <QtOpenGL/QGLWidget>

#include <cassert>

////////////////////////////////////////////////////////////////////////////////
ObjectWorkspace::ObjectWorkspace()
  : mLoadShaderDefAction(NULL)
  , mLoadGeometryAction(NULL)
  , mExitAct(NULL)
{
   resize(1024, 768);

   mResourceDock = new ResourceDock;
   addDockWidget(Qt::LeftDockWidgetArea, mResourceDock);

   mAnimationControlDock = new AnimationControlDock;
   addDockWidget(Qt::RightDockWidgetArea, mAnimationControlDock);

   mNodeToolsDock = new QDockWidget;
   addDockWidget(Qt::LeftDockWidgetArea, mNodeToolsDock);

   // Additional dock object setup
   mNodeTree = new dtQt::NodeTreePanel();
   mNodeToolsDock->setWidget(mNodeTree);

   // Create all program actions
   CreateFileMenuActions();
   CreateEditingToolbarAction();
   CreateDisplayToolbarActions();
   CreateShaderToolbarActions();

   CreateMenus();
   statusBar();
   CreateToolbars();

   QWidget* glParent = new QWidget();

   mCentralLayout = new QHBoxLayout();
   mCentralLayout->setMargin(0);
   glParent->setLayout(mCentralLayout);
   setCentralWidget(glParent);

   setAcceptDrops(true);
}

////////////////////////////////////////////////////////////////////////////////
ObjectWorkspace::~ObjectWorkspace()
{
}

////////////////////////////////////////////////////////////////////////////////
QObject* ObjectWorkspace::GetResourceObject()
{
   return mResourceDock;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::GetRecursiveFileInfoFromDir(const QString& rootDir, const QStringList& nameFilters, QFileInfoList& outList)
{
   QDir directory;

   if (directory.cd(rootDir))
   {
      outList = directory.entryInfoList(nameFilters, QDir::Files);

      QFileInfoList dirList = directory.entryInfoList(QStringList(), QDir::AllDirs);

      while (!dirList.empty())
      {
         QFileInfoList subDirectoryFiles;
         QFileInfo fileInfo = dirList.takeFirst();

         if (fileInfo.fileName() != "." && fileInfo.fileName() != "..")
         {
            GetRecursiveFileInfoFromDir(fileInfo.absoluteFilePath(), nameFilters, subDirectoryFiles);
         }

         outList += subDirectoryFiles;
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::dragEnterEvent(QDragEnterEvent* event)
{
   if (event->mimeData()->hasFormat("text/uri-list"))
   {
      event->acceptProposedAction();
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::dropEvent(QDropEvent *event)
{
   QList<QUrl> urlList = event->mimeData()->urls();

   if (!urlList.empty())
   {
      QString filename = urlList.first().toLocalFile();

      bool attemptToLoad = true;

      // Is this an unsupported xml like a map?
      if (filename.endsWith(".dtMap"))
      {
         attemptToLoad = false;

         // Note: maps are tricky since the filename does not necessarily
         // correspond to the map's actual name.  Furthermore, map names
         // are not guaranteed to be unique.  ...drag and drop not recommended.
         QMessageBox::information(this, "Info", "Drag & drop not supported for maps.", QMessageBox::Ok);
      }

      // If the file was identified as either static or skeletal mesh
      if (attemptToLoad)
      {
         OnLoadGeometry(filename.toStdString());
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::CreateMenus()
{
   QMenu* windowMenu   = menuBar()->addMenu("&File");
   QMenu* viewMenu     = menuBar()->addMenu("&View");
   QMenu* settingsMenu = menuBar()->addMenu("&Settings");

   QMenu* toolBarMenu = viewMenu->addMenu("&Toolbars");

   windowMenu->addAction(mLoadShaderDefAction);
   windowMenu->addAction(mLoadGeometryAction);
   windowMenu->addAction(mChangeContextAction);

   QAction* toggleShadeToolbarAction = toolBarMenu->addAction(tr("Shading toolbar"));
   toggleShadeToolbarAction->setCheckable(true);
   toggleShadeToolbarAction->setChecked(true);

   mGenerateTangentsAction = settingsMenu->addAction(tr("Generate tangent attribute on geometry load."));
   mGenerateTangentsAction->setCheckable(true);
   mGenerateTangentsAction->setChecked(true);

   connect(toggleShadeToolbarAction, SIGNAL(triggered()), this, SLOT(OnToggleShadingToolbar()));
   connect(mGenerateTangentsAction, SIGNAL(triggered()), this, SLOT(OnToggleGenerateTangents()));

   menuBar()->addSeparator();
   windowMenu->addAction(mExitAct);

   viewMenu->addAction(mToggleDockAnimationControl);
   viewMenu->addAction(mToggleDockResources);
   viewMenu->addAction(mToggleDockNodeTools);
   
   connect(mToggleDockAnimationControl, SIGNAL(toggled(bool)), mAnimationControlDock, SLOT(setVisible(bool)));
   connect(mAnimationControlDock, SIGNAL(visibilityChanged(bool)), mToggleDockAnimationControl, SLOT(setChecked(bool)));

   connect(mToggleDockResources, SIGNAL(toggled(bool)), mResourceDock, SLOT(setVisible(bool)));
   connect(mResourceDock, SIGNAL(visibilityChanged(bool)), mToggleDockResources, SLOT(setChecked(bool)));

   connect(mToggleDockNodeTools, SIGNAL(toggled(bool)), mNodeToolsDock, SLOT(setVisible(bool)));
   connect(mNodeToolsDock, SIGNAL(visibilityChanged(bool)), mToggleDockNodeTools, SLOT(setChecked(bool)));
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::CreateFileMenuActions()
{
   mExitAct = new QAction(tr("E&xit"), this);
   mExitAct->setShortcut(tr("Ctrl+Q"));
   mExitAct->setStatusTip(tr("Exit the application"));
   connect(mExitAct, SIGNAL(triggered()), this, SLOT(close()));

   mLoadShaderDefAction = new QAction(tr("&Load Shader Definitions..."), this);
   mLoadShaderDefAction->setStatusTip(tr("Open an existing shader definition file."));
   connect(mLoadShaderDefAction, SIGNAL(triggered()), this, SLOT(OnLoadShaderDefinition()));

   mLoadGeometryAction = new QAction(tr("Load Geometry..."), this);
   mLoadGeometryAction->setStatusTip(tr("Open an existing shader definition file."));
   connect(mLoadGeometryAction, SIGNAL(triggered()), this, SLOT(OnLoadGeometry()));

   mChangeContextAction = new QAction(tr("Change Project..."), this);
   mChangeContextAction->setStatusTip(tr("Change the project context directory."));
   connect(mChangeContextAction, SIGNAL(triggered()), this, SLOT(OnChangeContext()));
   
   mToggleDockAnimationControl = new QAction(tr("Anim Controls"), this);
   mToggleDockAnimationControl->setCheckable(true);
   mToggleDockAnimationControl->setChecked(true);

   mToggleDockResources = new QAction(tr("Resources"), this);
   mToggleDockResources->setCheckable(true);
   mToggleDockResources->setChecked(true);

   mToggleDockNodeTools = new QAction(tr("Node Tools"), this);
   mToggleDockNodeTools->setCheckable(true);
   mToggleDockNodeTools->setChecked(true);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::CreateEditingToolbarAction()
{
   QActionGroup* coordinateGroup = new QActionGroup(this);
   coordinateGroup->setExclusive(true);

   QIcon worldSpaceIcon(":/images/worldspace.png");
   QIcon localSpaceIcon(":/images/localspace.png");
   QIcon translationModeIcon(":/images/translatemode.png");
   QIcon rotationModeIcon(":/images/rotatemode.png");

   mWorldSpaceAction = coordinateGroup->addAction(worldSpaceIcon, "World Space");
   mLocalSpaceAction = coordinateGroup->addAction(localSpaceIcon, "Local Space");

   mWorldSpaceAction->setCheckable(true);
   mLocalSpaceAction->setCheckable(true);

   mLocalSpaceAction->setChecked(true);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::CreateDisplayToolbarActions()
{
   // The actiongroup is used to make the action behave like radio buttons
   QActionGroup *actionGroup = new QActionGroup(this);
   actionGroup->setExclusive(true);

   QIcon wireframeIcon(":/images/wireframe.png");
   QIcon shadedIcon(":/images/shaded.png");
   QIcon shadedWireIcon(":/images/shadedwire.png");
   QIcon gridIcon(":/images/xygrid.png");
   QIcon statIcon(":/images/statistics.png");

   mWireframeAction  = actionGroup->addAction(wireframeIcon, "Wireframe");
   mShadedAction     = actionGroup->addAction(shadedIcon, "Shaded");
   mShadedWireAction = actionGroup->addAction(shadedWireIcon, "Shaded Wireframe");

   mGridAction = new QAction(gridIcon, "Toggle Grid", this);
   connect(mGridAction, SIGNAL(toggled(bool)), this, SLOT(OnToggleGridClicked(bool)));

   mStatisticsAction = new QAction(statIcon, "Stats", this);

   mWireframeAction->setCheckable(true);
   mShadedAction->setCheckable(true);
   mShadedWireAction->setCheckable(true);
   mGridAction->setCheckable(true);

   mShadedAction->setChecked(true);
   mGridAction->setChecked(true);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::CreateShaderToolbarActions()
{
   QIcon compileIcon(":/images/recompile.png");
   QIcon vertexSourceIcon(":/images/vertexShaderSource.png");
   QIcon geomSourceIcon(":/images/geomShaderSource.png");
   QIcon fragmentSourceIcon(":/images/fragmentShaderSource.png");

   mRecompileAction = new QAction(compileIcon, tr("Recompile Shaders"), this);
   mOpenVertexShaderAction = new QAction(vertexSourceIcon, tr("Open Current Vertex Shader"), this);
   mOpenGeometryShaderAction = new QAction(geomSourceIcon, tr("Open Current Geometry Shader"), this);
   mOpenFragmentShaderAction = new QAction(fragmentSourceIcon, tr("Open Current Fragment Shader"), this);

   mOpenVertexShaderAction->setEnabled(false);
   mOpenFragmentShaderAction->setEnabled(false);
   mOpenGeometryShaderAction->setEnabled(false);
   mRecompileAction->setEnabled(false);

   connect(mRecompileAction, SIGNAL(triggered()), this, SLOT(OnRecompileClicked()));

   connect(mOpenVertexShaderAction, SIGNAL(triggered()),
           mResourceDock, SLOT(OnOpenCurrentVertexShaderSources()));

   connect(mOpenGeometryShaderAction, SIGNAL(triggered()),
           mResourceDock, SLOT(OnOpenCurrentGeometryShaderSources()));

   connect(mOpenFragmentShaderAction, SIGNAL(triggered()),
           mResourceDock, SLOT(OnOpenCurrentFragmentShaderSources()));
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::CreateToolbars()
{
   mCoordinateToolbar = addToolBar("Coordinate Toolbar");
   mCoordinateToolbar->addAction(mLocalSpaceAction);
   mCoordinateToolbar->addAction(mWorldSpaceAction);

   mDisplayToolbar = addToolBar("Display toolbar");
   mDisplayToolbar->addAction(mWireframeAction);
   mDisplayToolbar->addAction(mShadedAction);
   mDisplayToolbar->addAction(mShadedWireAction);
   mDisplayToolbar->addSeparator();
   mDisplayToolbar->addAction(mGridAction);
   mDisplayToolbar->addSeparator();
   mDisplayToolbar->addAction(mStatisticsAction);

   mShaderToolbar = addToolBar("Shader toolbar");
   mShaderToolbar->addAction(mOpenVertexShaderAction);
   mShaderToolbar->addAction(mOpenGeometryShaderAction);
   mShaderToolbar->addAction(mOpenFragmentShaderAction);
   mShaderToolbar->addAction(mRecompileAction);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnInitialization()
{
   QSettings settings("MOVES", "Object Viewer");
   QStringList files = settings.value("projectContextPath").toStringList();

   if (files.empty())
   {
      //If the user has not selected a project context, then just let them do it later.
      //mContextPath = GetContextPathFromUser();  (this had been causing a crash)
      return;
   }
   else
   {
      mContextPath = files.at(0).toStdString();
   }

   // Load additional shader files.
   int shaderSize = settings.beginReadArray("shaderList");

   mAdditionalShaderFiles.clear();
   for (int shaderIndex = 0; shaderIndex < shaderSize; shaderIndex++)
   {
      settings.setArrayIndex(shaderIndex);
      mAdditionalShaderFiles.append(settings.value("shaderFile").toString().toStdString());
   }
   settings.endArray();

   SaveCurrentContextPath();
   SaveCurrentShaderFiles();
   UpdateResourceLists();
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnToggleResourceDock()
{
   if (mResourceDock->isHidden())
   {
      mResourceDock->show();
   }
   else
   {
      mResourceDock->hide();
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnToggleAnimationControlDock()
{
   if (mAnimationControlDock->isHidden())
   {
      mAnimationControlDock->show();
   }
   else
   {
      mAnimationControlDock->hide();
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnToggleNodeToolsDock()
{
   if (mNodeToolsDock->isHidden())
   {
      mNodeToolsDock->show();
   }
   else
   {
      mNodeToolsDock->hide();
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnToggleShadingToolbar()
{
   if (mDisplayToolbar->isHidden())
   {
      mDisplayToolbar->show();
   }
   else
   {
      mDisplayToolbar->hide();
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnToggleGenerateTangents()
{
   emit SetGenerateTangentAttribute(mGenerateTangentsAction->isChecked());
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnRecompileClicked()
{
   emit ReloadShaderDefinition();
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnToggleVertexShaderSource(bool enabled)
{
   mOpenVertexShaderAction->setEnabled(enabled);

   // If a shader is editable, allow recompile
   mRecompileAction->setEnabled(mOpenVertexShaderAction->isEnabled() ||
                                mOpenFragmentShaderAction->isEnabled());
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnToggleGeometryShaderSource(bool enabled)
{
   mOpenGeometryShaderAction->setEnabled(enabled);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnToggleFragmentShaderSource(bool enabled)
{
   mOpenFragmentShaderAction->setEnabled(enabled);

   // If a shader is editable, allow recompile
   mRecompileAction->setEnabled(mOpenVertexShaderAction->isEnabled() ||
                                mOpenFragmentShaderAction->isEnabled());
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnRemoveShaderDef(const std::string& filename)
{
   if (mAdditionalShaderFiles.contains(filename))
   {
      mAdditionalShaderFiles.removeOne(filename);
      SaveCurrentShaderFiles();
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::UpdateResourceLists()
{
   // If the context path in the registry is not valid, this will not be entered
   if (dtCore::Project::GetInstance().IsContextValid())
   {
      assert(!mContextPath.empty());
      UpdateShaderList();
      UpdateGeometryList();
      UpdateMapList();
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::UpdateGeometryList()
{
   QDir directory(mContextPath.c_str());

   // Populate the static mesh list.
   QString staticMeshDir = QString(mContextPath.c_str()) + "/" + dtCore::DataType::STATIC_MESH.GetName().c_str();

   QStringList staticMeshNameFilters;
   staticMeshNameFilters << "*.ive" << "*.osg" << "*.earth";

   QFileInfoList staticMeshFileList;
   GetRecursiveFileInfoFromDir(staticMeshDir, staticMeshNameFilters, staticMeshFileList);

   while (!staticMeshFileList.empty())
   {
      QFileInfo fileInfo = staticMeshFileList.takeFirst();

      std::string pathName = fileInfo.absolutePath().toStdString();
      std::string fileName = fileInfo.fileName().toStdString();
      mResourceDock->OnNewGeometry(pathName, fileName);
   }

   // Populate the skeletal mesh list.
   QString skeletalMeshDir = QString(mContextPath.c_str()) + "/" + dtCore::DataType::SKELETAL_MESH.GetName().c_str();

   QStringList skeltalMeshNameFilter;
   skeltalMeshNameFilter << "*.dtChar";

   QFileInfoList skeletalMeshFileList;
   GetRecursiveFileInfoFromDir(skeletalMeshDir, skeltalMeshNameFilter, skeletalMeshFileList);

   while (!skeletalMeshFileList.empty())
   {
      QFileInfo fileInfo = skeletalMeshFileList.takeFirst();

      std::string pathName = fileInfo.absolutePath().toStdString();
      std::string fileName = fileInfo.fileName().toStdString();
      mResourceDock->OnNewSkinnedMesh(pathName, fileName);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::UpdateShaderList()
{
   QDir directory(mContextPath.c_str());

   if (directory.cd(QString(mContextPath.c_str()) + "/" + dtCore::DataType::SHADER.GetName().c_str()))
   {
      QStringList nameFilters;
      nameFilters << "*.dtShader" << "*.xml";

      QFileInfoList fileList = directory.entryInfoList(nameFilters, QDir::Files);

      // Try to load all definitions
      while (!fileList.empty())
      {
         QFileInfo fileInfo = fileList.takeFirst();
         mShaderDefinitionName = QString("%1/%2/%3").arg(QString(mContextPath.c_str()), QString(dtCore::DataType::SHADER.GetName().c_str()), fileInfo.fileName());
         emit LoadShaderDefinition(mShaderDefinitionName);
      }

      directory.cdUp();
   }

   // Now load all the additional shader files in the shader lists.
   for (int shaderIndex = 0; shaderIndex < mAdditionalShaderFiles.size(); shaderIndex++)
   {
      emit LoadShaderDefinition(mAdditionalShaderFiles.at(shaderIndex).c_str());
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::UpdateMapList()
{
   QDir directory(mContextPath.c_str());

   // Populate the map list.
   QStringList mapList;
   std::set<std::string> mapNames = dtCore::Project::GetInstance().GetMapNames();
   for (std::set<std::string>::iterator map = mapNames.begin(); map != mapNames.end(); map++)
   {
      mapList << map->c_str();
   }

   for (int mapIndex = 0; mapIndex < mapList.size(); mapIndex++)
   {
      mResourceDock->OnNewMap(mapList.at(mapIndex).toStdString());
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnLoadShaderDefinition()
{
   QString filename = QFileDialog::getOpenFileName(this, tr("Load Shader Definition File"),
      mContextPath.c_str(), tr("Shaders(*.dtShader)") + " " + tr("Shaders(*.xml)") );

   QString statusMessage;

   if (!filename.isEmpty())
   {
      if (dtUtil::FileUtils::GetInstance().FileExists(filename.toStdString()))
      {
         emit LoadShaderDefinition(filename);
         statusMessage = QString(tr("File Loaded"));

         if (!mAdditionalShaderFiles.contains(filename.toStdString()))
         {
            mAdditionalShaderFiles.append(filename.toStdString());
            SaveCurrentShaderFiles();
         }
      }
   }
   else
   {
      statusMessage = QString(tr("Unable to load file: %1")).arg(filename);
   }

    statusBar()->showMessage(statusMessage, 2000);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnLoadMap(const std::string& mapName)
{
   QTreeWidgetItem* mapItem = mResourceDock->FindMapItem(mapName);

   if (mapItem)
   {
      mapItem->setCheckState(0, Qt::Checked);
   }
   else
   {
      QMessageBox::critical(this, "Error", "Map cannot be loaded because it is not part of the current project!", QMessageBox::Ok);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnLoadGeometry()
{
   QString filename = QFileDialog::getOpenFileName(this,
                                                   tr("Load Geometry File"),
                                                   mContextPath.c_str(),
                                                   tr("Geometry(*.osg *.ive *.flt *.3ds *.txp *.xml *.earth *.dae *)") );

   QString statusMessage;

   if (!filename.isEmpty())
   {
      OnLoadGeometry(filename.toStdString());
   }
   else
   {
      statusMessage = QString(tr("Unable to load file: %1")).arg(filename);
   }

   statusBar()->showMessage(statusMessage, 2000);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnLoadGeometry(const std::string& fullName)
{
   if (dtUtil::FileUtils::GetInstance().FileExists(fullName))
   {
      QFileInfo fileInfo(fullName.c_str());
      QTreeWidgetItem* geometryItem = mResourceDock->FindGeometryItem(fullName);
      
      // NOTE: The resource dock sends a signal to the viewer
      // load the actual file when the resource dock adds an
      // item to its treeview.

      // Remove the current model if it currently exists.
      if (geometryItem != NULL)
      {
         mResourceDock->RemoveGeometryItem(geometryItem);
         geometryItem = mResourceDock->FindGeometryItem(fullName);
      }

      // Only reload the item if it does not exist.
      if (geometryItem == NULL)
      {
         // Give the required information to the resource manager(dock)
         mResourceDock->OnNewGeometry(fileInfo.absolutePath().toStdString(),
            fileInfo.fileName().toStdString());

         // Display the geometry right away
         mResourceDock->SetGeometry(fileInfo.absoluteFilePath().toStdString(), true);
      }
      else
      {
         QMessageBox::information(this, "Warning", "Geometry already loaded!", QMessageBox::Ok);
      }

      mAnimationControlDock->OnGeometryLoaded(mViewer->GetDeltaObject());

      mNodeTree->SetNode(mViewer->GetDeltaObject()->GetOSGNode());
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnChangeContext()
{
   std::string newPath = GetContextPathFromUser();

   if (!newPath.empty())
   {
      mContextPath = newPath;
      SaveCurrentContextPath();

      // Remove data from the current context
      mResourceDock->ClearAll();

      // Re-populate the list using the new context
      UpdateResourceLists();

      mNodeTree->SetNode(NULL);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnToggleGridClicked(bool toggledOn)
{
   emit ToggleGrid(toggledOn);
}

////////////////////////////////////////////////////////////////////////////////
std::string ObjectWorkspace::GetContextPathFromUser()
{
   dtQt::ProjectContextDialog dialog(this);

   if (dialog.exec() == QDialog::Accepted)
   {
      return(dialog.getProjectPath().toStdString());
   }

   return std::string();
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::SaveCurrentContextPath()
{
   QSettings settings("MOVES", "Object Viewer");

   try
   {
      dtCore::Project::GetInstance().SetContext(mContextPath);

      // Update the registry entry based on the current valid context
      settings.setValue("projectContextPath", mContextPath.c_str());
      settings.sync();
   }
   catch (const dtUtil::Exception &e)
   {
      // The context path is not valid, clear the registry entry
      mContextPath = "";
      settings.remove("projectContextPath");
      settings.sync();

      QMessageBox::critical((QWidget *)this, tr("Error"), tr(e.What().c_str()), tr("Ok"));
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::SaveCurrentShaderFiles()
{
   QSettings settings("MOVES", "Object Viewer");

   try
   {
      settings.beginWriteArray("shaderList");

      for (int shaderIndex = 0; shaderIndex < (int)mAdditionalShaderFiles.size(); shaderIndex++)
      {
         settings.setArrayIndex(shaderIndex);
         settings.setValue("shaderFile", mAdditionalShaderFiles.at(shaderIndex).c_str());
      }
      settings.endArray();
   }
   catch (const dtUtil::Exception &e)
   {
      QMessageBox::critical((QWidget *)this, tr("Error"), tr(e.What().c_str()), tr("Ok"));
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::SetViewer(ObjectViewer* viewer)
{
   mViewer = viewer;

   dtQt::OSGGraphicsWindowQt* osgGraphWindow = dynamic_cast<dtQt::OSGGraphicsWindowQt*>(mViewer->GetWindow()->GetOsgViewerGraphicsWindow());

   if (osgGraphWindow == NULL)
   {
      LOG_ERROR("Unable to initialize. The deltawin could not be created with a QGLWidget.");
      return;
   }

   //stuff the QGLWidget into it's parent widget placeholder and ensure it gets
   //resized to fit the parent
   QWidget* widget = osgGraphWindow->GetQGLWidget();
   if (widget != NULL)
   {
      widget->setGeometry(centralWidget()->geometry());
      mCentralLayout->addWidget(widget);
   }

   SetupConnectionsWithViewer();
}


////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::SetupConnectionsWithViewer()
{
   // Menu connections
   connect(this, SIGNAL(LoadShaderDefinition(const QString&)),
      mViewer, SLOT(OnLoadShaderFile(const QString&)));

   connect((QObject*)this, SIGNAL(SetGenerateTangentAttribute(bool)),
      mViewer, SLOT(OnSetGenerateTangentAttribute(bool)));

   connect(this, SIGNAL(ReloadShaderDefinition()),
      this->GetResourceObject(), SLOT(OnReloadShaderFiles()));

   // Resource dock connections
   connect(this->GetResourceObject(), SIGNAL(LoadGeometry(const std::string&)),
      mViewer, SLOT(OnLoadGeometryFile(const std::string&)));

   connect(this->GetResourceObject(), SIGNAL(LoadMap(const std::string&)),
      mViewer, SLOT(OnLoadMapFile(const std::string&)));

   connect(this->GetResourceObject(), SIGNAL(UnloadGeometry()),
      mViewer, SLOT(OnUnloadGeometryFile()));

   connect(mViewer, SIGNAL(ShaderLoaded(const std::string&, const std::string&, const std::string&)),
      this->GetResourceObject(), SLOT(OnNewShader(const std::string&, const std::string&, const std::string&)));

   connect(this->GetResourceObject(), SIGNAL(ApplyShader(const std::string&, const std::string&)),
      mViewer, SLOT(OnApplyShader(const std::string&, const std::string&)));

   connect(mViewer, SIGNAL(LightUpdate(const LightInfo&)),
      this->GetResourceObject(), SLOT(OnLightUpdate(const LightInfo&)));

   connect(this->GetResourceObject(), SIGNAL(RemoveShader()),
      mViewer, SLOT(OnRemoveShader()));

   connect(this->GetResourceObject(), SIGNAL(FixLights()),
      mViewer, SLOT(OnFixLights()));

   connect(this->GetResourceObject(), SIGNAL(SetLightEnabled(int, bool)),
      mViewer, SLOT(OnSetLightEnabled(int, bool)));

   connect(this->GetResourceObject(), SIGNAL(SetLightType(int, int)),
      mViewer, SLOT(OnSetLightType(int, int)));

   connect(this->GetResourceObject(), SIGNAL(SetLightPosition(int, const osg::Vec3&)),
      mViewer, SLOT(OnSetLightPosition(int, const osg::Vec3&)));

   connect(this->GetResourceObject(), SIGNAL(SetLightRotation(int, const osg::Vec3&)),
      mViewer, SLOT(OnSetLightRotation(int, const osg::Vec3&)));

   connect(this->GetResourceObject(), SIGNAL(SetAmbient(int, const osg::Vec4&)),
      mViewer, SLOT(OnSetAmbient(int, const osg::Vec4&)));

   connect(this->GetResourceObject(), SIGNAL(SetDiffuse(int, const osg::Vec4&)),
      mViewer, SLOT(OnSetDiffuse(int, const osg::Vec4&)));

   connect(this->GetResourceObject(), SIGNAL(SetSpecular(int, const osg::Vec4&)),
      mViewer, SLOT(OnSetSpecular(int, const osg::Vec4&)));

   connect(this->GetResourceObject(), SIGNAL(SetLightCutoff(int, float)),
      mViewer, SLOT(OnSetLightCutoff(int, float)));

   connect(this->GetResourceObject(), SIGNAL(SetLightExponent(int, float)),
      mViewer, SLOT(OnSetLightExponent(int, float)));

   connect(this->GetResourceObject(), SIGNAL(SetLightConstant(int, float)),
      mViewer, SLOT(OnSetLightConstant(int, float)));

   connect(this->GetResourceObject(), SIGNAL(SetLightLinear(int, float)),
      mViewer, SLOT(OnSetLightLinear(int, float)));

   connect(this->GetResourceObject(), SIGNAL(SetLightQuadratic(int, float)),
      mViewer, SLOT(OnSetLightQuadratic(int, float)));

   connect(this, SIGNAL(ToggleGrid(bool)), mViewer, SLOT(OnToggleGrid(bool)));
   connect(mStatisticsAction, SIGNAL(triggered()), mViewer, SLOT(OnNextStatistics()));

   connect(this->GetResourceObject(), SIGNAL(ToggleVertexShaderSources(bool)),
      this, SLOT(OnToggleVertexShaderSource(bool)));

   connect(this->GetResourceObject(), SIGNAL(ToggleGeometryShaderSources(bool)),
      this, SLOT(OnToggleGeometryShaderSource(bool)));

   connect(this->GetResourceObject(), SIGNAL(ToggleFragmentShaderSources(bool)),
      this, SLOT(OnToggleFragmentShaderSource(bool)));

   connect(this->GetResourceObject(), SIGNAL(RemoveShaderDef(const std::string&)),
      this, SLOT(OnRemoveShaderDef(const std::string&)));

   // Toolbar connections
   connect((QObject*)this->mShadedAction, SIGNAL(triggered()), mViewer, SLOT(OnSetShaded()));
   connect((QObject*)this->mWireframeAction, SIGNAL(triggered()), mViewer, SLOT(OnSetWireframe()));
   connect((QObject*)this->mShadedWireAction, SIGNAL(triggered()), mViewer, SLOT(OnSetShadedWireframe()));

   // Editing connections
   connect((QObject*)this->mWorldSpaceAction, SIGNAL(triggered()), mViewer, SLOT(OnWorldSpaceMode()));
   connect((QObject*)this->mLocalSpaceAction, SIGNAL(triggered()), mViewer, SLOT(OnLocalSpaceMode()));
}

