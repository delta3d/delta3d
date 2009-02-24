#ifndef basemanager_h__
#define basemanager_h__

#include <dtInspectorQt/imanager.h>
#include "ui_dtinspectorqt.h"
#include <dtCore/observerptr.h>
#include <qtCore/QObject>
#include <dtCore/base.h>


namespace dtInspectorQt
{
   ///Handles the properties of dtCore::Base
   class BaseManager : public IManager
   {
      Q_OBJECT

   public:
   	BaseManager(Ui::InspectorWidget &ui);
   	~BaseManager();

      void OperateOn(dtCore::Base* b);

   protected slots:
      void OnNameChange(const QString& text);

   signals:
      void NameChanged(const QString& text);

   private:
      Ui::InspectorWidget* mUI;
      dtCore::ObserverPtr<dtCore::Base> mOperateOn;
      void Update();
   };
}
#endif // basemanager_h__
