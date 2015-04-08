#include "MainWindow.h"
#include "AnimationTableWidget.h"
#include "PoseMeshView.h"
#include "PoseMeshScene.h"
#include "PoseMeshProperties.h"
#include "PoseMeshItem.h"
#include "Viewer.h"
#include "ResourceDelegates.h"
#include "ResourceDialogs.h"
#include <dtQt/osggraphicswindowqt.h>
#include <dtQt/projectcontextdialog.h>

#include <osg/Geode> ///needed for the node builder
#include <dtAnim/cal3ddatabase.h>
#include <dtAnim/animnodebuilder.h>
#include <dtAnim/animationwrapper.h>
#include <dtAnim/chardrawable.h>
#include <dtCore/deltawin.h>
#include <dtCore/shadermanager.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>

#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QtGui/QComboBox>
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
#include <QtGui/QDragEnterEvent>
#include <QtGui/QTabWidget>
#include <QtGui/QListWidget>
#include <QtGui/QLabel>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QProgressBar>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QItemDelegate>
#include <QtGui/QTextEdit>
#include <QtCore/QUrl>
#include <QtOpenGL/QGLWidget>

#include <QtGui/QStandardItemModel>
#include <QtGui/QStandardItem>

#include <QtGui/QGraphicsEllipseItem>
#include <cassert>



////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////////////
dtUtil::RefString APP_SETTINGS_NAME("delta3d");
dtUtil::RefString APP_TITLE("Animation Viewer");
dtUtil::RefString SETTING_PROJECT_CONTEXT("projectContextPath");
dtUtil::RefString SETTING_SHADERDEFS_FILE("shaderDefsFile");



////////////////////////////////////////////////////////////////////////////////
// Helper Function
void ClearTableWidget(QTableWidget& widget)
{
   while (widget.rowCount()>0)
   {
      widget.removeRow(0);
   }
}

////////////////////////////////////////////////////////////////////////////////
// Helper Function
void ClearStandardItemModel(QStandardItemModel& widget)
{
   while (widget.rowCount()>0)
   {
      widget.removeRow(0);
   }
}

////////////////////////////////////////////////////////////////////////////////
// Helper Function
void ClearTreeWidgetItem(QTreeWidgetItem& item)
{
   while (item.childCount() > 0)
   {
      QTreeWidgetItem* childItem = item.child(0);
      item.removeChild(childItem);
      delete childItem;
   }
}

////////////////////////////////////////////////////////////////////////////////
// Helper Function
void AddItemsToTreeWidgetItem(QTreeWidgetItem& item,
   const dtAnim::Cal3DModelData& modelData, dtAnim::Cal3DModelData::CalFileType fileType)
{
   dtAnim::Cal3DModelData::StrArray nameList;
   modelData.GetObjectNameListForFileTypeSorted(fileType, nameList);

   std::string curName;
   std::string curFile;
   QTreeWidgetItem* curItem = NULL;
   dtAnim::Cal3DModelData::StrArray::const_iterator curIter = nameList.begin();
   dtAnim::Cal3DModelData::StrArray::const_iterator endIter = nameList.end();
   for (; curIter != endIter; ++curIter)
   {
      curName = *curIter;
      curFile = modelData.GetFileForObjectName(fileType, curName);

      QString labelName(curName.c_str());
      QString labelFile(curFile.c_str());
      curItem = new QTreeWidgetItem();
      curItem->setText(0, labelName);
      curItem->setText(1, labelFile);
      curItem->setFlags(curItem->flags() | Qt::ItemIsEditable);
      item.addChild(curItem);
   }
}



////////////////////////////////////////////////////////////////////////////////
// MAIN WINDOW CODE
////////////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow()
  : mExitAct(NULL)
  , mNewCharAct(NULL)
  , mLoadCharAct(NULL)
  , mSaveCharAct(NULL)
  , mCloseCharAction(NULL)
  , mToggleDockProperties(NULL)
  , mToggleDockResources(NULL)
  , mToggleDockTools(NULL)
  , mScaleFactorSpinner(NULL)
  , mAttachmentOffsetXSpinner(NULL)
  , mAttachmentOffsetYSpinner(NULL)
  , mAttachmentOffsetZSpinner(NULL)
  , mAttachmentRotXSpinner(NULL)
  , mAttachmentRotYSpinner(NULL)
  , mAttachmentRotZSpinner(NULL)
  , mCurrentAttachment("")
  , mAnimListWidget(NULL)
  , mMeshListWidget(NULL)
  , mSubMorphTargetListWidget(NULL)
  , mSubMorphAnimationListWidget(NULL)
  , mMaterialModel(NULL)
  , mMaterialView(NULL)
  , mPoseDock(NULL)
  , mPoseMeshViewer(NULL)
  , mPoseMeshScene(NULL)
  , mPoseMeshProperties(NULL)
  , mFileLabel(NULL)
  , mFileAdd(NULL)
  , mFileTree(NULL)
  , mFileGroupSkel(NULL)
  , mFileGroupAnim(NULL)
  , mFileGroupMesh(NULL)
  , mFileGroupMat(NULL)
  , mFileGroupMorph(NULL)
  , mFileDelegate(NULL)
  , mObjectNameDelegate(NULL)
{
   resize(1024, 800);

   mAnimListWidget = new AnimationTableWidget(this);
   mAnimListWidget->setColumnCount(7);
   mAnimListWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

   connect(mAnimListWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(OnAnimationClicked(QTableWidgetItem*)));
   connect(mAnimListWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(OnItemChanged(QTableWidgetItem*)));
   connect(mAnimListWidget, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(OnItemDoubleClicked(QTableWidgetItem*)));

   QStringList headers;
   headers << "Name" << "Weight (L)" << "Delay (L)" << "Delay In (A)" << "Delay Out (A)" << "Mixer Blend" << "Frame Time";
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


   mSubMorphAnimationListWidget = new AnimationTableWidget(this);
   mSubMorphAnimationListWidget->setColumnCount(5);
   mSubMorphAnimationListWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

   connect(mSubMorphAnimationListWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(OnMorphAnimationClicked(QTableWidgetItem*)));
   connect(mSubMorphAnimationListWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(OnMorphItemChanged(QTableWidgetItem*)));
   connect(mSubMorphAnimationListWidget, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(OnMorphItemDoubleClicked(QTableWidgetItem*)));

   {
      QStringList headers;
      headers << "Name" << "Weight" << "Delay In" << "Delay Out" << "Mixer Blend";
      mSubMorphAnimationListWidget->setHorizontalHeaderLabels(headers);
   }


   CreateActions();
   CreateMenus();
   statusBar();
   CreateToolbars();
   CreateDockWidgets();

   QWidget* glParent = new QWidget();

   mCentralLayout = new QHBoxLayout(glParent);
   mCentralLayout->setMargin(0);
   glParent->setLayout(mCentralLayout);
   setCentralWidget(glParent);


   //accept drag & drop operations
   setAcceptDrops(true);

   SetCurrentFile("");
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
   viewMenu->addAction(mBoneLabelAction);
   viewMenu->addSeparator();
   viewMenu->addAction(mToggleDockProperties);
   viewMenu->addAction(mToggleDockResources);
   viewMenu->addAction(mToggleDockTools);

   windowMenu->addAction(mNewCharAct);
   windowMenu->addAction(mLoadCharAct);
   windowMenu->addAction(mSaveCharAct);
   windowMenu->addSeparator();
   windowMenu->addAction(mCloseCharAction);
   windowMenu->addSeparator();

   QAction* toggleShadeToolbarAction = toolBarMenu->addAction("Shading toolbar");

   toggleShadeToolbarAction->setCheckable(true);
   toggleShadeToolbarAction->setChecked(true);

   connect(toggleShadeToolbarAction, SIGNAL(triggered()), this, SLOT(OnToggleShadingToolbar()));

   for (int actionIndex = 0; actionIndex < 5; ++actionIndex)
   {
      windowMenu->addAction(mRecentFilesAct[actionIndex]);
   }

   windowMenu->addSeparator();
   QAction* loadShaderDef = new QAction(tr("Load Shader Defs..."), this);
   loadShaderDef->setStatusTip(tr("Set the shader defs that may contain custom character shaders."));
   connect(loadShaderDef, SIGNAL(triggered()), this, SLOT(OnLoadShaderDefinition()));
   windowMenu->addAction(loadShaderDef);

   menuBar()->addSeparator();
   windowMenu->addSeparator();
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

   mNewCharAct = new QAction(tr("&New"), this);
   mNewCharAct->setShortcut(tr("Ctrl+N"));
   mNewCharAct->setStatusTip(tr("Create a new character file."));
   connect(mNewCharAct, SIGNAL(triggered()), this, SLOT(OnNewCharFile()));

   mLoadCharAct = new QAction(tr("&Open..."), this);
   mLoadCharAct->setShortcut(tr("Ctrl+O"));
   mLoadCharAct->setStatusTip(tr("Open an existing character file."));
   connect(mLoadCharAct, SIGNAL(triggered()), this, SLOT(OnOpenCharFile()));

   mSaveCharAct = new QAction(tr("&Save..."), this);
   mSaveCharAct->setShortcut(tr("Ctrl+S"));
   mSaveCharAct->setStatusTip(tr("Save to an existing or new character file."));
   connect(mSaveCharAct, SIGNAL(triggered()), this, SLOT(OnSaveCharFile()));

   for (int actionIndex = 0; actionIndex < 5; actionIndex++)
   {
      mRecentFilesAct[actionIndex] = new QAction(this);
      mRecentFilesAct[actionIndex]->setVisible(false);
      connect(mRecentFilesAct[actionIndex], SIGNAL(triggered()), this, SLOT(OpenRecentFile()));
   }

   mCloseCharAction = new QAction(tr("&Close"), this);
   mCloseCharAction->setStatusTip(tr("Close the character file."));
   connect(mCloseCharAction, SIGNAL(triggered()), this, SLOT(OnCloseCharFile()));

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

   mBoneLabelAction = new QAction(tr("Use Bone Labeling"), this);
   mBoneLabelAction->setCheckable(true);
   mBoneLabelAction->setChecked(true);

   mToggleDockProperties = new QAction(tr("Properties"), this);
   mToggleDockProperties->setCheckable(true);
   mToggleDockProperties->setChecked(true);

   mToggleDockResources = new QAction(tr("Resources"), this);
   mToggleDockResources->setCheckable(true);
   mToggleDockResources->setChecked(true);

   mToggleDockTools = new QAction(tr("Tools"), this);
   mToggleDockTools->setCheckable(true);
   mToggleDockTools->setChecked(true);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::CreateToolbars()
{
   mShadingToolbar = addToolBar("Shading toolbar");
   mShadingToolbar->setToolTip("Shading toolbar");

   mShadingToolbar->addAction(mWireframeAction);
   mShadingToolbar->addAction(mShadedAction);
   mShadingToolbar->addAction(mShadedWireAction);
   mShadingToolbar->addSeparator();
   mShadingToolbar->addAction(mBoneBasisAction);

   //QIcon diffuseIcon(":/images/diffuseLight.png");
   //QIcon pointLightIcon(":/images/pointLight.png");

   //mLightingToolbar->addAction(diffuseIcon, "Diffuse Light");
   //mLightingToolbar->addAction(pointLightIcon, "Point Light");
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
void MainWindow::OnNewCharFile()
{
   OnClearCharacterData();

   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

   emit NewFile();

   QApplication::restoreOverrideCursor();
   statusBar()->showMessage(tr("New file created"), 2000);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnOpenCharFile()
{
   QString filename = QFileDialog::getOpenFileName(this,
      tr("Load Character File"), ".", tr("Characters (*.xml; *.dtchar)") );

   if (!filename.isEmpty())
   {
      LoadCharFile(filename);
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnSaveCharFile()
{
   QString filename = QFileDialog::getSaveFileName(this,
      tr("Save Character File"), ".", tr("Characters (*.xml; *.dtchar)"));

   if (!filename.isEmpty())
   {
      SaveCharFile(filename);
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::LoadCharFile(const QString& filename)
{
   if (dtUtil::FileUtils::GetInstance().FileExists(filename.toStdString()))
   {
      OnClearCharacterData();

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
void MainWindow::SaveCharFile(const QString& filename)
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

   emit FileToSave(filename);

   QApplication::restoreOverrideCursor();
   statusBar()->showMessage(tr("File saved"), 2000);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::ReloadCharFile()
{
   // Keep track of the current file since the
   // current variable may change during reload.
   std::string curFile(mCurrentFile);

   mFileTree->setUpdatesEnabled(false);
   emit ReloadFile();
   mFileTree->setUpdatesEnabled(true);
   emit ClearTempFile();

   // Reset the current file.
   mCurrentFile = curFile;
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::LoadAttachment(const QString& filename)
{
   if(mCurrentAttachment == filename)
   {
      return;
   }

   if (dtUtil::FileUtils::GetInstance().FileExists(filename.toStdString()))
   {
      emit AttachmentToLoad(filename);
      mCurrentAttachment = filename;
      statusBar()->showMessage(tr("Attachment loaded"), 2000);
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

   { //frame time
      QTableWidgetItem* item = new QTableWidgetItem(tr("0.0"));
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      mAnimListWidget->setItem(id, 6, item);
   }

   mAnimListWidget->resizeColumnToContents(0);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnNewMorphAnimation(unsigned int id, const QString& animationName,
                                unsigned int trackCount, unsigned int keyframes,
                                float duration)
{
   mSubMorphAnimationListWidget->insertRow(mSubMorphAnimationListWidget->rowCount());

   { //name
      QTableWidgetItem* item = new QTableWidgetItem(animationName);
      item->setCheckState(Qt::Unchecked);
      item->setData(Qt::UserRole, id);
      item->setData(Qt::UserRole+1, trackCount);
      item->setData(Qt::UserRole+2, keyframes);
      item->setData(Qt::UserRole+3, duration);
      item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      mSubMorphAnimationListWidget->setItem(id, 0, item);
   }

   { //weight
      QTableWidgetItem* item = new QTableWidgetItem(tr("1.0"));
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
      mSubMorphAnimationListWidget->setItem( id, 1, item );
   }

   { //delay in
      QTableWidgetItem* item = new QTableWidgetItem(tr("0.5"));
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
      mSubMorphAnimationListWidget->setItem(id, 2, item);
   }

   { //delay out
      QTableWidgetItem* item = new QTableWidgetItem(tr("0.5"));
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
      mSubMorphAnimationListWidget->setItem(id, 3, item);
   }

   { //mixer blend
      QProgressBar* mixerBlend = new QProgressBar;
      mixerBlend->setMaximum(100);
      mixerBlend->setMinimum(0);
      mixerBlend->setValue(0);

      mSubMorphAnimationListWidget->setCellWidget(id, 4, mixerBlend);
   }

   mSubMorphAnimationListWidget->resizeColumnToContents(0);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnNewMesh(int meshID, const QString& meshName, const std::vector<std::string>& boneNames)
{
   QListWidgetItem *meshItem = new QListWidgetItem();
   meshItem->setText(meshName);
   meshItem->setData(Qt::UserRole, meshID);

   meshItem->setFlags(Qt::ItemIsSelectable |
                      Qt::ItemIsUserCheckable |
                      Qt::ItemIsEnabled);

   meshItem->setCheckState(Qt::Checked);

   mMeshListWidget->addItem(meshItem);

   mAttachmentParent->clear();
   std::vector<std::string>::const_iterator i;
   for(i = boneNames.begin(); i != boneNames.end(); ++i)
   {
      mAttachmentParent->addItem(i->c_str());
   }
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
   QAction* lookAtCameraAction = actionGroup->addAction(modeBlendIcon, "Look At Camera.");

   poseModesToolbar->addAction(grabAction);
   poseModesToolbar->addAction(pickAction);

   // Not full implemented so leave out
   // poseModesToolbar->addAction(lookAtCameraAction);
   //poseModesToolbar->addAction(errorAction);

   grabAction->setCheckable(true);
   pickAction->setCheckable(true);
   errorAction->setCheckable(true);
   lookAtCameraAction->setCheckable(true);

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
   connect(lookAtCameraAction, SIGNAL(triggered()), this, SLOT(OnSelectLookAtCamera()));

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
void MainWindow::OnCharacterDataLoaded(dtAnim::Cal3DModelData* modelData,
                                       dtAnim::Cal3DModelWrapper* modelWrapper)
{
   using namespace dtAnim;

   // Update the resource tree widget.
   mFileDelegate->SetCharModelData(modelData);
   mFileDelegate->SetCharModelWrapper(modelWrapper);
   mObjectNameDelegate->SetCharModelData(modelData);
   mObjectNameDelegate->SetCharModelWrapper(modelWrapper);
   mFileLabel->setText(modelData->GetModelName().c_str());
   AddItemsToTreeWidgetItem(*mFileGroupSkel, *modelData, Cal3DModelData::SKEL_FILE);
   AddItemsToTreeWidgetItem(*mFileGroupAnim, *modelData, Cal3DModelData::ANIM_FILE);
   AddItemsToTreeWidgetItem(*mFileGroupMesh, *modelData, Cal3DModelData::MESH_FILE);
   AddItemsToTreeWidgetItem(*mFileGroupMat, *modelData, Cal3DModelData::MAT_FILE);
   AddItemsToTreeWidgetItem(*mFileGroupMorph, *modelData, Cal3DModelData::MORPH_FILE);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnBlendUpdate(const std::vector<std::pair<float, float> >& animWeightTimeList, const std::vector<float>& morphWeightList)
{
   // Animation Progress bars.
   for (size_t rowIndex = 0; rowIndex < animWeightTimeList.size(); ++rowIndex)
   {
      // Show progress as a whole number
      float newValue = animWeightTimeList[rowIndex].first * 100.0f;
      float time =  animWeightTimeList[rowIndex].second;

      QProgressBar* meter = (QProgressBar*)mAnimListWidget->cellWidget(rowIndex, 5);

      // If posemeshes fail to load, the list widget will not
      // have been populated yet and this will be NULL
      if (meter)
      {
         meter->setValue(newValue);

         if (mAnimListWidget->item(rowIndex, 0)->checkState() == Qt::Checked)
         {
            // Update the weight display only when the box is checked
            // This will allow a user to manually enter a weight while unchecked
            disconnect(mAnimListWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(OnItemChanged(QTableWidgetItem*)));
            mAnimListWidget->item(rowIndex, 1)->setData(Qt::DisplayRole, QString("%1").arg(animWeightTimeList[rowIndex].first));
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
      mAnimListWidget->item(rowIndex, 6)->setText(QString::number(time));
   }

   // Morph Animation Progress bars.
   for (size_t rowIndex = 0; rowIndex < morphWeightList.size(); ++rowIndex)
   {
      // Show progress as a whole number
      float newValue = morphWeightList[rowIndex] * 100.0f;

      QProgressBar* meter = (QProgressBar*)mSubMorphAnimationListWidget->cellWidget(rowIndex, 4);
      meter->setValue(newValue);
   }

   // Allow the IK tab to update it's blend display if it exists
   if (mPoseMeshProperties)
   {
      mPoseMeshProperties->OnBlendUpdate(animWeightTimeList);
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
void MainWindow::OnMorphAnimationClicked(QTableWidgetItem* item)
{
   if (item->column() != 0) return;

   if (item->checkState() == Qt::Checked)
   {
      //item->setCheckState(Qt::Unchecked);
      //emit StopAnimation( item->data(Qt::UserRole).toUInt() );

      //OnStartAnimation(item->row());
   }
   else
   {
      //item->setCheckState(Qt::Checked);
      //emit StartAnimation( item->data(Qt::UserRole).toUInt() );

      //OnStopAnimation(item->row());
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
   mScaleFactorSpinner->blockSignals(true);
   mScaleFactorSpinner->setValue(1.0);
   mScaleFactorSpinner->blockSignals(false);
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
   QSettings settings(APP_SETTINGS_NAME.c_str(), APP_TITLE.c_str());
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


/////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnToggleLightingToolbar()
{

}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::UpdateRecentFileActions()
{
   QSettings settings(APP_SETTINGS_NAME.c_str(), APP_TITLE.c_str());
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
      setWindowTitle(tr(APP_TITLE.c_str()));
      mCloseCharAction->setEnabled(false);
      return;
   }

   mCurrentFile = filename.toStdString();

   setWindowTitle(tr("%1 - %2").arg(QFileInfo(filename).fileName()).arg(tr(APP_TITLE.c_str())));
   mCloseCharAction->setEnabled(true);

   QSettings settings(APP_SETTINGS_NAME.c_str(), APP_TITLE.c_str());
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

//////////////////////////////////////////////////////////////////////////
void MainWindow::OnStartMorphAnimation(int row, bool looping)
{
   float weight = 1.0f;
   float delayIn = 0.0f;
   float delayOut = 0.0f;

   if (mSubMorphAnimationListWidget->item(row, 1))
   {
      weight = mSubMorphAnimationListWidget->item(row, 1)->text().toFloat();
   }

   if (mSubMorphAnimationListWidget->item(row, 2))
   {
      delayIn = mSubMorphAnimationListWidget->item(row, 2)->text().toFloat();
   }

   if (mSubMorphAnimationListWidget->item(row, 3))
   {
      delayOut = mSubMorphAnimationListWidget->item(row, 3)->text().toFloat();
   }

   emit PlayMorphAnimation(row, weight, delayIn, delayOut, looping);
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::OnStopMorphAnimation(int row, float delay)
{
   emit StopMorphAnimation(row, delay);
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
void MainWindow::OnMorphItemChanged(QTableWidgetItem* item)
{
   if (item->column() <= 2)
   {
      if (mSubMorphAnimationListWidget->item(item->row(),0)->checkState() == Qt::Checked)
      {
         OnStartMorphAnimation(item->row(), true);
      }
      else
      {
         float delayOut = 0.0f;
         if (mSubMorphAnimationListWidget->item(item->row(), 3))
         {
            delayOut = mSubMorphAnimationListWidget->item(item->row(), 3)->text().toFloat();
         }

         OnStopMorphAnimation(item->row(), delayOut);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnMorphItemDoubleClicked(QTableWidgetItem* item)
{
   if (item->column() == 0)
   {
      OnStartMorphAnimation(item->row(), mSubMorphAnimationListWidget->item(item->row(),0)->checkState() == Qt::Checked);
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
void MainWindow::OnSelectLookAtCamera()
{

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
      QString fileName(urls[0].toLocalFile().toLower());
      if (fileName.endsWith(".xml") || fileName.endsWith(".dtchar"))
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

//////////////////////////////////////////////////////////////////////////
void MainWindow::OnCloseCharFile()
{
   SetCurrentFile("");

   emit UnloadFile();
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::OnClearCharacterData()
{
   //wipe out previous mesh, animation, material ui's

   //mAnimListWidget->clear(); //note, this also removes the header items
   mMeshListWidget->clear();

   // Make sure we start fresh
   DestroyPoseResources();

   ClearTableWidget(*mAnimListWidget);
   ClearTableWidget(*mSubMorphTargetListWidget);
   ClearTableWidget(*mSubMorphAnimationListWidget);
   ClearStandardItemModel(*mMaterialModel);

   // reset the scale spinbox
   mScaleFactorSpinner->setValue(1.0f);

   // Clear resource information.
   mFileDelegate->SetCharModelData(NULL);
   mObjectNameDelegate->SetCharModelData(NULL);
   mFileLabel->setText("");
   ClearTreeWidgetItem(*mFileGroupSkel);
   ClearTreeWidgetItem(*mFileGroupAnim);
   ClearTreeWidgetItem(*mFileGroupMesh);
   ClearTreeWidgetItem(*mFileGroupMat);
   ClearTreeWidgetItem(*mFileGroupMorph);

   mCurrentFile = "";
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::OnLoadAttachment()
{
   QString filename = QFileDialog::getOpenFileName(this,
      tr("Load Attachment Mesh"), ".", tr("Meshes (*.osg *.ive *.dae *.3ds *.obj)") );

   if (!filename.isEmpty())
   {
      LoadAttachment(filename);
   }
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::OnChangeAttachmentSettings()
{
   float x = mAttachmentOffsetXSpinner->value();
   float y = mAttachmentOffsetYSpinner->value();
   float z = mAttachmentOffsetZSpinner->value();
   std::string boneName = mAttachmentParent->currentText().toStdString();

   float rx = mAttachmentRotXSpinner->value();
   float ry = mAttachmentRotYSpinner->value();
   float rz = mAttachmentRotZSpinner->value();
   emit AttachmentSettingsChanged(boneName, x, y, z, rx, ry, rz);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnResourceEditStart(int fileType, const std::string& objectName)
{
   using namespace dtAnim;

   // Time to update character and UI states prior to an object resource change.

   Cal3DModelData::CalFileType calFileType = Cal3DModelData::CalFileType(fileType);
   switch (calFileType)
   {
   case Cal3DModelData::SKEL_FILE:
      // TODO:
      break;

   case Cal3DModelData::ANIM_FILE:
      // TODO:
      break;

   case Cal3DModelData::MESH_FILE:
      // TODO:
      break;

   case Cal3DModelData::MAT_FILE:
      // TODO:
      break;

   case Cal3DModelData::MORPH_FILE:
      // TODO:
      break;

   default:
      break;
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnResourceEditEnd(int fileType, const std::string& objectName)
{
   using namespace dtAnim;

   bool reload = false;

   Cal3DModelData::CalFileType calFileType = Cal3DModelData::CalFileType(fileType);
   switch (calFileType)
   {
   case Cal3DModelData::SKEL_FILE:
      reload = true;
      break;

   case Cal3DModelData::ANIM_FILE:
      // Reload not necessary since animations are hot-swappable.
      break;

   case Cal3DModelData::MESH_FILE:
      reload = true;
      break;

   case Cal3DModelData::MAT_FILE:
      reload = true;
      break;

   case Cal3DModelData::MORPH_FILE:
      reload = true;
      break;

   default:
      break;
   }

   if (reload)
   {
      ReloadCharFile();
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnResourceNameChanged(int fileType,
   const std::string& oldName, const std::string& newName) const
{
   using namespace dtAnim;

   Cal3DModelData::CalFileType calFileType = Cal3DModelData::CalFileType(fileType);
   switch (calFileType)
   {
   case Cal3DModelData::SKEL_FILE:
      break;

   case Cal3DModelData::ANIM_FILE:
      {
         dtAnim::Cal3DModelData* modelData = mObjectNameDelegate->GetCharModelData();
         const AnimationWrapper* wrapper = modelData->GetAnimationWrapperByName(newName);
         if (wrapper != NULL)
         {
            int animId = wrapper->GetID();
            if (animId >= 0)
            {
               QTableWidgetItem* item = mAnimListWidget->item(animId, 0);
               QString qName(newName.c_str());
               item->setText(qName);
            }
         }
      }
      break;

   case Cal3DModelData::MESH_FILE:
      // TODO:
      break;

   case Cal3DModelData::MAT_FILE:
      // TODO:
      break;

   case Cal3DModelData::MORPH_FILE:
      // TODO:
      break;

   default:
      break;
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnResourceAdd()
{
   ResAddDialog* dialog = new ResAddDialog(this);
   dialog->SetModelData(mFileDelegate->GetCharModelData());

   int retCode = dialog->exec();

   // If OK...
   bool reload = false;
   if (retCode == QDialog::Accepted && dialog->IsDataChanged())
   {
      reload = true;
   }

   delete dialog;

   if (reload)
   {
      emit ReloadFile();
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnResourceRemoved(int fileType, const std::string& objectName)
{
   using namespace dtAnim;

   QTreeWidgetItem* item = NULL;

   // Get the resource file group widget that is affected by the change.
   Cal3DModelData::CalFileType calFileType = Cal3DModelData::CalFileType(fileType);
   switch (calFileType)
   {
   case Cal3DModelData::SKEL_FILE:
      item = mFileGroupSkel;
      break;

   case Cal3DModelData::ANIM_FILE:
      item = mFileGroupAnim;
      break;

   case Cal3DModelData::MESH_FILE:
      item = mFileGroupMesh;
      break;

   case Cal3DModelData::MAT_FILE:
      item = mFileGroupMat;
      break;

   case Cal3DModelData::MORPH_FILE:
      item = mFileGroupMorph;
      break;

   default:
      break;
   }

   Cal3DModelData* modelData = mFileDelegate->GetCharModelData();
   if (modelData != NULL && item != NULL)
   {
      ClearTreeWidgetItem(*item);
      AddItemsToTreeWidgetItem(*item, *modelData, calFileType);
   }

   // TODO: Update other affected UIs.
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnError(const std::string& title, const std::string& message)
{
   QString qTitle(title.c_str());
   QString qMessage(message.c_str());
   QMessageBox msgBox;
   msgBox.setText(qTitle);
   msgBox.setInformativeText(qMessage);
   msgBox.setStandardButtons(QMessageBox::Ok);
   msgBox.setDefaultButton(QMessageBox::Ok);
   msgBox.exec();
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::SetViewer(Viewer* viewer)
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
void MainWindow::SetupConnectionsWithViewer()
{
   connect(this, SIGNAL(NewFile()), mViewer, SLOT(OnNewCharFile()));
   connect(this, SIGNAL(ReloadFile()), mViewer, SLOT(OnReloadCharFile()));
   connect(this, SIGNAL(FileToLoad(const QString&)), mViewer, SLOT(OnLoadCharFile(const QString&)));
   connect(this, SIGNAL(FileToSave(const QString&)), mViewer, SLOT(OnSaveCharFile(const QString&)));
   connect(this, SIGNAL(ClearTempFile()), mViewer, SLOT(OnClearTempFile()));
   connect(this, SIGNAL(AttachmentToLoad(const QString&)), mViewer, SLOT(OnLoadAttachmentFile(const QString&)));
   connect(this, SIGNAL(AttachmentSettingsChanged(const std::string&, float, float, float, float, float, float)),
           mViewer, SLOT(OnAttachmentSettingsChanged(const std::string&, float, float, float, float, float, float)));

   connect(this, SIGNAL(UnloadFile()), mViewer, SLOT(OnUnloadCharFile()) );
   connect(mViewer, SIGNAL(ClearCharacterData()), this, SLOT(OnClearCharacterData()));

   connect(mViewer, SIGNAL(AnimationLoaded(unsigned int,const QString &,unsigned int,unsigned int,float)),
      this, SLOT(OnNewAnimation(unsigned int,const QString&,unsigned int,unsigned int,float)));

   connect(mViewer, SIGNAL(MorphAnimationLoaded(unsigned int,const QString &,unsigned int,unsigned int,float)),
      this, SLOT(OnNewMorphAnimation(unsigned int,const QString&,unsigned int,unsigned int,float)));

   connect(mViewer, SIGNAL(MeshLoaded(int,const QString&, const std::vector<std::string>&)),
           this, SLOT(OnNewMesh(int,const QString&, const std::vector<std::string>&)));

   connect(mViewer, SIGNAL(PoseMeshesLoaded(const std::vector<dtAnim::PoseMesh*>&, dtAnim::CharDrawable*)),
      this, SLOT(OnPoseMeshesLoaded(const std::vector<dtAnim::PoseMesh*>&, dtAnim::CharDrawable*)));

   connect(mViewer, SIGNAL(MaterialLoaded(int,const QString&,const QColor&,const QColor&,const QColor&,float)),
      this, SLOT(OnNewMaterial(int,const QString&,const QColor&,const QColor&,const QColor&,float)));

   connect(mViewer, SIGNAL(CharacterDataLoaded(dtAnim::Cal3DModelData*, dtAnim::Cal3DModelWrapper*)),
      this, SLOT(OnCharacterDataLoaded(dtAnim::Cal3DModelData*, dtAnim::Cal3DModelWrapper*)));

   connect(this, SIGNAL(ShowMesh(int)), mViewer, SLOT(OnShowMesh(int)));
   connect(this, SIGNAL(HideMesh(int)), mViewer, SLOT(OnHideMesh(int)));

   connect(mViewer, SIGNAL(ErrorOccured(const QString&)), this, SLOT(OnDisplayError(const QString&)));

   connect(this, SIGNAL(StartAnimation(unsigned int,float,float)), mViewer, SLOT(OnStartAnimation(unsigned int,float,float)));
   connect(this, SIGNAL(StopAnimation(unsigned int,float)), mViewer, SLOT(OnStopAnimation(unsigned int,float)));
   connect(this, SIGNAL(StartAction(unsigned int,float,float)), mViewer, SLOT(OnStartAction(unsigned int,float,float)));
   connect(this, SIGNAL(LODScale_Changed(float)), mViewer, SLOT(OnLODScale_Changed(float)));
   connect(this, SIGNAL(SpeedChanged(float)), mViewer, SLOT(OnSpeedChanged(float)));
   connect(this, SIGNAL(ScaleFactorChanged(float)), mViewer, SLOT(OnScaleFactorChanged(float)));

   //connect(&mTimer, SIGNAL(timeout()), mViewer, SLOT(OnTimeout()));
   connect(mViewer, SIGNAL(BlendUpdate(const std::vector<std::pair<float, float> >&, const std::vector<float>&)),
            this, SLOT(OnBlendUpdate(const std::vector<std::pair<float, float> >&, const std::vector<float>&)));

   connect(this->mShadedAction, SIGNAL(triggered()), mViewer, SLOT(OnSetShaded()));
   connect(this->mWireframeAction, SIGNAL(triggered()), mViewer, SLOT(OnSetWireframe()));
   connect(this->mShadedWireAction, SIGNAL(triggered()), mViewer, SLOT(OnSetShadedWireframe()));
   connect(this->mBoneBasisAction, SIGNAL(toggled(bool)), mViewer, SLOT(OnSetBoneBasisDisplay(bool)));
   connect(this->mBoneLabelAction, SIGNAL(toggled(bool)), mViewer, SLOT(OnSetBoneLabelDisplay(bool)));

   connect(mViewer, SIGNAL(SubMorphTargetLoaded(int,int,int,const QString&)), this, SLOT(OnNewSubMorphTarget(int,int,int,const QString&)));
   connect(this, SIGNAL(SubMorphTargetChanged(int,int,int,float)), mViewer, SLOT(OnMorphChanged(int,int,int,float)));
   connect(this, SIGNAL(PlayMorphAnimation(int, float, float, float, bool)), mViewer, SLOT(OnPlayMorphAnimation(int, float, float, float, bool)));
   connect(this, SIGNAL(StopMorphAnimation(int, float)), mViewer, SLOT(OnStopMorphAnimation(int, float)));
   
   connect(mViewer, SIGNAL(SignalError(const std::string&, const std::string&)),
      this, SLOT(OnError(const std::string&, const std::string&)));
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::CreateDockWidget_Properties()
{
   mTabs = new QTabWidget(this);
   mTabs->addTab(mAnimListWidget, tr("Animations"));
   mTabs->addTab(mMeshListWidget, tr("Meshes"));
   mTabs->addTab(mMaterialView, tr("Materials"));
   mTabs->addTab(mSubMorphTargetListWidget, tr("SubMorphTargets"));
   mTabs->addTab(mSubMorphAnimationListWidget, tr("SubMorphAnimations"));

   QDockWidget* tabsDockWidget = new QDockWidget(tr("Properties"), this);
   tabsDockWidget->setWidget(mTabs);

   addDockWidget(Qt::BottomDockWidgetArea, tabsDockWidget);

   connect(mToggleDockProperties, SIGNAL(toggled(bool)), tabsDockWidget, SLOT(setVisible(bool)));
   connect(tabsDockWidget, SIGNAL(visibilityChanged(bool)), mToggleDockProperties, SLOT(setChecked(bool)));
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::CreateDockWidget_Tools()
{
   QDockWidget* toolsDockWidget = new QDockWidget(tr("Tools"), this);
   addDockWidget(Qt::LeftDockWidgetArea, toolsDockWidget);
   QWidget* toolWidget = new QWidget();
   toolsDockWidget->setWidget(toolWidget);

   connect(mToggleDockTools, SIGNAL(toggled(bool)), toolsDockWidget, SLOT(setVisible(bool)));
   connect(toolsDockWidget, SIGNAL(visibilityChanged(bool)), mToggleDockTools, SLOT(setChecked(bool)));

   QVBoxLayout* toolVLayout = new QVBoxLayout(toolWidget);

   QGridLayout* toolGridLayout = new QGridLayout();
   toolVLayout->addLayout(toolGridLayout);

   {  //LOD scaling
      QLabel* lodScaleLabel = new QLabel("LOD Scale");
      toolGridLayout->addWidget(lodScaleLabel, 0, 0);

      QDoubleSpinBox* lodScaleSpinner = new QDoubleSpinBox();
      lodScaleSpinner->setRange(0.01, 500.0);
      lodScaleSpinner->setSingleStep(0.01);
      lodScaleSpinner->setValue(1);
      lodScaleSpinner->setToolTip(tr("Level of Detail scale"));
      toolGridLayout->addWidget(lodScaleSpinner, 0, 1);
      connect(lodScaleSpinner, SIGNAL(valueChanged(double)), this, SLOT(OnLODScale_Changed(double)));
   }

   {  //animation playback scale
      QLabel* speenLabel = new QLabel("Playback Scale");
      toolGridLayout->addWidget(speenLabel, 1, 0);

      QDoubleSpinBox* speedSpinner = new QDoubleSpinBox();
      speedSpinner->setRange(0.0, 100.0);
      speedSpinner->setSingleStep(0.01);
      speedSpinner->setValue(1.0);
      speedSpinner->setToolTip(tr("Animation Speed Scale Factor"));
      toolGridLayout->addWidget(speedSpinner, 1, 1);
      connect(speedSpinner, SIGNAL(valueChanged(double)), this, SLOT(OnSpeedChanged(double)));
   }

   {  //size scale factor
      QGroupBox* box = new QGroupBox("Size Scale");
      toolVLayout->addWidget(box);

      QGridLayout* layout = new QGridLayout();
      box->setLayout(layout);

      mScaleFactorSpinner = new QDoubleSpinBox(this);
      mScaleFactorSpinner->setRange(0.001, 500.0);
      mScaleFactorSpinner->setSingleStep(0.01);
      mScaleFactorSpinner->setValue(1);
      mScaleFactorSpinner->setToolTip(tr("Scale Factor"));
      layout->addWidget(mScaleFactorSpinner, 0, 0);
      connect(mScaleFactorSpinner, SIGNAL(editingFinished()), this, SLOT(OnChangeScaleFactor()));

      QPushButton* applyScaleFactorButton = new QPushButton(tr("Apply"), this);
      applyScaleFactorButton->adjustSize();
      applyScaleFactorButton->setToolTip(tr("Apply scale factor"));
      layout->addWidget(applyScaleFactorButton, 0, 1);
      connect(applyScaleFactorButton, SIGNAL(clicked()), this, SLOT(OnChangeScaleFactor()));
   }

   {  //attachments
      QGroupBox* box = new QGroupBox("Attachments");
      toolVLayout->addWidget(box);

      QGridLayout* layout = new QGridLayout();
      box->setLayout(layout);

      QLabel* buttonLabel = new QLabel(tr("Load Model"));
      layout->addWidget(buttonLabel, 0, 0);

      QPushButton* loadAttachmentButton = new QPushButton("...");
      layout->addWidget(loadAttachmentButton, 0, 1);
      connect(loadAttachmentButton, SIGNAL(clicked()), this, SLOT(OnLoadAttachment()));

      QLabel* parentLabel = new QLabel("Bone Parent");
      layout->addWidget(parentLabel, 1, 0);

      mAttachmentParent = new QComboBox();
      mAttachmentParent->setToolTip(tr("Parent Name"));
      mAttachmentParent->setMinimumSize(QSize(150, 0));
      layout->addWidget(mAttachmentParent, 1, 1);
      connect(mAttachmentParent, SIGNAL(currentIndexChanged(int)), this, SLOT(OnChangeAttachmentSettings()));

      QLabel* xLabel = new QLabel("X");
      layout->addWidget(xLabel, 2, 0);

      mAttachmentOffsetXSpinner = new QDoubleSpinBox();
      mAttachmentOffsetXSpinner->setSingleStep(0.01);
      mAttachmentOffsetXSpinner->setToolTip(tr("Attachment Offset X"));
      mAttachmentOffsetXSpinner->setRange(-500.0, 500.0);
      layout->addWidget(mAttachmentOffsetXSpinner, 2, 1);
      connect(mAttachmentOffsetXSpinner, SIGNAL(valueChanged(double)), this, SLOT(OnChangeAttachmentSettings()));

      QLabel* yLabel = new QLabel("Y");
      layout->addWidget(yLabel, 3, 0);

      mAttachmentOffsetYSpinner = new QDoubleSpinBox();
      mAttachmentOffsetYSpinner->setSingleStep(0.01);
      mAttachmentOffsetYSpinner->setToolTip(tr("Attachment Offset Y"));
      mAttachmentOffsetYSpinner->setRange(-500.0, 500.0);
      layout->addWidget(mAttachmentOffsetYSpinner, 3, 1);
      connect(mAttachmentOffsetYSpinner, SIGNAL(valueChanged(double)), this, SLOT(OnChangeAttachmentSettings()));

      QLabel* zLabel = new QLabel("Z");
      layout->addWidget(zLabel, 4, 0);

      mAttachmentOffsetZSpinner = new QDoubleSpinBox();
      mAttachmentOffsetZSpinner->setSingleStep(0.01);
      mAttachmentOffsetZSpinner->setToolTip(tr("Attachment Offset Z"));
      mAttachmentOffsetZSpinner->setRange(-500.0, 500.0);
      layout->addWidget(mAttachmentOffsetZSpinner, 4, 1);
      connect(mAttachmentOffsetZSpinner, SIGNAL(valueChanged(double)), this, SLOT(OnChangeAttachmentSettings()));

      QLabel* xRotLabel = new QLabel("X Rot");
      layout->addWidget(xRotLabel, 5, 0);

      mAttachmentRotXSpinner = new QDoubleSpinBox();
      mAttachmentRotXSpinner->setSingleStep(0.01);
      mAttachmentRotXSpinner->setToolTip(tr("Rotation Axis X"));
      mAttachmentRotXSpinner->setRange(-180.0, 180.0);
      layout->addWidget(mAttachmentRotXSpinner, 5, 1);
      connect(mAttachmentRotXSpinner, SIGNAL(valueChanged(double)), this, SLOT(OnChangeAttachmentSettings()));

      QLabel* yRotLabel = new QLabel("Y Rot");
      layout->addWidget(yRotLabel, 6, 0);

      mAttachmentRotYSpinner = new QDoubleSpinBox(this);
      mAttachmentRotYSpinner->setSingleStep(0.01);
      mAttachmentRotYSpinner->setToolTip(tr("Rotation Axis Y"));
      mAttachmentRotYSpinner->setRange(-180.0, 180.0);
      layout->addWidget(mAttachmentRotYSpinner, 6, 1);
      connect(mAttachmentRotYSpinner, SIGNAL(valueChanged(double)), this, SLOT(OnChangeAttachmentSettings()));

      QLabel* zRotLabel = new QLabel("Z Rot");
      layout->addWidget(zRotLabel, 7, 0);

      mAttachmentRotZSpinner = new QDoubleSpinBox(this);
      mAttachmentRotZSpinner->setSingleStep(0.01);
      mAttachmentRotZSpinner->setToolTip(tr("Rotation Axis Z"));
      mAttachmentRotZSpinner->setRange(-180.0, 180.0);
      layout->addWidget(mAttachmentRotZSpinner, 7, 1);
      connect(mAttachmentRotZSpinner, SIGNAL(valueChanged(double)), this, SLOT(OnChangeAttachmentSettings()));
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::CreateDockWidget_Resources()
{
   QDockWidget* resDockWidget = new QDockWidget(tr("Resources"), this);
   addDockWidget(Qt::RightDockWidgetArea, resDockWidget);
   QWidget* resWidget = new QWidget();
   resDockWidget->setWidget(resWidget);

   connect(mToggleDockResources, SIGNAL(toggled(bool)), resDockWidget, SLOT(setVisible(bool)));
   connect(resDockWidget, SIGNAL(visibilityChanged(bool)), mToggleDockResources, SLOT(setChecked(bool)));

   QVBoxLayout* resVLayout = new QVBoxLayout(resWidget);

   QGridLayout* resGridLayout = new QGridLayout();
   resVLayout->addLayout(resGridLayout);

   QColor bgColor(200, 200, 200);
   QIcon iconSkel(":/images/fileIconSkel.png");
   QIcon iconAnim(":/images/fileIconAnim.png");
   QIcon iconMesh(":/images/fileIconMesh.png");
   QIcon iconMat(":/images/fileIconMat.png");
   QIcon iconMorph(":/images/fileIconMorph.png");
   QIcon iconAdd(":/images/add.png");

   mFileLabel = new QLabel("");
   resGridLayout->addWidget(mFileLabel, 0, 0);

   mFileAdd = new QPushButton(resDockWidget);
   mFileAdd->setIcon(iconAdd);
   resGridLayout->addWidget(mFileAdd, 0, 1);

   mFileTree = new QTreeWidget();
   QStringList labels;
   labels << "Object Name" << "File";
   mFileTree->setHeaderLabels(labels);
   resGridLayout->addWidget(mFileTree, 1, 0, 1, 2);

   mObjectNameDelegate = new ObjectNameItemDelegate(mFileTree);
   mFileTree->setItemDelegateForColumn(0, mObjectNameDelegate);
   mFileDelegate = new FileItemDelegate(mFileTree);
   mFileTree->setItemDelegateForColumn(1, mFileDelegate);

   mFileGroupSkel = new QTreeWidgetItem();
   mFileGroupSkel->setIcon(0, iconSkel);
   mFileGroupSkel->setText(0, "Skeleton");
   mFileGroupSkel->setBackgroundColor(0, bgColor);
   mFileGroupSkel->setBackgroundColor(1, bgColor);

   mFileGroupAnim = new QTreeWidgetItem();
   mFileGroupAnim->setIcon(0, iconAnim);
   mFileGroupAnim->setText(0, "Animations");
   mFileGroupAnim->setBackgroundColor(0, bgColor);
   mFileGroupAnim->setBackgroundColor(1, bgColor);

   mFileGroupMesh = new QTreeWidgetItem();
   mFileGroupMesh->setIcon(0, iconMesh);
   mFileGroupMesh->setText(0, "Meshes");
   mFileGroupMesh->setBackgroundColor(0, bgColor);
   mFileGroupMesh->setBackgroundColor(1, bgColor);

   mFileGroupMat = new QTreeWidgetItem();
   mFileGroupMat->setIcon(0, iconMat);
   mFileGroupMat->setText(0, "Materials");
   mFileGroupMat->setBackgroundColor(0, bgColor);
   mFileGroupMat->setBackgroundColor(1, bgColor);

   mFileGroupMorph = new QTreeWidgetItem();
   mFileGroupMorph->setIcon(0, iconMorph);
   mFileGroupMorph->setText(0, "Morph Targets");
   mFileGroupMorph->setBackgroundColor(0, bgColor);
   mFileGroupMorph->setBackgroundColor(1, bgColor);

   mFileTree->addTopLevelItem(mFileGroupSkel);
   mFileTree->addTopLevelItem(mFileGroupAnim);
   mFileTree->addTopLevelItem(mFileGroupMesh);
   mFileTree->addTopLevelItem(mFileGroupMat);
   mFileTree->addTopLevelItem(mFileGroupMorph);

   // Connect the delegates.
   connect(mFileDelegate, SIGNAL(SignalResourceEditStart(int, const std::string&)), this, SLOT(OnResourceEditStart(int, const std::string&)));
   connect(mFileDelegate, SIGNAL(SignalResourceEditEnd(int, const std::string&)), this, SLOT(OnResourceEditEnd(int, const std::string&)));
   connect(mObjectNameDelegate, SIGNAL(SignalResourceNameChanged(int, const std::string&, const std::string&)),
      this, SLOT(OnResourceNameChanged(int, const std::string&, const std::string&)));
   connect(mFileAdd, SIGNAL(clicked(bool)), this, SLOT(OnResourceAdd()));
   connect(mFileDelegate, SIGNAL(SignalResourceRemoved(int, const std::string&)),
      this, SLOT(OnResourceRemoved(int, const std::string&)));
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::CreateDockWidgets()
{
   CreateDockWidget_Properties();
   CreateDockWidget_Tools();
   CreateDockWidget_Resources();
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnInitialization()
{
   //theoretically, everything is in place, the window is rendering, openGL
   //context is valid, etc.
   mHardwareSkinningAction->setChecked(IsAnimNodeBuildingUsingHW());

   QSettings settings(APP_SETTINGS_NAME.c_str(), APP_TITLE.c_str());
   mContextPath = settings.value(
      SETTING_PROJECT_CONTEXT.c_str()).toString().toStdString();
   mShaderDefFile = settings.value(
      SETTING_SHADERDEFS_FILE.c_str()).toString().toStdString();

   if (EnsureShaderDefFileValid())
   {
      QString qstr(mShaderDefFile.c_str());
      OnLoadShaderFile(qstr);
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::SaveSettings()
{
   QSettings settings(APP_SETTINGS_NAME.c_str(), APP_TITLE.c_str());

   try
   {
      // Update the registry entry based on the current valid context
      settings.setValue(SETTING_PROJECT_CONTEXT.c_str(), mContextPath.c_str());
      settings.setValue(SETTING_SHADERDEFS_FILE.c_str(), mShaderDefFile.c_str());
      settings.sync();
   }
   catch (const dtUtil::Exception &e)
   {
      // The context path is not valid, clear the registry entry
      settings.remove(SETTING_PROJECT_CONTEXT.c_str());
      settings.remove(SETTING_SHADERDEFS_FILE.c_str());
      settings.sync();

      QMessageBox::critical((QWidget *)this, tr("Error"), tr(e.What().c_str()), tr("Ok"));
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnLoadShaderDefinition()
{
   QString filename = QFileDialog::getOpenFileName(this, tr("Load Shader Definition File"),
      mContextPath.c_str(), tr("Shaders(*.dtShader)") + " " + tr("Shaders(*.xml)") );

   if (!filename.isEmpty())
   {
      if (dtUtil::FileUtils::GetInstance().FileExists(filename.toStdString()))
      {
         OnLoadShaderFile(filename);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnLoadShaderFile(const QString& filename)
{
   try
   {
      dtCore::ShaderManager& shaderManager = dtCore::ShaderManager::GetInstance();

      // Since the shader manager cannot deal with duplicate shader names,
      // we clear it out before we load each file.  This means that in order
      // to reference shaders later, the file just be reloaded.
      shaderManager.Clear();

      mShaderDefFile = filename.toStdString();
      shaderManager.LoadShaderDefinitions(mShaderDefFile);

      mContextPath = dtUtil::FileUtils::GetInstance().GetFileInfo(mShaderDefFile).path;

      SaveSettings();
   }
   catch (dtUtil::Exception& e)
   {
      QMessageBox::critical(NULL, "Error", e.ToString().c_str());
   }
}

///////////////////////////////////////////////////////////////////////////////
bool MainWindow::IsShaderDefFileValid() const
{
   return dtUtil::FileUtils::GetInstance().FileExists(mShaderDefFile);
}

///////////////////////////////////////////////////////////////////////////////
bool MainWindow::EnsureShaderDefFileValid()
{
   bool valid = IsShaderDefFileValid();

   if ( ! valid)
   {
      if (AskUserToLoadShaderDef())
      {
         OnLoadShaderDefinition();

         valid = IsShaderDefFileValid();
      }
   }

   if ( ! valid)
   {
      std::string title("Invalid ShaderDef");
      std::string message("Could not load shader definition file:\n\t" + mShaderDefFile
         + "\nCharacter models may not display correctly.");
      QMessageBox::warning(NULL, title.c_str(), message.c_str());
   }

   return valid;
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::OnReloadShaderFiles()
{
   if (EnsureShaderDefFileValid())
   {
      dtCore::ShaderManager& shaderManager = dtCore::ShaderManager::GetInstance();
      shaderManager.Clear();

      try
      {
         shaderManager.LoadShaderDefinitions(mShaderDefFile);
      }
      catch (dtUtil::Exception& e)
      {
         QMessageBox::critical(NULL, "Error", e.ToString().c_str());
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
bool MainWindow::AskUserToLoadShaderDef()
{
   std::string title("Load ShaderDef File?");
   std::string message("Character models may need custom shaders to render. Would you like to load your project's ShaderDef file?");

   return QMessageBox::Ok == QMessageBox::information(NULL, title.c_str(), message.c_str(), QMessageBox::Ok, QMessageBox::Cancel);
}
