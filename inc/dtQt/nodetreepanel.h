/* -*-c++-*-
* Copyright (C) 2015, Caper Holdings LLC
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* Chris Rodgers
*/

#ifndef DELTA_NODETREEPANEL_H
#define DELTA_NODETREEPANEL_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/export.h>
#include <dtCore/observerptr.h>
#include <dtQt/typedefs.h>
#include <QtGui/qwidget.h>
#include <QtGui/qtreewidget.h>
#include <QtGui/qstyleditemdelegate.h>
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

      /**
       * Sets values on the node from values contained in the UI for this item.
       */
      void UpdateData();

      /**
       * Sets values on the UI from values  contained in the node.
       */
      void UpdateUI();
      
      void UpdateDescription();

   protected:
      dtCore::ObserverPtr<osg::Node> mNode;
      osg::Node::NodeMask mVisibilityMask;
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

   signals:
      void SignalNodeSelected(OsgNodePtr node);
      void SignalNodesSelected(OsgNodePtrVector nodeArray);

   public slots:
      void UpdateColumns();
      void OnItemSelectionChanged();
      void OnNodeFilterClicked();
      void OnNodeFilterNameChanged();
      
      void OnItemChanged(QTreeWidgetItem* item, int column);

   protected:
      virtual void CreateConnections();

      Ui::NodeTreePanel* mUI;

      dtCore::ObserverPtr<osg::Node> mNode;
   };
}

#endif
