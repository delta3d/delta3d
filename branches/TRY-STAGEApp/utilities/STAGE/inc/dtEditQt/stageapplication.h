#ifndef stageapplication_h__
#define stageapplication_h__

#include <dtABC/application.h>

namespace dtEditQt
{
   class STAGEApplication : public dtABC::Application
   {
   public:
      STAGEApplication(dtCore::DeltaWin* win=NULL);
      virtual void Config();
   protected:
      virtual ~STAGEApplication();
   	
   private:
   };
}
#endif // stageapplication_h__