
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ui_nodetreepanel.h"
#include <QtCore/QString.h>
#include <QtGui/qtreewidget.h>
#include <dtQt/nodetreepanel.h>
#include <osg/Group>
#include <osg/NodeVisitor>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   static const QString ICON_NODE("");
   static const QString ICON_BONE(":dtQt/icons/nodes/bone.png");
   static const QString ICON_DOF(":dtQt/icons/nodes/dof.png");
   static const QString ICON_GEODE(":dtQt/icons/nodes/geode.png");
   static const QString ICON_GEOMETRY(":dtQt/icons/nodes/geometry.png");
   static const QString ICON_GROUP(":dtQt/icons/nodes/group.png");
   static const QString ICON_MATRIX(":dtQt/icons/nodes/matrix.png");
   static const QString ICON_SKELETON(":dtQt/icons/nodes/skeleton.png");
   static const QString ICON_STATESET(":dtQt/icons/nodes/stateset.png");
   
   

   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   NodeTreeItem::NodeTreeItem(osg::Node& node, QTreeWidget* parent)
      : BaseClass(parent)
      , mNode(&node)
   {
      UpdateDescription();
   }

   NodeTreeItem::~NodeTreeItem()
   {}

   void NodeTreeItem::SetNode(osg::Node* node)
   {
      mNode = node;

      UpdateDescription();
   }

   osg::Node* NodeTreeItem::GetNode() const
   {
      return mNode.get();
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
         , mFilterOutTransforms(false)
      {}

      bool IsNodeAllowed(osg::Node& node) const
      {
         bool answer = true;

         std::string nodeType(node.className());

         if (nodeType.compare("MatrixTransform") == 0)
         {
            answer = ! mFilterOutTransforms;
         }
         else if (nodeType.compare("Group") == 0)
         {
            answer = ! mFilterOutGroups;
         }
         else if (nodeType.compare("Geode") == 0
            || nodeType.compare("Geometry") == 0)
         {
            answer = ! mFilterOutGeodes;
         }
         else if (nodeType.compare("DOFTransform") == 0)
         {
            answer = ! mFilterOutDOFs;
         }

         return answer;
      }

      bool IsEnabled() const
      {
         return mFilterOutDOFs
            || mFilterOutGeodes
            || mFilterOutGroups
            || mFilterOutTransforms;
      }

      bool mFilterOutDOFs;
      bool mFilterOutGeodes;
      bool mFilterOutGroups;
      bool mFilterOutTransforms;

   protected:
      virtual ~NodeFilter() {}
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
         mNodeItemMap.insert(std::make_pair(&node, item));

         QIcon icon(GetIconForNodeType(node));
         item->setIcon(0, icon);

         if (node.getStateSet() != nullptr)
         {
            QIcon statesetIcon(ICON_STATESET);
            item->setIcon(1, statesetIcon);
            QString qtip("Stateset");
            item->setToolTip(1, qtip);
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
         const QString* icon = &ICON_NODE;

         std::string nodeType(node.className());

         if (nodeType.compare("MatrixTransform") == 0)
         {
            icon = &ICON_MATRIX;
         }
         else if (nodeType.compare("Group") == 0)
         {
            icon = &ICON_GROUP;
         }
         else if (nodeType.compare("Geode") == 0)
         {
            icon = &ICON_GEODE;
         }
         else if (nodeType.compare("Geometry") == 0)
         {
            icon = &ICON_GEOMETRY;
         }
         else if (nodeType.compare("DOFTransform") == 0)
         {
            icon = &ICON_DOF;
         }
         else if (nodeType.compare("Bone") == 0)
         {
            icon = &ICON_BONE;
         }
         else if (nodeType.compare("Skeleton") == 0)
         {
            icon = &ICON_SKELETON;
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
      connect(mUI->mButtonFilterTransforms, SIGNAL(clicked()),
         this, SLOT(OnNodeFilterClicked()));

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
         filter->mFilterOutTransforms = mUI->mButtonFilterTransforms->isChecked();
         if ( ! filter->IsEnabled())
         {
            filter = nullptr;
         }

         // Create node items for tree view.
         NodeTreeBuilder builder(*mUI->mTree, filter.get());
         mNode->accept(builder);

         QString qstr = QString::number(builder.GetNodeCount());
         mUI->mNodeCount->setText(qstr);

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
      OsgNodeArray nodeArray;

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

      osg::Node* node = nodeItem->GetNode();
      if (node != nullptr)
      {
         node->setName(nodeItem->text(0).toStdString());
      }
   }

}
