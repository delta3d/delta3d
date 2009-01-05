#include "MainWindow.h"
#include "AnimationTableWidget.h"
#include "PoseMeshView.h"
#include "PoseMeshScene.h"
#include "PoseMeshProperties.h"
#include "PoseMeshItem.h"
#include "OSGAdapterWidget.h"

#include <osg/Geode> ///needed for the node builder
#include <dtAnim/cal3ddatabase.h>
#include <dtAnim/animnodebuilder.h>
#include <dtAnim/chardrawable.h>

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
#include <QtGui/QHBoxLayout>
#include <QtGui/QProgressBar>
#include <QtGui/QTreeWidgetItem>
#include <QtCore/QUrl>

#include <QtGui/QStandardItemModel>
#include <QtGui/QStandardItem>

#include <QtGui/QGraphicsEllipseItem>
#include <cassert>

////////////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow()
  : mExitAct(NULL)
  , mLoadCharAct(NULL)
  , mScaleFactorSpinner(NULL)
  , mAnimListWidget(NULL)
  , mMeshListWidget(NULL)
  , mMaterialModel(NULL)
  , mMaterialView(NULL)
  , mPoseDock(NULL)
  , mPoseMeshViewer(NULL)
  , mPoseMeshScene(NULL)
  , mPoseMeshProperties(NULL)
  , mGLWidget(NULL)
{
   resize(800, 800);

   mAnimListWidget = new AnimationTableWidget(this);
   mAnimListWidget->setColumnCount(6);
   mAnimListWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

   connect(mAnimListWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(OnAnimationClicked(QTableWidgetItem*)));
   connect(mAnimListWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(OnItemChanged(QTableWidgetItem*)));
   connect(mAnimListWidget, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(OnItemDoubleClicked(QTableWidgetItem*)));

   QStringList headers;
   headers << "Name" << "Weight (L)" << "Delay (L)" << "Delay In (A)" << "Delay Out (A)" << "Mixer Blend";
   mAnimListWidget->setHorizontalHeaderLabels(headers);

   mMeshListWidget = new QListWidget(this);
   connect(mMeshListWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(OnMeshActivated(QListWidgetItem*)));


   mMaterialModel = new QStandardItemModel(this);
   mMaterialView = new QTableView(this);
   mMaterialView->setModel(mMaterialModel);

   {
      QStringList headers;
      headers << "ID" << "Name" << "Diffuse" << "Ambient" << "Specular" << "Shininess";
      mMaterialModel->setHorizontalHeaderLabels(headers);
   }


   mSubMorphTargetListWidget = new QTableWidget(this);
   mSubMorphTargetListWidget->setColumnCount(4);
   mSubMorphTargetListWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
   {
      QStringList headers;
      headers << "MeshID" << "SubMeshID" << "MorphName" << "Weight";
      mSubMorphTargetListWidget->setHorizontalHeaderLabels(headers);
   }

   connect(mSubMorphTargetListWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(OnSubMorphChanged(QTableWidgetItem*)));


   CreateActions();
   CreateMenus();
   statusBar();
   CreateToolbars();

   mTabs = new QTabWidget(this);
   mTabs->addTab(mAnimListWidget, tr("Animations"));
   mTabs->addTab(mMeshListWidget, tr("Meshes"));
   mTabs->addTab(mMaterialView, tr("Materials"));
   mTabs->addTab(mSubMorphTargetListWidget, tr("SubMorphTargets"));

   QWidget* glParent = new QWidget(this);

   mGLWidget = new dtQt::OSGAdapterWidget(false, glParent);

   QHBoxLayout* hbLayout = new QHBoxLayout(glParent);
   hbLayout->setMargin(0);
   glParent->setLayout(hbLayout);
   hbLayout->addWidget(mGLWidget);
   setCentralWidget(glParent);

   QDockWidget* tabsDockWidget = new QDockWidget();
   tabsDockWidget->setWidget(mTabs);

   addDockWidget(Qt::BottomDockWidgetArea, tabsDockWidget);

   //accept drag & drop operations
   setAcceptDrops(true);

}

////////////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::CreateMenus()
{
   QMenu* windowMenu  = menuBar()->addMenu("&File");
   QMenu* viewMenu    = menuBar()->addMenu("&View");
   QMenu* toolBarMenu = viewMenu->addMenu("&Toolbars");

   viewMenu->addAction(mHardwareSkinningAction);
   
   windowMenu->addAction(mLoadCharAct);

   QAction* toggleShadeToolbarAction = toolBarMenu->addAction("Shading toolbar");
   QAction* toggleLODScaleToolbarAction  = toolBarMenu->addAction("LOD Scale toolbar");
   //QAction *toggleLightToolBarAction = toolBarMenu->addAction("Lighting toolbar");
   QAction* toggleScalingToolbarAction  = toolBarMenu->addAction("Scaling toolbar");

   toggleShadeToolbarAction->setCheckable(true);
   toggleShadeToolbarAction->setChecked(true);
   toggleLODScaleToolbarAction->setCheckable(true);
   toggleLODScaleToolbarAction->setChecked(true);
   //toggleLightToolBarAction->setCheckable(true);
   //toggleLightToolBarAction->setChecked(true);
   toggleScalingToolbarAction->setCheckable(true);
   toggleScalingToolbarAction->setChecked(true);

   connect(toggleShadeToolbarAction, SIGNAL(triggered()), this, SLOT(OnToggleShadingToolbar()));
   connect(toggleLODScaleToolbarAction, SIGNAL(triggered()), this, SLOT(OnToggleLODScaleToolbar()));
   connect(toggleScalingToolbarAction, SIGNAL(triggered()), this, SLOT(OnToggleScalingToolbar()));

   for (int actionIndex = 0; actionIndex < 5; ++actionIndex)
   {
      windowMenu->addAction(mRecentFilesAct[actionIndex]);
   }

   menuBar()->addSeparator();
   windowMenu->addAction(mExitAct);

   UpdateRecentFileActions();
}

////////////////////////////////////////////////////////////////////////////////
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

   for (int actionIndex = 0; actionIndex < 5; actionIndex++)
   {
      mRecentFilesAct[actionIndex] = new QAction(this);
      mRecentFilesAct[actionIndex]->setVisible(false);
      connect(mRecentFilesAct[actionIndex], SIGNAL(triggered()), this, SLOT(OpenRecentFile()));
   }

   // The actiongroup is used to make the action behave like radio buttons
   QActionGroup* actionGroup = new QActionGroup(this);
   actionGroup->setExclusive(true);

   QIcon wireframeIcon(":/images/wireframe.png");
   QIcon shadedIcon(":/images/shaded.png");
   QIcon shadedWireIcon(":/images/shadedwire.png");
   QIcon boneBasisIcon(":/images/boneBasis.png");

   mWireframeAction  = actionGroup->addAction(wireframeIcon, "Wireframe");
   mShadedAction     = actionGroup->addAction(shadedIcon, "Shaded");
   mShadedWireAction = actionGroup->addAction(shadedWireIcon, "Shaded Wireframe");

   mBoneBasisAction = new QAction(boneBasisIcon, "Bone Orientation", NULL);

   mWireframeAction->setCheckable(true);
   mShadedAction->setCheckable(true);
   mShadedWireAction->setCheckable(true);
   mBoneBasisAction->setCheckable(true);

   mShadedAction->setChecked(true);

   mHardwareSkinningAction = new QAction(tr("Use Hardware Skinning"), this);
   mHardwareSkinningAction->setCheckable(true);
   mHardwareSkinningAction->setChecked(false); //will get init'd properly when everything's up and running
   connect(mHardwareSkinningAction, SIGNAL(triggered()), this, SLOT(OnToggleHardwareSkinning()));
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::CreateToolbars()
{
   QDoubleSpinBox* lodScaleSpinner = new QDoubleSpinBox(this);
   lodScaleSpinner->setRange(0.01, 500.0);
   lodScaleSpinner->setSingleStep(0.01);
   lodScaleSpinner->setValue(1);
   lodScaleSpinner->setToolTip(tr("Level of Detail scale"));

   QDoubleSpinBox* speedSpinner = new QDoubleSpinBox(this);
   speedSpinner->setRange(0.0, 100.0);
   speedSpinner->setSingleStep(0.01);
   speedSpinner->setValue(1.0);
   speedSpinner->setToolTip(tr("Animation Speed Scale Factor"));

   mScaleFactorSpinner = new QDoubleSpinBox(this);
   mScaleFactorSpinner->setRange(0.001, 500.0);
   mScaleFactorSpinner->setSingleStep(0.01);
   mScaleFactorSpinner->setValue(1);
   mScaleFactorSpinner->setToolTip(tr("Scale Factor"));

   mShadingToolbar = addToolBar("Shading toolbar");
   mShadingToolbar->setToolTip("Shading toolbar");
   mLODScaleToolbar= addToolBar("LOD Scale toolbar");
   mLODScaleToolbar->setToolTip("LOD Scale toolbar");
   mSpeedToolbar   = addToolBar("Animation Speed toolbar");
   mSpeedToolbar->setToolTip("Animation Speed toolbar");
   //mLightingToolbar = addToolBar("Lighting toolbar");
   mScalingToolbar = addToolBar("Scaling toolbar");
   mScalingToolbar->setToolTip("Scaling toolbar");

   mShadingToolbar->addAction(mWireframeAction);
   mShadingToolbar->addAction(mShadedAction);
   mShadingToolbar->addAction(mShadedWireAction);
   mShadingToolbar->addSeparator();
   mShadingToolbar->addAction(mBoneBasisAction);

   mLODScaleToolbar->addWidget(lodScaleSpinner);
   mSpeedToolbar->addWidget(speedSpinner);
   mScalingToolbar->addWidget(mScaleFactorSpinner);
   QPushButton* applyScaleFactorButton = new QPushButton(tr("Apply"), this);
   applyScaleFactorButton->adjustSize();
   applyScaleFactorButton->setToolTip(tr("Apply scale factor"));
   mScalingToolbar->addWidget(applyScaleFactorButton);

   //QIcon diffuseIcon(":/images/diffuseLight.png");
   //QIcon pointLightIcon(":/images/pointLight.png");

   //mLightingToolbar->addAction(diffuseIcon, "Diffuse Light");
   //mLightingToolbar->addAction(pointLightIcon, "Point Light");

   connect(lodScaleSpinner, SIGNAL(valueChanged(double)), this, SLOT(OnLODScale_Changed(double)));
   connect(speedSpinner, SIGNAL(valueChanged(double)), this, SLOT(OnSpeedChanged(double)));
   connect(applyScaleFactorButton, SIGNAL(clicked()), this, SLOT(OnChangeScaleFactor()));
   connect(mScaleFactorSpinner, SIGNAL(editingFinished()), this, SLOT(OnChangeScaleFactor()));
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::DestroyPoseResources()
{
   if (mPoseMeshViewer)     { delete mPoseMeshViewer;     mPoseMeshViewer = NULL;     }
   if (mPoseMeshScene)      { delete mPoseMeshScene;      mPoseMeshScene = NULL;      }
   if (mPoseMeshProperties) { delete mPoseMeshProperties; mPoseMeshProperties = NULL; }
   if (mPoseDock)           { delete mPoseDock;           mPoseDock = NULL;           }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnOpenCharFile()
{
   QString filename = QFileDialog::getOpenFileName(this,
      tr("Load Character File"), ".", tr("Characters (*.xml)") );

   if (!filename.isEmpty())
   {
      LoadCharFile(filename);
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::LoadCharFile(const QString& filename)
{
   if (dtUtil::FileUtils::GetInstance().FileExists(filename.toStdString()))
   {
      //mAnimListWidget->clear(); //note, this also removes the header items
      mMeshListWidget->clear();

      // Make sure we start fresh
      DestroyPoseResources();

      while (mAnimListWidget->rowCount()>0)
      {
         mAnimListWidget->removeRow(0);
      }

      while (mSubMorphTargetListWidget->rowCount()>0)
      {
         mSubMorphTargetListWidget->removeRow(0);
      }

      while (mMaterialModel->rowCount() > 0)
      {
         mMaterialModel->removeRow(0);
      }

      // reset the scale spinbox
      mScaleFactorSpinner->setValue(1.0f);

      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

      emit FileToLoad(filename);

      SetCurrentFile(filename);

      QApplication::restoreOverrideCursor();
      statusBar()->showMessage(tr("File loaded"), 2000);
   }
   else
   {
      QString errorString = QString("File not found: %1").arg(filename);
      QMessageBox::warning(this, "Warning", errorString, "&Ok");
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnNewAnimation(unsigned int id, const QString& animationName,
                                unsigned int trackCount, unsigned int keyframes,
                                float duration)
{
   mAnimListWidget->insertRow(mAnimListWidget->rowCount());

   { //name
      QTableWidgetItem* item = new QTableWidgetItem(animationName);
      item->setCheckState(Qt::Unchecked);
      item->setData(Qt::UserRole, id);
      item->setData(Qt::UserRole+1, trackCount);
      item->setData(Qt::UserRole+2, keyframes);
      item->setData(Qt::UserRole+3, duration);
      item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      mAnimListWidget->setItem(id, 0, item);
   }

   { //weight
      QTableWidgetItem* item = new QTableWidgetItem(tr("1.0"));
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
      mAnimListWidget->setItem( id, 1, item );
   }

   { //delay
      QTableWidgetItem* item = new QTableWidgetItem(tr("0.0"));
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
      mAnimListWidget->setItem(id, 2, item);
   }

   { //delay in
      QTableWidgetItem* item = new QTableWidgetItem(tr("0.0"));
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
      mAnimListWidget->setItem(id, 3, item);
   }

   { //delay out
      QTableWidgetItem* item = new QTableWidgetItem(tr("0.0"));
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
      mAnimListWidget->setItem(id, 4, item);
   }

   { //mixer blend
      QProgressBar* mixerBlend = new QProgressBar;
      mixerBlend->setMaximum(100);
      mixerBlend->setMinimum(0);
      mixerBlend->setValue(0);

      mAnimListWidget->setCellWidget(id, 5, mixerBlend);
   }

   mAnimListWidget->resizeColumnToContents(0);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnNewMesh(int meshID, const QString& meshName)
{
   QListWidgetItem *meshItem = new QListWidgetItem();
   meshItem->setText(meshName);
   meshItem->setData(Qt::UserRole, meshID);

   meshItem->setFlags(Qt::ItemIsSelectable |
                      Qt::ItemIsUserCheckable |
                      Qt::ItemIsEnabled);

   meshItem->setCheckState(Qt::Checked);

   mMeshListWidget->addItem(meshItem);
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::OnNewSubMorphTarget(int meshID, int subMeshID,
                                     int morphID, const QString& morphName)
{
   mSubMorphTargetListWidget->insertRow(mSubMorphTargetListWidget->rowCount());
   { //meshID
      QTableWidgetItem* item = new QTableWidgetItem(QString::number(meshID));
      item->setData(Qt::UserRole, morphID);

      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      mSubMorphTargetListWidget->setItem(morphID, 0, item);
   }

   { //submeshid
      QTableWidgetItem* item = new QTableWidgetItem(QString::number(subMeshID));
      item->setData(Qt::UserRole, morphID);

      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      mSubMorphTargetListWidget->setItem(morphID, 1, item);
   }

   { //name
      QTableWidgetItem* item = new QTableWidgetItem(morphName);
      item->setData(Qt::UserRole, morphID);

      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      mSubMorphTargetListWidget->setItem(morphID, 2, item);
   }

   { //weight
      QTableWidgetItem* item = new QTableWidgetItem(tr("0.0"));
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
      mSubMorphTargetListWidget->setItem( morphID, 3, item );
   }

   mSubMorphTargetListWidget->resizeColumnToContents(0);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnPoseMeshesLoaded(const std::vector<dtAnim::PoseMesh*>& poseMeshList,
                                    dtAnim::CharDrawable* model)
{
   assert(!mPoseMeshScene);
   assert(!mPoseMeshViewer);
   assert(!mPoseMeshProperties);

   mPoseMeshScene  = new PoseMeshScene(this);
   mPoseMeshViewer = new PoseMeshView(mPoseMeshScene, this);

   mPoseDock = new QDockWidget("Pose Mesh Viewer");
   mPoseDock->setWidget(mPoseMeshViewer);

   addDockWidget(Qt::RightDockWidgetArea, mPoseDock);

   // Create icons for the mode toolbar
   QIcon modeGrabIcon(QPixmap(":/images/handIcon.png"));
   QIcon modeBlendIcon(QPixmap(":/images/reticle.png"));
   QIcon modeErrorIcon(QPixmap(":/images/epsilon.png"));

   QToolBar* poseModesToolbar = new QToolBar;

   // The actiongroup is used to make the action behave like radio buttons
   QActionGroup* actionGroup = new QActionGroup(poseModesToolbar);
   actionGroup->setExclusive(true);

   QAction* grabAction  = actionGroup->addAction(modeGrabIcon, "Click-drag mode.");
   QAction* pickAction  = actionGroup->addAction(modeBlendIcon, "Blend pick mode.");
   QAction* errorAction = actionGroup->addAction(modeErrorIcon, "Error pick mode.");

   poseModesToolbar->addAction(grabAction);
   poseModesToolbar->addAction(pickAction);

   // Not full implemented so leave out
   //poseModesToolbar->addAction(errorAction);

   grabAction->setCheckable(true);
   pickAction->setCheckable(true);
   errorAction->setCheckable(true);

   pickAction->setChecked(true);

   poseModesToolbar->addSeparator();
   poseModesToolbar->addSeparator();

   QIcon displayEdgesIcon(QPixmap(":/images/displayEdges.png"));
   QIcon displayErrorIcon(QPixmap(":/images/displayError.png"));
   QIcon flipVerticalIcon(QPixmap(":/images/verticalFlip.png"));
   QIcon flipHorizontalIcon(QPixmap(":/images/horizontalFlip.png"));

   QAction* displayEdgesAction   = poseModesToolbar->addAction(displayEdgesIcon, "Display Edges.");
   QAction* displayErrorAction   = poseModesToolbar->addAction(displayErrorIcon, "Display Error Samples.");
   QAction* flipVerticalAction   = poseModesToolbar->addAction(flipVerticalIcon, "Flip the vertical axis.");
   QAction* flipHorizontalAction = poseModesToolbar->addAction(flipHorizontalIcon, "Flip the horizontal axis.");

   displayEdgesAction->setCheckable(true);
   displayErrorAction->setCheckable(true);

   displayEdgesAction->setChecked(true);

   mPoseDock->setTitleBarWidget(poseModesToolbar);

   // Add the properties tab
   mPoseMeshProperties = new PoseMeshProperties;

   mTabs->addTab(mPoseMeshProperties, tr("IK"));
   mTabs->setCurrentWidget(mPoseMeshProperties);

   // Establish connections from the properties tab
   connect(mPoseMeshProperties, SIGNAL(ViewPoseMesh(const std::string&)),
           mPoseMeshViewer, SLOT(OnZoomToPoseMesh(const std::string&)));

   connect(mPoseMeshProperties, SIGNAL(PoseMeshStatusChanged(const std::string&, bool)),
           mPoseMeshScene, SLOT(OnPoseMeshStatusChanged(const std::string&, bool)));

   // Establish connections from the scene
   connect(mPoseMeshScene, SIGNAL(ViewPoseMesh(const std::string&)),
           mPoseMeshViewer, SLOT(OnZoomToPoseMesh(const std::string&)));

   connect(mPoseMeshScene, SIGNAL(PoseMeshItemAdded(const PoseMeshItem*)),
           mPoseMeshProperties, SLOT(OnItemAdded(const PoseMeshItem*)));

   connect(grabAction, SIGNAL(triggered()), this, SLOT(OnSelectModeGrab()));
   connect(pickAction, SIGNAL(triggered()), this, SLOT(OnSelectModeBlendPick()));
   connect(errorAction, SIGNAL(triggered()), this, SLOT(OnSelectModeErrorPick()));

   connect(displayEdgesAction, SIGNAL(toggled(bool)), SLOT(OnToggleDisplayEdges(bool)));
   connect(displayErrorAction, SIGNAL(toggled(bool)), SLOT(OnToggleDisplayError(bool)));
   connect(flipVerticalAction, SIGNAL(triggered()), SLOT(OnToggleFlipVertical()));
   connect(flipHorizontalAction, SIGNAL(triggered()), SLOT(OnToggleFlipHorizontal()));

   for (size_t poseIndex = 0; poseIndex < poseMeshList.size(); ++poseIndex)
   {
      dtAnim::PoseMesh *newMesh = poseMeshList[poseIndex];

      // Add new pose mesh visualization and properties
      mPoseMeshScene->AddMesh(*newMesh, model);
      mPoseMeshProperties->AddMesh(*newMesh, *model->GetCal3DWrapper());
   }

   // Set the default mode
   OnSelectModeBlendPick();
}

//////////////////////////////////////////////////////////////////////////
QString MainWindow::MakeColorString(const QColor& color) const
{
   QString colorString;
   colorString = tr("R:%1 G:%2 B:%3 A:%4").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());
   return colorString;
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnNewMaterial(int matID, const QString& name,
                               const QColor& diff, const QColor& amb, const QColor& spec,
                               float shininess)
{
   QStandardItem* idItem = new QStandardItem(QString::number(matID));
   idItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

   QStandardItem* nameItem = new QStandardItem(name);
   nameItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

   QStandardItem* diffItem = new QStandardItem(MakeColorString(diff));
   diffItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
   diffItem->setData(diff, Qt::BackgroundRole);
 
   QStandardItem* ambItem = new QStandardItem(MakeColorString(amb));
   ambItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
   ambItem->setData(amb, Qt::BackgroundRole);
 
   QStandardItem* specItem = new QStandardItem(MakeColorString(spec));
   specItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
   specItem->setData(amb, Qt::BackgroundRole);
 
   QStandardItem* shinItem = new QStandardItem(QString::number(shininess));
   shinItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

   QList<QStandardItem*> items;
   items << idItem << nameItem << diffItem << ambItem << specItem << shinItem;
   mMaterialModel->appendRow( items);

   //resize the columns to fit the data width
   mMaterialView->resizeColumnsToContents();
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnBlendUpdate(const std::vector<float>& weightList)
{
   if (weightList.size() != (size_t)mAnimListWidget->rowCount()) { return; }

   for (size_t rowIndex = 0; rowIndex < weightList.size(); ++rowIndex)
   {
      // Show progress as a whole number
      float newValue = weightList[rowIndex] * 100.0f;

      QProgressBar* meter = (QProgressBar*)mAnimListWidget->cellWidget(rowIndex, 5);
      meter->setValue(newValue);

      if (mAnimListWidget->item(rowIndex, 0)->checkState() == Qt::Checked)
      {
         // Update the weight display only when the box is checked
         // This will allow a user to manually enter a weight while unchecked
         disconnect(mAnimListWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(OnItemChanged(QTableWidgetItem*)));
         mAnimListWidget->item(rowIndex, 1)->setData(Qt::DisplayRole, QString("%1").arg(weightList[rowIndex]));
         connect(mAnimListWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(OnItemChanged(QTableWidgetItem*)));

         if (!newValue)
         {
            // If animations were turned off from the pose mesh viewer
            // mark them as turned off in the animation table
            mAnimListWidget->item(rowIndex, 0)->setCheckState(Qt::Unchecked);
         }
      }
      else if (newValue)
      {
         // If animations were turned on from the pose mesh viewer
         // mark them as turned on in the animation table
         mAnimListWidget->item(rowIndex, 0)->setCheckState(Qt::Checked);
      }
   }

   // Allow the IK tab to update it's blend display if it exists
   if (mPoseMeshProperties)
   {
      mPoseMeshProperties->OnBlendUpdate(weightList);
   }

   // Allow the pose scene to update in response to the blend
   if (mPoseMeshScene)
   {
      mPoseMeshScene->OnBlendUpdate();
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnAnimationClicked(QTableWidgetItem* item)
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

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnMeshActivated(QListWidgetItem* item)
{
   int meshID = item->data(Qt::UserRole).toInt();

   if (item->checkState() == Qt::Checked)
   {
      //Show the mesh on the CalModel
      emit ShowMesh(meshID);
   }
   else
   {
      //Hide the mesh on CalModel from view
      emit HideMesh(meshID);
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnLODScale_Changed(double newValue)
{
   emit LODScale_Changed(float(newValue));
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnSpeedChanged(double newValue)
{
   emit SpeedChanged(float(newValue));
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnChangeScaleFactor()
{
   emit ScaleFactorChanged(float(mScaleFactorSpinner->value()));

   //reset the scaling factor to 1.0
   mScaleFactorSpinner->setValue(1.0);
}

/////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnToggleHardwareSkinning()
{
   dtAnim::AnimNodeBuilder& nodeBuilder = dtAnim::Cal3DDatabase::GetInstance().GetNodeBuilder();
   QAction* action = qobject_cast<QAction*>(sender());
   bool usingHardwareSkinning = action->isChecked();

   if (usingHardwareSkinning)
   {
      nodeBuilder.SetCreate(dtAnim::AnimNodeBuilder::CreateFunc(&nodeBuilder, &dtAnim::AnimNodeBuilder::CreateHardware));
   }
   else
   {
      nodeBuilder.SetCreate(dtAnim::AnimNodeBuilder::CreateFunc(&nodeBuilder, &dtAnim::AnimNodeBuilder::CreateSoftware));
   }
   QSettings settings("MOVES", "Animation Viewer");
   QStringList files = settings.value("recentFileList").toStringList();
   LoadCharFile(files.first());
}

////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnToggleLODScaleToolbar()
{
   if (mLODScaleToolbar->isHidden())
   {
      mLODScaleToolbar->show();
   }
   else
   {
      mLODScaleToolbar->hide();
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnToggleScalingToolbar()
{
   if (mScalingToolbar->isHidden())
   {
      mScalingToolbar->show();
   }
   else
   {
      mScalingToolbar->hide();
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnToggleLightingToolbar()
{

}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::UpdateRecentFileActions()
{
   QSettings settings("MOVES", "Animation Viewer");
   QStringList files = settings.value("recentFileList").toStringList();

   int numRecentFiles = qMin(files.size(), 5);

   for (int actionIndex = 0; actionIndex < numRecentFiles; ++actionIndex)
   {
      QString text = tr("&%1 %2").arg(actionIndex + 1).arg(QFileInfo(files[actionIndex]).fileName());
      mRecentFilesAct[actionIndex]->setText(text);
      mRecentFilesAct[actionIndex]->setData(files[actionIndex]);
      mRecentFilesAct[actionIndex]->setVisible(true);
   }

   for (int fileIndex = numRecentFiles; fileIndex < 5; ++fileIndex)
   {
      mRecentFilesAct[fileIndex]->setVisible(false);
   }

}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::SetCurrentFile(const QString& filename)
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

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OpenRecentFile()
{
   QAction* action = qobject_cast<QAction*>(sender());

   if (action)
   {
      LoadCharFile(action->data().toString());
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnItemChanged(QTableWidgetItem* item)
{
   if (item->column() == 1 || item->column() == 2)
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

////////////////////////////////////////////////////////////////////////////////
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
      emit StartAnimation(mAnimListWidget->item(row,0)->data(Qt::UserRole).toUInt(), weight, delay);
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnStopAnimation(int row)
{
   float delay = 0.f;

   if (mAnimListWidget->item(row, 2))
   {
      delay = mAnimListWidget->item(row, 2)->text().toFloat();
   }

   if (mAnimListWidget->item(row,0))
   {
      emit StopAnimation(mAnimListWidget->item(row,0)->data(Qt::UserRole).toUInt(), delay);
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnItemDoubleClicked(QTableWidgetItem* item)
{
   if (item->column() == 0)
   {
      OnStartAction(item->row());
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnSelectModeGrab()
{
   mPoseMeshViewer->SetMode(PoseMeshView::MODE_GRAB);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnSelectModeBlendPick()
{
   mPoseMeshViewer->SetMode(PoseMeshView::MODE_BLEND_PICK);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnSelectModeErrorPick()
{
   mPoseMeshViewer->SetMode(PoseMeshView::MODE_ERROR_PICK);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnToggleDisplayEdges(bool shouldDisplay)
{
   mPoseMeshViewer->SetDisplayEdges(shouldDisplay);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnToggleDisplayError(bool shouldDisplay)
{
   mPoseMeshViewer->SetDisplayError(shouldDisplay);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnToggleFlipVertical()
{
   static float verticalScalar = 1.0f;
   verticalScalar *= -1.0f;

   const std::vector<PoseMeshItem*>& itemList = mPoseMeshScene->GetPoseMeshItemList();

   // Flip the +/- directions of the vertical axis
   for (size_t itemIndex = 0; itemIndex < itemList.size(); ++itemIndex)
   {
      itemList[itemIndex]->SetVerticalScale(verticalScalar);
   }

   mPoseMeshScene->update();
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnToggleFlipHorizontal()
{
   static float horizontalScalar = 1.0f;
   horizontalScalar *= -1.0f;

   const std::vector<PoseMeshItem*>& itemList = mPoseMeshScene->GetPoseMeshItemList();

   // Flip the +/- directions of the horizontal axis
   for (size_t itemIndex = 0; itemIndex < itemList.size(); ++itemIndex)
   {
      itemList[itemIndex]->SetHorizontalScale(horizontalScalar);
   }

   mPoseMeshScene->update();
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnStartAction(int row)
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

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnDisplayError(const QString& msg)
{
   QMessageBox::warning(this, "AnimationViewer", msg);
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
   //accept local file drops that have a .xml extension
   if (event->mimeData()->hasUrls())
   {
      QList<QUrl> urls = event->mimeData()->urls();
      if (urls[0].toLocalFile().toLower().endsWith(".xml"))
      {
         event->acceptProposedAction();
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::dropEvent(QDropEvent* event)
{
   QList<QUrl> urls = event->mimeData()->urls();
   if (!urls.empty())
   {
      QString filename = urls[0].toLocalFile();

      LoadCharFile(filename);

      event->acceptProposedAction();
   }
}

//////////////////////////////////////////////////////////////////////////
bool MainWindow::IsAnimNodeBuildingUsingHW() const
{
   //Not really a great way to see what method we're using.  A better way would
   //be to query a hardware skinning object to see if its being used or something.
   //This'll have to do for now.
   dtAnim::AnimNodeBuilder& nodeBuilder = dtAnim::Cal3DDatabase::GetInstance().GetNodeBuilder();

   return (nodeBuilder.SupportsHardware());
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::OnConfiged()
{
   //theoretically, everything is in place, the window is rendering, openGL 
   //context is valid, etc.
   mHardwareSkinningAction->setChecked(IsAnimNodeBuildingUsingHW());
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::OnSubMorphChanged(QTableWidgetItem* item)
{
   if (item->column() == 3) //the "weight" column
   {
      const float weight = item->text().toFloat();
      const int morphID = item->row();

      const int meshID = mSubMorphTargetListWidget->item(item->row(), 0)->text().toInt();
      const int subMeshID = mSubMorphTargetListWidget->item(item->row(), 1)->text().toInt();

      emit SubMorphTargetChanged(meshID, subMeshID, morphID, weight);
   }
}
