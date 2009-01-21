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
   SetupParticleViewer();
   SetupLayersBrowser();
   SetupParticlesTab();
   SetupCounterTab();
   SetupPlacerTab();
   SetupShooterTab();
   SetupProgramTab();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupParticleViewer()
{
   connect(mUI.LayerList, SIGNAL(currentRowChanged(int)), mpParticleViewer, SLOT(UpdateSelectionIndex(int)));
   connect(mUI.NewLayerButton, SIGNAL(clicked()), mpParticleViewer, SLOT(CreateNewParticleLayer()));
   connect(mUI.TrashLayerButton, SIGNAL(clicked()), mpParticleViewer, SLOT(DeleteSelectedLayer()));
   connect(mUI.HideLayerCheckbox, SIGNAL(clicked()), mpParticleViewer, SLOT(ToggleSelectedLayerHidden()));
   connect(mUI.ResetParticlesButton, SIGNAL(clicked()), mpParticleViewer, SLOT(ResetEmitters()));
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

   connect(mpParticleViewer, SIGNAL(LayerHiddenChanged(bool)), mUI.HideLayerCheckbox, SLOT(setChecked(bool)));

   mpLayersBrowser->SetupUI();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupParticlesTab()
{
   mpParticlesTab = new ParticlesTab();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupCounterTab()
{
   mpCounterTab = new CounterTab();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupPlacerTab()
{
   mpPlacerTab = new PlacerTab();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupShooterTab()
{
   mpShooterTab = new ShooterTab();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::SetupProgramTab()
{
   mpProgramTab = new ProgramTab();
}

///////////////////////////////////////////////////////////////////////////////
