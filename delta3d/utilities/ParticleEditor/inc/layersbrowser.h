#ifndef DELTA_LAYERS_BROWSER
#define DELTA_LAYERS_BROWSER

#include <QtGui/QListWidget>
#include <QtGui/QPushButton>

class LayersBrowser : QWidget
{
   Q_OBJECT

public:
   LayersBrowser();
   ~LayersBrowser();

   void SetLayerList(QListWidget* layerList) { mpLayerList = layerList; }
   void SetNewLayerButton(QPushButton* newLayerButton) { mpNewLayerButton = newLayerButton; }
   void SetTrashLayerButton(QPushButton* trashLayerButton) { mpTrashLayerButton = trashLayerButton; }
   void SetHideLayerButton(QPushButton* hideLayerButton) { mpHideLayerButton = hideLayerButton; }
   void SetRenameLayerButton(QPushButton* renameLayerButton) { mpRenameLayerButton = renameLayerButton; }
   void SetResetParticleButton(QPushButton* resetParticleButton) { mpResetParticleButton = resetParticleButton; }

   void SetupUI();

public slots:
   void NewLayerButtonPushed();
   void TrashLayerButtonPushed();
   void RenameLayerButtonPushed();
   void ResetParticleButtonPushed();

private:
   QListWidget* mpLayerList;
   QPushButton* mpNewLayerButton;
   QPushButton* mpTrashLayerButton;
   QPushButton* mpHideLayerButton;
   QPushButton* mpRenameLayerButton;
   QPushButton* mpResetParticleButton;
};

#endif /*DELTA_LAYERS_BROWSER*/
