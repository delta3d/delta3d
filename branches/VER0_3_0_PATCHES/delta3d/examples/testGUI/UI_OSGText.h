#pragma once
#include "UI/IUI_Font.h"
#include <osgText/text>

class CUI_OSGText : public IUI_Font
{
public:
   CUI_OSGText(void);
   ~CUI_OSGText(void){}


   //! A generic text rendering function.
   /*! All text should be squeezed into [0,1] unit square. 
   Must be implemented.  Overload it for more useful text rendering.
   \param text the text to be rendered
   \param dtime the current time (for animation)
   */
   virtual bool RenderText( double dtime, IUI_Renderer *renderer, char *text );

   //! Render specifying number of rows & columns and where to start
   /*! Used by CUI_TextBox\n\n
   Much more useful for multiline text chunks\n\n
   Also returns information about how much space the text itself
   uses.
   */
   virtual bool RenderText( 	double dtime, IUI_Renderer *renderer,
      char *text, unsigned int columns, unsigned int rows,
      unsigned int startColumn, unsigned int startRow, bool wrap,
      unsigned int &textColumns, unsigned int &textRows,
      bool invert = false,
      unsigned int cursorPosition = 0, unsigned short cursorType = UI_CURSOR_NONE
      );

   //! Count the number of columns and rows without rendering
   virtual bool Count( char *text, unsigned int columns, unsigned int rows,
      bool wrap, unsigned int &textColumns, unsigned int &textRows );

private:
   osgText::Text *mTextNode;
};



//
// Automatic library inclusion macros that use the #pragma/lib feature
//
#undef _AUTOLIBNAME
#define _AUTOLIBNAME  "osgtext.lib"


/* You may turn off this include message by defining _NOAUTOLIB */
#ifndef _NOAUTOLIBMSG
#pragma message( "Will automatically link with " _AUTOLIBNAME )
#endif

#pragma comment(lib, _AUTOLIBNAME)

