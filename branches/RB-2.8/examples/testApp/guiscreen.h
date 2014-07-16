/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 */

#ifndef DELTA_TEST_APP_GUI_SCREEN
#define DELTA_TEST_APP_GUI_SCREEN

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "export.h"
#include <dtCore/refptr.h>
#include <osg/Referenced>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace CEGUI
{
   class Window;
}

typedef CEGUI::Window GuiNode;



namespace dtExample
{
   //////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   //////////////////////////////////////////////////////////////////////////
   class TEST_APP_EXPORT GuiScreen : public osg::Referenced
   {
      public:
         typedef osg::Referenced BaseClass;

         static const std::string DEFAULT_NAME;

         GuiScreen(const std::string& name, const std::string& layoutFile);

         GuiNode* GetRoot();
         const GuiNode* GetRoot() const;

         void SetName(const std::string& name);
         const std::string& GetName() const;

         const std::string& GetLayoutFileName() const;

         /**
         * Set the visibility of the screen.
         * @param visible TRUE if the screen should be made visible.
         */
         virtual void SetVisible(bool visible);
         virtual bool IsVisible() const;

         /**
          * Perform the initial setup of the object.
          * @param root Delta object that acts as the root attach point
          *        for the screen drawables. It is up to the implementation
          *        to cast the root and handle attaching elements to it.
          */
         virtual void Setup(GuiNode* root = NULL);

      protected:
         virtual ~GuiScreen();

      private:
         std::string mName;
         std::string mLayoutFile;

         // GUI Elements
         GuiNode* mRoot;
   };
}

#endif
