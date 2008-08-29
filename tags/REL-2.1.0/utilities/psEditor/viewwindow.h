#ifndef viewwindow_h__
#define viewwindow_h__
#include <dtABC/fl_dt_window.h>

///Little class used to hold the Delta3D rendering surface
class ViewWindow : public Fl_DT_Window<dtABC::Widget>
{
public:
   typedef  Fl_DT_Window<dtABC::Widget>   MyParent;

	ViewWindow();

   ViewWindow(int x, int y, int w, int h);

	~ViewWindow();

   virtual void Config( const dtABC::WinData* data = NULL );

};
#endif // viewwindow_h__
