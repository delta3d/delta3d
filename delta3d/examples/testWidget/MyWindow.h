#ifndef DELTA_MYWINDOW
#define DELTA_MYWINDOW

#include <dtABC/fl_dt_window.h>
#include "MyWidget.h"

class MyWindow :  public   Fl_DT_Window<MyWidget>
{
   DECLARE_MANAGEMENT_LAYER( MyWindow)

   typedef  Fl_DT_Window<MyWidget> MyParent;

   static   const char* PATH;
   static   const char* FILE;

public:
                  MyWindow();
                  MyWindow( int w, int h, const char* l = 0L );
                  MyWindow( int x, int y, int w, int h, const char* l = 0L );
protected:
   virtual        ~MyWindow();
public:
   virtual  void  show( void );

private:
            void  ctor( void );
};

#endif // DELTA_MYWINDOW
