#pragma once

#include <Fl_DT_Window.h>
#include "MyWidget.h"



class MyWindow :  public   Fl_DT_Window<MyWidget>
{
   typedef  Fl_DT_Window<MyWidget> MyParent;

   static   const char* PATH;
   static   const char* FILE;

public:
                  MyWindow();
                  MyWindow( int w, int h, const char* l = 0L );
                  MyWindow( int x, int y, int w, int h, const char* l = 0L );
   virtual        ~MyWindow();

   virtual  void  show( void );

private:
            void  ctor( void );
};
