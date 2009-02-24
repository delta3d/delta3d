#ifndef inspector_h__
#define inspector_h__

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
   class IManager;

   ///The rendered Qt window for InspectorQt.  Uses a .ui file to define the widgets.
   class DT_INSPECTORQT_EXPORT InspectorWindow : public QMainWindow
   {

      Q_OBJECT

   public:
      InspectorWindow(QWidget* parent = NULL);
      ~InspectorWindow();
   
   public slots:
      void OnSelection(QListWidgetItem* current, QListWidgetItem* prev);
      void OnNameChanged(const QString& text);

   private:
      Ui::InspectorWidget ui;

      QList<IManager*> mManagerContainer;
      void UpdateInstances();
   };
}
#endif // inspector_h__
