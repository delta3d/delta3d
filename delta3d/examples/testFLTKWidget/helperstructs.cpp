#include "helperstructs.h"

   /// static default values
   const int   WinRect::DEF_X(0L);     ///default x position
   const int   WinRect::DEF_Y(0L);     ///default y position
   const int   WinRect::DEF_W(640L);   ///default width
   const int   WinRect::DEF_H(480L);   ///default height



/**
 * Default Constructor.
 *
 * @param x horizontal position
 * @param y vertical position
 * @param w width
 * @param h height
 */
WinRect::WinRect( int x, int y, int w, int h )
:  pos_x(x),
   pos_y(y),
   width(w),
   height(h)
{
}



/**
 * Copy Constructor.
 *
 * @param that object to copy from
 */
WinRect::WinRect( const WinRect& that )
:  pos_x(that.pos_x),
   pos_y(that.pos_y),
   width(that.width),
   height(that.height)
{
}



/**
 * Copy Operator.
 *
 * @param that object to copy from
 *
 * @return a reference to this object
 */
WinRect&
WinRect::operator=( const WinRect& that )
{
   pos_x    = that.pos_x;
   pos_y    = that.pos_y;
   width    = that.width;
   height   = that.height;

   return   *this;
}



/**
 * Default Constructor.
 *
 * @param hw window handle
 * @param x horizontal position
 * @param y vertical position
 * @param w width
 * @param h height
 */
WinData::WinData( HWND hw, int x, int y, int w, int h )
:  WinRect(x,y,w,h),
   hwnd(hw)
{
}



/**
 * Copy Constructor.
 *
 * @param that object to copy from
 */
WinData::WinData( const WinData& that )
:  WinRect(that),
   hwnd(that.hwnd)
{
}



/**
 * Copy Constructor.
 *
 * @param that object to copy from
 */
WinData::WinData( const WinRect& that )
:  WinRect(that),
hwnd(0L)
{
}



/**
 * Copy Operator.
 *
 * @param that object to copy from
 *
 * @return a reference to this object
 */
WinData&
WinData::operator=( const WinData& that )
{
   WinRect(*this) = WinRect(that);
   hwnd           = that.hwnd;

   return   *this;
}

