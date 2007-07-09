#include "MainWindow.h"
#include "AnimationTableWidget.h"
#include "TrackView.h"
#include "TrackScene.h"
#include <dtUtil/fileutils.h>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QtGui/QFileDialog>
#include <QtGui/QTableWidget>
#include <QtCore/QSettings>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QDockWidget>
#include <QtGui/QMessageBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QListWidget>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGridLayout>

#include <QtGui/QStandardItemModel>
#include <QtGui/QStandardItem>

#include <QtGui/QGraphicsEllipseItem>

#include <cassert>

MainWindow::MainWindow():
mExitAct(NULL),
mLoadCharAct(NULL),
mAnimListWidget(NULL),
mMeshListWidget(NULL),
mMaterialModel(NULL),
mMaterialView(NULL)
{
   resize(640, 300);   

   mAnimListWidget = new AnimationTableWidget(this);
   mAnimListWidget->setColumnCount(5);
   mAnimListWidget->setSelectionBehavior(QAbstractItemView::SelectRows);   

   connect(mAnimListWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(OnAnimationClicked(QTableWidgetItem*)));
   connect(mAnimListWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(OnItemChanged(QTableWidgetItem*)));
   connect(mAnimListWidget, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(OnItemDoubleClicked(QTableWidgetItem*)));

   QStringList headers;
   headers << "Name" << "Weight (L)" << "Delay (L)" << "Delay In (A)" << "Delay Out (A)";
   mAnimListWidget->setHorizontalHeaderLabels(headers );    

   mMeshListWidget = new QListWidget(this);
   connect(mMeshListWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(OnMeshActivated(QListWidgetItem*)));


   mMaterialModel = new QStandardItemModel(this);
   mMaterialView = new QTableView(this);
   mMaterialView->setModel( mMaterialModel );
   {
      QStringList headers;
      headers << "ID" << "Name" << "Diffuse" << "Ambient" << "Specular" << "Shininess";
      mMaterialModel->setHorizontalHeaderLabels(headers);
   }

   CreateActions();
   CreateMenus();
   (void)statusBar();
   CreateToolbars();
   //CreateTrackEditor(); 

   mTabs = new QTabWidget(this);
   mTabs->addTab(mAnimListWidget, tr("Animations"));
   mTabs->addTab(mMeshListWidget, tr("Meshes"));
   mTabs->addTab(mMaterialView, tr("Materials"));
   //mTabs->addTab(mTrackViewer, tr("Tracks"));   

   setCentralWidget(mTabs);
}

MainWindow::~MainWindow()
{
}

void MainWindow::CreateTrackEditor()
{  
   mTrackScene  = new TrackScene(this);
   mTrackViewer = new TrackView(mTrackScene, this);     
   
   mTrackViewer->setDragMode(QGraphicsView::ScrollHandDrag);
}

void MainWindow::CreateMenus()
{
   QMenu *windowMenu = menuBar()->addMenu("&File");
   QMenu *viewMenu   = menuBar()->addMenu("&View");
   QMenu *toolBarMenu = viewMenu->addMenu("&Toolbars");

   windowMenu->addAction(mLoadCharAct);

   QAction *toggleShadeToolbarAction = toolBarMenu->addAction("Shading toolbar");
   QAction *toggleLODToolbarAction  = toolBarMenu->addAction("LOD toolbar");
   //QAction *toggleLightToolBarAction = toolBarMenu->addAction("Lighting toolbar");

   toggleShadeToolbarAction->setCheckable(true);
   toggleShadeToolbarAction->setChecked(true);
   toggleLODToolbarAction->setCheckable(true);
   toggleLODToolbarAction->setChecked(true);
   //toggleLightToolBarAction->setCheckable(true);
   //toggleLightToolBarAction->setChecked(true);   

   connect(toggleShadeToolbarAction, SIGNAL(triggered()), this, SLOT(OnToggleShadingToolbar()));
   connect(toggleLODToolbarAction, SIGNAL(triggered()), this, SLOT(OnToggleLODToolbar()));

   for (int i=0; i<5; ++i)
   {
      windowMenu->addAction(mRecentFilesAct[i]);
   }

   menuBar()->addSeparator();
   windowMenu->addAction(mExitAct);

   UpdateRecentFileActions();

}

void MainWindow::CreateActions()
{
   mExitAct = new QAction(tr("E&xit"), this);
   mExitAct->setShortcut(tr("Ctrl+Q"));
   mExitAct->setStatusTip(tr("Exit the application"));
   connect(mExitAct, SIGNAL(triggered()), this, SLOT(close()));


   mLoadCharAct = new QAction(tr("&Open..."), this);
   mLoadCharAct->setShortcut(tr("Ctrl+O"));
   mLoadCharAct->setStatusTip(tr("Open an existing character file."));
   connect(mLoadCharAct, SIGNAL(triggered()), this, SLOT(OnOpenCharFile()));

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

   mWireframeAction  = actionGroup->addAction(wireframeIcon, "Wireframe");
   mShadedAction     = actionGroup->addAction(shadedIcon, "Shaded");
   mShadedWireAction = actionGroup->addAction(shadedWireIcon, "Shaded Wireframe");     

   mWireframeAction->setCheckable(true);
   mShadedAction->setCheckable(true); 
   mShadedWireAction->setCheckable(true);   

   mShadedAction->setChecked(true);
}

void MainWindow::CreateToolbars()
{
   QDoubleSpinBox *lodSpinner = new QDoubleSpinBox(this);
   lodSpinner->setRange(0, 1);
   lodSpinner->setSingleStep(0.01);  
   lodSpinner->setValue(1);

   mShadingToolbar = addToolBar("Shading toolbar"); 
   mLODToolbar     = addToolBar("LOD toolbar");
   //mLightingToolbar = addToolBar("Lighting toolbar");


   mShadingToolbar->addAction(mWireframeAction);
   mShadingToolbar->addAction(mShadedAction);
   mShadingToolbar->addAction(mShadedWireAction);    

   mLODToolbar->addWidget(lodSpinner);   

   //QIcon diffuseIcon(":/images/diffuseLight.png");
   //QIcon pointLightIcon(":/images/pointLight.png");

   //mLightingToolbar->addAction(diffuseIcon, "Diffuse Light");
   //mLightingToolbar->addAction(pointLightIcon, "Point Light");

   connect(lodSpinner, SIGNAL(valueChanged(double)), this, SLOT(OnLOD_Changed(double)));      
}

void MainWindow::OnOpenCharFile()
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

void MainWindow::LoadCharFile( const QString &filename )
{
   if (dtUtil::FileUtils::GetInstance().FileExists( filename.toStdString() ))
   {
      //mAnimListWidget->clear(); //note, this also removes the header items
      mMeshListWidget->clear();

      while (mAnimListWidget->rowCount()>0)
      {
         mAnimListWidget->removeRow(0);
      }

      while (mMaterialModel->rowCount() > 0 )
      {
         mMaterialModel->removeRow(0);
      }

      emit FileToLoad( filename );

      SetCurrentFile( filename );     

      statusBar()->showMessage(tr("File loaded"), 2000);
   }
   else
   {
      QString errorString = QString("File not found: %1").arg( filename );
      QMessageBox::warning( this, "Warning", errorString, "&Ok" );
   }    
}


void MainWindow::OnNewAnimation(unsigned int id, const QString &animationName, 
                                unsigned int trackCount, unsigned int keyframes,
                                float duration)
{
   mAnimListWidget->insertRow( mAnimListWidget->rowCount() );

   { //name
      QTableWidgetItem *item = new QTableWidgetItem( animationName );
      item->setCheckState(Qt::Unchecked);
      item->setData(Qt::UserRole, id);
      item->setData(Qt::UserRole+1, trackCount);
      item->setData(Qt::UserRole+2, keyframes);
      item->setData(Qt::UserRole+3, duration);
      item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      mAnimListWidget->setItem( id, 0, item );
   } 

   { //weight
      QTableWidgetItem *item = new QTableWidgetItem( tr("1.0") );
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
      mAnimListWidget->setItem( id, 1, item );
   }

   { //delay
      QTableWidgetItem *item = new QTableWidgetItem( tr("0.0") );
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
      mAnimListWidget->setItem( id, 2, item );
   }

   { //delay in
      QTableWidgetItem *item = new QTableWidgetItem( tr("0.0") );
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
      mAnimListWidget->setItem( id, 3, item );
   }

   { //delay out
      QTableWidgetItem *item = new QTableWidgetItem( tr("0.0") );
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
      mAnimListWidget->setItem( id, 4, item );
   }

   mAnimListWidget->resizeColumnToContents(0);

}

void MainWindow::OnNewMesh(int meshID, const QString &meshName)
{
   QListWidgetItem *meshItem = new QListWidgetItem();
   meshItem->setText(meshName);
   meshItem->setData( Qt::UserRole, meshID );

   meshItem->setFlags(Qt::ItemIsSelectable |
                      Qt::ItemIsUserCheckable |
                      Qt::ItemIsEnabled);

   meshItem->setCheckState(Qt::Checked);

   mMeshListWidget->addItem(meshItem);
}

void MainWindow::OnNewMaterial( int matID, const QString &name, 
                               const QColor &diff, const QColor &amb, const QColor &spec,
                               float shininess )
{
   QString tooltip;

   QStandardItem *idItem = new QStandardItem(QString::number(matID) );
   idItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

   QStandardItem *nameItem = new QStandardItem( name );
   nameItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

   QStandardItem *diffItem = new QStandardItem( diff.name() );
   diffItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
   diffItem->setData(diff, Qt::BackgroundRole);
   tooltip = tr("R:%1\nG:%2\nB:%3\nA:%4").arg(diff.red()).arg(diff.green()).arg(diff.blue()).arg(diff.alpha());
   diffItem->setData( tooltip, Qt::ToolTipRole);

   QStandardItem *ambItem = new QStandardItem( amb.name() );
   ambItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
   ambItem->setData(amb, Qt::BackgroundRole);
   tooltip = tr("R:%1\nG:%2\nB:%3\nA:%4").arg(amb.red()).arg(amb.green()).arg(amb.blue()).arg(amb.alpha());
   ambItem->setData( tooltip, Qt::ToolTipRole);

   QStandardItem *specItem = new QStandardItem( spec.name() );
   specItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
   specItem->setData(amb, Qt::BackgroundRole);
   tooltip = tr("R:%1\nG:%2\nB:%3\nA:%4").arg(spec.red()).arg(spec.green()).arg(spec.blue()).arg(spec.alpha());
   specItem->setData( tooltip, Qt::ToolTipRole);

   QStandardItem *shinItem = new QStandardItem( QString::number(shininess) );
   shinItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

   QList<QStandardItem*> items;
   items << idItem << nameItem << diffItem << ambItem << specItem << shinItem;
   mMaterialModel->appendRow( items);
}

void MainWindow::OnAnimationClicked( QTableWidgetItem *item )
{
   if (item->column() != 0) return;


   if (item->checkState() == Qt::Checked)
   {
      //item->setCheckState(Qt::Unchecked);
      //emit StopAnimation( item->data(Qt::UserRole).toUInt() );
      OnStartAnimation(item->row());
   }
   else
   {
      //item->setCheckState(Qt::Checked);
      //emit StartAnimation( item->data(Qt::UserRole).toUInt() );
      OnStopAnimation(item->row());
   }
}

void MainWindow::OnMeshActivated( QListWidgetItem *item )
{
   int meshID = item->data(Qt::UserRole).toInt();

   if (item->checkState() == Qt::Checked)
   {
      //attach the mesh to the CalModel
      emit AttachMesh(meshID);
   }
   else
   {
      //detach the mesh from the CalModel
      emit DetachMesh(meshID);
   }
}


void MainWindow::OnLOD_Changed(double newValue)
{   
   emit LOD_Changed(newValue);
}

void MainWindow::OnToggleShadingToolbar()
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

void MainWindow::OnToggleLODToolbar()
{
   if (mLODToolbar->isHidden())
   {
      mLODToolbar->show();
   }
   else
   {
      mLODToolbar->hide();
   }   
}

void MainWindow::OnToggleLightingToolbar()
{

}

void MainWindow::UpdateRecentFileActions()
{
   QSettings settings("MOVES", "Animation Viewer");
   QStringList files = settings.value("recentFileList").toStringList();

   int numRecentFiles = qMin(files.size(), 5);

   for (int i = 0; i < numRecentFiles; ++i) {
      QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName() );
      mRecentFilesAct[i]->setText(text);
      mRecentFilesAct[i]->setData(files[i]);
      mRecentFilesAct[i]->setVisible(true);
   }
   for (int j = numRecentFiles; j < 5; ++j)
      mRecentFilesAct[j]->setVisible(false);

}

void MainWindow::SetCurrentFile( const QString &filename )
{
   if (filename.isEmpty())
   {
      setWindowTitle(tr("Animation Viewer"));
   }
   else
   {
      setWindowTitle(tr("%1 - %2").arg(QFileInfo(filename).fileName()).arg(tr("Animation Viewer")));
   }

   QSettings settings("MOVES", "Animation Viewer");
   QStringList files = settings.value("recentFileList").toStringList();
   files.removeAll(filename);
   files.prepend(filename);

   while (files.size() > 5)
   {
      files.removeLast();
   }

   settings.setValue("recentFileList", files);
   UpdateRecentFileActions();
}

void MainWindow::OpenRecentFile()
{
   QAction *action = qobject_cast<QAction*>(sender());

   if (action)
   {
      LoadCharFile( action->data().toString() );
   }
}

void MainWindow::OnItemChanged( QTableWidgetItem *item )
{
   if (item->column() == 1 ||
      item->column() == 2) 
   {
      if (mAnimListWidget->item(item->row(),0)->checkState() == Qt::Checked)
      {
         OnStartAnimation(item->row());
      }
      else
      {
         OnStopAnimation(item->row());
      }
   }
}

void MainWindow::OnStartAnimation(int row)
{
   float weight = 0.f;
   float delay = 0.f;

   if (mAnimListWidget->item(row, 1))
   {
      weight = mAnimListWidget->item(row, 1)->text().toFloat();
   }

   if (mAnimListWidget->item(row, 2))
   {
      delay = mAnimListWidget->item(row, 2)->text().toFloat();
   }

   if (mAnimListWidget->item(row,0))
   {
      emit StartAnimation( mAnimListWidget->item(row,0)->data(Qt::UserRole).toUInt(), weight, delay );
   }  
}

void MainWindow::OnStopAnimation(int row)
{
   float delay = 0.f;

   if (mAnimListWidget->item(row, 2))
   {
      delay = mAnimListWidget->item(row, 2)->text().toFloat();
   }


   if (mAnimListWidget->item(row,0))
   {
      emit StopAnimation( mAnimListWidget->item(row,0)->data(Qt::UserRole).toUInt(), delay );
   } 
}

void MainWindow::OnItemDoubleClicked(QTableWidgetItem *item)
{
   if (item->column() == 0)
   {
      OnStartAction(item->row());
   }
}


void MainWindow::OnStartAction( int row )
{
   float delayIn = 0.f;
   float delayOut = 0.f;
   if (mAnimListWidget->item(row, 3))
   {
      delayIn = mAnimListWidget->item(row, 3)->text().toFloat();
   }

   if (mAnimListWidget->item(row, 4))
   {
      delayOut = mAnimListWidget->item(row, 4)->text().toFloat();
   }

   if (mAnimListWidget->item(row, 0))
   {
      emit StartAction(mAnimListWidget->item(row,0)->data(Qt::UserRole).toUInt(), delayIn, delayOut);
   }

}

void MainWindow::OnDisplayError( const QString &msg )
{
   QMessageBox::warning(this, "AnimationViewer", msg );
}



