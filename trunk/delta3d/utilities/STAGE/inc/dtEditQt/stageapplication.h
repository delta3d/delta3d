#ifndef stageapplication_h__
#define stageapplication_h__

#include <dtABC/application.h>

namespace dtEditQt
{
   ///simple Application only really needed to kill the default DeltaWin 
   ///inherited in Application.
   class STAGEApplication : public dtABC::Application
   {
   public:
      STAGEApplication(dtCore::DeltaWin* win=NULL);
      virtual void Config();

      virtual bool KeyPressed(const dtCore::Keyboard* keyboard, int kc);

   protected:
      virtual ~STAGEApplication();
   	
   private:
   };
}
#endif // stageapplication_h__
