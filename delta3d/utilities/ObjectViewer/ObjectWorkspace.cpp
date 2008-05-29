#include "ObjectWorkspace.h"
#include "OSGAdapterWidget.h"
#include "DialogProjectContext.h"

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
#include <QtCore/QSettings>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QDockWidget>
#include <QtGui/QMessageBox>
#include <QtGui/QTabWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QHBoxLayout>
#include <QtGui/QTreeWidgetItem>

#include <QtGui/QStandardItemModel>
#include <QtGui/QStandardItem>

#include <QtGui/QGraphicsEllipseItem>
#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////
ObjectWorkspace::ObjectWorkspace()
  : mExitAct(NULL)
  , mLoadShaderDefAction(NULL)
  , mLoadGeometryAction(NULL)
  , mShaderTreeWidget(NULL)
  , mGLWidget(NULL)
  , mPoseDock(NULL)
{
   resize(1024, 768);

   dtAnim::AnimNodeBuilder& nodeBuilder = dtAnim::Cal3DDatabase::GetInstance().GetNodeBuilder();
   nodeBuilder.SetCreate(dtAnim::AnimNodeBuilder::CreateFunc(&nodeBuilder, &dtAnim::AnimNodeBuilder::CreateSoftware));

   mShaderTreeWidget = new QTreeWidget(this);
   connect(mShaderTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), 
           this, SLOT(OnShaderItemChanged(QTreeWidgetItem *, int)));
   //connect(mShaderListWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(OnMeshActivated(QListWidgetItem*)));

   CreateActions();
   CreateMenus();
   statusBar();
   CreateToolbars();  

   mTabs = new QTabWidget(this);
   mTabs->addTab(mShaderTreeWidget, tr("Shaders"));   

   QWidget* glParent = new QWidget(this);

   mGLWidget = new dtQt::OSGAdapterWidget(false, glParent);

   QHBoxLayout* hbLayout = new QHBoxLayout(glParent);
   hbLayout->setMargin(0);
   glParent->setLayout(hbLayout);
   hbLayout->addWidget(mGLWidget);
   setCentralWidget(glParent);

   QDockWidget* tabsDockWidget = new QDockWidget();
   tabsDockWidget->setWidget(mTabs);

   addDockWidget(Qt::RightDockWidgetArea, tabsDockWidget); 
}

/////////////////////////////////////////////////////////////////////////////////////////
ObjectWorkspace::~ObjectWorkspace()
{
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::CreateMenus()
{
   QMenu *windowMenu = menuBar()->addMenu("&File");
   QMenu *viewMenu   = menuBar()->addMenu("&View");
   QMenu *toolBarMenu = viewMenu->addMenu("&Toolbars");   

   windowMenu->addAction(mLoadShaderDefAction);
   windowMenu->addAction(mLoadGeometryAction);
   windowMenu->addAction(mChangeContextAction);

   QAction *toggleShadeToolbarAction = toolBarMenu->addAction("Shading toolbar");

   toggleShadeToolbarAction->setCheckable(true);
   toggleShadeToolbarAction->setChecked(true);

   connect(toggleShadeToolbarAction, SIGNAL(triggered()), this, SLOT(OnToggleShadingToolbar()));

   for (int i=0; i<5; ++i)
   {
      windowMenu->addAction(mRecentFilesAct[i]);
   }

   menuBar()->addSeparator();
   windowMenu->addAction(mExitAct);

   UpdateRecentFileActions();
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::CreateActions()
{
   mExitAct = new QAction(tr("E&xit"), this);
   mExitAct->setShortcut(tr("Ctrl+Q"));
   mExitAct->setStatusTip(tr("Exit the application"));
   connect(mExitAct, SIGNAL(triggered()), this, SLOT(close()));

   mLoadShaderDefAction = new QAction(tr("&Load Shader Definitions..."), this);
   //mLoadShaderDefAction->setShortcut(tr("Ctrl+O"));
   mLoadShaderDefAction->setStatusTip(tr("Open an existing shader definition file."));
   connect(mLoadShaderDefAction, SIGNAL(triggered()), this, SLOT(OnLoadShaderDefinition()));

   mLoadGeometryAction = new QAction(tr("Load Geometry..."), this);
   //mLoadGeometryAction->setShortcut(tr("Ctrl+O"));
   mLoadGeometryAction->setStatusTip(tr("Open an existing shader definition file."));
   connect(mLoadGeometryAction, SIGNAL(triggered()), this, SLOT(OnLoadGeometry()));   

   mChangeContextAction = new QAction(tr("Change Project..."), this);
   mChangeContextAction->setStatusTip(tr("Change the project context directory."));
   connect(mChangeContextAction, SIGNAL(triggered()), this, SLOT(OnChangeContext()));   

   for (int i=0; i<5; i++)
   {
      mRecentFilesAct[i] = new QAction(this);
      mRecentFilesAct[i]->setVisible(false);
      connect(mRecentFilesAct[i], SIGNAL(triggered()), this, SLOT(OpenRecentFile()));
   }

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

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::CreateToolbars()
{
   mShadingToolbar = addToolBar("Shading toolbar"); 
   mShadingToolbar->addAction(mWireframeAction);
   mShadingToolbar->addAction(mShadedAction);
   mShadingToolbar->addAction(mShadedWireAction);
   mShadingToolbar->addSeparator();
   mShadingToolbar->addAction(mGridAction);
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnOpenCharFile()
{
   QString filename = QFileDialog::getOpenFileName(this,
      tr("Load Character File"),
      ".",
      tr("Characters (*.xml)") );

   if (!filename.isEmpty())
   {
      LoadCharFile(filename);
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::LoadCharFile( const QString &filename )
{
   if (dtUtil::FileUtils::GetInstance().FileExists( filename.toStdString() ))
   {        
      emit FileToLoad( filename );

      SetCurrentFile(filename);

      statusBar()->showMessage(tr("File loaded"), 2000);
   }
   else
   {
      QString errorString = QString("File not found: %1").arg( filename );
      QMessageBox::warning( this, "Warning", errorString, "&Ok" );
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
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
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnNewShader(const std::string &shaderGroup, const std::string &shaderProgram)
{
   // We don't want this signal emitted when we're adding a shader
   disconnect(mShaderTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), 
              this, SLOT(OnShaderItemChanged(QTreeWidgetItem *, int)));

   QTreeWidgetItem *shaderItem = new QTreeWidgetItem();
   QTreeWidgetItem *programItem = new QTreeWidgetItem();

   shaderItem->setText(0, shaderGroup.c_str());
   programItem->setText(0, shaderProgram.c_str());
 
   //meshItem->setData( Qt::UserRole, meshID );

   shaderItem->setFlags(Qt::ItemIsSelectable |
                        Qt::ItemIsUserCheckable |
                        Qt::ItemIsEnabled);

   // The shader itself should have a checkbox
   programItem->setCheckState(0, Qt::Unchecked);

   mShaderTreeWidget->addTopLevelItem(shaderItem); 
   shaderItem->addChild(programItem);

   connect(mShaderTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), 
           this, SLOT(OnShaderItemChanged(QTreeWidgetItem *, int)));
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnToggleShadingToolbar()
{
   if (mShadingToolbar->isHidden())
   {
      mShadingToolbar->show();
   }
   else
   {
      mShadingToolbar->hide();
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::UpdateRecentFileActions()
{
   //QSettings settings("MOVES", "Shader Viewer");
   //QStringList files = settings.value("recentFileList").toStringList();

   //int numRecentFiles = qMin(files.size(), 5);

   //for (int i = 0; i < numRecentFiles; ++i) {
   //   QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName() );
   //   mRecentFilesAct[i]->setText(text);
   //   mRecentFilesAct[i]->setData(files[i]);
   //   mRecentFilesAct[i]->setVisible(true);
   //}
   //for (int j = numRecentFiles; j < 5; ++j)
   //   mRecentFilesAct[j]->setVisible(false);
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::SetCurrentFile( const QString &filename )
{
   //if (filename.isEmpty())
   //{
   //   setWindowTitle(tr("Shader Viewer"));
   //}
   //else
   //{
   //   setWindowTitle(tr("%1 - %2").arg(QFileInfo(filename).fileName()).arg(tr("Animation Viewer")));
   //}

   //QSettings settings("MOVES", "Shader Viewer");
   //QStringList files = settings.value("recentFileList").toStringList();
   //files.removeAll(filename);
   //files.prepend(filename);

   //while (files.size() > 5)
   //{
   //   files.removeLast();
   //}

   //settings.setValue("recentFileList", files);
   //UpdateRecentFileActions();
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OpenRecentFile()
{
   //QAction *action = qobject_cast<QAction*>(sender());

   //if (action)
   //{
   //   LoadCharFile( action->data().toString() );
   //}
}

/////////////////////////////////////////////////////////////////////////////////////////
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
         SetCurrentFile(filename);
         statusMessage = QString(tr("File Loaded"));
      }      
   }
   else
   {
      statusMessage = QString(tr("Unable to load file: %1")).arg(filename);
   }

    statusBar()->showMessage(statusMessage, 2000);
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnLoadGeometry()
{   
   QString filename = QFileDialog::getOpenFileName(this,
                                                   tr("Load Geometry File"),
                                                   mContextPath.c_str(),
                                                   tr("Geometry(*.osg *.ive *.flt *.3ds *.txp *.xml *)") );

   QString statusMessage;

   if (!filename.isEmpty())
   {       
      if (dtUtil::FileUtils::GetInstance().FileExists(filename.toStdString()))
      {
         emit LoadGeometry(filename);
         SetCurrentFile(filename);
         statusMessage = QString(tr("File Loaded"));
      }      
   }
   else
   {
      statusMessage = QString(tr("Unable to load file: %1")).arg(filename);
   }

   statusBar()->showMessage(statusMessage, 2000);
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnChangeContext()
{
   std::string newPath = GetContextPathFromUser();

   if (!newPath.empty())
   {
      mContextPath = newPath;
      SaveCurrentContextPath();
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnShaderItemChanged(QTreeWidgetItem *item, int column)
{ 
   if (column == 0)
   {
      QString programName = item->text(0);
      QString groupName   = item->parent()->text(0);

      if (item->checkState(0) == Qt::Checked)
      {
         emit ApplyShader(groupName.toStdString(), programName.toStdString());         
      }
      else if (item->checkState(0) == Qt::Unchecked)
      {
         emit RemoveShader();
      }
   }  
}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnDoubleclickShaderItem(QTreeWidgetItem *item, int column)
{

}

/////////////////////////////////////////////////////////////////////////////////////////
void ObjectWorkspace::OnToggleGridClicked(bool toggledOn)
{
   emit ToggleGrid(toggledOn);
}

/////////////////////////////////////////////////////////////////////////////////////////
std::string ObjectWorkspace::GetContextPathFromUser()
{
   ProjectContextDialog dialog(this);

   if (dialog.exec() == QDialog::Accepted)
   {
      return(dialog.getProjectPath().toStdString());
   }  
   
   return std::string();
}

/////////////////////////////////////////////////////////////////////////////////////////
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
