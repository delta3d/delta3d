#ifndef DELTA_NODETREEPANEL_H
#define DELTA_NODETREEPANEL_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/export.h>
#include <dtCore/observerptr.h>
#include <QtGui/qwidget.h>
#include <QtGui/qtreewidget.h>
#include <osg/Node>



////////////////////////////////////////////////////////////////////////////////
// IFORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace Ui
{
   class NodeTreePanel;
}



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT NodeTreeItem : public QTreeWidgetItem
   {
   public:
      typedef QTreeWidgetItem BaseClass;

      NodeTreeItem(osg::Node& node, QTreeWidget* parent = NULL);
      virtual ~NodeTreeItem();

      void SetNode(osg::Node* node);
      osg::Node* GetNode() const;

   protected:
      dtCore::ObserverPtr<osg::Node> mNode;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT NodeTreePanel : public QWidget
   {
      Q_OBJECT
   public:
      typedef QWidget BaseClass;

      NodeTreePanel(QWidget* parent = NULL);

      virtual ~NodeTreePanel();

      virtual void UpdateUI();

      void SetNode(osg::Node* node, bool updateUI = true);
      osg::Node* GetNode() const;

   protected:
      virtual void CreateConnections();

      Ui::NodeTreePanel* mUI;

      dtCore::ObserverPtr<osg::Node> mNode;
   };
}

#endif
