#ifndef DELTA_MYWINDOW
#define DELTA_MYWINDOW

#include <dtABC/fl_dt_window.h>
#include <string>
#include "MyWidget.h"

class MyWindow : public Fl_DT_Window<MyWidget>
{
   typedef  Fl_DT_Window<MyWidget> MyParent;

   static std::string PATH;
   static std::string FILE;

public:
                  MyWindow();
                  MyWindow( int w, int h, const char* l = 0L );
                  MyWindow( int x, int y, int w, int h, const char* l = 0L );
protected:
   virtual        ~MyWindow();
public:
   virtual  void  show();

private:
            void  ctor();
};

#endif // DELTA_MYWINDOW
