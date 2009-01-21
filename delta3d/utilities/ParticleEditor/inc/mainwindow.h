#ifndef ___MAIN_WINDOW_H___
#define ___MAIN_WINDOW_H___

#include <particleviewer.h>
#include <viewwindow.h>
#include <layersbrowser.h>
#include <particlestab.h>
#include <countertab.h>
#include <placertab.h>
#include <shootertab.h>
#include <programtab.h>
#include <viewwindow.h>
#include "ui_particleeditor.h"

#include <QtGui/QWidget>

namespace psEditor
{

   /** The main display window for the Client application
   */
   class MainWindow :  public QMainWindow
   {
      Q_OBJECT

   public:
      MainWindow(QMainWindow *parent=NULL);
      virtual ~MainWindow(void);

      ViewWindow* GetGLWidget() { return mpViewWindow; }
      void SetParticleViewer(ParticleViewer* particleViewer);
      void SetupUI();

   private:
      void SetupParticleViewer();
      void SetupViewWindow();
      void SetupLayersBrowser();
      void SetupParticlesTab();
      void SetupCounterTab();
      void SetupPlacerTab();
      void SetupShooterTab();
      void SetupProgramTab();

      Ui::MainWindow mUI;

      ParticleViewer* mpParticleViewer;
      ViewWindow*     mpViewWindow;
      LayersBrowser*  mpLayersBrowser;
      ParticlesTab*   mpParticlesTab;
      CounterTab*     mpCounterTab;
      PlacerTab*      mpPlacerTab;
      ShooterTab*     mpShooterTab;
      ProgramTab*     mpProgramTab;
   };
}

#endif //___MAIN_WINDOW_H___

