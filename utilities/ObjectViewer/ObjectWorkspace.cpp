#include "ObjectWorkspace.h"
#include "OSGAdapterWidget.h"
#include "DialogProjectContext.h"
#include "ResourceDock.h"

#include <osg/Geode> ///needed for the node builder
#include <dtAnim/cal3ddatabase.h>
#include <dtAnim/animnodebuilder.h>
#include <dtAnim/chardrawable.h>

#include <dtUtil/fileutils.h>
#include <dtDAL/project.h>

#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QtGui/QFileDialog>
#include <QtGui/QTableWidget>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QDockWidget>
#include <QtGui/QMessageBox>
#include <QtGui/QHBoxLayout>

#include <QtGui/QStandardItemModel>
#include <QtGui/QStandardItem>

#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QUrl>

#include <assert.h>

////////////////////////////////////////////////////////////////////////////////
ObjectWorkspace::ObjectWorkspace()
  : mLoadShaderDefAction(NULL)
  , mLoadGeometryAction(NULL)
  , mExitAct(NULL)
  , mGLWidget(NULL)
{
   resize(1024, 768);

   mResourceDock = new ResourceDock;
   addDockWidget(Qt::LeftDockWidgetArea, mResourceDock); 

   // Create all program actions
   CreateFileMenuActions();
   CreateModeToolbarActions();
   CreateDisplayToolbarActions();
   CreateShaderToolbarActions();

   CreateMenus();
   statusBar();
   CreateToolbars();     

   QWidget* glParent = new QWidget(this);

   mGLWidget = new dtQt::OSGAdapterWidget(false, this);

   QHBoxLayout* hbLayout = new QHBoxLayout(glParent);
   hbLayout->setMargin(0);
   glParent->setLayout(hbLayout);
   hbLayout->addWidget(mGLWidget);
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
   QMenu* windowMenu = menuBar()->addMenu("&File");
   QMenu* viewMenu   = menuBar()->addMenu("&View");
   QMenu* toolBarMenu = viewMenu->addMenu("&Toolbars");   

   windowMenu->addAction(mLoadShaderDefAction);
   windowMenu->addAction(mLoadGeometryAction);
   windowMenu->addAction(mChangeContextAction);

   QAction* toggleShadeToolbarAction = toolBarMenu->addAction("Shading toolbar");

   toggleShadeToolbarAction->setCheckable(true);
   toggleShadeToolbarAction->setChecked(true);

   connect(toggleShadeToolbarAction, SIGNAL(triggered()), this, SLOT(OnToggleShadingToolbar()));  

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
   QSettings settings("MOVES", "Shader Viewer");
   QStringList files = settings.value("projectContextPath").toStringList();

   // If the user has not selected a project context, have them do so now
   if (files.empty())
   {
      mContextPath = GetContextPathFromUser();
   }
   else
   {
      mContextPath = files.at(0).toStdString();
   }

   SaveCurrentContextPath();   
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
void ObjectWorkspace::OnRecompileClicked()
{
   emit ReloadShaderDefinition(mShaderDefinitionName);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::UpdateResourceLists()
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
         mShaderDefinitionName = QString("shaders/%1").arg(fileInfo.fileName());
         emit LoadShaderDefinition(mShaderDefinitionName);
      }

      directory.cdUp();
   }

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
      }      
   }
   else
   {
      statusMessage = QString(tr("Unable to load file: %1")).arg(filename);
   }

    statusBar()->showMessage(statusMessage, 2000);
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
   ProjectContextDialog dialog(this);

   if (dialog.exec() == QDialog::Accepted)
   {
      return(dialog.getProjectPath().toStdString());
   }  
   
   return std::string();
}

////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::SaveCurrentContextPath()
{
   QSettings settings("MOVES", "Shader Viewer");

   try
   {      
      dtDAL::Project::GetInstance().SetContext(mContextPath);     

      settings.setValue("projectContextPath", mContextPath.c_str());
      settings.sync();
   }
   catch (const dtUtil::Exception &e)
   {
      QMessageBox::critical((QWidget *)this, tr("Error"), tr(e.What().c_str()), tr("Ok"));
   }
}
