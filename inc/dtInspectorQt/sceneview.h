#ifndef sceneview_h__
#define sceneview_h__

#include <QtCore/QObject>
#include <dtInspectorQt/iview.h>
#include <dtCore/scene.h>
#include <dtCore/observerptr.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace Ui
{
   class InspectorWidget;
}
/// @endcond


namespace dtInspectorQt
{
   ///Handles the properties of dtCore::Scene
   class SceneView : public IView
   {
      Q_OBJECT

   public:
   	SceneView(Ui::InspectorWidget& ui);
   	~SceneView();

      virtual void OperateOn(dtCore::Base* b);
      bool IsOfType(QString name, dtCore::Base* object);

   protected slots:
      void OnUseSceneLight(int state);
      void OnRenderStateChanged(int mode);
   	
   private:
      Ui::InspectorWidget* mUI;
      dtCore::ObserverPtr<dtCore::Scene> mOperateOn;
      void Update();
   };
}
#endif // sceneview_h__
