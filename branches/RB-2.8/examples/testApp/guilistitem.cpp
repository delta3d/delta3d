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
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "guilistitem.h"



namespace dtExample
{
   //////////////////////////////////////////////////////////////////////////
   // CUSTOM WINDOW FACTORIES
   //////////////////////////////////////////////////////////////////////////
   CEGUI_DEFINE_WINDOW_FACTORY(GuiListItem)



   //////////////////////////////////////////////////////////////////////////
   // CUSTOM WIDGET FUNCTIONS
   //////////////////////////////////////////////////////////////////////////
   void GuiListItem::BindFactory()
   {
      // Register special window factories.
      CEGUI::WindowFactoryManager& wfm = CEGUI::WindowFactoryManager::getSingleton();
      wfm.addFactory( &CEGUI_WINDOW_FACTORY(GuiListItem) );
   }

   //////////////////////////////////////////////////////////////////////////
   GuiListItem* GuiListItem::Create(const std::string& itemName)
   {
      CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();
      CEGUI::Window* item = wm->createWindow(GuiListItem::WIDGET_TYPE.Get(), itemName);

      return static_cast<GuiListItem*>(item);
   }



   //////////////////////////////////////////////////////////////////////////
   // LIST ITEM CODE
   //////////////////////////////////////////////////////////////////////////
   const CEGUI::String GuiListItem::WidgetTypeName("TestApp/GuiListItem");
   const dtUtil::RefString GuiListItem::WIDGET_TYPE(GuiListItem::WidgetTypeName.c_str());
   const dtUtil::RefString GuiListItem::LISTBOX_TYPE("WindowsLook/ItemListbox");

   //////////////////////////////////////////////////////////////////////////
   GuiListItem::GuiListItem( CEGUI::String type, CEGUI::String name )
      : BaseClass(type,name)
   {
      rename( name );
   }

   //////////////////////////////////////////////////////////////////////////
   GuiListItem::~GuiListItem()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   CEGUI::Size GuiListItem::getItemPixelSize(void) const
   {
      return CEGUI::Size( 10.0f, 10.0f );
   }
}
