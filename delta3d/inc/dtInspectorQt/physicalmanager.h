#ifndef physicalmanager_h__
#define physicalmanager_h__
#include <dtInspectorQt/imanager.h>
#include <dtCore/observerptr.h>
#include <dtCore/physical.h>

namespace Ui
{
   class InspectorWidget;
}

namespace dtInspectorQt
{
   class PhysicalManager : public IManager
   {
      Q_OBJECT

   public:
   	PhysicalManager(Ui::InspectorWidget& ui);
   	virtual ~PhysicalManager();

      virtual void OperateOn(dtCore::Base* b);

   protected slots:
      void OnDynamicsToggled(int checked);
      void OnMassChanged(double mass);
   	
   private:
      Ui::InspectorWidget* mUI;
      dtCore::ObserverPtr<dtCore::Physical> mOperateOn;
      void Update();

   };
}
#endif // physicalmanager_h__