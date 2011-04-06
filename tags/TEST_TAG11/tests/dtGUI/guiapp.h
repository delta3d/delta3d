/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, MOVES Institute
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
*
* @author John K. Grant
*/
#include <prefix/unittestprefix.h>
#ifndef _TESTS_GUI_APP_H_
#define _TESTS_GUI_APP_H_

#include <dtABC/application.h>  // for base class
#include <dtCore/refptr.h>      // for member

namespace dtGUI
{
   class GUIManager;
   class ScriptModule;
}

namespace dtTest
{
   class GUIApp : public dtABC::Application
   {
   public:
      typedef dtABC::Application BaseClass;

      GUIApp();

      void Config();

      dtGUI::GUIManager* GetGUIManager();

      dtGUI::ScriptModule* GetScriptModule();

   protected:
      virtual ~GUIApp();

   private:
      dtCore::RefPtr<dtGUI::GUIManager> mGUIManager;
      dtGUI::ScriptModule* mScriptModule;
   };
}

#endif  // _TESTS_GUI_APP_H_
