#ifndef DELTA_MYWIDGET
#define DELTA_MYWIDGET

#include "dt.h"
#include "dtabc.h"

class MyWidget :  public   dtABC::Widget
{
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
            osg::ref_ptr<dtCore::LogicalInputDevice> mInputDevice;
            osg::ref_ptr<dtCore::MotionModel>        mMotionModel;
};

#endif // DELTA_MYWIDGET
