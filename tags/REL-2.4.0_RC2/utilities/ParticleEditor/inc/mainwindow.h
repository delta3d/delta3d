#ifndef ___MAIN_WINDOW_H___
#define ___MAIN_WINDOW_H___

#include <particleviewer.h>
#include <layersbrowser.h>
#include <particlestab.h>
#include <countertab.h>
#include <placertab.h>
#include <shootertab.h>
#include <programtab.h>
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
      MainWindow(QMainWindow* parent = NULL);
      virtual ~MainWindow(void);

      void SetParticleViewer(ParticleViewer* particleViewer);
      void SetupUI();
      void LoadFile(QString filename, bool import = false);

   public slots:
      void UpdateWindowTitle(const QString& title);
      void UpdateHistory(const QString& filename);
      void OnReferenceObjectLoaded(const QString &filename);

   private:
      void SetupMenus();
      void SetupMenuConnections();
      void SetupLayersBrowser();
      void SetupLayersBrowserConnections();
      void SetupParticlesTab();
      void SetupParticlesTabConnections();
      void SetupCounterTab();
      void SetupCounterTabConnections();
      void SetupPlacerTab();
      void SetupPlacerTabConnections();
      void SetupShooterTab();
      void SetupShooterTabConnections();
      void SetupProgramTab();
      void SetupProgramTabConnections();
      void UpdateRecentFileActions();

      Ui::MainWindow mUI;

      QAction*        mpRecentFilesActions[5];
      ParticleViewer* mpParticleViewer;
      LayersBrowser*  mpLayersBrowser;
      ParticlesTab*   mpParticlesTab;
      CounterTab*     mpCounterTab;
      PlacerTab*      mpPlacerTab;
      ShooterTab*     mpShooterTab;
      ProgramTab*     mpProgramTab;
   };
}

#endif //___MAIN_WINDOW_H___

