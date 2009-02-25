#ifndef imanager_h__
#define imanager_h__

#include <QtCore/QObject>

namespace dtCore
{
   class Base;
}

namespace dtInspectorQt
{
   ///Interface class to the handlers of dtCore::Base derivatives
   class IManager : public QObject
   {
   public:
      IManager() {};
      ~IManager() {};

      virtual void OperateOn(dtCore::Base* b) = 0;
   };
}
#endif // imanager_h__
