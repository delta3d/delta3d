#ifndef DELTA_ACTORTREEWIDGET_H
#define DELTA_ACTORTREEWIDGET_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/export.h>
#include <QtGui/qtreewidget.h>
#include <dtGame/gameactorproxy.h>
#include <dtQt/typedefs.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // TYPE DEFINITIONS
   /////////////////////////////////////////////////////////////////////////////
   typedef dtCore::BaseActorObject BaseActor;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class ActorIconProviderBase : public osg::Referenced
   {
   public:
      ActorIconProviderBase() {}

      /**
       * Main method to implement to determine the icon resource location for
       * the provided actor.
       * @param actor Object for which to find a list item icon.
       * @return Icon resource string in Qt icon resource path format.
       */
      virtual std::string GetIconPath(const BaseActor& actor) = 0;

      /**
       * Convenience method for short hand to find an icon for an actor without
       * having to handle a resource path string. This method calls GetIconPath internally.
       * @param actor Object for which to find a list item icon.
       * @return Icon that represents the provided actor or actor type; a default icon will be used if a valid icon path could not be found.
       */
      virtual QIcon GetIcon(const BaseActor& actor);

      virtual QIcon GetIcon(const std::string& iconPath);

      virtual bool IsIconPathValid(const std::string& iconPath) const;

      virtual QIcon GetDefaultIcon() const;

   protected:
      virtual ~ActorIconProviderBase() {}
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class ActorIconProviderDefault : public ActorIconProviderBase
   {
   public:
      typedef ActorIconProviderBase BaseClass;

      ActorIconProviderDefault() {}

      /*virtual*/ std::string GetIconPath(const BaseActor& actor);

   protected:
      virtual ~ActorIconProviderDefault() {}
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT ActorTreeItem : public QTreeWidgetItem
   {
   public:
      typedef QTreeWidgetItem BaseClass;

      ActorTreeItem(BaseActor& actor, QTreeWidget* parent = NULL);
      virtual ~ActorTreeItem();

      void SetActor(BaseActor* actor);
      BaseActor* GetActor() const;

      dtGame::GameActorProxy* GetGameActor() const;
      
      void UpdateDescription();

   protected:
      ActorWeakPtr mActor;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT ActorTreeWidget : public QTreeWidget
   {
      Q_OBJECT
   public:
      typedef QTreeWidget BaseClass;

      ActorTreeWidget(QWidget* parent = NULL);

      virtual ~ActorTreeWidget();

      void SetActors(ActorWeakPtrVector actors, bool updateUI = true);
      ActorWeakPtrVector GetActors() const;

      void SetIconProvider(ActorIconProviderBase& iconProvider);
      ActorIconProviderBase& GetIconProvider() const;

      unsigned GetActorCount() const;

      virtual void UpdateUI();

      virtual void mousePressEvent(QMouseEvent* mouseEvent);

      virtual void rowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end );
      virtual void rowsInserted ( const QModelIndex & parent, int start, int end );

   signals:
      void SignalUpdatedUI();
      void SignalActorsSelected(ActorRefPtrVector actorArray);
      void SignalActorAttach(dtCore::ActorPtr actor, dtCore::ActorPtr oldParent, dtCore::ActorPtr newParent);
      void SignalActorDetach(dtCore::ActorPtr actor, dtCore::ActorPtr oldParent);

   public slots:
      void UpdateColumns();
      void OnItemSelectionChanged();

   protected:
      virtual void CreateConnections();

      virtual bool dropMimeData(QTreeWidgetItem* parent, int index, const QMimeData* data, Qt::DropAction action);
      
      ActorWeakPtrVector mActors;
      dtCore::RefPtr<ActorIconProviderBase> mIconProvider;

      bool mMouseDragging;
   };

}

#endif
