#include "ui_osgtext.h"

CUI_OSGText::CUI_OSGText(void)
{
   mTextNode = new osgText::Text();
   osgText::Font* font = osgText::readFontFile("arial.ttf");
   mTextNode->setFont(font);
   mTextNode->setCharacterSize(20.f);
}


//! A generic text rendering function.
/*! All text should be squeezed into [0,1] unit square. 
Must be implemented.  Overload it for more useful text rendering.
\param text the text to be rendered
\param dtime the current time (for animation)
*/
bool CUI_OSGText::RenderText( double dtime, IUI_Renderer *renderer, char *text )
{

   return false;
}

//! Render specifying number of rows & columns and where to start
/*! Used by CUI_TextBox\n\n
Much more useful for multiline text chunks\n\n
Also returns information about how much space the text itself
uses.
*/
bool CUI_OSGText::RenderText( 	double dtime, IUI_Renderer *renderer,
                        char *text, unsigned int columns, unsigned int rows,
                        unsigned int startColumn, unsigned int startRow, bool wrap,
                        unsigned int &textColumns, unsigned int &textRows,
                        bool invert,
                        unsigned int cursorPosition, unsigned short cursorType
                        )
{


return false;
}

//! Count the number of columns and rows without rendering
bool CUI_OSGText::Count( char *text, unsigned int columns, unsigned int rows,
                   bool wrap, unsigned int &textColumns, unsigned int &textRows )
{
   return false;

}
