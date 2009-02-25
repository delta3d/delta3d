#ifndef scenemanager_h__
#define scenemanager_h__

#include <QtCore/QObject>
#include <dtInspectorQt/imanager.h>
#include "ui_dtinspectorqt.h"
#include <dtCore/scene.h>
#include <dtCore/observerptr.h>


namespace dtInspectorQt
{
   ///Handles the properties of dtCore::Scene
   class SceneManager : public IManager
   {
      Q_OBJECT

   public:
   	SceneManager(Ui::InspectorWidget& ui);
   	~SceneManager();

      virtual void OperateOn(dtCore::Base* b);

   protected slots:
      void OnUseSceneLight(int state);
      void OnRenderStateChanged(int mode);
   	
   private:
      Ui::InspectorWidget* mUI;
      dtCore::ObserverPtr<dtCore::Scene> mOperateOn;
      void Update();
   };
}
#endif // scenemanager_h__
