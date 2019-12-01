#ifndef transformableview_h__
#define transformableview_h__

#include <QtCore/QObject>
#include <dtCore/observerptr.h>
#include <dtCore/transformable.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace Ui
{
   class InspectorWidget;
}
/// @endcond

namespace dtInspectorQt
{
   /** Handles the properties of dtCore::Transformable.
    *  Not really a view by itself, but meant to be used by other views. The
    *  TransformableView is a singleton which can be called upon by other views
    *  which need to display Transformable information.
    *  @see GetInstance()
    *  @see Destroy()
    *  @see OperateOn()
    */
   class TransformableView : public QObject
   {
      Q_OBJECT

   public:
      ///Get the singleton instance
      static TransformableView& GetInstance(Ui::InspectorWidget& ui);

      ///Destroy this singleton
      static void Destroy();

      /** 
        * Operate on the supplied Transformable. Will update the UI with information
        * related to this instance.
        * @param trans The Transformable to control/display with this view
        */
      void OperateOn(dtCore::Transformable* trans);
   
   protected:
      ~TransformableView();

   protected slots:
      void OnXYZHPRChanged(double val);
      void Update();

   private:
      TransformableView(Ui::InspectorWidget& ui);
      dtCore::ObserverPtr<dtCore::Transformable> mOperateOn;
      Ui::InspectorWidget* mUI;
      static TransformableView* mInstance;
   };
}
#endif // transformableview_h__
