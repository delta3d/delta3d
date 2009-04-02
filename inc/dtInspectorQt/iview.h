#ifndef iview_h__
#define iview_h__

#include <QtCore/QObject>

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
      IView() {};
      ~IView() {};

      virtual void OperateOn(dtCore::Base* b) = 0;
   };
}
#endif // iview_h__
