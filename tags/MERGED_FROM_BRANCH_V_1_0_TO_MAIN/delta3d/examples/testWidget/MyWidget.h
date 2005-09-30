#ifndef DELTA_MYWIDGET
#define DELTA_MYWIDGET

#include "dtCore/dt.h"
#include "dtABC/dtabc.h"

class MyWidget :  public   dtABC::Widget
{

   DECLARE_MANAGEMENT_LAYER( MyWidget )

      enum  MyEvents
      {
         FILELOADED  = BIT(0),
      };


public:
                  MyWidget( std::string name = "MyWidget" );
   virtual        ~MyWidget();

protected:
   virtual  void  Config( const dtABC::WinData* data = NULL );
   virtual  void  OnMessage( dtCore::Base::MessageData* data );

private:
   inline   void  ObjectLoaded( dtCore::Object* obj );
   inline   void  InitInputDevices( void );

private:
            dtCore::RefPtr<dtCore::LogicalInputDevice> mInputDevice;
            dtCore::RefPtr<dtCore::MotionModel>        mMotionModel;
};

#endif // DELTA_MYWIDGET
