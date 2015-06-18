#ifndef DELTA_NODETREEPANEL_H
#define DELTA_NODETREEPANEL_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/export.h>
#include <dtCore/observerptr.h>
#include <dtCore/refptr.h>
#include <QtGui/qwidget.h>
#include <QtGui/qtreewidget.h>
#include <osg/Node>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
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
      
      void UpdateDescription();

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
      typedef dtCore::RefPtr<osg::Node> OsgNodePtr;
      typedef std::vector<OsgNodePtr > OsgNodeArray;

      NodeTreePanel(QWidget* parent = NULL);

      virtual ~NodeTreePanel();

      virtual void UpdateUI();

      void SetNode(osg::Node* node, bool updateUI = true);
      osg::Node* GetNode() const;

   signals:
      void SignalNodeSelected(OsgNodePtr node);
      void SignalNodesSelected(OsgNodeArray nodeArray);

   public slots:
      void UpdateColumns();
      void OnItemSelectionChanged();
      void OnNodeFilterClicked();

      void OnItemChanged(QTreeWidgetItem* item, int column);

   protected:
      virtual void CreateConnections();

      Ui::NodeTreePanel* mUI;

      dtCore::ObserverPtr<osg::Node> mNode;
   };
}

#endif
