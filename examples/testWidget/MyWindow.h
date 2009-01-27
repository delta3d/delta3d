/* -*-c++-*-
* testWidget - MyWindow (.h & .cpp) - Using 'The MIT License'
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

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
