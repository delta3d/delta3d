#include "MainWindow.h"
#include <dtUtil/fileutils.h>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QTableWidget>
#include <QSettings>
#include <QStatusBar>
#include <QToolBar>
#include <QPushButton>
#include <QRadioButton>
#include <QDockWidget>
#include <QMessageBox>
#include <QDoubleSpinBox>

#include <cassert>

MainWindow::MainWindow():
mExitAct(NULL),
mLoadCharAct(NULL),
mAnimListWidget(NULL)
{
   resize(640, 300);

   CreateActions();
   CreateMenus();
   (void)statusBar();
   CreateToolbars();   
}

MainWindow::~MainWindow()
{
}

void MainWindow::CreateMenus()
{
   QMenu *windowMenu = menuBar()->addMenu("&File");
   QMenu *viewMenu   = menuBar()->addMenu("&View");
   QMenu *toolBarMenu = viewMenu->addMenu("&Toolbars");

   windowMenu->addAction(mLoadCharAct);

   QAction *toggleShadeToolbarAction = toolBarMenu->addAction("Shading toolbar");
   QAction *toggleTempToolbarAction  = toolBarMenu->addAction("Temp toolbar");
   QAction *toggleLightToolBarAction = toolBarMenu->addAction("Lighting toolbar");

   toggleShadeToolbarAction->setCheckable(true);
   toggleShadeToolbarAction->setChecked(true);
   toggleTempToolbarAction->setCheckable(true);
   toggleTempToolbarAction->setChecked(true);
   toggleLightToolBarAction->setCheckable(true);
   toggleLightToolBarAction->setChecked(true);   

   connect(toggleShadeToolbarAction, SIGNAL(triggered()), this, SLOT(OnToggleShadingToolbar()));

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
}

void MainWindow::CreateToolbars()
{
   QDoubleSpinBox *lodSpinner = new QDoubleSpinBox(this);
   lodSpinner->setRange(0, 1);
   lodSpinner->setSingleStep(0.01);  
   lodSpinner->setValue(1);

   mShadingToolbar  = addToolBar("Shading toolbar"); 
   mTempToolbar     = addToolBar("Temp toolbar");
   mLightingToolbar = addToolBar("Lighting toolbar");

   // The actiongroup is used to make the action behave like radio buttons
   QActionGroup *actionGroup = new QActionGroup(this);
   actionGroup->setExclusive(true);     

   QIcon wireframeIcon(":/images/wireframe.png");  
   QIcon shadedIcon(":/images/shaded.png");
   QIcon shadedWireIcon(":/images/shadedwire.png");
   QIcon diffuseIcon(":/images/diffuseLight.png");
   QIcon pointLightIcon(":/images/pointLight.png");

   mWireframeAction  = actionGroup->addAction(wireframeIcon, "Wireframe");
   mShadedAction     = actionGroup->addAction(shadedIcon, "Shaded");
   mShadedWireAction = actionGroup->addAction(shadedWireIcon, "Shaded Wireframe");     

   mWireframeAction->setCheckable(true);
   mShadedAction->setCheckable(true); 
   mShadedWireAction->setCheckable(true);   

   mShadedAction->setChecked(true);

   mShadingToolbar->addAction(mWireframeAction);
   mShadingToolbar->addAction(mShadedAction);
   mShadingToolbar->addAction(mShadedWireAction);    

   mTempToolbar->addWidget(lodSpinner);   

   mLightingToolbar->addAction(diffuseIcon, "Diffuse Light");
   mLightingToolbar->addAction(pointLightIcon, "Point Light");

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
      delete mAnimListWidget;  

      mAnimListWidget = new QTableWidget();
      mAnimListWidget->setColumnCount(5);
      mAnimListWidget->setSelectionBehavior(QAbstractItemView::SelectRows);   

      connect(mAnimListWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(OnAnimationClicked(QTableWidgetItem*)));
      connect(mAnimListWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(OnItemChanged(QTableWidgetItem*)));
      connect(mAnimListWidget, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(OnItemDoubleClicked(QTableWidgetItem*)));

      QStringList headers;
      headers << "Name" << "Weight (L)" << "Delay (L)" << "Delay In (A)" << "Delay Out (A)";
      mAnimListWidget->setHorizontalHeaderLabels(headers );  

      setCentralWidget(mAnimListWidget);

      emit FileToLoad( filename );

      SetCurrentFile( filename );     

      //statusBar()->showMessage(tr("File loaded"), 2000);
   }
   else
   {
      QString errorString = QString("File not found: %1").arg( filename );
      QMessageBox::warning( this, "Warning", errorString, "&Ok" );
   }    
}


void MainWindow::OnNewAnimation(unsigned int id, const QString &filename )
{
   mAnimListWidget->insertRow( mAnimListWidget->rowCount() );

   { //name
      QTableWidgetItem *item = new QTableWidgetItem( filename );
      item->setCheckState(Qt::Unchecked);
      item->setData(Qt::UserRole, id);
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

void MainWindow::OnToggleTempToolbar()
{

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
