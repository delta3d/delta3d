#ifndef transformablemanager_h__
#define transformablemanager_h__

#include <dtInspectorQt/imanager.h>
#include <dtCore/observerptr.h>
#include <dtCore/transformable.h>
#include "ui_dtinspectorqt.h"

namespace dtInspectorQt
{
   ///Handles the properties of dtCore::Transformable
   class TransformableManager : public IManager
   {
      Q_OBJECT

   public:
   	TransformableManager(Ui::InspectorWidget& ui);
   	~TransformableManager();

      virtual void OperateOn(dtCore::Base* b);

   protected slots:
      void OnXYZHPRChanged(double val);
      void OnCollisionDetection(int checked);
      void OnRenderCollision(int checked);
      void Update();
   	
   private:
      dtCore::ObserverPtr<dtCore::Transformable> mOperateOn;
      Ui::InspectorWidget* mUI;
   };
}
#endif // transformablemanager_h__
