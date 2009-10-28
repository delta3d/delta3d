#ifndef inspectorwindow_h__
#define inspectorwindow_h__

#include <dtInspectorQt/export.h>
#include <QtGui/QMainWindow>
#include <QtGui/QListWidgetItem>
#include <QtCore/QList>

namespace dtCore
{
   class Base;
}

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace Ui
{
   class InspectorWidget;
}
/// @endcond

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

      QWidget* GetPropertyContainerWidget();
      void AddCustomView(IView* customView);

   public slots:
      void OnSelection(QListWidgetItem* current, QListWidgetItem* prev);
      void RefreshCurrentItem();
      void OnNameChanged(const QString& text);
      void UpdateInstances();
      void SortList(bool sorted);
      void RefreshFilters();
      void FilterSelected(const QString& text);

   private:
      Ui::InspectorWidget* ui;

      QList<IView*> mViewContainer;

      QString mFilterName;
   };
}
#endif // inspectorwindow_h__
