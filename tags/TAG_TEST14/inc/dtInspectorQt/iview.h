#ifndef iview_h__
#define iview_h__

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QList>

#include <dtGame/gamemanager.h>

namespace dtCore
{
   class Base;
}

namespace dtInspectorQt
{
   ///Interface class to the handlers of dtCore::Base derivatives
   class IView : public QObject
   {
   public:
      struct EntryData
      {
         QString  name;
         QString  type;
         QVariant itemData;

         QList<IView::EntryData> children;
      };

      dtGame::GameManager* mGameManager;
      QString mFilterName;

      IView(): mGameManager(NULL) {}
      ~IView() {}

      //virtual void OperateOn(dtCore::Base* b) = 0;
      //virtual bool IsOfType(QString name, dtCore::Base* object) = 0;

      void SetGameManager(dtGame::GameManager* gm)
      {
         mGameManager = gm;
      }

      virtual void Build(QList<EntryData>& itemList) {}
      virtual void OperateOn(const QVariant& itemData) {}
   };
} // namespace dtInspectorQt

#endif // iview_h__
