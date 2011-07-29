#ifndef baseview_h__
#define baseview_h__

#include <dtInspectorQt/iview.h>
#include <dtCore/observerptr.h>
#include <QtCore/QObject>
#include <dtCore/baseactorobject.h>
#include <dtQt/basepropertyeditor.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace Ui
{
   class InspectorWidget;
}
/// @endcond

namespace dtInspectorQt
{
   ///Handles the properties of dtCore::Base
   class ActorView : public IView
   {
      Q_OBJECT

   public:
   	ActorView(Ui::InspectorWidget &ui);
   	~ActorView();

      virtual void Build(QList<EntryData>& itemList);
      virtual void OperateOn(const QVariant& itemData);

   protected slots:
      void OnNameChange(const QString& text);

   signals:
         void NameChanged(const QString& text);

   private:
      Ui::InspectorWidget* mUI;
      dtCore::ObserverPtr<dtCore::BaseActorObject> mOperateOn;
      void Update();
   };
}
#endif // baseview_h__
