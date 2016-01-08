#include "ObjectWorkspace.h"
#include "AnimationControlDock.h"
#include "MotionModelToolbar.h"
#include "ResourceDock.h"
#include "ObjectViewer.h"
#include "TextureVisitor.h"

#include <dtQt/nodetreepanel.h>
#include <dtQt/osggraphicswindowqt.h>
#include <dtQt/osgwriteroptionsdialog.h>
#include <dtQt/projectcontextdialog.h>
#include <dtQt/propertypanel.h>
#include <dtQt/propertypanelbuilder.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/xercesparser.h>
#include <dtCore/datatype.h>
#include <dtCore/deltawin.h>
#include <dtCore/map.h>
#include <dtCore/osgpropertycontainers.h>
#include <dtCore/project.h>
#include <dtCore/shaderprogram.h>
//#include <dtCore/shaderpropertycontainer.h>
//#include <dtUtil/nodeactions.h>

#include <QtWidgets/QMenuBar>
#include <QtWidgets/QAction>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QHBoxLayout>

#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtCore/QMimeData>
#include <QtGui/QDragEnterEvent>
#include <QtOpenGL/QGLWidget>

#include <osgDB/FileNameUtils>
#include <osgDB/Registry>
#include <osgDB/WriteFile>
#include <cassert>

////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////////////
static const QString ORGINIZATION("delta3d");
static const QString APP_TITLE("Object Viewer");

static const QString GROUP_VIEWS("Views");
static const QString SETTING_ANIMATION_DOCK_VIS("AnimControlDock.Visible");
static const QString SETTING_NODEGRAPH_DOCK_VIS("NodeGraphDock.Visible");
static const QString SETTING_NODETOOLS_DOCK_VIS("NodeToolsDock.Visible");
static const QString SETTING_PROPERTIES_DOCK_VIS("PropertiesDock.Visible");
static const QString SETTING_RESOURCE_DOCK_VIS("ResourceDock.Visible");
static const QString SETTING_STATESET_DOCK_VIS("StateSetDock.Visible");



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
ObjectWorkspace::ObjectWorkspace()
  : mLoadShaderDefAction(nullptr)
  , mLoadGeometryAction(nullptr)
  , mSaveAsAction(nullptr)
  , mChangeContextAction(nullptr)
  , mExitAct(nullptr)
{
   resize(1024, 768);

   //mUndoSystem = new dtCore::UndoSystem;

   mResourceDock = new ResourceDock;
   addDockWidget(Qt::LeftDockWidgetArea, mResourceDock);

   mAnimationControlDock = new AnimationControlDock;
   addDockWidget(Qt::RightDockWidgetArea, mAnimationControlDock);

   mNodeToolsDock = new QDockWidget;
   addDockWidget(Qt::LeftDockWidgetArea, mNodeToolsDock);

   // Additional dock object setup
   mNodeTree = new dtQt::NodeTreePanel();
   mNodeToolsDock->setWidget(mNodeTree);

   mNodeGraphDock = new QDockWidget;
   mNodeGraphDock->setWindowTitle("Node Graph");
   addDockWidget(Qt::BottomDockWidgetArea, mNodeGraphDock);
   mNodeGraph = new dtQt::NodeGraphViewerPanel;
   mNodeGraphDock->setWidget(mNodeGraph);

   // Properties dock setup
   mPropertyPanel = new dtQt::PropertyPanel;
   mPropertiesDock = new QDockWidget;
   mPropertiesDock->setWindowTitle("Node Properties");
   mPropertiesDock->setWidget(mPropertyPanel);
   addDockWidget(Qt::RightDockWidgetArea, mPropertiesDock);

   mStateSetPanel = new dtQt::StateSetPanel;
   mStateSetDock = new QDockWidget;
   mStateSetDock->setWindowTitle("Node StateSet");
   mStateSetDock->setWidget(mStateSetPanel);
   addDockWidget(Qt::RightDockWidgetArea, mStateSetDock);

   connect(mNodeTree, SIGNAL(SignalNodeSelected(OsgNodePtr)),
      this, SLOT(OnNodeSelected(OsgNodePtr)));

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
void ObjectWorkspace::closeEvent(QCloseEvent* eventClose)
{
   OnShutdown();
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::CreateMenus()
{
   QMenu* windowMenu   = menuBar()->addMenu("&File");
   //QMenu* editMenu     = menuBar()->addMenu("&Edit");
   QMenu* viewMenu     = menuBar()->addMenu("&View");
   QMenu* settingsMenu = menuBar()->addMenu("&Settings");

   QMenu* toolBarMenu = viewMenu->addMenu("&Toolbars");

   windowMenu->addAction(mLoadShaderDefAction);
   windowMenu->addAction(mLoadGeometryAction);
   windowMenu->addAction(mSaveAsAction);
   windowMenu->addAction(mChangeContextAction);

   //editMenu->addAction(mUndo);
   //editMenu->addAction(mRedo);

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
   viewMenu->addAction(mToggleDockNodeGraph);
   viewMenu->addAction(mToggleDockNodeTools);
   viewMenu->addAction(mToggleDockProperties);
   viewMenu->addAction(mToggleDockStateSet);
   
   connect(mToggleDockAnimationControl, SIGNAL(toggled(bool)), mAnimationControlDock, SLOT(setVisible(bool)));
   connect(mAnimationControlDock, SIGNAL(visibilityChanged(bool)), mToggleDockAnimationControl, SLOT(setChecked(bool)));

   connect(mToggleDockResources, SIGNAL(toggled(bool)), mResourceDock, SLOT(setVisible(bool)));
   connect(mResourceDock, SIGNAL(visibilityChanged(bool)), mToggleDockResources, SLOT(setChecked(bool)));

   connect(mToggleDockNodeTools, SIGNAL(toggled(bool)), mNodeToolsDock, SLOT(setVisible(bool)));
   connect(mNodeToolsDock, SIGNAL(visibilityChanged(bool)), mToggleDockNodeTools, SLOT(setChecked(bool)));

   connect(mToggleDockNodeGraph, SIGNAL(toggled(bool)), mNodeGraphDock, SLOT(setVisible(bool)));
   connect(mNodeGraphDock, SIGNAL(visibilityChanged(bool)), mToggleDockNodeGraph, SLOT(setChecked(bool)));

   connect(mToggleDockProperties, SIGNAL(toggled(bool)), mPropertiesDock, SLOT(setVisible(bool)));
   connect(mPropertiesDock, SIGNAL(visibilityChanged(bool)), mToggleDockProperties, SLOT(setChecked(bool)));

   connect(mToggleDockStateSet, SIGNAL(toggled(bool)), mStateSetDock, SLOT(setVisible(bool)));
   connect(mStateSetDock, SIGNAL(visibilityChanged(bool)), mToggleDockStateSet, SLOT(setChecked(bool)));
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

   //mUndo = new QAction(tr("Undo"), this);
   //connect(mUndo, SIGNAL(triggered()), this, SLOT(OnUndo()));

   //mRedo = new QAction(tr("Redo"), this);
   //connect(mRedo, SIGNAL(triggered()), this, SLOT(OnRedo()));

   mSaveAsAction = new QAction(tr("&Save As..."), this);
   mSaveAsAction->setStatusTip(tr("Save a model as a different file name or format."));
   mSaveAsAction->setEnabled(false); // Should only be enabled when there is something available to save.
   connect(mSaveAsAction, SIGNAL(triggered()), this, SLOT(OnSaveAs()));

   mChangeContextAction = new QAction(tr("Change Project..."), this);
   mChangeContextAction->setStatusTip(tr("Change the project context directory."));
   connect(mChangeContextAction, SIGNAL(triggered()), this, SLOT(OnChangeContext()));
   
   mToggleDockAnimationControl = new QAction(tr("Anim Controls"), this);
   mToggleDockAnimationControl->setCheckable(true);
   mToggleDockAnimationControl->setChecked(true);

   mToggleDockResources = new QAction(tr("Resources"), this);
   mToggleDockResources->setCheckable(true);
   mToggleDockResources->setChecked(true);

   mToggleDockNodeGraph = new QAction(tr("Node Graph"), this);
   mToggleDockNodeGraph->setCheckable(true);
   mToggleDockNodeGraph->setChecked(true);

   mToggleDockNodeTools = new QAction(tr("Node Tools"), this);
   mToggleDockNodeTools->setCheckable(true);
   mToggleDockNodeTools->setChecked(true);

   mToggleDockProperties = new QAction(tr("Properties"), this);
   mToggleDockProperties->setCheckable(true);
   mToggleDockProperties->setChecked(true);

   mToggleDockStateSet = new QAction(tr("StateSet"), this);
   mToggleDockStateSet->setCheckable(true);
   mToggleDockStateSet->setChecked(true);
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

   // Add special toolbars
   mMotionModelToolbar = new MotionModelToolbar;
   addToolBar(mMotionModelToolbar);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnInitialization()
{
   QSettings settings(ORGINIZATION, APP_TITLE);

   LoadSettings();

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
void ObjectWorkspace::OnShutdown()
{
   SaveSettings();
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
      nameFilters << "*.dtshader" << "*.xml";

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
      mContextPath.c_str(), tr("Shaders (*.dtshader)") + ";;" + tr("Shaders (*.xml)"));

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
      if (geometryItem != nullptr)
      {
         mResourceDock->RemoveGeometryItem(geometryItem);
         geometryItem = mResourceDock->FindGeometryItem(fullName);
      }

      // Only reload the item if it does not exist.
      if (geometryItem == nullptr)
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

      OnGeometryChanged();
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnSaveAs()
{
   // Build the supported 3D format list.
   std::ostringstream filterStr;
   filterStr << "OpenSceneGraph(*.osg *.ive);;Object(*.obj);;OpenFlight (*.flt);;3DStudio (*.3ds)";
   osgDB::Registry* reg = osgDB::Registry::instance();
   
   if (reg->getReaderWriterForExtension("dae"))
   {
      filterStr << ";;Collada (*.dae)";
   }

   if (reg->getReaderWriterForExtension("fbx"))
   {
      filterStr << ";;Autodesk FBX (*.fbx)";
   }

   // Determine the target file path.
   QString filename = QFileDialog::getSaveFileName(this,
      tr("Save As File"),
      mContextPath.c_str(),
      tr(filterStr.str().c_str()));

   std::string ext = osgDB::getFileExtension(filename.toStdString());

   QString statusMessage;

   if ( ! filename.isEmpty())
   {
      dtCore::Object* obj = mViewer->GetDeltaObject();
      if (obj != nullptr && obj->GetOSGNode() != nullptr)
      {
         // Ask the user for export options.
         dtQt::OsgWriterOptionsDialog osgDialog;
         osgDialog.GetWriterOptionsPanel().SetBinaryMode(ext == "ive" || ext == "osgb");
         if (osgDialog.exec() == QDialog::Accepted)
         {
            dtCore::RefPtr<osg::Node> model = obj->GetOSGNode();

            if (osgDialog.IsOptimizerChecked())
            {
               osgUtil::Optimizer::OptimizationOptions options = osgDialog.GetOptimizerOptionsPanel().GetOptions();

               model = OptimizeModel(*model, options, true);
            }

            // Access the options that the user selected.
            dtQt::OsgWriterOptionsPanel& panel = osgDialog.GetWriterOptionsPanel();
            dtCore::RefPtr<osgDB::Options> options = panel.GetOptions();

            SaveModel(*model, *options, panel.GetTextureCompressionOption(), filename.toStdString());
         }
      }
      else
      {
         QString msg = "No object available. Could not save file: " + filename;
         QMessageBox::warning(this, tr("No Object to Save"), msg);
      }
   }
   else
   {
      statusMessage = QString(tr("Unable to save file: %1")).arg(filename);
   }

   statusBar()->showMessage(statusMessage, 2000);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnGeometryChanged()
{
   osg::Node* node = nullptr;
   
   bool objectValid = mViewer->GetDeltaObject() != nullptr;
   if (objectValid)
   {
      node = mViewer->GetDeltaObject()->GetOSGNode();
   }

   mNodeTree->SetNode(node);

   if (node == nullptr)
   {
      mNodeGraph->GetNodeGraphView().GetNodeGraphScene()->ClearNodeItems();
   }
   else
   {
      dtQt::BaseNodeWrapperArray nodeWrappers;
      osg::Node* node = mViewer->GetDeltaObject()->GetOSGNode();
      nodeWrappers.push_back(new dtQt::OsgNodeWrapper(*node));
      mNodeGraph->GetNodeGraphView().GetNodeGraphScene()->AddNodes(nodeWrappers, true);
   }

   mSaveAsAction->setEnabled(objectValid);
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

      mNodeTree->SetNode(nullptr);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnNodeSelected(OsgNodePtr node)
{
   osg::StateSet* ss = node.valid() ? node->getStateSet() : nullptr;
   mStateSetPanel->SetStateSet(ss);

   mPropertyPanel->Clear();

   // Update the properties panel for the selected node.
   if (node.valid())
   {
      dtCore::RefPtr<dtCore::OsgNodePropertiesBuilder> propBuilder = new dtCore::OsgNodePropertiesBuilder;
      mProperties = propBuilder->CreatePropertiesForNode(*node);

      if (mProperties.valid())
      {
         dtCore::RefPtr<dtQt::PropertyPanelBuilder> builder = new dtQt::PropertyPanelBuilder;
         builder->AddPropertiesToPanel(*mProperties, *mPropertyPanel);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnToggleGridClicked(bool toggledOn)
{
   emit ToggleGrid(toggledOn);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnUndo()
{
   //mUndoSystem->Undo();

   UpdateUndoRedoActions();
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnRedo()
{
   //mUndoSystem->Redo();

   UpdateUndoRedoActions();
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::UpdateUndoRedoActions()
{
   /*dtUtil::ActionCommandRefPtr undo = mUndoSystem->GetFrontUndo();
   mUndo->setEnabled(undo.valid());
   if (undo.valid())
   {
      std::string text = "Undo " + undo->GetType().GetDisplayName();
      mUndo->setText(text.c_str());
   }
   else
   {
      mUndo->setText("Undo");
   }

   dtUtil::ActionCommandRefPtr redo = mUndoSystem->GetFrontRedo();
   mRedo->setEnabled(redo.valid());
   if (redo.valid())
   {
      std::string text = "Redo " + redo->GetType().GetDisplayName();
      mRedo->setText(text.c_str());
   }
   else
   {
      mRedo->setText("Redo");
   }*/
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
   QSettings settings(ORGINIZATION, APP_TITLE);

   try
   {
      dtCore::Project::GetInstance().SetContext(mContextPath);

      // Update the registry entry based on the current valid context
      settings.setValue("projectContextPath", mContextPath.c_str());
      settings.sync();
   }
   catch (const dtUtil::Exception& e)
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
   QSettings settings(ORGINIZATION, APP_TITLE);

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

   if (osgGraphWindow == nullptr)
   {
      LOG_ERROR("Unable to initialize. The deltawin could not be created with a QGLWidget.");
      return;
   }

   //stuff the QGLWidget into it's parent widget placeholder and ensure it gets
   //resized to fit the parent
   QWidget* widget = osgGraphWindow->GetQGLWidget();
   if (widget != nullptr)
   {
      widget->setGeometry(centralWidget()->geometry());
      mCentralLayout->addWidget(widget);
   }

   SetupConnectionsWithViewer();
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnShaderApplied(ShaderProgramPtr shaderProgram)
{
   // Show properties on property panel. To be committed soon...
   if (shaderProgram.valid())
   {
      /*dtCore::RefPtr<dtCore::ShaderPropertyContainer> shaderProps = new dtCore::ShaderPropertyContainer;
      shaderProps->SetShaderProgram(shaderProgram);
      mProperties = shaderProps.get();

      mPropertyPanel->Clear();

      dtCore::RefPtr<dtQt::PropertyPanelBuilder> propBuilder = new dtQt::PropertyPanelBuilder;
      propBuilder->AddPropertiesToPanel(*mProperties, *mPropertyPanel);*/
   }
}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Node> ObjectWorkspace::OptimizeModel(osg::Node& model, osgUtil::Optimizer::OptimizationOptions options, bool optimizeCopy)
{
   dtCore::RefPtr<osg::Node> resultModel = &model;

   if (optimizeCopy)
   {
      resultModel = dynamic_cast<osg::Node*>(model.clone(osg::CopyOp::DEEP_COPY_ALL));

      if ( ! resultModel.valid())
      {
         std::string msg = "Failed to clone and optimize the model: " + model.getName();
         QMessageBox::warning(this, tr("Optimize Failed"), tr(msg.c_str()));

         // Send back the original model.
         return &model;
      }
   }

   osgUtil::Optimizer optimizer;
   optimizer.optimize(resultModel.get(), options);

   return resultModel;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::SaveModel(osg::Node& model, osgDB::Options& options, osg::Texture::InternalFormatMode textureMode, const std::string& filepath)
{
   osgDB::Registry::instance()->setOptions(&options);

   // Change the currnt directory so that writing of external texture files
   // are written to a proper location relative to the target model file.
   std::string currentDir = osgDB::getCurrentWorkingDirectory();
   std::string targetDir = osgDB::getFilePath(filepath);
   osgDB::setCurrentWorkingDirectory(targetDir);

   // Determine if the textures should be compressed.
   /*if (textureMode != osg::Texture::USE_IMAGE_DATA_FORMAT)
   {
      dtCore::RefPtr<CompressTexturesVisitor> visitor = new CompressTexturesVisitor(textureMode);
      model.accept(*visitor);
      visitor->compress();

      std::string ext = osgDB::getFileExtension(filepath);
      if (ext != "ive" || (options.getOptionString().find("noTexturesInIVEFile") != std::string::npos))
      {
         visitor->write(osgDB::getFilePath(filepath));
      }
   }*/

   // Optimize the model to not keep image data after creating textures.
   // The unref flag is saved to file, so this step is important for optimization.
   dtCore::RefPtr<TextureVisitor> visitor = new TextureVisitor;
   model.accept(*visitor);
   visitor->SetUnRefImageData(true);

   // Write the file.
   if ( ! osgDB::writeNodeFile(model, filepath, &options))
   {
      std::string msg = "Failed writing file: " + filepath;
      QMessageBox::warning(this, tr("Save Failed"), tr(msg.c_str()));
   }

   // Set textures back to keep image data for the sake of the editor for
   // subsequent image writing. If not, image data will go NULL an not
   // allow textures to be written along with their model.
   visitor->SetUnRefImageData(false);

   // Restore the original current directory.
   osgDB::setCurrentWorkingDirectory(currentDir);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::SetupConnectionsWithViewer()
{
   // Main Window Events
   this->
   // Menu connections
   connect(this, SIGNAL(LoadShaderDefinition(const QString&)),
      mViewer, SLOT(OnLoadShaderFile(const QString&)));

   connect((QObject*)this, SIGNAL(SetGenerateTangentAttribute(bool)),
      mViewer, SLOT(OnSetGenerateTangentAttribute(bool)));

   // Resource dock connections
   QObject* resObj = GetResourceObject();

   connect(this, SIGNAL(ReloadShaderDefinition()),
      resObj, SLOT(OnReloadShaderFiles()));

   connect(resObj, SIGNAL(LoadGeometry(const std::string&)),
      mViewer, SLOT(OnLoadGeometryFile(const std::string&)));

   connect(resObj, SIGNAL(LoadMap(const std::string&)),
      mViewer, SLOT(OnLoadMapFile(const std::string&)));

   connect(resObj, SIGNAL(UnloadGeometry()),
      mViewer, SLOT(OnUnloadGeometryFile()));

   connect(mViewer, SIGNAL(ShaderLoaded(const std::string&, const std::string&, const std::string&)),
      resObj, SLOT(OnNewShader(const std::string&, const std::string&, const std::string&)));

   connect(resObj, SIGNAL(ApplyShader(const std::string&, const std::string&)),
      mViewer, SLOT(OnApplyShader(const std::string&, const std::string&)));

   connect(mViewer, SIGNAL(LightUpdate(const LightInfo&)),
      resObj, SLOT(OnLightUpdate(const LightInfo&)));

   connect(resObj, SIGNAL(RemoveShader()),
      mViewer, SLOT(OnRemoveShader()));

   connect(resObj, SIGNAL(FixLights()),
      mViewer, SLOT(OnFixLights()));

   connect(resObj, SIGNAL(SetLightEnabled(int, bool)),
      mViewer, SLOT(OnSetLightEnabled(int, bool)));

   connect(resObj, SIGNAL(SetLightType(int, int)),
      mViewer, SLOT(OnSetLightType(int, int)));

   connect(resObj, SIGNAL(SetLightPosition(int, const osg::Vec3&)),
      mViewer, SLOT(OnSetLightPosition(int, const osg::Vec3&)));

   connect(resObj, SIGNAL(SetLightRotation(int, const osg::Vec3&)),
      mViewer, SLOT(OnSetLightRotation(int, const osg::Vec3&)));

   connect(resObj, SIGNAL(SetAmbient(int, const osg::Vec4&)),
      mViewer, SLOT(OnSetAmbient(int, const osg::Vec4&)));

   connect(resObj, SIGNAL(SetDiffuse(int, const osg::Vec4&)),
      mViewer, SLOT(OnSetDiffuse(int, const osg::Vec4&)));

   connect(resObj, SIGNAL(SetSpecular(int, const osg::Vec4&)),
      mViewer, SLOT(OnSetSpecular(int, const osg::Vec4&)));

   connect(resObj, SIGNAL(SetLightCutoff(int, float)),
      mViewer, SLOT(OnSetLightCutoff(int, float)));

   connect(resObj, SIGNAL(SetLightExponent(int, float)),
      mViewer, SLOT(OnSetLightExponent(int, float)));

   connect(resObj, SIGNAL(SetLightConstant(int, float)),
      mViewer, SLOT(OnSetLightConstant(int, float)));

   connect(resObj, SIGNAL(SetLightLinear(int, float)),
      mViewer, SLOT(OnSetLightLinear(int, float)));

   connect(resObj, SIGNAL(SetLightQuadratic(int, float)),
      mViewer, SLOT(OnSetLightQuadratic(int, float)));

   connect(this, SIGNAL(ToggleGrid(bool)), mViewer, SLOT(OnToggleGrid(bool)));
   connect(mStatisticsAction, SIGNAL(triggered()), mViewer, SLOT(OnNextStatistics()));

   connect(resObj, SIGNAL(ToggleVertexShaderSources(bool)),
      this, SLOT(OnToggleVertexShaderSource(bool)));

   connect(resObj, SIGNAL(ToggleGeometryShaderSources(bool)),
      this, SLOT(OnToggleGeometryShaderSource(bool)));

   connect(resObj, SIGNAL(ToggleFragmentShaderSources(bool)),
      this, SLOT(OnToggleFragmentShaderSource(bool)));

   connect(resObj, SIGNAL(RemoveShaderDef(const std::string&)),
      this, SLOT(OnRemoveShaderDef(const std::string&)));

   connect(mViewer, SIGNAL(SignalShaderApplied(ShaderProgramPtr)),
      this, SLOT(OnShaderApplied(ShaderProgramPtr)));

   // Node Tree Panel
   connect(resObj, SIGNAL(LoadGeometry(const std::string&)),
      this, SLOT(OnGeometryChanged()));

   connect(resObj, SIGNAL(UnloadGeometry()),
      this, SLOT(OnGeometryChanged()));

   connect(mNodeTree, SIGNAL(SignalNodesSelected(OsgNodePtrVector)),
      mViewer, SLOT(OnNodesSelected(OsgNodePtrVector)));

   // Toolbar connections
   connect((QObject*)this->mShadedAction, SIGNAL(triggered()), mViewer, SLOT(OnSetShaded()));
   connect((QObject*)this->mWireframeAction, SIGNAL(triggered()), mViewer, SLOT(OnSetWireframe()));
   connect((QObject*)this->mShadedWireAction, SIGNAL(triggered()), mViewer, SLOT(OnSetShadedWireframe()));
   connect(mMotionModelToolbar, SIGNAL(SignalMotionModelSelected(MotionModelTypeE)),
      mViewer, SLOT(OnMotionModelSelected(MotionModelTypeE)));
   connect(mMotionModelToolbar, SIGNAL(SignalMotionModelSpeedChanged(MotionModelTypeE, float)),
      mViewer, SLOT(OnMotionModelSpeedChanged(MotionModelTypeE, float)));

   // Editing connections
   connect((QObject*)this->mWorldSpaceAction, SIGNAL(triggered()), mViewer, SLOT(OnWorldSpaceMode()));
   connect((QObject*)this->mLocalSpaceAction, SIGNAL(triggered()), mViewer, SLOT(OnLocalSpaceMode()));
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::LoadSettings()
{
   QSettings settings(ORGINIZATION, APP_TITLE);

   if ( ! settings.contains(SETTING_NODETOOLS_DOCK_VIS))
   {
      return;
   }

   settings.beginGroup(GROUP_VIEWS);
   mAnimationControlDock->setVisible(settings.value(SETTING_ANIMATION_DOCK_VIS, false).toBool());
   mNodeGraphDock->setVisible(settings.value(SETTING_NODEGRAPH_DOCK_VIS, true).toBool());
   mNodeToolsDock->setVisible(settings.value(SETTING_NODETOOLS_DOCK_VIS, true).toBool());
   mPropertiesDock->setVisible(settings.value(SETTING_PROPERTIES_DOCK_VIS, true).toBool());
   mStateSetDock->setVisible(settings.value(SETTING_STATESET_DOCK_VIS, true).toBool());
   mResourceDock->setVisible(settings.value(SETTING_RESOURCE_DOCK_VIS, true).toBool());
   settings.endGroup();
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::SaveSettings()
{
   QSettings settings(ORGINIZATION, APP_TITLE);

   settings.beginGroup(GROUP_VIEWS);
   settings.setValue(SETTING_ANIMATION_DOCK_VIS, mAnimationControlDock->isVisible());
   settings.setValue(SETTING_NODEGRAPH_DOCK_VIS, mNodeGraphDock->isVisible());
   settings.setValue(SETTING_NODETOOLS_DOCK_VIS, mNodeToolsDock->isVisible());
   settings.setValue(SETTING_PROPERTIES_DOCK_VIS, mPropertiesDock->isVisible());
   settings.setValue(SETTING_STATESET_DOCK_VIS, mStateSetDock->isVisible());
   settings.setValue(SETTING_RESOURCE_DOCK_VIS, mResourceDock->isVisible());
   settings.endGroup();
}


