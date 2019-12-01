#ifndef DELTA_[!output PROJECT_NAME]
#define DELTA_[!output PROJECT_NAME]

// [!output PROJECT_NAME].cpp : declares the interface of the application

#include <dtABC/application.h>

class [!output PROJECT_NAME] : public dtABC::Application
{
   public:
      [!output PROJECT_NAME](const std::string& configFilename = "config.xml");
 
      virtual void Config();

      virtual bool KeyPressed(const dtCore::Keyboard* keyboard, int kc);

      virtual void PreFrame(const double deltaFrameTime);
      virtual void PostFrame(const double deltaFrameTime);

   protected:
     virtual ~[!output PROJECT_NAME]();
      
   private:
      //place member variables here
   
};

#endif // DELTA_[!output PROJECT_NAME]
