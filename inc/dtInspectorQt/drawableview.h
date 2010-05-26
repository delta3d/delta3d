#ifndef drawableview_h__
#define drawableview_h__

#include <QtCore/QObject>
#include <dtInspectorQt/iview.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/observerptr.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace Ui
{
   class InspectorWidget;
}
/// @endcond

namespace dtInspectorQt
{
   ///Handles the properties of dtCore::DeltaDrawable
   class DrawableView : public IView
   {
      Q_OBJECT

   public:
   	DrawableView(Ui::InspectorWidget& ui);
   	~DrawableView();

      virtual void OperateOn(dtCore::Base* b);
      bool IsOfType(QString name, dtCore::Base* object);

   protected slots:
      void OnRenderProxyNode(int checked);
      void OnActive(int checked);

   	
   private:
      Ui::InspectorWidget* mUI;
      dtCore::ObserverPtr<dtCore::DeltaDrawable> mOperateOn;
      void Update();
   };
}
#endif // drawableview_h__
