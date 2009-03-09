#ifndef baseview_h__
#define baseview_h__

#include <dtInspectorQt/iview.h>
#include "ui_dtinspectorqt.h"
#include <dtCore/observerptr.h>
#include <QtCore/QObject>
#include <dtCore/base.h>


namespace dtInspectorQt
{
   ///Handles the properties of dtCore::Base
   class BaseView : public IView
   {
      Q_OBJECT

   public:
   	BaseView(Ui::InspectorWidget &ui);
   	~BaseView();

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
#endif // baseview_h__
