#include "ObjectWorkspace.h"
#include <dtQt/osggraphicswindowqt.h>
#include <dtQt/projectcontextdialog.h>
#include "ResourceDock.h"
#include "ObjectViewer.h"

#include <dtUtil/fileutils.h>
#include <dtDAL/project.h>
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

   // Create all program actions
   CreateFileMenuActions();
   CreateModeToolbarActions();
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
void ObjectWorkspace::dragEnterEvent(QDragEnterEvent *event)
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
      OnLoadGeometry(filename.toStdString());
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
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::CreateModeToolbarActions()
{
   QIcon lightModeIcon(":/images/lightMode.png");
   QIcon objectModeIcon(":/images/objectMode.png");

   QActionGroup *modeGroup = new QActionGroup(this);
   modeGroup->setExclusive(true);

   mObjectModeAction = modeGroup->addAction(objectModeIcon, "Object Mode");
   mLightModeAction  = modeGroup->addAction(lightModeIcon, "Light Mode");

   mObjectModeAction->setCheckable(true);
   mObjectModeAction->setChecked(true);

   mLightModeAction->setCheckable(true);
   //mLightModeAction->setDisabled(true);
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

   mWireframeAction  = actionGroup->addAction(wireframeIcon, "Wireframe");
   mShadedAction     = actionGroup->addAction(shadedIcon, "Shaded");
   mShadedWireAction = actionGroup->addAction(shadedWireIcon, "Shaded Wireframe");

   mGridAction = new QAction(gridIcon, "Toggle Grid", this);
   connect(mGridAction, SIGNAL(toggled(bool)), this, SLOT(OnToggleGridClicked(bool)));

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
   QIcon fragmentSourceIcon(":/images/fragmentShaderSource.png");

   mRecompileAction = new QAction(compileIcon, tr("Recompile Shaders"), this);
   mOpenVertexShaderAction = new QAction(vertexSourceIcon, tr("Open Current Vertex Shader"), this);
   mOpenFragmentShaderAction = new QAction(fragmentSourceIcon, tr("Open Current Fragment Shader"), this);

   mOpenVertexShaderAction->setEnabled(false);
   mOpenFragmentShaderAction->setEnabled(false);

   connect(mRecompileAction, SIGNAL(triggered()), this, SLOT(OnRecompileClicked()));

   connect(mOpenVertexShaderAction, SIGNAL(triggered()),
           mResourceDock, SLOT(OnOpenCurrentVertexShaderSources()));

   connect(mOpenFragmentShaderAction, SIGNAL(triggered()),
           mResourceDock, SLOT(OnOpenCurrentFragmentShaderSources()));
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::CreateToolbars()
{
   mModeToolbar = addToolBar("Mode toolbar");
   mModeToolbar->addAction(mObjectModeAction);
   mModeToolbar->addAction(mLightModeAction);

   mCoordinateToolbar = addToolBar("Coordinate Toolbar");
   mCoordinateToolbar->addAction(mLocalSpaceAction);
   mCoordinateToolbar->addAction(mWorldSpaceAction);

   mDisplayToolbar = addToolBar("Display toolbar");
   mDisplayToolbar->addAction(mWireframeAction);
   mDisplayToolbar->addAction(mShadedAction);
   mDisplayToolbar->addAction(mShadedWireAction);
   mDisplayToolbar->addSeparator();
   mDisplayToolbar->addAction(mGridAction);

   mShaderToolbar = addToolBar("Shader toolbar");
   mShaderToolbar->addAction(mOpenVertexShaderAction);
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
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnToggleFragmentShaderSource(bool enabled)
{
   mOpenFragmentShaderAction->setEnabled(enabled);
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
   if (dtDAL::Project::GetInstance().IsContextValid())
   {
      assert(!mContextPath.empty());

      QDir directory(mContextPath.c_str());

      if (directory.cd(QString(mContextPath.c_str()) + "/shaders"))
      {
         QStringList nameFilters;
         nameFilters << "*.xml";

         QFileInfoList fileList = directory.entryInfoList(nameFilters, QDir::Files);

         // Try to load all definitions
         while (!fileList.empty())
         {
            QFileInfo fileInfo = fileList.takeFirst();
            mShaderDefinitionName = QString("%1/shaders/%2").arg(QString(mContextPath.c_str()), fileInfo.fileName());
            emit LoadShaderDefinition(mShaderDefinitionName);
         }

         directory.cdUp();
      }

      // Now load all the additional shader files in the shader lists.
      for (int shaderIndex = 0; shaderIndex < mAdditionalShaderFiles.size(); shaderIndex++)
      {
         emit LoadShaderDefinition(mAdditionalShaderFiles.at(shaderIndex).c_str());
      }

      // Populate the map list.
      QStringList mapList;
      std::set<std::string> mapNames = dtDAL::Project::GetInstance().GetMapNames();
      for (std::set<std::string>::iterator map = mapNames.begin(); map != mapNames.end(); map++)
      {
         mapList << map->c_str();
      }

      for (int mapIndex = 0; mapIndex < mapList.size(); mapIndex++)
      {
         mResourceDock->OnNewMap(mapList.at(mapIndex).toStdString());
      }

      // Populate the object list.
      QString staticMeshDir = QString(mContextPath.c_str()) + "/staticmeshes";

      if (directory.cd(staticMeshDir))
      {
         QStringList nameFilters;
         nameFilters << "*.ive" << "*.osg";

         QFileInfoList fileList = directory.entryInfoList(nameFilters, QDir::Files);

         while (!fileList.empty())
         {
            QFileInfo fileInfo = fileList.takeFirst();
            mResourceDock->OnNewGeometry(staticMeshDir.toStdString(), fileInfo.fileName().toStdString());
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnLoadShaderDefinition()
{
   QString filename = QFileDialog::getOpenFileName(this,
                                                   tr("Load Shader Definition File"),
                                                   mContextPath.c_str(),
                                                   tr("Shaders(*.xml)") );

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
                                                   tr("Geometry(*.osg *.ive *.flt *.3ds *.txp *.xml *)") );

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
void ObjectWorkspace::OnLoadGeometry(const std::string &fullName)
{
   if (dtUtil::FileUtils::GetInstance().FileExists(fullName))
   {
      QFileInfo fileInfo(fullName.c_str());
      QTreeWidgetItem *geometryItem = mResourceDock->FindGeometryItem(fullName);

      // Only reload the item if it has not already been loaded
      if (!geometryItem)
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
      dtDAL::Project::GetInstance().SetContext(mContextPath);

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

   connect(this->GetResourceObject(), SIGNAL(SetCurrentLight(int)),
      mViewer, SLOT(OnSetCurrentLight(int)));

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

   connect(this->GetResourceObject(), SIGNAL(ToggleVertexShaderSources(bool)),
      this, SLOT(OnToggleVertexShaderSource(bool)));

   connect(this->GetResourceObject(), SIGNAL(ToggleFragmentShaderSources(bool)),
      this, SLOT(OnToggleFragmentShaderSource(bool)));

   connect(this->GetResourceObject(), SIGNAL(RemoveShaderDef(const std::string&)),
      this, SLOT(OnRemoveShaderDef(const std::string&)));

   // Toolbar connections
   connect((QObject*)this->mShadedAction, SIGNAL(triggered()), mViewer, SLOT(OnSetShaded()));
   connect((QObject*)this->mWireframeAction, SIGNAL(triggered()), mViewer, SLOT(OnSetWireframe()));
   connect((QObject*)this->mShadedWireAction, SIGNAL(triggered()), mViewer, SLOT(OnSetShadedWireframe()));
   connect((QObject*)this->mLightModeAction, SIGNAL(triggered()), mViewer, SLOT(OnEnterLightMode()));
   connect((QObject*)this->mObjectModeAction, SIGNAL(triggered()), mViewer, SLOT(OnEnterObjectMode()));

   // Editing connections
   connect((QObject*)this->mWorldSpaceAction, SIGNAL(triggered()), mViewer, SLOT(OnWorldSpaceMode()));
   connect((QObject*)this->mLocalSpaceAction, SIGNAL(triggered()), mViewer, SLOT(OnLocalSpaceMode()));

}

