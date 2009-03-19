#ifndef DELTA_LAYERS_BROWSER
#define DELTA_LAYERS_BROWSER

#include <QtGui/QListWidget>
#include <QtGui/QPushButton>

class LayersBrowser : public QObject
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
signals:
   void ToggleTabs(bool enabled);
   void RenameLayer(const QString& name);

public slots:
   void ClearLayerList();
   void AddLayerToLayerList(const QString &layerName);
   void SelectIndexOfLayersList(int newIndex);
   void NewLayerButtonPushed();
   void TrashLayerButtonPushed();
   void RenameLayerButtonPushed();
   void ToggleLayers(bool enabled);

private:
   QListWidget* mpLayerList;
   QPushButton* mpNewLayerButton;
   QPushButton* mpTrashLayerButton;
   QPushButton* mpHideLayerButton;
   QPushButton* mpRenameLayerButton;
   QPushButton* mpResetParticleButton;
};

#endif /*DELTA_LAYERS_BROWSER*/
