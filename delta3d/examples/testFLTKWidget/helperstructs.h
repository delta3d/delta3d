#ifndef DELTA_HELPERSTRUCTS
#define DELTA_HELPERSTRUCTS

#include <FL/x.H>



#if   !  defined(BIT)
///BIT  helper definition for enumerated values and bit packing
#define  BIT(a)   (1<<a)
#endif

#ifndef _WIN32
typedef unsigned long HWND; 
#endif // _WIN32

///WinRect  struct for passing window dimensions
struct   WinRect
{
   static   const int   DEF_X;   ///default x position
   static   const int   DEF_Y;   ///default y position
   static   const int   DEF_W;   ///default width
   static   const int   DEF_H;   ///default height

   int   pos_x;                  ///window x position
   int   pos_y;                  ///window y position
   int   width;                  ///window width
   int   height;                 ///window height

   WinRect( int x = DEF_X, int y = DEF_Y, int w = DEF_W, int h = DEF_H );
   WinRect( const WinRect& that );
   WinRect& operator=( const WinRect& that );
};



///WinData  struct for passing window handle and dimensions
struct   WinData   :  public   WinRect
{
   HWND  hwnd;                   ///window handle

   WinData( HWND hw = 0L, int x = DEF_X, int y = DEF_Y, int w = DEF_W, int h = DEF_H );
   WinData( const WinData& that );
   WinData( const WinRect& that );
   WinData& operator=( const WinData& that );
};

#endif // DELTA_HELPERSTRUCTS
