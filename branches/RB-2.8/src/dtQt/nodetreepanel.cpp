
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ui_nodetreepanel.h"
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
   static const QString ICON_GEODE(":dtQt/icons/nodes/geode.png");
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
   {}

   NodeTreeItem::~NodeTreeItem()
   {}

   void NodeTreeItem::SetNode(osg::Node* node)
   {
      mNode = node;
   }

   osg::Node* NodeTreeItem::GetNode() const
   {
      return mNode.get();
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class NodeTreeBuilder : public osg::NodeVisitor
   {
   public:
      typedef osg::NodeVisitor BaseClass;

      typedef std::map<osg::Node*, NodeTreeItem*> NodeItemMap;

      NodeTreeBuilder(QTreeWidget& targetTree)
         : BaseClass(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
         , mTree(&targetTree)
      {}

      NodeTreeItem* GetItem(osg::Node& node)
      {
         NodeItemMap::iterator foundItem = mNodeItemMap.find(&node);

         return (foundItem != mNodeItemMap.end() ? foundItem->second : NULL);
      }

      NodeTreeItem* GetParentItem(osg::Node& node)
      {
         return node.getParents().empty() ? NULL
            : GetItem(*node.getParents().front());
      }

      NodeTreeItem* CreateItem(osg::Node& node)
      {
         NodeTreeItem* item = new NodeTreeItem(node);
         mNodeItemMap.insert(std::make_pair(&node, item));

         QIcon icon(GetIconForNodeType(node));
         item->setIcon(0, icon);

         if (node.getStateSet() != NULL)
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

         if (item == NULL)
         {
            item = CreateItem(node);

            QTreeWidgetItem* parentItem = GetParentItem(node);
            if (parentItem != NULL)
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
         NodeTreeItem* item = GetOrCreateItem(node);
         QString qname(node.getName().c_str());

         if (qname.isEmpty())
         {
            qname = "[Node]";
         }

         item->setText(0, qname);

         QString qnodeType(node.className());
         item->setToolTip(0, qnodeType);

         traverse(node);
      }

      int GetNodeCount() const
      {
         return (int)(mNodeItemMap.size());
      }

      QTreeWidget* mTree;
      NodeItemMap mNodeItemMap;
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
      mUI = NULL;
   }

   void NodeTreePanel::CreateConnections()
   {
      connect(mUI->mButtonExpandAll, SIGNAL(clicked()),
         mUI->mTree, SLOT(expandAll()));
      connect(mUI->mButtonCollapseAll, SIGNAL(clicked()),
         mUI->mTree, SLOT(collapseAll()));
   }

   void NodeTreePanel::UpdateUI()
   {
      mUI->mTree->clear();

      if (mNode.valid())
      {
         NodeTreeBuilder builder(*mUI->mTree);
         mNode->accept(builder);

         QString qstr = QString::number(builder.GetNodeCount());
         mUI->mNodeCount->setText(qstr);
      }
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
         }
      }
   }

   osg::Node* NodeTreePanel::GetNode() const
   {
      return mNode.get();
   }

}
