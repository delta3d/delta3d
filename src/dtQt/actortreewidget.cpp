
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/actortreewidget.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   static const std::string ICON_ACTOR_DEFAULT("");
   static const std::string ICON_ACTOR(":dtQt/icons/actors/actor.png");
   static const std::string ICON_ACTOR_TRANSFORM(":dtQt/icons/actors/transform.png");
   static const std::string ICON_ACTOR_DRAWABLE(":dtQt/icons/actors/drawable.png");



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   QIcon ActorIconProviderBase::GetIcon(const BaseActor& actor)
   {
      return GetIcon(GetIconPath(actor));
   }

   QIcon ActorIconProviderBase::GetIcon(const std::string& iconPath)
   {
      QIcon icon(GetDefaultIcon());

      if (IsIconPathValid(iconPath))
      {
         QString qstr(iconPath.c_str());
         icon = QIcon(qstr);
      }

      return icon;
   }

   bool ActorIconProviderBase::IsIconPathValid(const std::string& iconPath) const
   {
      // TODO:
      // Determine the true validity of the path.

      return ! iconPath.empty();
   }

   QIcon ActorIconProviderBase::GetDefaultIcon() const
   {
      QString iconPath(ICON_ACTOR_DEFAULT.c_str());
      return QIcon(iconPath);
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   std::string ActorIconProviderDefault::GetIconPath(const BaseActor& actor)
   {
      const std::string* icon = &ICON_ACTOR;

      if (actor.GetDrawable() != NULL)
      {
         icon = &ICON_ACTOR_DRAWABLE;
      }
      else if (dynamic_cast<const dtGame::GameActorProxy*>(&actor) != NULL)
      {
         icon = &ICON_ACTOR_TRANSFORM;
      }

      return *icon;
   }
   
   

   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   ActorTreeItem::ActorTreeItem(BaseActor& actor, QTreeWidget* parent)
      : BaseClass(parent)
      , mActor(&actor)
   {
      UpdateDescription();
   }

   ActorTreeItem::~ActorTreeItem()
   {}

   void ActorTreeItem::SetActor(BaseActor* actor)
   {
      mActor = actor;

      UpdateDescription();
   }

   BaseActor* ActorTreeItem::GetActor() const
   {
      return mActor.get();
   }

   dtGame::GameActorProxy* ActorTreeItem::GetGameActor() const
   {
      return dynamic_cast<dtGame::GameActorProxy*>(mActor.get());
   }

   void ActorTreeItem::UpdateDescription()
   {
      QString qstr;

      if (mActor.valid())
      {
         std::string desc(mActor->GetActorType().GetName() + " (" + mActor->GetId().ToString() + ")");
         qstr = desc.c_str();
      }
      
      setText(1, qstr);
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class ActorTreeBuilder
   {
   public:
      typedef std::map<BaseActor*, ActorTreeItem*> ActorItemMap;

      ActorTreeBuilder(QTreeWidget& targetTree, ActorIconProviderBase& iconProvider)
         : mTree(&targetTree)
         , mIconProvider(&iconProvider)
      {}

      ActorTreeItem* GetItem(BaseActor& actor)
      {
         ActorItemMap::iterator foundItem = mActorItemMap.find(&actor);

         return (foundItem != mActorItemMap.end() ? foundItem->second : NULL);
      }

      ActorTreeItem* GetParentItem(BaseActor& actor)
      {
         ActorTreeItem* item = NULL;
         dtGame::GameActorProxy* gameActor = dynamic_cast<dtGame::GameActorProxy*>(&actor);

         if (gameActor != NULL)
         {
            BaseActor* parent = gameActor->GetParentActor();

            if (parent != NULL)
            {
               item = GetItem(*parent);
            }
         }

         return item;
      }

      ActorTreeItem* CreateItem(BaseActor& actor)
      {
         ActorTreeItem* item = new ActorTreeItem(actor);
         mActorItemMap.insert(std::make_pair(&actor, item));

         QIcon icon = mIconProvider->GetIcon(actor);
         item->setIcon(0, icon);

         // TODO: Any setup for the second column.

         return item;
      }

      ActorTreeItem* GetOrCreateItem(BaseActor& actor)
      {
         ActorTreeItem* item = GetItem(actor);

         if (item == NULL)
         {
            item = CreateItem(actor);

            QTreeWidgetItem* parentItem = GetParentItem(actor);
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

      virtual void ProcessActor(BaseActor& actor)
      {
         ActorTreeItem* item = GetOrCreateItem(actor);
         QString qname(actor.GetName().c_str());

         if (qname.isEmpty())
         {
            qname = "[Actor]";
         }

         item->setText(0, qname);

         QString qactorType(actor.GetActorType().GetName().c_str());
         item->setToolTip(0, qactorType);
      }

      void Traverse(ActorWeakPtrVector& actorList)
      {
         BaseActor* curActor = NULL;
         ActorWeakPtrVector::iterator curIter = actorList.begin();
         ActorWeakPtrVector::iterator endIter = actorList.end();
         for (; curIter != endIter; ++curIter)
         {
            curActor = curIter->get();

            if (curActor != NULL)
            {
               Traverse(*curActor);
            }
         }
      }

      void Traverse(BaseActor& actor)
      {
         dtGame::GameActorProxy* gameActor = dynamic_cast<dtGame::GameActorProxy*>(&actor);

         if (gameActor == NULL)
         {
            ProcessActor(actor);
         }
         else
         {
            BaseActor* curActor = NULL;
            dtGame::GameActorProxy::iterator curIter = gameActor->begin();
            dtGame::GameActorProxy::iterator endIter = gameActor->end();
            for (; curIter != endIter; ++curIter)
            {
               curActor = curIter->value;

               if (curActor != NULL)
               {
                  ProcessActor(*curActor);
               }
            }
         }
      }

      int GetActorCount() const
      {
         return (int)(mActorItemMap.size());
      }

      QTreeWidget* mTree;
      ActorItemMap mActorItemMap;
      dtCore::RefPtr<ActorIconProviderBase> mIconProvider;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   ActorTreeWidget::ActorTreeWidget(QWidget* parent)
      : BaseClass(parent)
      , mIconProvider(new ActorIconProviderDefault)
      , mMouseDragging(false)
   {
      CreateConnections();

      UpdateUI();
   }

   ActorTreeWidget::~ActorTreeWidget()
   {}

   void ActorTreeWidget::CreateConnections()
   {
      // ITEMS
      connect(this, SIGNAL(itemSelectionChanged()),
         this, SLOT(OnItemSelectionChanged()));
   }

   void ActorTreeWidget::SetActors(ActorWeakPtrVector actors, bool updateUI)
   {
      mActors = actors;

      if (updateUI)
      {
         UpdateUI();

         expandAll();
         
         UpdateColumns();
      }
   }

   ActorWeakPtrVector ActorTreeWidget::GetActors() const
   {
      return mActors;
   }

   void ActorTreeWidget::SetIconProvider(ActorIconProviderBase& iconProvider)
   {
      mIconProvider = &iconProvider;
   }

   ActorIconProviderBase& ActorTreeWidget::GetIconProvider() const
   {
      return *mIconProvider;
   }

   unsigned ActorTreeWidget::GetActorCount() const
   {
      return mActors.size();
   }

   void ActorTreeWidget::UpdateColumns()
   {
      resizeColumnToContents(0);
      resizeColumnToContents(1);
   }

   void ActorTreeWidget::UpdateUI()
   {
      clear();

      ActorTreeBuilder builder(*this, *mIconProvider);
      builder.Traverse(mActors);

      emit SignalUpdatedUI();
   }

   void ActorTreeWidget::OnItemSelectionChanged()
   {
      ActorRefPtrVector actorArray;

      typedef QList<QTreeWidgetItem*> ActorItemsArray;
      const ActorItemsArray items = selectedItems();

      ActorTreeItem* curItem = NULL;
      ActorItemsArray::const_iterator curIter = items.begin();
      ActorItemsArray::const_iterator endIter = items.end();
      for (; curIter != endIter; ++curIter)
      {
         curItem = dynamic_cast<ActorTreeItem*>(*curIter);
         if (curItem != NULL)
         {
            BaseActor* actor = curItem->GetActor();
            if (actor != NULL)
            {
               actorArray.push_back(actor);
            }
         }
      }

      if ( ! actorArray.empty())
      {
         // Signal for multiple actors.
         emit SignalActorsSelected(actorArray);
      }
   }

   void ActorTreeWidget::mousePressEvent(QMouseEvent* mouseEvent)
   {
      BaseClass::mousePressEvent(mouseEvent);

      mMouseDragging = true;
   }

   void ActorTreeWidget::rowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end )
   {
      // DEBUG:
      //printf("\n\tROW REMOVED\n");

      BaseClass::rowsAboutToBeRemoved(parent, start, end);
   }

   void ActorTreeWidget::rowsInserted(const QModelIndex & parentIndex, int start, int end )
   {
      BaseClass::rowsInserted(parentIndex, start, end);

      if ( ! mMouseDragging)
      {
         return;
      }

      mMouseDragging = false;

      // DEBUG:
      //printf("\n\tROW INSERTED\n");

      ActorTreeItem* itemParent = (ActorTreeItem*)itemFromIndex(parentIndex);
      if (itemParent != NULL)
      {
         dtCore::ActorPtr oldParent;
         dtCore::ActorPtr newParent = itemParent->GetActor();
         dtCore::ActorPtr actor;

         // One or more items may have been inserted for this parent.
         // Attach all actors to the parent that are associated with the child items.
         int childCount = itemParent->childCount();
         for (int i = start; i <= end && i < childCount; ++i)
         {
            ActorTreeItem* item = (ActorTreeItem*)itemParent->child(i);
            actor = item == NULL ? NULL : item->GetActor();

            dtCore::ActorComponentContainer* gameActor = dynamic_cast<dtCore::ActorComponentContainer*>(actor.get());
            if (gameActor != NULL)
            {
               oldParent = gameActor->GetParentBaseActor();
            }

            // DEBUG:
            printf("\t\tItem: %s\n", ( ! actor.valid()?"":actor->GetName().c_str()));

            // Notify the system that the actor hierarchy has changed.
            emit SignalActorAttach(actor, oldParent, newParent);
         }
      }
      else // Detach because parent is NULL
      {
         // Get the root items of the widget.
         int itemCount = topLevelItemCount();
         dtCore::RefPtr<dtGame::GameActorProxy> actor;
         for (int i = start; i <= end && i < itemCount; ++i)
         {
            ActorTreeItem* item = (ActorTreeItem*)topLevelItem(i);
            actor = item == NULL ? NULL : item->GetGameActor();
         
            if (actor != NULL)
            {
               dtCore::BaseActorObject* oldParent = actor->GetParentActor();
               if (oldParent != NULL)
               {
                  emit SignalActorDetach(actor, oldParent);
               }
            }
         }
      }

      emit SignalUpdatedUI();
   }

   bool ActorTreeWidget::dropMimeData(QTreeWidgetItem* parent, int index,
      const QMimeData* data, Qt::DropAction action)
   {
      //BaseClass::dropMimeData(parent, index, data, action);

      // TODO:
      //printf("\n\tDROP - %d\n", currentItem());

      return false;
   }

}
