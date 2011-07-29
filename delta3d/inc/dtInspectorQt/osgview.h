#ifndef OSGVIEW_H
#define OSGVIEW_H

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

namespace osg
{
   class Group;
}
/// @endcond

namespace dtInspectorQt
{
   ///Handles the properties of dtCore::Base
   class OSGView : public IView
   {
      Q_OBJECT

   public:
     OSGView(Ui::InspectorWidget& ui);
     ~OSGView();

      virtual void Build(QList<EntryData>& itemList);
      virtual void OperateOn(const QVariant& itemData);

   protected slots:
      void OnNameChange(const QString& text);

   signals:
         void NameChanged(const QString& text);

   private:
      Ui::InspectorWidget* mUI;
      dtCore::ObserverPtr<dtCore::BaseActorObject> mOperateOn;

      void BuildChildren(osg::Group* parent, QList<EntryData>& childList);
      void Update();
   };
}
#endif // OSGVIEW_H
