#ifndef DELTA_ACTORTREEPANEL_H
#define DELTA_ACTORTREEPANEL_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/export.h>
#include <dtQt/actortreewidget.h>
#include <dtGame/gameactorproxy.h>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace Ui
{
   class ActorTreePanel;
}



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT ActorTreePanel : public QWidget
   {
      Q_OBJECT
   public:
      typedef QWidget BaseClass;

      ActorTreePanel(QWidget* parent = NULL);

      virtual ~ActorTreePanel();

      ActorTreeWidget& GetTreeWidget();

      virtual void UpdateUI();

   public slots:
      void OnActorsSelected(ActorRefPtrVector& actors);
      void OnTreeWidgetUpdated();

   protected:
      virtual void CreateConnections();

      Ui::ActorTreePanel* mUI;
   };
}

#endif
