#include <mainwindow.h>

using namespace psEditor;

///////////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow(QMainWindow *parent):
QMainWindow(parent)
{
   mUI.setupUi(this);

   SetupViewWindow();
}

///////////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetParticleViewer(ParticleViewer* particleViewer)
{
   mpParticleViewer = particleViewer;
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupUI()
{
   // Pass the UI to the various classes that need their information
   SetupLayersBrowser();
   SetupParticlesTab();
   SetupCounterTab();
   SetupPlacerTab();
   SetupShooterTab();
   SetupProgramTab();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupViewWindow()
{
   mpViewWindow = new ViewWindow(false, dynamic_cast<QWidget*>(mUI.ParticleViewer));
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
   // Connections from UI to particle viewer
   connect(mUI.LayerList, SIGNAL(currentRowChanged(int)), mpParticleViewer, SLOT(UpdateSelectionIndex(int)));
   connect(mUI.NewLayerButton, SIGNAL(clicked()), mpParticleViewer, SLOT(CreateNewParticleLayer()));
   connect(mUI.TrashLayerButton, SIGNAL(clicked()), mpParticleViewer, SLOT(DeleteSelectedLayer()));
   connect(mUI.HideLayerCheckbox, SIGNAL(clicked()), mpParticleViewer, SLOT(ToggleSelectedLayerHidden()));
   connect(mUI.ResetParticlesButton, SIGNAL(clicked()), mpParticleViewer, SLOT(ResetEmitters()));

   // Connections from particle viewer to UI
   connect(mpParticleViewer, SIGNAL(LayerHiddenChanged(bool)), mUI.HideLayerCheckbox, SLOT(setChecked(bool)));
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
   mpParticlesTab->SetTexturePreview(mUI.TexturePreview);

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
   connect(mpParticlesTab, SIGNAL(TextureChanged(QString, bool, bool)), mpParticleViewer, SLOT(TextureChanged(QString, bool, bool)));

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
   connect(mpParticleViewer, SIGNAL(TextureUpdated(QString, bool, bool)), mpParticlesTab, SLOT(TextureUpdated(QString, bool, bool)));

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
   connect(mpParticleViewer, SIGNAL(MultiSegmentPlacerXUpdated(double)), mUI.MultiSegmentPlacerXSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(MultiSegmentPlacerYUpdated(double)), mUI.MultiSegmentPlacerYSpinBox, SLOT(setValue(double)));
   connect(mpParticleViewer, SIGNAL(MultiSegmentPlacerZUpdated(double)), mUI.MultiSegmentPlacerZSpinBox, SLOT(setValue(double)));
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupShooterTab()
{
   mpShooterTab = new ShooterTab();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupShooterTabConnections()
{
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupProgramTab()
{
   mpProgramTab = new ProgramTab();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupProgramTabConnections()
{
}

///////////////////////////////////////////////////////////////////////////////
