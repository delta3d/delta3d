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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "guiscreen.h"

#ifdef None
#undef None
#endif
#include <CEGUI/CEGUI.h>


namespace dtExample
{
   //////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   //////////////////////////////////////////////////////////////////////////
   const std::string GuiScreen::DEFAULT_NAME("dtExample::GuiScreen");

   //////////////////////////////////////////////////////////////////////////
   GuiScreen::GuiScreen( const std::string& name, const std::string& layoutFile )
      : BaseClass()
      , mName(name)
      , mLayoutFile(layoutFile)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   GuiScreen::~GuiScreen()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   GuiNode* GuiScreen::GetRoot()
   {
      return mRoot;
   }

   //////////////////////////////////////////////////////////////////////////
   const GuiNode* GuiScreen::GetRoot() const
   {
      return mRoot;
   }

   //////////////////////////////////////////////////////////////////////////
   void GuiScreen::SetName(const std::string& name)
   {
      mName = name;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& GuiScreen::GetName() const
   {
      return mName;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& GuiScreen::GetLayoutFileName() const
   {
      return mLayoutFile;
   }

   //////////////////////////////////////////////////////////////////////////
   void GuiScreen::SetVisible( bool visible )
   {
      if( mRoot != NULL )
      {
         mRoot->setVisible( visible );
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool GuiScreen::IsVisible() const
   {
      return mRoot != NULL && mRoot->isVisible();
   }

   //////////////////////////////////////////////////////////////////////////
   void GuiScreen::Setup(GuiNode* root)
   {
      CEGUI::WindowManager* wm = CEGUI::WindowManager::getSingletonPtr();

      mRoot = wm->loadWindowLayout( mLayoutFile );
      root->addChildWindow( mRoot );
   }

}
