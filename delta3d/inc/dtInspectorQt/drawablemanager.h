#ifndef drawablemanager_h__
#define drawablemanager_h__

#include <QtCore/QObject>
#include <dtInspectorQt/imanager.h>
#include "ui_dtinspectorqt.h"
#include <dtCore/deltadrawable.h>
#include <dtCore/observerptr.h>


namespace dtInspectorQt
{
   ///Handles the properties of dtCore::DeltaDrawable
   class DrawableManager : public IManager
   {
      Q_OBJECT

   public:
   	DrawableManager(Ui::InspectorWidget& ui);
   	~DrawableManager();

      virtual void OperateOn(dtCore::Base* b);

   protected slots:
      void OnRenderProxyNode(int checked);
      void OnActive(int checked);

   	
   private:
      Ui::InspectorWidget* mUI;
      dtCore::ObserverPtr<dtCore::DeltaDrawable> mOperateOn;
      void Update();
   };
}
#endif // drawablemanager_h__
