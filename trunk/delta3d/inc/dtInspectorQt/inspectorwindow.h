#ifndef inspectorwindow_h__
#define inspectorwindow_h__

#include <dtInspectorQt/export.h>
#include <dtInspectorQt/iview.h>
#include <QtGui/QMainWindow>
#include <QtGui/QTreeWidgetItem>
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
namespace dtGame
{
   class GameManager;
}
/// @endcond

namespace dtInspectorQt
{
   ///The rendered Qt window for InspectorQt.  Uses a .ui file to define the widgets.
   class DT_INSPECTORQT_EXPORT InspectorWindow : public QMainWindow
   {

      Q_OBJECT

   public:
      InspectorWindow(QWidget* parent = NULL);
      ~InspectorWindow();

      QWidget* GetPropertyContainerWidget();
      void AddCustomView(IView* customView);
      void SetGameManager(dtGame::GameManager* gm);

   public slots:
      void OnSelection(QTreeWidgetItem* current, QTreeWidgetItem* prev);
      void RefreshCurrentItem();
      void OnNameChanged(const QString& text);
      void UpdateInstances();
      void SortList(bool sorted);
      void RefreshFilters();
      void FilterSelected(const QString& text);

   private:

      void BuildItemTree(QTreeWidgetItem* parent, IView::EntryData& data);

      Ui::InspectorWidget* ui;
      dtGame::GameManager* mGameManager;

      QList<IView*> mViewContainer;

      int mFilterIndex;
   };
}
#endif // inspectorwindow_h__
