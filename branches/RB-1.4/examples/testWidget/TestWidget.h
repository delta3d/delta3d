// generated by Fast Light User Interface Designer (fluid) version 1.0104

#ifndef TestWidget_h
#define TestWidget_h
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include "MyWidget.h"
#include "MyWindow.h"

class TestWidget {
public:
  TestWidget();
private:
  Fl_Window *MainWindow;
  MyWindow *ViewPort;
public:
  void make_window();
};
#endif