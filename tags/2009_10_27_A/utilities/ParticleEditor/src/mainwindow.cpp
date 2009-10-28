#include <mainwindow.h>
#include <QtCore/QSettings>
#include <QtCore/QFileInfo>
#include <QtOpenGL/QGLWidget>
#include <dtQt/osggraphicswindowqt.h>
#include <dtCore/deltawin.h>
#include <dtUtil/log.h>
using namespace psEditor;

///////////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow(QMainWindow* parent):
QMainWindow(parent)
{
   mUI.setupUi(this);
}

///////////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetParticleViewer(ParticleViewer* particleViewer)
{
   mpParticleViewer = particleViewer;
   dtQt::OSGGraphicsWindowQt* osgGraphWindow = dynamic_cast<dtQt::OSGGraphicsWindowQt*>(mpParticleViewer->GetWindow()->GetOsgViewerGraphicsWindow());

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
      widget->setGeometry(mUI.ParticleViewer->geometry());
      widget->setParent(mUI.ParticleViewer);
   }
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupUI()
{
   connect(mpParticleViewer, SIGNAL(UpdateWindowTitle(const QString&)), this, SLOT(UpdateWindowTitle(const QString&)));
   connect(mpParticleViewer, SIGNAL(UpdateHistory(const QString&)), this, SLOT(UpdateHistory(const QString&)));
   connect(mpParticleViewer, SIGNAL(ReferenceObjectLoaded(const QString&)), this, SLOT(OnReferenceObjectLoaded(const QString&)));

   // Pass the UI to the various classes that need their information
   SetupMenus();
   SetupMenuConnections();
   SetupLayersBrowser();
   SetupParticlesTab();
   SetupCounterTab();
   SetupPlacerTab();
   SetupShooterTab();
   SetupProgramTab();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::LoadFile(QString filename, bool import/* = false*/)
{
   mpParticleViewer->LoadFile(filename, import);
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::UpdateWindowTitle(const QString& title)
{
   setWindowTitle(title);
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::UpdateHistory(const QString& filename)
{
   QSettings settings("MOVES", "Particle Editor");
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

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupMenus()
{
   mUI.menuOpen_Previous->clear();
   for (int actionIndex = 0; actionIndex < 5; actionIndex++)
   {
      mpRecentFilesActions[actionIndex] = new QAction(this);
      mpRecentFilesActions[actionIndex]->setVisible(false);
      mUI.menuOpen_Previous->addAction(mpRecentFilesActions[actionIndex]);
   }
   UpdateRecentFileActions();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupMenuConnections()
{
   connect(mUI.actionNew, SIGNAL(triggered()), mpParticleViewer, SLOT(CreateNewParticleSystem()));
   connect(mUI.actionOpen, SIGNAL(triggered()), mpParticleViewer, SLOT(OpenParticleSystem()));
   for (int actionIndex = 0; actionIndex < 5; ++actionIndex)
   {
      connect(mpRecentFilesActions[actionIndex], SIGNAL(triggered()), mpParticleViewer, SLOT(OpenRecentParticleSystem()));
   }
   connect(mUI.actionImport, SIGNAL(triggered()), mpParticleViewer, SLOT(ImportParticleSystem()));
   connect(mUI.actionLoad_Reference, SIGNAL(triggered()), mpParticleViewer, SLOT(LoadReferenceObject()));
   connect(mUI.actionSave, SIGNAL(triggered()), mpParticleViewer, SLOT(SaveParticleToFile()));
   connect(mUI.actionSave_As, SIGNAL(triggered()), mpParticleViewer, SLOT(SaveParticleAs()));
   connect(mUI.actionExit, SIGNAL(triggered()), this, SLOT(close()));
   connect(mUI.actionCompass, SIGNAL(triggered(bool)), mpParticleViewer, SLOT(ToggleCompass(bool)));
   connect(mUI.actionXY_Grid, SIGNAL(triggered(bool)), mpParticleViewer, SLOT(ToggleXYGrid(bool)));
   connect(mUI.actionYZ_Grid, SIGNAL(triggered(bool)), mpParticleViewer, SLOT(ToggleYZGrid(bool)));
   connect(mUI.actionXZ_Grid, SIGNAL(triggered(bool)), mpParticleViewer, SLOT(ToggleXZGrid(bool)));
   connect(mUI.actionReference_Object, SIGNAL(triggered(bool)), mpParticleViewer, SLOT(ToggleReferenceObject(bool)));
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupLayersBrowser()
{
   mpLayersBrowser = new LayersBrowser();
   mpLayersBrowser->SetLayerList(mUI.LayerList);
   mpLayersBrowser->SetNewLayerButton(mUI.NewLayerButton);
   mpLayersBrowser->SetTrashLayerButton(mUI.TrashLayerButton);
   mpLayersBrowser->SetHideLayerButton(mUI.HideLayerButton);
   mpLayersBrowser->SetRenameLayerButton(mUI.RenameLayerButton);
   mpLayersBrowser->SetResetParticleButton(mUI.ResetParticlesButton);

   SetupLayersBrowserConnections();

   mpLayersBrowser->SetupUI();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupLayersBrowserConnections()
{
   // Connections from Layer Browser to tabs
   connect(mpLayersBrowser, SIGNAL(ToggleTabs(bool)), mUI.Tabs, SLOT(setEnabled(bool)));
   connect(mpLayersBrowser, SIGNAL(RenameLayer(const QString&)), mpParticleViewer, SLOT(RenameParticleLayer(const QString&)));

   
   // Connections from UI to particle viewer
   connect(mUI.LayerList, SIGNAL(currentRowChanged(int)), mpParticleViewer, SLOT(UpdateSelectionIndex(int)));
   connect(mUI.NewLayerButton, SIGNAL(clicked()), mpParticleViewer, SLOT(CreateNewParticleLayer()));
   connect(mUI.TrashLayerButton, SIGNAL(clicked()), mpParticleViewer, SLOT(DeleteSelectedLayer()));
   connect(mUI.HideLayerCheckbox, SIGNAL(clicked()), mpParticleViewer, SLOT(ToggleSelectedLayerHidden()));
   connect(mUI.ResetParticlesButton, SIGNAL(clicked()), mpParticleViewer, SLOT(ResetEmitters()));
   connect(mUI.LayerRenderBinEntry, SIGNAL(valueChanged(int)), mpParticleViewer, SLOT(SetParticleLayerRenderBin(int)));

   // Connections from particle viewer to UI
   connect(mpParticleViewer, SIGNAL(ClearLayerList()), mpLayersBrowser, SLOT(ClearLayerList()));
   connect(mpParticleViewer, SIGNAL(AddLayerToLayerList(const QString&)), mpLayersBrowser, SLOT(AddLayerToLayerList(const QString&)));
   connect(mpParticleViewer, SIGNAL(SelectIndexOfLayersList(int)), mpLayersBrowser, SLOT(SelectIndexOfLayersList(int)));
   connect(mpParticleViewer, SIGNAL(LayerHiddenChanged(bool)), mUI.HideLayerCheckbox, SLOT(setChecked(bool)));
   connect(mpParticleViewer, SIGNAL(LayerRenderBinChanged(int)), mUI.LayerRenderBinEntry, SLOT(setValue(int)));
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupParticlesTab()
{
   mpParticlesTab = new ParticlesTab();

   mpParticlesTab->SetAlignmentBox(mUI.AlignmentComboBox);
   mpParticlesTab->SetShapeBox(mUI.ShapeComboBox);

   mpParticlesTab->SetEmissiveBox(mUI.EmissiveCheckbox);
   mpParticlesTab->SetLightingBox(mUI.LightingCheckbox);

   mpParticlesTab->SetLifeSpinBox(mUI.LifeSpinBox);
   mpParticlesTab->SetLifeSlider(mUI.LifeSlider);
   mpParticlesTab->SetRadiusSpinBox(mUI.RadiusSpinBox);
   mpParticlesTab->SetRadiusSlider(mUI.RadiusSlider);
   mpParticlesTab->SetMassSpinBox(mUI.MassSpinBox);
   mpParticlesTab->SetMassSlider(mUI.MassSlider);
   mpParticlesTab->SetSizeFromSpinBox(mUI.SizeFromSpinBox);
   mpParticlesTab->SetSizeFromSlider(mUI.SizeFromSlider);
   mpParticlesTab->SetSizeToSpinBox(mUI.SizeToSpinBox);
   mpParticlesTab->SetSizeToSlider(mUI.SizeToSlider);

   mpParticlesTab->SetTextureLineEdit(mUI.TextureLineEdit);
   mpParticlesTab->SetTextureButton(mUI.TexturesBrowseButton);
   mpParticlesTab->SetTexturePreview(mUI.TexturePreviewer);

   mpParticlesTab->SetRFromSpinBox(mUI.RFromSpinBox);
   mpParticlesTab->SetRFromSlider(mUI.RFromSlider);
   mpParticlesTab->SetRToSpinBox(mUI.RToSpinBox);
   mpParticlesTab->SetRToSlider(mUI.RToSlider);
   mpParticlesTab->SetGFromSpinBox(mUI.GFromSpinBox);
   mpParticlesTab->SetGFromSlider(mUI.GFromSlider);
   mpParticlesTab->SetGToSpinBox(mUI.GToSpinBox);
   mpParticlesTab->SetGToSlider(mUI.GToSlider);
   mpParticlesTab->SetBFromSpinBox(mUI.BFromSpinBox);
   mpParticlesTab->SetBFromSlider(mUI.BFromSlider);
   mpParticlesTab->SetBToSpinBox(mUI.BToSpinBox);
   mpParticlesTab->SetBToSlider(mUI.BToSlider);
   mpParticlesTab->SetAFromSpinBox(mUI.AFromSpinBox);
   mpParticlesTab->SetAFromSlider(mUI.AFromSlider);
   mpParticlesTab->SetAToSpinBox(mUI.AToSpinBox);
   mpParticlesTab->SetAToSlider(mUI.AToSlider);

   mpParticlesTab->SetCustomColorFromButton(mUI.CustomColorFromButton);
   mpParticlesTab->SetCustomColorToButton(mUI.CustomColorToButton);

   mpParticlesTab->SetEmitterLifeSpinBox(mUI.EmitterLifeSpinBox);
   mpParticlesTab->SetEmitterLifeSlider(mUI.EmitterLifeSlider);
   mpParticlesTab->SetEmitterStartSpinBox(mUI.EmitterStartSpinBox);
   mpParticlesTab->SetEmitterStartSlider(mUI.EmitterStartSlider);
   mpParticlesTab->SetEmitterResetSpinBox(mUI.EmitterResetSpinBox);
   mpParticlesTab->SetEmitterResetSlider(mUI.EmitterResetSlider);

   mpParticlesTab->SetForeverBox(mUI.ForeverCheckbox);

   SetupParticlesTabConnections();

   mpParticlesTab->SetupUI();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupParticlesTabConnections()
{
   ///> Connections from UI to particle viewer
   // Particle UI
   connect(mUI.AlignmentComboBox, SIGNAL(currentIndexChanged(int)), mpParticleViewer, SLOT(AlignmentChanged(int)));
   connect(mUI.ShapeComboBox, SIGNAL(currentIndexChanged(int)), mpParticleViewer, SLOT(ShapeChanged(int)));
   connect(mUI.EmissiveCheckbox, SIGNAL(clicked(bool)), mpParticleViewer, SLOT(ToggleEmissive(bool)));
   connect(mUI.LightingCheckbox, SIGNAL(clicked(bool)), mpParticleViewer, SLOT(ToggleLighting(bool)));
   connect(mUI.LifeSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(LifeValueChanged(double)));
   connect(mUI.RadiusSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(RadiusValueChanged(double)));
   connect(mUI.MassSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(MassValueChanged(double)));
   connect(mUI.SizeFromSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(SizeFromValueChanged(double)));
   connect(mUI.SizeToSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(SizeToValueChanged(double)));

   // Texture UI
   connect(mpParticlesTab, SIGNAL(TextureChanged(QString)), mpParticleViewer, SLOT(TextureChanged(QString)));

   // Color UI
   connect(mUI.RFromSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(RFromValueChanged(double)));
   connect(mUI.RToSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(RToValueChanged(double)));
   connect(mUI.GFromSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(GFromValueChanged(double)));
   connect(mUI.GToSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(GToValueChanged(double)));
   connect(mUI.BFromSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(BFromValueChanged(double)));
   connect(mUI.BToSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(BToValueChanged(double)));
   connect(mUI.AFromSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(AFromValueChanged(double)));
   connect(mUI.AToSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(AToValueChanged(double)));

   // Emitter UI
   connect(mUI.EmitterLifeSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(EmitterLifeValueChanged(double)));
   connect(mUI.EmitterStartSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(EmitterStartValueChanged(double)));
   connect(mUI.EmitterResetSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(EmitterResetValueChanged(double)));
   connect(mUI.ForeverCheckbox, SIGNAL(clicked(bool)), mpParticleViewer, SLOT(EndlessLifetimeChanged(bool)));

   ///> Connections from particle viewer to UI
   // Particle UI
   connect(mpParticleViewer, SIGNAL(AlignmentUpdated(int)), mUI.AlignmentComboBox, SLOT(setCurrentIndex(int)));
   connect(mpParticleViewer, SIGNAL(ShapeUpdated(int)), mUI.ShapeComboBox, SLOT(setCurrentIndex(int)));
   connect(mpParticleViewer, SIGNAL(EmissiveUpdated(bool)), mUI.EmissiveCheckbox, SLOT(setChecked(bool)));
   connect(mpParticleViewer, SIGNAL(LightingUpdated(bool)), mUI.LightingCheckbox, SLOT(setChecked(bool)));
   connect(mpParticleViewer, SIGNAL(LifeUpdated(double)), mUI.LifeSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(RadiusUpdated(double)), mUI.RadiusSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(MassUpdated(double)), mUI.MassSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(SizeFromUpdated(double)), mUI.SizeFromSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(SizeToUpdated(double)), mUI.SizeToSpinBox, SLOT(setValue(double)));

   // Texture UI
   connect(mpParticleViewer, SIGNAL(TextureUpdated(QString)), mpParticlesTab, SLOT(TextureUpdated(QString)));

   // Color UI
   connect(mpParticleViewer, SIGNAL(RFromUpdated(double)), mUI.RFromSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(RToUpdated(double)), mUI.RToSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(GFromUpdated(double)), mUI.GFromSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(GToUpdated(double)), mUI.GToSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(BFromUpdated(double)), mUI.BFromSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(BToUpdated(double)), mUI.BToSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(AFromUpdated(double)), mUI.AFromSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(AToUpdated(double)), mUI.AToSpinBox, SLOT(setValue(double)));

   // Emitter UI
   connect(mpParticleViewer, SIGNAL(EmitterLifeUpdated(double)), mUI.EmitterLifeSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(EmitterStartUpdated(double)), mUI.EmitterStartSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(EmitterResetUpdated(double)), mUI.EmitterResetSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(EndlessLifetimeUpdated(bool)), mUI.ForeverCheckbox, SLOT(setChecked(bool)));
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupCounterTab()
{
   mpCounterTab = new CounterTab();

   mpCounterTab->SetCounterTypeBox(mUI.CounterTypeComboBox);
   mpCounterTab->SetRandomRateMinRateSpinBox(mUI.RandomRateMinRateSpinBox);
   mpCounterTab->SetRandomRateMinRateSlider(mUI.RandomRateMinRateSlider);
   mpCounterTab->SetRandomRateMaxRateSpinBox(mUI.RandomRateMaxRateSpinBox);
   mpCounterTab->SetRandomRateMaxRateSlider(mUI.RandomRateMaxRateSlider);

   SetupCounterTabConnections();

   mpCounterTab->SetupUI();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupCounterTabConnections()
{
   // Connections from UI to particle viewer
   connect(mUI.CounterTypeComboBox, SIGNAL(currentIndexChanged(int)), mpParticleViewer, SLOT(CounterTypeBoxValueChanged(int)));
   connect(mUI.RandomRateMinRateSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(RandomRateMinRateValueChanged(double)));
   connect(mUI.RandomRateMaxRateSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(RandomRateMaxRateValueChanged(double)));

   // Connections from particle viewer to UI
   connect(mpParticleViewer, SIGNAL(CounterTypeBoxUpdated(int)), mUI.CounterTypeComboBox, SLOT(setCurrentIndex(int)));
   connect(mpParticleViewer, SIGNAL(RandomRateMinRateUpdated(double)), mUI.RandomRateMinRateSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(RandomRateMaxRateUpdated(double)), mUI.RandomRateMaxRateSpinBox, SLOT(setValue(double)));
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupPlacerTab()
{
   mpPlacerTab = new PlacerTab();

   mpPlacerTab->SetPlacerTypeBox(mUI.PlacerTypeComboBox);

   // Point Placer UI
   mpPlacerTab->SetPointPlacerXSpinBox(mUI.PointPlacerXSpinBox);
   mpPlacerTab->SetPointPlacerXSlider(mUI.PointPlacerXSlider);
   mpPlacerTab->SetPointPlacerYSpinBox(mUI.PointPlacerYSpinBox);
   mpPlacerTab->SetPointPlacerYSlider(mUI.PointPlacerYSlider);
   mpPlacerTab->SetPointPlacerZSpinBox(mUI.PointPlacerZSpinBox);
   mpPlacerTab->SetPointPlacerZSlider(mUI.PointPlacerZSlider);

   // Sector Placer UI
   mpPlacerTab->SetSectorPlacerXSpinBox(mUI.SectorPlacerXSpinBox);
   mpPlacerTab->SetSectorPlacerXSlider(mUI.SectorPlacerXSlider);
   mpPlacerTab->SetSectorPlacerYSpinBox(mUI.SectorPlacerYSpinBox);
   mpPlacerTab->SetSectorPlacerYSlider(mUI.SectorPlacerYSlider);
   mpPlacerTab->SetSectorPlacerZSpinBox(mUI.SectorPlacerZSpinBox);
   mpPlacerTab->SetSectorPlacerZSlider(mUI.SectorPlacerZSlider);
   mpPlacerTab->SetSectorPlacerMinRadiusSpinBox(mUI.SectorPlacerMinRadiusSpinBox);
   mpPlacerTab->SetSectorPlacerMinRadiusSlider(mUI.SectorPlacerMinRadiusSlider);
   mpPlacerTab->SetSectorPlacerMaxRadiusSpinBox(mUI.SectorPlacerMaxRadiusSpinBox);
   mpPlacerTab->SetSectorPlacerMaxRadiusSlider(mUI.SectorPlacerMaxRadiusSlider);
   mpPlacerTab->SetSectorPlacerMinPhiSpinBox(mUI.SectorPlacerMinPhiSpinBox);
   mpPlacerTab->SetSectorPlacerMinPhiSlider(mUI.SectorPlacerMinPhiSlider);
   mpPlacerTab->SetSectorPlacerMaxPhiSpinBox(mUI.SectorPlacerMaxPhiSpinBox);
   mpPlacerTab->SetSectorPlacerMaxPhiSlider(mUI.SectorPlacerMaxPhiSlider);

   // Segment Placer UI
   mpPlacerTab->SetSegmentPlacerVertexAXSpinBox(mUI.SegmentPlacerVertexAXSpinBox);
   mpPlacerTab->SetSegmentPlacerVertexAXSlider(mUI.SegmentPlacerVertexAXSlider);
   mpPlacerTab->SetSegmentPlacerVertexAYSpinBox(mUI.SegmentPlacerVertexAYSpinBox);
   mpPlacerTab->SetSegmentPlacerVertexAYSlider(mUI.SegmentPlacerVertexAYSlider);
   mpPlacerTab->SetSegmentPlacerVertexAZSpinBox(mUI.SegmentPlacerVertexAZSpinBox);
   mpPlacerTab->SetSegmentPlacerVertexAZSlider(mUI.SegmentPlacerVertexAZSlider);
   mpPlacerTab->SetSegmentPlacerVertexBXSpinBox(mUI.SegmentPlacerVertexBXSpinBox);
   mpPlacerTab->SetSegmentPlacerVertexBXSlider(mUI.SegmentPlacerVertexBXSlider);
   mpPlacerTab->SetSegmentPlacerVertexBYSpinBox(mUI.SegmentPlacerVertexBYSpinBox);
   mpPlacerTab->SetSegmentPlacerVertexBYSlider(mUI.SegmentPlacerVertexBYSlider);
   mpPlacerTab->SetSegmentPlacerVertexBZSpinBox(mUI.SegmentPlacerVertexBZSpinBox);
   mpPlacerTab->SetSegmentPlacerVertexBZSlider(mUI.SegmentPlacerVertexBZSlider);

   // Multi Segment Placer UI
   mpPlacerTab->SetMultiSegmentPlacerVerticesList(mUI.MultiSegmentPlacerVerticesList);
   mpPlacerTab->SetVertexParametersStackedWidget(mUI.VertexParametersStackedWidget);
   mpPlacerTab->SetMultiSegmentPlacerAddVertexButton(mUI.MultiSegmentPlacerAddVertexButton);
   mpPlacerTab->SetMultiSegmentPlacerDeleteVertexButton(mUI.MultiSegmentPlacerDeleteVertexButton);
   mpPlacerTab->SetMultiSegmentPlacerXSpinBox(mUI.MultiSegmentPlacerXSpinBox);
   mpPlacerTab->SetMultiSegmentPlacerXSlider(mUI.MultiSegmentPlacerXSlider);
   mpPlacerTab->SetMultiSegmentPlacerYSpinBox(mUI.MultiSegmentPlacerYSpinBox);
   mpPlacerTab->SetMultiSegmentPlacerYSlider(mUI.MultiSegmentPlacerYSlider);
   mpPlacerTab->SetMultiSegmentPlacerZSpinBox(mUI.MultiSegmentPlacerZSpinBox);
   mpPlacerTab->SetMultiSegmentPlacerZSlider(mUI.MultiSegmentPlacerZSlider);

   SetupPlacerTabConnections();

   mpPlacerTab->SetupUI();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupPlacerTabConnections()
{
   ///> Connections from UI to particle viewer
   connect(mUI.PlacerTypeComboBox, SIGNAL(currentIndexChanged(int)), mpParticleViewer, SLOT(PlacerTypeBoxValueChanged(int)));
   // Point Placer UI
   connect(mUI.PointPlacerXSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(PointPlacerXValueChanged(double)));
   connect(mUI.PointPlacerYSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(PointPlacerYValueChanged(double)));
   connect(mUI.PointPlacerZSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(PointPlacerZValueChanged(double)));
   // Sector Placer UI
   connect(mUI.SectorPlacerXSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(SectorPlacerXValueChanged(double)));
   connect(mUI.SectorPlacerYSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(SectorPlacerYValueChanged(double)));
   connect(mUI.SectorPlacerZSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(SectorPlacerZValueChanged(double)));
   connect(mUI.SectorPlacerMinRadiusSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(SectorPlacerMinRadiusValueChanged(double)));
   connect(mUI.SectorPlacerMaxRadiusSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(SectorPlacerMaxRadiusValueChanged(double)));
   connect(mUI.SectorPlacerMinPhiSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(SectorPlacerMinPhiValueChanged(double)));
   connect(mUI.SectorPlacerMaxPhiSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(SectorPlacerMaxPhiValueChanged(double)));
   // Segment Placer UI
   connect(mUI.SegmentPlacerVertexAXSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(SegmentPlacerVertexAXValueChanged(double)));
   connect(mUI.SegmentPlacerVertexAYSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(SegmentPlacerVertexAYValueChanged(double)));
   connect(mUI.SegmentPlacerVertexAZSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(SegmentPlacerVertexAZValueChanged(double)));
   connect(mUI.SegmentPlacerVertexBXSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(SegmentPlacerVertexBXValueChanged(double)));
   connect(mUI.SegmentPlacerVertexBYSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(SegmentPlacerVertexBYValueChanged(double)));
   connect(mUI.SegmentPlacerVertexBZSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(SegmentPlacerVertexBZValueChanged(double)));
   // Multi Segment Placer UI
   connect(mUI.MultiSegmentPlacerVerticesList, SIGNAL(currentRowChanged(int)), mpParticleViewer, SLOT(UpdateMultiSegmentPlacerSelectionIndex(int)));
   connect(mUI.MultiSegmentPlacerAddVertexButton, SIGNAL(clicked()), mpParticleViewer, SLOT(MultiSegmentPlacerAddVertex()));
   connect(mUI.MultiSegmentPlacerDeleteVertexButton, SIGNAL(clicked()), mpParticleViewer, SLOT(MultiSegmentPlacerDeleteVertex()));
   connect(mUI.MultiSegmentPlacerXSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(MultiSegmentPlacerXValueChanged(double)));
   connect(mUI.MultiSegmentPlacerYSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(MultiSegmentPlacerYValueChanged(double)));
   connect(mUI.MultiSegmentPlacerZSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(MultiSegmentPlacerZValueChanged(double)));

   ///> Connections from particle viewer to UI
   connect(mpParticleViewer, SIGNAL(PlacerTypeBoxUpdated(int)), mUI.PlacerTypeComboBox, SLOT(setCurrentIndex(int)));
   // Point Placer UI
   connect(mpParticleViewer, SIGNAL(PointPlacerXUpdated(double)), mUI.PointPlacerXSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(PointPlacerYUpdated(double)), mUI.PointPlacerYSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(PointPlacerZUpdated(double)), mUI.PointPlacerZSpinBox, SLOT(setValue(double)));
   // Sector Placer UI
   connect(mpParticleViewer, SIGNAL(SectorPlacerXUpdated(double)), mUI.SectorPlacerXSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(SectorPlacerYUpdated(double)), mUI.SectorPlacerYSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(SectorPlacerZUpdated(double)), mUI.SectorPlacerZSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(SectorPlacerMinRadiusUpdated(double)), mUI.SectorPlacerMinRadiusSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(SectorPlacerMaxRadiusUpdated(double)), mUI.SectorPlacerMaxRadiusSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(SectorPlacerMinPhiUpdated(double)), mUI.SectorPlacerMinPhiSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(SectorPlacerMaxPhiUpdated(double)), mUI.SectorPlacerMaxPhiSpinBox, SLOT(setValue(double)));
   // Segment Placer UI
   connect(mpParticleViewer, SIGNAL(SegmentPlacerVertexAXUpdated(double)), mUI.SegmentPlacerVertexAXSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(SegmentPlacerVertexAYUpdated(double)), mUI.SegmentPlacerVertexAYSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(SegmentPlacerVertexAZUpdated(double)), mUI.SegmentPlacerVertexAZSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(SegmentPlacerVertexBXUpdated(double)), mUI.SegmentPlacerVertexBXSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(SegmentPlacerVertexBYUpdated(double)), mUI.SegmentPlacerVertexBYSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(SegmentPlacerVertexBZUpdated(double)), mUI.SegmentPlacerVertexBZSpinBox, SLOT(setValue(double)));
   // Multi Segment Placer UI
   connect(mpParticleViewer, SIGNAL(ClearMultiSegmentPlacerVertexList()), mpPlacerTab, SLOT(MultiSegmentPlacerClearVertexList()));
   connect(mpParticleViewer, SIGNAL(AddVertexToMultiSegmentPlacerVertexList(double, double, double)), mpPlacerTab, SLOT(MultiSegmentPlacerAddVertexToList(double, double, double)));
   connect(mpParticleViewer, SIGNAL(SelectIndexOfMultiSegmentPlacerVertexList(int)), mpPlacerTab, SLOT(MultiSegmentPlacerSelectIndexOfVertexList(int)));
   connect(mpParticleViewer, SIGNAL(MultiSegmentPlacerXUpdated(double)), mUI.MultiSegmentPlacerXSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(MultiSegmentPlacerYUpdated(double)), mUI.MultiSegmentPlacerYSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(MultiSegmentPlacerZUpdated(double)), mUI.MultiSegmentPlacerZSpinBox, SLOT(setValue(double)));
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupShooterTab()
{
   mpShooterTab = new ShooterTab();

   mpShooterTab->SetShooterTypeBox(mUI.ShooterTypeComboBox);

   // Radial Shooter UI
   mpShooterTab->SetRadialShooterElevationMinSpinBox(mUI.RadialShooterElevationMinSpinBox);
   mpShooterTab->SetRadialShooterElevationMinSlider(mUI.RadialShooterElevationMinSlider);
   mpShooterTab->SetRadialShooterElevationMaxSpinBox(mUI.RadialShooterElevationMaxSpinBox);
   mpShooterTab->SetRadialShooterElevationMaxSlider(mUI.RadialShooterElevationMaxSlider);
   mpShooterTab->SetRadialShooterAzimuthMinSpinBox(mUI.RadialShooterAzimuthMinSpinBox);
   mpShooterTab->SetRadialShooterAzimuthMinSlider(mUI.RadialShooterAzimuthMinSlider);
   mpShooterTab->SetRadialShooterAzimuthMaxSpinBox(mUI.RadialShooterAzimuthMaxSpinBox);
   mpShooterTab->SetRadialShooterAzimuthMaxSlider(mUI.RadialShooterAzimuthMaxSlider);
   mpShooterTab->SetRadialShooterInitialVelocityMinSpinBox(mUI.RadialShooterInitialVelocityMinSpinBox);
   mpShooterTab->SetRadialShooterInitialVelocityMinSlider(mUI.RadialShooterInitialVelocityMinSlider);
   mpShooterTab->SetRadialShooterInitialVelocityMaxSpinBox(mUI.RadialShooterInitialVelocityMaxSpinBox);
   mpShooterTab->SetRadialShooterInitialVelocityMaxSlider(mUI.RadialShooterInitialVelocityMaxSlider);
   mpShooterTab->SetRadialShooterInitialMinRotationXSpinBox(mUI.RadialShooterInitialMinRotationXSpinBox);
   mpShooterTab->SetRadialShooterInitialMinRotationXSlider(mUI.RadialShooterInitialMinRotationXSlider);
   mpShooterTab->SetRadialShooterInitialMinRotationYSpinBox(mUI.RadialShooterInitialMinRotationYSpinBox);
   mpShooterTab->SetRadialShooterInitialMinRotationYSlider(mUI.RadialShooterInitialMinRotationYSlider);
   mpShooterTab->SetRadialShooterInitialMinRotationZSpinBox(mUI.RadialShooterInitialMinRotationZSpinBox);
   mpShooterTab->SetRadialShooterInitialMinRotationZSlider(mUI.RadialShooterInitialMinRotationZSlider);
   mpShooterTab->SetRadialShooterInitialMaxRotationXSpinBox(mUI.RadialShooterInitialMaxRotationXSpinBox);
   mpShooterTab->SetRadialShooterInitialMaxRotationXSlider(mUI.RadialShooterInitialMaxRotationXSlider);
   mpShooterTab->SetRadialShooterInitialMaxRotationYSpinBox(mUI.RadialShooterInitialMaxRotationYSpinBox);
   mpShooterTab->SetRadialShooterInitialMaxRotationYSlider(mUI.RadialShooterInitialMaxRotationYSlider);
   mpShooterTab->SetRadialShooterInitialMaxRotationZSpinBox(mUI.RadialShooterInitialMaxRotationZSpinBox);
   mpShooterTab->SetRadialShooterInitialMaxRotationZSlider(mUI.RadialShooterInitialMaxRotationZSlider);

   SetupShooterTabConnections();

   mpShooterTab->SetupUI();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupShooterTabConnections()
{
   ///> Connections from UI to particle viewer
   connect(mUI.ShooterTypeComboBox, SIGNAL(currentIndexChanged(int)), mpParticleViewer, SLOT(ShooterTypeBoxValueChanged(int)));
   // Radial Shooter UI
   connect(mUI.RadialShooterElevationMinSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(RadialShooterElevationMinValueChanged(double)));
   connect(mUI.RadialShooterElevationMaxSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(RadialShooterElevationMaxValueChanged(double)));
   connect(mUI.RadialShooterAzimuthMinSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(RadialShooterAzimuthMinValueChanged(double)));
   connect(mUI.RadialShooterAzimuthMaxSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(RadialShooterAzimuthMaxValueChanged(double)));
   connect(mUI.RadialShooterInitialVelocityMinSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(RadialShooterInitialVelocityMinValueChanged(double)));
   connect(mUI.RadialShooterInitialVelocityMaxSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(RadialShooterInitialVelocityMaxValueChanged(double)));
   connect(mUI.RadialShooterInitialMinRotationXSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(RadialShooterInitialMinRotationXValueChanged(double)));
   connect(mUI.RadialShooterInitialMinRotationYSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(RadialShooterInitialMinRotationYValueChanged(double)));
   connect(mUI.RadialShooterInitialMinRotationZSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(RadialShooterInitialMinRotationZValueChanged(double)));
   connect(mUI.RadialShooterInitialMaxRotationXSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(RadialShooterInitialMaxRotationXValueChanged(double)));
   connect(mUI.RadialShooterInitialMaxRotationYSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(RadialShooterInitialMaxRotationYValueChanged(double)));
   connect(mUI.RadialShooterInitialMaxRotationZSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(RadialShooterInitialMaxRotationZValueChanged(double)));

   ///> Connections from particle viewer to UI
   connect(mpParticleViewer, SIGNAL(PlacerTypeBoxUpdated(int)), mUI.PlacerTypeComboBox, SLOT(setCurrentIndex(int)));
   // Radial Shooter UI
   connect(mpParticleViewer, SIGNAL(RadialShooterElevationMinUpdated(double)), mUI.RadialShooterElevationMinSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(RadialShooterElevationMaxUpdated(double)), mUI.RadialShooterElevationMaxSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(RadialShooterAzimuthMinUpdated(double)), mUI.RadialShooterAzimuthMinSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(RadialShooterAzimuthMaxUpdated(double)), mUI.RadialShooterAzimuthMaxSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(RadialShooterInitialVelocityMinUpdated(double)), mUI.RadialShooterInitialVelocityMinSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(RadialShooterInitialVelocityMaxUpdated(double)), mUI.RadialShooterInitialVelocityMaxSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(RadialShooterInitialMinRotationXUpdated(double)), mUI.RadialShooterInitialMinRotationXSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(RadialShooterInitialMinRotationYUpdated(double)), mUI.RadialShooterInitialMinRotationYSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(RadialShooterInitialMinRotationZUpdated(double)), mUI.RadialShooterInitialMinRotationZSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(RadialShooterInitialMaxRotationXUpdated(double)), mUI.RadialShooterInitialMaxRotationXSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(RadialShooterInitialMaxRotationYUpdated(double)), mUI.RadialShooterInitialMaxRotationYSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(RadialShooterInitialMaxRotationZUpdated(double)), mUI.RadialShooterInitialMaxRotationZSpinBox, SLOT(setValue(double)));
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupProgramTab()
{
   mpProgramTab = new ProgramTab();

   // Operators UI
   mpProgramTab->SetOperatorsList(mUI.OperatorsList);
   mpProgramTab->SetOperatorsStackedWidget(mUI.OperatorsStackedWidget);
   mpProgramTab->SetOperatorsNewForceButton(mUI.OperatorsNewForceButton);
   mpProgramTab->SetOperatorsNewAccelerationButton(mUI.OperatorsNewAccelerationButton);
   mpProgramTab->SetOperatorsNewFluidFrictionButton(mUI.OperatorsNewFluidFrictionButton);
   mpProgramTab->SetOperatorsDeleteOperatorsButton(mUI.OperatorsDeleteOperatorButton);

   // Force UI
   mpProgramTab->SetForceXSpinBox(mUI.ForceParametersXSpinBox);
   mpProgramTab->SetForceXSlider(mUI.ForceParametersXSlider);
   mpProgramTab->SetForceYSpinBox(mUI.ForceParametersYSpinBox);
   mpProgramTab->SetForceYSlider(mUI.ForceParametersYSlider);
   mpProgramTab->SetForceZSpinBox(mUI.ForceParametersZSpinBox);
   mpProgramTab->SetForceZSlider(mUI.ForceParametersZSlider);

   // Acceleration UI
   mpProgramTab->SetAccelerationXSpinBox(mUI.AccelerationParametersXSpinBox);
   mpProgramTab->SetAccelerationXSlider(mUI.AccelerationParametersXSlider);
   mpProgramTab->SetAccelerationYSpinBox(mUI.AccelerationParametersYSpinBox);
   mpProgramTab->SetAccelerationYSlider(mUI.AccelerationParametersYSlider);
   mpProgramTab->SetAccelerationZSpinBox(mUI.AccelerationParametersZSpinBox);
   mpProgramTab->SetAccelerationZSlider(mUI.AccelerationParametersZSlider);

   // Force UI
   mpProgramTab->SetDensitySpinBox(mUI.FluidFrictionDensitySpinBox);
   mpProgramTab->SetDensitySlider(mUI.FluidFrictionDensitySlider);
   mpProgramTab->SetViscositySpinBox(mUI.FluidFrictionViscositySpinBox);
   mpProgramTab->SetViscositySlider(mUI.FluidFrictionViscositySlider);
   mpProgramTab->SetOverrideRadiusSpinBox(mUI.FluidFrictionOverrideRadiusSpinBox);
   mpProgramTab->SetOverrideRadiusSlider(mUI.FluidFrictionOverrideRadiusSlider);

   SetupProgramTabConnections();

   mpProgramTab->SetupUI();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupProgramTabConnections()
{
   ///> Connections from UI to particle viewer
   // Operators UI
   connect(mpProgramTab, SIGNAL(OperatorSelectionUpdate(int, const QString&)), mpParticleViewer, SLOT(UpdateOperatorsSelectionIndex(int, const QString&)));
   connect(mUI.OperatorsNewForceButton, SIGNAL(clicked()), mpParticleViewer, SLOT(OperatorsAddNewForce()));
   connect(mUI.OperatorsNewAccelerationButton, SIGNAL(clicked()), mpParticleViewer, SLOT(OperatorsAddNewAcceleration()));
   connect(mUI.OperatorsNewFluidFrictionButton, SIGNAL(clicked()), mpParticleViewer, SLOT(OperatorsAddNewFluidFriction()));
   connect(mUI.OperatorsDeleteOperatorButton, SIGNAL(clicked()), mpParticleViewer, SLOT(OperatorsDeleteCurrentOperator()));
   // Force UI
   connect(mUI.ForceParametersXSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(OperatorsForceXValueChanged(double)));
   connect(mUI.ForceParametersYSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(OperatorsForceYValueChanged(double)));
   connect(mUI.ForceParametersZSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(OperatorsForceZValueChanged(double)));
   // Acceleration UI
   connect(mUI.AccelerationParametersXSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(OperatorsAccelerationXValueChanged(double)));
   connect(mUI.AccelerationParametersYSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(OperatorsAccelerationYValueChanged(double)));
   connect(mUI.AccelerationParametersZSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(OperatorsAccelerationZValueChanged(double)));
   // Fluid Friction UI
   connect(mUI.FluidFrictionAirButton, SIGNAL(clicked()), mpParticleViewer, SLOT(FluidFrictionAirButtonPressed()));
   connect(mUI.FluidFrictionWaterButton, SIGNAL(clicked()), mpParticleViewer, SLOT(FluidFrictionWaterButtonPressed()));
   connect(mUI.FluidFrictionDensitySpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(OperatorsFluidFrictionDensityValueChanged(double)));
   connect(mUI.FluidFrictionViscositySpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(OperatorsFluidFrictionViscosityValueChanged(double)));
   connect(mUI.FluidFrictionOverrideRadiusSpinBox, SIGNAL(valueChanged(double)), mpParticleViewer, SLOT(OperatorsFluidFrictionOverrideRadiusValueChanged(double)));

   ///> Connections from particle viewer to UI
   // Operators UI
   connect(mpParticleViewer, SIGNAL(ClearOperatorsList()), mpProgramTab, SLOT(ClearOperatorsList()));
   connect(mpParticleViewer, SIGNAL(AddOperatorToOperatorsList(const QString &)), mpProgramTab, SLOT(AddOperatorToOperatorsList(const QString &)));
   connect(mpParticleViewer, SIGNAL(SelectIndexOfOperatorsList(int)), mpProgramTab, SLOT(SelectIndexOfOperatorsList(int)));
   // Force UI
   connect(mpParticleViewer, SIGNAL(OperatorsForceXUpdated(double)), mUI.ForceParametersXSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(OperatorsForceYUpdated(double)), mUI.ForceParametersYSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(OperatorsForceZUpdated(double)), mUI.ForceParametersZSpinBox, SLOT(setValue(double)));
   // Acceleration UI
   connect(mpParticleViewer, SIGNAL(OperatorsAccelerationXUpdated(double)), mUI.AccelerationParametersXSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(OperatorsAccelerationYUpdated(double)), mUI.AccelerationParametersYSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(OperatorsAccelerationZUpdated(double)), mUI.AccelerationParametersZSpinBox, SLOT(setValue(double)));
   // Fluid Friction UI
   connect(mpParticleViewer, SIGNAL(OperatorsFluidFrictionDensityUpdated(double)), mUI.FluidFrictionDensitySpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(OperatorsFluidFrictionViscosityUpdated(double)), mUI.FluidFrictionViscositySpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(OperatorsFluidFrictionOverrideRadiusUpdated(double)), mUI.FluidFrictionOverrideRadiusSpinBox, SLOT(setValue(double)));
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::UpdateRecentFileActions()
{
   QSettings settings("MOVES", "Particle Editor");
   QStringList files = settings.value("recentFileList").toStringList();

   int numRecentFiles = qMin(files.size(), 5);

   for (int actionIndex = 0; actionIndex < numRecentFiles; ++actionIndex)
   {
      QString text = tr("&%1 %2").arg(actionIndex + 1).arg(QFileInfo(files[actionIndex]).fileName());
      mpRecentFilesActions[actionIndex]->setText(text);
      mpRecentFilesActions[actionIndex]->setData(files[actionIndex]);
      mpRecentFilesActions[actionIndex]->setVisible(true);
   }

   for (int fileIndex = numRecentFiles; fileIndex < 5; ++fileIndex)
   {
      mpRecentFilesActions[fileIndex]->setVisible(false);
   }
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::OnReferenceObjectLoaded(const QString &filename)
{
   mUI.actionReference_Object->setEnabled(true);
}
