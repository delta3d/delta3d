#ifndef physicalview_h__
#define physicalview_h__
#include <dtInspectorQt/iview.h>
#include <dtCore/observerptr.h>
#include <dtCore/physical.h>

namespace Ui
{
   class InspectorWidget;
}

namespace dtInspectorQt
{
   class PhysicalView : public IView
   {
      Q_OBJECT

   public:
   	PhysicalView(Ui::InspectorWidget& ui);
   	virtual ~PhysicalView();

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
#endif // physicalview_h__

