#ifndef inspectorwindow_h__
#define inspectorwindow_h__

#include "ui_dtinspectorqt.h"
#include <dtInspectorQt/export.h>
#include <QtGui/QMainWindow>
#include <QtCore/QList>

namespace dtCore
{
   class Base;
}

namespace dtInspectorQt
{
   class IView;

   ///The rendered Qt window for InspectorQt.  Uses a .ui file to define the widgets.
   class DT_INSPECTORQT_EXPORT InspectorWindow : public QMainWindow
   {

      Q_OBJECT

   public:
      InspectorWindow(QWidget* parent = NULL);
      ~InspectorWindow();
   
   public slots:
      void OnSelection(QListWidgetItem* current, QListWidgetItem* prev);
      void RefreshCurrentItem();
      void OnNameChanged(const QString& text);
      void UpdateInstances();

   private:
      Ui::InspectorWidget ui;

      QList<IView*> mViewContainer;
   };
}
#endif // inspectorwindow_h__
