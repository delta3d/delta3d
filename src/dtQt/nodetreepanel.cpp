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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ui_nodetreepanel.h"
#include <QtCore/QString>
#include <QtGui/QTreeWidget>
#include <dtQt/constants.h>
#include <dtQt/nodetreepanel.h>
#include <osg/Group>
#include <osg/NodeVisitor>
#include <regex>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   const static int ITEM_COLUMN_NAME = 0;
   const static int ITEM_COLUMN_DETAILS = 1;
   
   

   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   NodeTreeItem::NodeTreeItem(osg::Node& node, QTreeWidget* parent)
      : BaseClass(parent)
      , mNode(&node)
      , mVisibilityMask(1)
   {
      UpdateUI();
      UpdateDescription();
   }

   NodeTreeItem::~NodeTreeItem()
   {}

   void NodeTreeItem::SetNode(osg::Node* node)
   {
      mNode = node;

      UpdateUI();
      UpdateDescription();
   }

   osg::Node* NodeTreeItem::GetNode() const
   {
      return mNode.get();
   }

   void NodeTreeItem::UpdateData()
   {
      if (mNode.valid())
      {
         // Determine if the node name should change.
         std::string itemText(text(ITEM_COLUMN_NAME).toStdString());
         if (itemText != mNode->getName())
         {
            mNode->setName(itemText);
         }

         osg::Node::NodeMask masks = mNode->getNodeMask();

         // Determine if the node should be visible.
         if (checkState(ITEM_COLUMN_NAME) == Qt::Checked)
         {
            if (masks == 0x0)
            {
               mNode->setNodeMask(mVisibilityMask);
            }
         }
         else // Unchecked
         {
            if (masks != 0x0)
            {
               mNode->setNodeMask(0x0);

               // Keep track of masks used for visibility.
               mVisibilityMask = masks;
            }
         }
      }
   }

   void NodeTreeItem::UpdateUI()
   {
      bool nodeVisible = mNode.valid() && mNode->getNodeMask() != 0x0;

      // Update Visibility Checkbox
      setCheckState(ITEM_COLUMN_NAME, nodeVisible ? Qt::Checked : Qt::Unchecked);

      // Update Text
      std::string itemText(text(ITEM_COLUMN_NAME).toStdString());
      if (mNode.valid() && itemText != mNode->getName())
      {
         QString qstr = mNode->getName().c_str();
         setText(ITEM_COLUMN_NAME, qstr);
      }
   }

   void NodeTreeItem::UpdateDescription()
   {
      QString qstr;

      if (mNode.valid())
      {
         typedef osg::Node::DescriptionList StrArray;
         
         StrArray& descriptions = mNode->getDescriptions();

         const std::string* curDesc = nullptr;
         StrArray::const_iterator curIter = descriptions.begin();
         StrArray::const_iterator endIter = descriptions.end();
         for (size_t i = 0; curIter != endIter; ++curIter, ++i)
         {
            curDesc = &(*curIter);

            qstr += QString(curDesc->c_str());
            if (i + 1 < descriptions.size())
            {
               qstr += QString("\n");
            }
         }
      }

      setText(1, qstr);
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class NodeFilter : public osg::Referenced
   {
   public:
      NodeFilter()
         : mFilterOutDOFs(false)
         , mFilterOutGeodes(false)
         , mFilterOutGroups(false)
         , mFilterOutOccluders(false)
         , mFilterOutTransforms(false)
         , mFilterNameInvalid(false)
      {}

      bool IsNodeAllowed(osg::Node& node) const
      {
         using namespace dtUtil;

         bool answer = true;

         std::string nodeClass(node.className());
         const NodeType* nodeType = NodeType::GetNodeTypeByClassName(nodeClass);

         if (nodeType == &NodeType::MATRIX)
         {
            answer = ! mFilterOutTransforms;
         }
         else if (nodeType == &NodeType::GROUP)
         {
            answer = ! mFilterOutGroups;
         }
         else if (nodeType == &NodeType::GEODE
            || nodeType == &NodeType::GEOMETRY)
         {
            answer = ! mFilterOutGeodes;
         }
         else if (nodeType == &NodeType::DOF)
         {
            answer = ! mFilterOutDOFs;
         }
         else if (nodeType == &NodeType::OCCLUDER
            || nodeType == &NodeType::OCCLUSION_QUERY)
         {
            answer = ! mFilterOutOccluders;
         }

         // If the node has not already been filtered out with answer == false...
         if (answer && ! mFilterName.empty())
         {
            // ...attempt a match by name.
            try
            {
               answer = std::regex_match(node.getName(), std::regex(mFilterName));
            }
            catch (...)
            {
               // Match filter is most likely malformed here.
               // It needs to be nullified to suppress any more
               // exception throwing.
               mFilterName = "";
               mFilterNameInvalid = true;
               answer = true;
            }
         }

         return answer;
      }

      bool IsEnabled() const
      {
         return mFilterOutDOFs
            || mFilterOutGeodes
            || mFilterOutGroups
            || mFilterOutOccluders
            || mFilterOutTransforms
            || ! mFilterName.empty();
      }

      void SetFilterName(const std::string& nameFilter)
      {
         mFilterName = nameFilter;

         if ( ! mFilterName.empty())
         {
            mFilterName = ".*" + mFilterName + ".*";
         }
      }

      const std::string GetFilterName() const
      {
         return mFilterName;
      }

      bool IsFilterNameInvalid() const
      {
         return mFilterNameInvalid;
      }

      bool mFilterOutDOFs;
      bool mFilterOutGeodes;
      bool mFilterOutGroups;
      bool mFilterOutOccluders;
      bool mFilterOutTransforms;

   protected:
      virtual ~NodeFilter() {}

      mutable std::string mFilterName;
      mutable bool mFilterNameInvalid;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class NodeTreeBuilder : public osg::NodeVisitor
   {
   public:
      typedef osg::NodeVisitor BaseClass;

      typedef std::map<osg::Node*, NodeTreeItem*> NodeItemMap;

      NodeTreeBuilder(QTreeWidget& targetTree, NodeFilter* filter = nullptr)
         : BaseClass(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
         , mTree(&targetTree)
         , mNodeFilter(filter)
      {}

      NodeTreeItem* GetItem(osg::Node& node)
      {
         NodeItemMap::iterator foundItem = mNodeItemMap.find(&node);

         return (foundItem != mNodeItemMap.end() ? foundItem->second : nullptr);
      }

      NodeTreeItem* GetParentItem(osg::Node& node)
      {
         return node.getParents().empty() ? nullptr
            : GetItem(*node.getParents().front());
      }

      NodeTreeItem* CreateItem(osg::Node& node)
      {
         NodeTreeItem* item = new NodeTreeItem(node);
         item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);

         mNodeItemMap.insert(std::make_pair(&node, item));

         // Icon
         QIcon icon(GetIconForNodeType(node));
         item->setIcon(ITEM_COLUMN_NAME, icon);

         // Details Field
         osg::StateSet* ss = node.getStateSet();
         if (ss != nullptr)
         {
            int col = ITEM_COLUMN_DETAILS;

            QIcon statesetIcon(Constants::ICON_STATESET);
            item->setIcon(col, statesetIcon);

            std::string str = "Stateset \"" + ss->getName() + "\"";
            QString qtip(str.c_str());
            item->setToolTip(col, qtip);
         }

         return item;
      }

      NodeTreeItem* GetOrCreateItem(osg::Node& node)
      {
         NodeTreeItem* item = GetItem(node);

         if (item == nullptr)
         {
            item = CreateItem(node);

            QTreeWidgetItem* parentItem = GetParentItem(node);
            if (parentItem != nullptr)
            {
               parentItem->addChild(item);
            }
            else
            {
               mTree->addTopLevelItem(item);
            }
         }

         return item;
      }
   
      const QString& GetIconForNodeType(const osg::Node& node)
      {
         const QString* icon = &Constants::ICON_NODE;

         std::string nodeType(node.className());

         const QString* result = Constants::GetIconPathByClassName(nodeType);

         if (result != nullptr)
         {
            icon = result;
         }

         return *icon;
      }

      virtual void apply(osg::Node& node)
      {
         bool allowNode = ! mNodeFilter.valid() || mNodeFilter->IsNodeAllowed(node);

         if (allowNode)
         {
            NodeTreeItem* item = GetOrCreateItem(node);
            QString qname(node.getName().c_str());

            if (qname.isEmpty())
            {
               qname = "[Node]";
            }

            item->setText(0, qname);

            QString qnodeType(node.className());
            item->setToolTip(0, qnodeType);
         }

         traverse(node);
      }

      int GetNodeCount() const
      {
         return (int)(mNodeItemMap.size());
      }

      QTreeWidget* mTree;
      NodeItemMap mNodeItemMap;
      dtCore::RefPtr<NodeFilter> mNodeFilter;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   NodeTreePanel::NodeTreePanel(QWidget* parent)
      : BaseClass(parent)
      , mUI(new Ui::NodeTreePanel)
   {
      mUI->setupUi(this);

      CreateConnections();

      UpdateUI();
   }

   NodeTreePanel::~NodeTreePanel()
   {
      delete mUI;
      mUI = nullptr;
   }

   void NodeTreePanel::CreateConnections()
   {
      // BUTTONS
      connect(mUI->mButtonExpandAll, SIGNAL(clicked()),
         mUI->mTree, SLOT(expandAll()));
      connect(mUI->mButtonCollapseAll, SIGNAL(clicked()),
         mUI->mTree, SLOT(collapseAll()));

      connect(mUI->mButtonExpandAll, SIGNAL(clicked()),
         this, SLOT(UpdateColumns()));
      connect(mUI->mButtonCollapseAll, SIGNAL(clicked()),
         this, SLOT(UpdateColumns()));

      connect(mUI->mButtonFilterDOFs, SIGNAL(clicked()),
         this, SLOT(OnNodeFilterClicked()));
      connect(mUI->mButtonFilterGeodes, SIGNAL(clicked()),
         this, SLOT(OnNodeFilterClicked()));
      connect(mUI->mButtonFilterGroups, SIGNAL(clicked()),
         this, SLOT(OnNodeFilterClicked()));
      connect(mUI->mButtonFilterOccluders, SIGNAL(clicked()),
         this, SLOT(OnNodeFilterClicked()));
      connect(mUI->mButtonFilterTransforms, SIGNAL(clicked()),
         this, SLOT(OnNodeFilterClicked()));

      connect(mUI->mTextFilterName, SIGNAL(editingFinished()),
         this, SLOT(OnNodeFilterNameChanged()));

      // ITEMS
      connect(mUI->mTree, SIGNAL(itemSelectionChanged()),
         this, SLOT(OnItemSelectionChanged()));

      connect(mUI->mTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
         this, SLOT(OnItemChanged(QTreeWidgetItem*, int)));
   }

   void NodeTreePanel::UpdateUI()
   {
      mUI->mTree->clear();

      if (mNode.valid())
      {
         // Determine if node types should be filtered.
         dtCore::RefPtr<NodeFilter> filter = new NodeFilter;
         filter->mFilterOutDOFs = mUI->mButtonFilterDOFs->isChecked();
         filter->mFilterOutGeodes = mUI->mButtonFilterGeodes->isChecked();
         filter->mFilterOutGroups = mUI->mButtonFilterGroups->isChecked();
         filter->mFilterOutOccluders = mUI->mButtonFilterOccluders->isChecked();
         filter->mFilterOutTransforms = mUI->mButtonFilterTransforms->isChecked();
         filter->SetFilterName(mUI->mTextFilterName->text().toStdString());

         if ( ! filter->IsEnabled())
         {
            filter = nullptr;
         }

         // Create node items for tree view.
         NodeTreeBuilder builder(*mUI->mTree, filter.get());
         mNode->accept(builder);

         QString qstr = QString::number(builder.GetNodeCount());
         mUI->mNodeCount->setText(qstr);

         // Update the color of the name filter to indicate
         // the validity of the search string.
         if (filter != nullptr && filter->IsFilterNameInvalid())
         {
            mUI->mTextFilterName->setStyleSheet("color: red");
            mUI->mTextFilterName->setToolTip("Regex string malformed.");
         }
         else
         {
            mUI->mTextFilterName->setStyleSheet("color: black");
            mUI->mTextFilterName->setToolTip("");
         }

         // Expand the tree to show all the results.
         mUI->mTree->expandAll();

         UpdateColumns();
      }
   }

   void NodeTreePanel::UpdateColumns()
   {
      mUI->mTree->resizeColumnToContents(0);
      mUI->mTree->resizeColumnToContents(1);
   }

   void NodeTreePanel::OnItemSelectionChanged()
   {
      OsgNodePtrVector nodeArray;

      typedef QList<QTreeWidgetItem*> NodeItemsArray;
      const NodeItemsArray items = mUI->mTree->selectedItems();

      NodeTreeItem* curItem = nullptr;
      NodeItemsArray::const_iterator curIter = items.begin();
      NodeItemsArray::const_iterator endIter = items.end();
      for (; curIter != endIter; ++curIter)
      {
         curItem = dynamic_cast<NodeTreeItem*>(*curIter);
         if (curItem != nullptr)
         {
            osg::Node* node = curItem->GetNode();
            if (node != nullptr)
            {
               nodeArray.push_back(node);
            }
         }
      }

      if ( ! nodeArray.empty())
      {
         // Signal for a single node.
         emit SignalNodeSelected(nodeArray.front());

         // Signal for multiple nodes.
         emit SignalNodesSelected(nodeArray);
      }
   }

   void NodeTreePanel::OnNodeFilterClicked()
   {
      UpdateUI();
   }

   void NodeTreePanel::OnNodeFilterNameChanged()
   {
      UpdateUI();
   }

   void NodeTreePanel::SetNode(osg::Node* node, bool updateUI)
   {
      if (mNode != node)
      {
         mNode = node;

         if (updateUI)
         {
            UpdateUI();

            mUI->mTree->expandAll();
            
            UpdateColumns();
         }
      }
   }

   osg::Node* NodeTreePanel::GetNode() const
   {
      return mNode.get();
   }

   void NodeTreePanel::OnItemChanged(QTreeWidgetItem* item, int column)
   {
      NodeTreeItem* nodeItem = static_cast<NodeTreeItem*>(item);
      nodeItem->UpdateData();
   }

}
